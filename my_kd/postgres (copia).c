#include "postgres.h"
#include "utils/array.h"
#include "utils/memutils.h"

#include "access/spgist.h"
#include "access/stratnum.h"
#include "catalog/pg_type.h"
#include "utils/builtins.h"
#include "utils/geo_decls.h"
#include <math.h>

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif


#define ARRPTR(x)  ( (float8 *) ARR_DATA_PTR(x) )
#define ARRNELEMS(x)  ArrayGetNItems(ARR_NDIM(x), ARR_DIMS(x))

Datum spg_mykd_config(PG_FUNCTION_ARGS);
Datum spg_mykd_choose(PG_FUNCTION_ARGS);
Datum spg_mykd_picksplit(PG_FUNCTION_ARGS);
Datum spg_mykd_inner_consistent(PG_FUNCTION_ARGS);
Datum spg_mykd_leaf_consistent(PG_FUNCTION_ARGS);



PG_FUNCTION_INFO_V1(distance);
PG_FUNCTION_INFO_V1(lesserthan);
PG_FUNCTION_INFO_V1(greaterthan);
PG_FUNCTION_INFO_V1(equalto);
Datum distance (PG_FUNCTION_ARGS)
{
	ArrayType *a=PG_GETARG_ARRAYTYPE_P(0);
	ArrayType *b=PG_GETARG_ARRAYTYPE_P(1);
	int			na,
				nb;
	int			n;
	double	   *da,
			   *db;
	double 		d,
				cont=0;

	na = ARRNELEMS(a);
	nb = ARRNELEMS(b);
	da = ARRPTR(a);
	db = ARRPTR(b);

	d = -1.0;
	if (na == nb){
		for (n = 0; n < na; n++)
		{
			double i = db[n] - da[n];
			cont += i*i;
		}
		d = sqrt(cont);
	}
	PG_RETURN_FLOAT8(d);
}

Datum lesserthan (PG_FUNCTION_ARGS)
{
	ArrayType *a=PG_GETARG_ARRAYTYPE_P(0);
	ArrayType *b=PG_GETARG_ARRAYTYPE_P(1);
	int			na,
				nb;
	int			n;
	double	   *da,
			   *db;

	na = ARRNELEMS(a);
	nb = ARRNELEMS(b);
	da = ARRPTR(a);
	db = ARRPTR(b);

	bool result = false;

	if (na == nb)
	{
		result = true;

		for (n = 0; n < na; n++)
		{
			if (da[n] >= db[n])
			{
				result = false;
				break;
			}
		}
	}

	PG_RETURN_BOOL(result);
} 

Datum greaterthan (PG_FUNCTION_ARGS)
{
	ArrayType *a=PG_GETARG_ARRAYTYPE_P(0);
	ArrayType *b=PG_GETARG_ARRAYTYPE_P(1);
	int			na,
				nb;
	int			n;
	double	   *da,
			   *db;

	na = ARRNELEMS(a);
	nb = ARRNELEMS(b);
	da = ARRPTR(a);
	db = ARRPTR(b);

	bool result = false;

	if (na == nb)
	{
		result = true;

		for (n = 0; n < na; n++)
		{
			if (da[n] <= db[n])
			{
				result = false;
				break;
			}
		}
	}
	PG_RETURN_BOOL(result);
} 

Datum equalto (PG_FUNCTION_ARGS)
{
	ArrayType *a=PG_GETARG_ARRAYTYPE_P(0);
	ArrayType *b=PG_GETARG_ARRAYTYPE_P(1);
	int			na,
				nb;
	int			n;
	double	   *da,
			   *db;

	na = ARRNELEMS(a);
	nb = ARRNELEMS(b);
	da = ARRPTR(a);
	db = ARRPTR(b);

	bool result = false;

	if (na == nb)
	{
		result = true;

		for (n = 0; n < na; n++)
		{
			if (da[n] != db[n])
			{
				result = false;
				break;
			}
		}
	}
	PG_RETURN_BOOL(result);
} 


Datum
spg_mykd_config(PG_FUNCTION_ARGS)
{
	/* spgConfigIn *cfgin = (spgConfigIn *) PG_GETARG_POINTER(0); */
	spgConfigOut *cfg = (spgConfigOut *) PG_GETARG_POINTER(1);

	cfg->prefixType = FLOAT8OID;
	cfg->labelType = VOIDOID;	/* we don't need node labels */
	cfg->canReturnData = true;
	cfg->longValuesOK = false;
	PG_RETURN_VOID();
}

static int
getSide(double coord, int pos, ArrayType *array)
{
	double		tstcoord;
	double		*ptr;

	ptr = ARRPTR(array);
	tstcoord = ptr[pos-1];

	if (coord == tstcoord)
		return 0;
	else if (coord > tstcoord)
		return 1;
	else
		return -1;
}

Datum
spg_mykd_choose(PG_FUNCTION_ARGS)
{
	spgChooseIn *in = (spgChooseIn *) PG_GETARG_POINTER(0);
	spgChooseOut *out = (spgChooseOut *) PG_GETARG_POINTER(1);
	ArrayType	  *inArray = DatumGetArrayTypeP(in->datum);
	double		coord;
	int 		pos, n;

	if (in->allTheSame)
		elog(ERROR, "error");

	Assert(in->hasPrefix);
	coord = DatumGetFloat8(in->prefixDatum);

	Assert(in->nNodes == 2);

	out->resultType = spgMatchNode;
	n=ARRNELEMS(inArray);
	if (in->level % n ==0)
		pos = n;
	else
		pos = in->level % n;

	if (getSide(coord, pos, inArray) > 0)
		out->result.matchNode.nodeN = 0;
	else
		out->result.matchNode.nodeN = 1;
	out->result.matchNode.levelAdd = 1;
	out->result.matchNode.restDatum = PointerGetDatum(inArray);

	PG_RETURN_VOID();
}

typedef struct SortedArray
{
	ArrayType   *a;
	int			ind;
	int 		axis;
} SortedArray;

static int
cmp(const void *a, const void *b)
{
	SortedArray *pa = (SortedArray *) a;
	SortedArray *pb = (SortedArray *) b;

	double		*ptra;
	double		*ptrb;
	int 		ind;

	ptra = ARRPTR(pa->a);
	ptrb = ARRPTR(pb->a);
	ind = pa->axis;

	if (ptra[ind] == ptrb[ind])
		return 0;
	return (ptra[ind] > ptrb[ind]) ? 1 : -1;
}

Datum
spg_mykd_picksplit(PG_FUNCTION_ARGS)
{
	spgPickSplitIn *in = (spgPickSplitIn *) PG_GETARG_POINTER(0);
	spgPickSplitOut *out = (spgPickSplitOut *) PG_GETARG_POINTER(1);
	int			i;
	int			middle;
	SortedArray *sorted;
	double		coord;
	double		*ptr;

	sorted = palloc(sizeof(*sorted) * in->nTuples); //size
	for (i = 0; i < in->nTuples; i++)
	{
		int n=ARRNELEMS(sorted[i].a);
		sorted[i].a = DatumGetArrayTypeP(in->datums[i]);
		sorted[i].ind = i;
		sorted[i].axis = (in->level % n == 0) ? n - 1 : (in->level % n) - 1;
	}

	qsort(sorted, in->nTuples, sizeof(*sorted), cmp);
	middle = in->nTuples >> 1;
	
	ptr = ARRPTR(sorted[middle].a);
	coord = ptr[sorted[middle].axis];

	out->hasPrefix = true;
	out->prefixDatum = Float8GetDatum(coord);

	out->nNodes = 2;
	out->nodeLabels = NULL;		/* we don't need node labels */

	out->mapTuplesToNodes = palloc(sizeof(int) * in->nTuples);
	out->leafTupleDatums = palloc(sizeof(Datum) * in->nTuples);

	/*
	 * Note: points that have coordinates exactly equal to coord may get
	 * classified into either node, depending on where they happen to fall in
	 * the sorted list.  This is okay as long as the inner_consistent function
	 * descends into both sides for such cases.  This is better than the
	 * alternative of trying to have an exact boundary, because it keeps the
	 * tree balanced even when we have many instances of the same point value.
	 * So we should never trigger the allTheSame logic.
	 */
	for (i = 0; i < in->nTuples; i++)
	{
		ArrayType	*a = sorted[i].a;
		int			n = sorted[i].ind;

		out->mapTuplesToNodes[n] = (i < middle) ? 0 : 1;
		out->leafTupleDatums[n] = PointerGetDatum(a);
	}

	PG_RETURN_VOID();
}

Datum
spg_mykd_inner_consistent(PG_FUNCTION_ARGS)
{
	spgInnerConsistentIn *in = (spgInnerConsistentIn *) PG_GETARG_POINTER(0);
	spgInnerConsistentOut *out = (spgInnerConsistentOut *) PG_GETARG_POINTER(1);
	double		coor;
	int			which;
	int			i;

	Assert(in->hasPrefix);
	coor = DatumGetFloat8(in->prefixDatum);

	if (in->allTheSame)
		elog(ERROR, "error");

	Assert(in->nNodes == 2);

	/* "which" is a bitmask of children that satisfy all constraints */
	which = (1 << 1) | (1 << 2);

	for (i = 0; i < in->nkeys; i++)
	{
		ArrayType  *query = DatumGetArrayTypeP(in->scankeys[i].sk_argument);

		int n = ARRNELEMS(query);
		int pos;
		double *ptr;
		ptr = ARRPTR(query);
		if(in->level % n == 0)
			pos=n - 1;
		else
			pos=(in->level % n) - 1; 

		switch (in->scankeys[i].sk_strategy)
		{
			case BTLessStrategyNumber: // <
				
				
				if (ptr[pos]<coor)
					which &= (1 << 1);
				break;
			case BTGreaterStrategyNumber: // >
				
				
				if (ptr[pos]>coor)
					which &= (1 << 2);
				break;
			case BTEqualStrategyNumber: //  = same 
				
				
				if (ptr[pos]<coor)
					which &= (1 << 1);
				else
					if (ptr[pos]>coor)
						which &= (1 << 2);
				break;
			
			
			default:
				elog(ERROR, "unrecognized strategy number: %d",
					 in->scankeys[i].sk_strategy);
				break;
		}

		if (which == 0)
			break;				/* no need to consider remaining conditions */
	}

	/* We must descend into the children identified by which */
	out->nodeNumbers = (int *) palloc(sizeof(int) * 2);
	out->nNodes = 0;
	for (i = 1; i <= 2; i++)
	{
		if (which & (1 << i))
			out->nodeNumbers[out->nNodes++] = i - 1;
	}

	/* Set up level increments, too */
	out->levelAdds = (int *) palloc(sizeof(int) * 2);
	out->levelAdds[0] = 1;
	out->levelAdds[1] = 1;

	PG_RETURN_VOID();
}


Datum
spg_mykd_leaf_consistent(PG_FUNCTION_ARGS)
{
	spgLeafConsistentIn *in = (spgLeafConsistentIn *) PG_GETARG_POINTER(0);
	spgLeafConsistentOut *out = (spgLeafConsistentOut *) PG_GETARG_POINTER(1);
	ArrayType   *datum = DatumGetArrayTypeP(in->leafDatum);
	bool		result;
	int			i;

	/* all tests are exact */
	out->recheck = false;

	/* leafDatum is what it is... */
	out->leafValue = in->leafDatum;

	/* Perform the required comparison(s) */
	result = true;
	for (i = 0; i < in->nkeys; i++)
	{
		ArrayType	   *query = DatumGetArrayTypeP(in->scankeys[i].sk_argument);

		switch (in->scankeys[i].sk_strategy)
		{
			int			na,
						nb;
			int			n;
			double	   *da,
					   *db;
		
			na = ARRNELEMS(datum);
			nb = ARRNELEMS(query);
			da = ARRPTR(datum);
			db = ARRPTR(query);
			case BTLessStrategyNumber: // <

				result = false;

				if (na == nb)
				{
					result = true;

					for (n = 0; n < na; n++)
					{
						if (da[n] >= db[n])
						{
							result = false;
							break;
						}
					}
				}
				break;
			case BTGreaterStrategyNumber: // >
				result = false;

				if (na == nb)
				{
					result = true;

					for (n = 0; n < na; n++)
					{
						if (da[n] <= db[n])
						{
							result = false;
							break;
						}
					}
				}

				break;
			case BTEqualStrategyNumber: //  = same 
				result = false;

				if (na == nb)
				{
					result = true;

					for (n = 0; n < na; n++)
					{
						if (da[n] != db[n])
						{
							result = false;
							break;
						}
					}
				}
				break;
		
			default:
				elog(ERROR, "unrecognized strategy number: %d",
					 in->scankeys[i].sk_strategy);
				break;
		}

		if (!result)
			break;
	}

	PG_RETURN_BOOL(result);
}