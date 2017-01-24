#include "postgres.h"

#include "access/gin.h"
#include "access/stratnum.h"
#include "utils/array.h"
#include "utils/builtins.h"
#include "utils/lsyscache.h"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif


#define GinOverlapStrategy		1
#define GinContainsStrategy		2
#define GinContainedStrategy	3
#define GinEqualStrategy		4

/** Functions **/
Datum my_ginarrayextract(PG_FUNCTION_ARGS);
Datum my_ginqueryarrayextract(PG_FUNCTION_ARGS);
Datum my_ginarrayconsistent(PG_FUNCTION_ARGS);
//Datum my_ginarraytriconsistent(PG_FUNCTION_ARGS)

Datum my_ginarrayextract(PG_FUNCTION_ARGS)
{
	
	ArrayType  *array = PG_GETARG_ARRAYTYPE_P_COPY(0);
	int32	   *nkeys = (int32 *) PG_GETARG_POINTER(1);
	bool	  **nullFlags = (bool **) PG_GETARG_POINTER(2);
	int16		elmlen;
	bool		elmbyval;
	char		elmalign;
	Datum	   *elems;
	bool	   *nulls;
	int			nelems;

	get_typlenbyvalalign(ARR_ELEMTYPE(array),
						 &elmlen, &elmbyval, &elmalign);

	deconstruct_array(array,
					  ARR_ELEMTYPE(array),
					  elmlen, elmbyval, elmalign,
					  &elems, &nulls, &nelems);

	*nkeys = nelems;
	*nullFlags = nulls;

	
	PG_RETURN_POINTER(elems);
}


Datum ginarrayextract_2args(PG_FUNCTION_ARGS)
{
	if (PG_NARGS() < 3)			
		elog(ERROR, "ginarrayextract requires three arguments");
	return ginarrayextract(fcinfo);
}

Datum my_ginqueryarrayextract(PG_FUNCTION_ARGS)
{
	
	ArrayType  *array = PG_GETARG_ARRAYTYPE_P_COPY(0);
	int32	   *nkeys = (int32 *) PG_GETARG_POINTER(1);
	StrategyNumber strategy = PG_GETARG_UINT16(2);

	/* bool   **pmatch = (bool **) PG_GETARG_POINTER(3); */
	/* Pointer	   *extra_data = (Pointer *) PG_GETARG_POINTER(4); */
	bool	  **nullFlags = (bool **) PG_GETARG_POINTER(5);
	int32	   *searchMode = (int32 *) PG_GETARG_POINTER(6);
	int16		elmlen;
	bool		elmbyval;
	char		elmalign;
	Datum	   *elems;
	bool	   *nulls;
	int			nelems;

	get_typlenbyvalalign(ARR_ELEMTYPE(array),
						 &elmlen, &elmbyval, &elmalign);

	deconstruct_array(array,
					  ARR_ELEMTYPE(array),
					  elmlen, elmbyval, elmalign,
					  &elems, &nulls, &nelems);

	*nkeys = nelems;
	*nullFlags = nulls;

	switch (strategy)
	{
		case GinOverlapStrategy:
			*searchMode = GIN_SEARCH_MODE_DEFAULT;
			break;
		case GinContainsStrategy:
			if (nelems > 0)
				*searchMode = GIN_SEARCH_MODE_DEFAULT;
			else	/* everything contains the empty set */
				*searchMode = GIN_SEARCH_MODE_ALL;
			break;
		case GinContainedStrategy:
			/* empty set is contained in everything */
			*searchMode = GIN_SEARCH_MODE_INCLUDE_EMPTY;
			break;
		case GinEqualStrategy:
			if (nelems > 0)
				*searchMode = GIN_SEARCH_MODE_DEFAULT;
			else
				*searchMode = GIN_SEARCH_MODE_INCLUDE_EMPTY;
			break;
		default:
			elog(ERROR, "ginqueryarrayextract: unknown strategy number: %d",
				 strategy);
	}

	
	PG_RETURN_POINTER(elems);
}


Datum my_ginarrayconsistent(PG_FUNCTION_ARGS)
{
	bool	   *check = (bool *) PG_GETARG_POINTER(0);
	StrategyNumber strategy = PG_GETARG_UINT16(1);

	/* ArrayType  *query = PG_GETARG_ARRAYTYPE_P(2); */
	int32		nkeys = PG_GETARG_INT32(3);

	/* Pointer	   *extra_data = (Pointer *) PG_GETARG_POINTER(4); */
	bool	   *recheck = (bool *) PG_GETARG_POINTER(5);

	/* Datum	   *queryKeys = (Datum *) PG_GETARG_POINTER(6); */
	bool	   *nullFlags = (bool *) PG_GETARG_POINTER(7);
	bool		res;
	int32		i;

	switch (strategy)
	{
		case GinOverlapStrategy:
			
			*recheck = false;

			res = false;
			for (i = 0; i < nkeys; i++)
			{
				if (check[i] && !nullFlags[i])
				{
					res = true;
					break;
				}
			}
			break;
		case GinContainsStrategy:
			*recheck = false;	
			res = true;
			for (i = 0; i < nkeys; i++)
			{
				if (!check[i] || nullFlags[i])
				{
					res = false;
					break;
				}
			}
			break;
		case GinContainedStrategy:
			
			*recheck = true;
			
			res = true;
			break;
		case GinEqualStrategy:
	
			*recheck = true;

			
			res = true;
			for (i = 0; i < nkeys; i++)
			{
				if (!check[i])
				{
					res = false;
					break;
				}
			}
			break;
		default:
			elog(ERROR, "ginarrayconsistent: unknown strategy number: %d",
				 strategy);
			res = false;
	}

	PG_RETURN_BOOL(res);
}


/*Datum my_ginarraytriconsistent(PG_FUNCTION_ARGS)
{
	GinTernaryValue *check = (GinTernaryValue *) PG_GETARG_POINTER(0);
	StrategyNumber strategy = PG_GETARG_UINT16(1);

	// ArrayType  *query = PG_GETARG_ARRAYTYPE_P(2); 
	int32		nkeys = PG_GETARG_INT32(3);

	// Pointer	   *extra_data = (Pointer *) PG_GETARG_POINTER(4); 
	// Datum	   *queryKeys = (Datum *) PG_GETARG_POINTER(5); 
	bool	   *nullFlags = (bool *) PG_GETARG_POINTER(6);
	GinTernaryValue res;
	int32		i;

	switch (strategy)
	{
		case GinOverlapStrategy:
			
			res = GIN_FALSE;
			for (i = 0; i < nkeys; i++)
			{
				if (!nullFlags[i])
				{
					if (check[i] == GIN_TRUE)
					{
						res = GIN_TRUE;
						break;
					}
					else if (check[i] == GIN_MAYBE && res == GIN_FALSE)
					{
						res = GIN_MAYBE;
					}
				}
			}
			break;
		case GinContainsStrategy:
		
			res = GIN_TRUE;
			for (i = 0; i < nkeys; i++)
			{
				if (check[i] == GIN_FALSE || nullFlags[i])
				{
					res = GIN_FALSE;
					break;
				}
				if (check[i] == GIN_MAYBE)
				{
					res = GIN_MAYBE;
				}
			}
			break;
		case GinContainedStrategy:
		
			res = GIN_MAYBE;
			break;
		case GinEqualStrategy:

			res = GIN_MAYBE;
			for (i = 0; i < nkeys; i++)
			{
				if (check[i] == GIN_FALSE)
				{
					res = GIN_FALSE;
					break;
				}
			}
			break;
		default:
			elog(ERROR, "ginarrayconsistent: unknown strategy number: %d",
				 strategy);
			res = false;
	}

	PG_RETURN_GIN_TERNARY_VALUE(res);
}*/
