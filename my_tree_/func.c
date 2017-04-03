#include "postgres.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "types.h"
#include "functions.h"

#define PI 3.14159265
#define distOp 15
PG_MODULE_MAGIC;

/// M tree

typedef struct key
{
	I dim,
	F rad,
	text center,
	I m
} key;

PG_FUNCTION_INFO_V1(mytree_gist_in);
PG_FUNCTION_INFO_V1(mytree_gist_out);


Datum mytree_gist_in(PG_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("tree_gist_in() no está implementado")));
	PG_RETURN_DATUM(0);
}

Datum mytree_gist_out(PG_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("tree_gist_out() no está implementado")));
	PG_RETURN_DATUM(0);
}


PG_FUNCTION_INFO_V1(mytree_compress);
PG_FUNCTION_INFO_V1(mytree_decompress);
PG_FUNCTION_INFO_V1(mytree_same);
PG_FUNCTION_INFO_V1(mytree_union);
PG_FUNCTION_INFO_V1(mytree_penalty);
PG_FUNCTION_INFO_V1(mytree_picksplit);
PG_FUNCTION_INFO_V1(mytree_consistent);
PG_FUNCTION_INFO_V1(mytree_distance);


Datum mytree_compress(PG_FUNCTION_ARGS){
    PG_RETURN_POINTER(PG_GETARG_POINTER (0));
}

Datum mytree_decompress(PG_FUNCTION_ARGS){
    PG_RETURN_POINTER(PG_GETARG_POINTER (0));
}

Datum mytree_penalty(PG_FUNCTION_ARGS){
	key  *origentry = (float8KEY *) DatumGetPointer(((GISTENTRY *) PG_GETARG_POINTER(0))->key);
	key  *newentry = (float8KEY *) DatumGetPointer(((GISTENTRY *) PG_GETARG_POINTER(1))->key);
	F	   *result = (F *) PG_GETARG_POINTER(2);

	D retval;
	
	F *o, *n;
	transform(origentry->center,o);
	transform(newentry->center,n);
	
	F d = dist(o,n);
	F cost = cost(o->rad, d);
	*result = cost;
	PG_RETURN_POINTER(result);

	free(o);
	free(n);
}

Datum mytree_picksplit(PG_FUNCTION_ARGS){
	GistEntryVector *entryvec = (GistEntryVector *) PG_GETARG_POINTER(0);
	GIST_SPLITVEC *v = (GIST_SPLITVEC *) PG_GETARG_POINTER(1);

	I         i,
              nbytes;
    OffsetNumber *left,
               *right;
    data_type  *tmp_union;
    data_type  *unionL;
    data_type  *unionR;
    GISTENTRY **raw_entryvec;

    maxoff = entryvec->n - 1;
    nbytes = (maxoff + 1) * sizeof(OffsetNumber);

    v->spl_left = (OffsetNumber *) palloc(nbytes);
    left = v->spl_left;
    v->spl_nleft = 0;

    v->spl_right = (OffsetNumber *) palloc(nbytes);
    right = v->spl_right;
    v->spl_nright = 0;

    unionL = NULL;
    unionR = NULL;
    I l=PickRandom(0, entryvec->n);
    I r=PickRandom(0, entryvec->n);
    raw_entryvec = (GISTENTRY **) malloc(entryvec->n * sizeof(void *));
    for (i = FirstOffsetNumber; i <= maxoff; i = OffsetNumberNext(i))
        raw_entryvec[i] = &(entryvec->vector[i]);

    for (i = FirstOffsetNumber; i <= maxoff; i = OffsetNumberNext(i))
    {
    	I real_index = raw_entryvec[i] - entryvec->vector;
    	tmp_union = getKey(entryvec->vector[real_index].key);
    	
    	F distance1 = dist(entryvec->vector[real_index].key->center, entryvec->vector[l].key->center);
    	F distance2 = dist(entryvec->vector[real_index].key->center, entryvec->vector[r].key->center);
		if (distance1 < distance2){
			if (unionL == NULL)
                unionL = tmp_union;
            else
                unionL = mytree_union(unionL, tmp_union);

            *left = real_index;
            ++left;
            ++(v->spl_nleft);
		}
		else{
			if (unionR == NULL)
                unionR = tmp_union;
            else
                unionR = mytree_union(unionR, tmp_union);

            *right = real_index;
            ++right;
            ++(v->spl_nright);
		}
    }
    v->spl_ldatum = getKey(unionL);
    v->spl_rdatum = getKey(unionR);
	PG_RETURN_POINTER(v);
}

Datum mytree_same(PG_FUNCTION_ARGS){
	key     *b1 = getKey(0);
	key     *b2 = getKey(1);
	B    *result = (B *) PG_GETARG_POINTER(2);

	if (b1 && b2){
		if (b1->dim==b2->dim && b1->rad==b2->rad && b1->center==b2->center && b1->m==b2->m)
			*result= true;
	}
        
	else
	    *result = (b1 == NULL && b2 == NULL) ? TRUE : FALSE;
	PG_RETURN_POINTER(result);
}

Datum mytree_union(PG_FUNCTION_ARGS){
	GistEntryVector *entryvec = (GistEntryVector *) PG_GETARG_POINTER(0);
    GISTENTRY  *ent = entryvec->vector;
    key  *out, *tmp,  *old;
    I numranges, i = 0;

    numranges = entryvec->n;
    tmp = getKey(ent[0].key);
    out = tmp;

    if (numranges == 1)
    {
        out = copy(tmp);

        PG_RETURN_TEXT(out);
    }

    for (i = 1; i < numranges; i++)
    {
        old = out;
        tmp = getKey(ent[i].key);
	    F *o, *n;
		transform(out->center,o);
		transform(tmp->center,n);
		
		F d = dist(o,n);
		if (d>out->rad)
        	out->rad = dist(d);
    }

    PG_RETURN_TEXT(out);
}

Datum mytree_consistent(PG_FUNCTION_ARGS){
	GISTENTRY  *entry = (GISTENTRY *) PG_GETARG_POINTER(0);
    text  *query = PG_GETARG_TEXTP(1);
    StrategyNumber strategy = (StrategyNumber) PG_GETARG_UINT16(2);
    /* Oid subtype = PG_GETARG_OID(3); */
    bool       *recheck = (bool *) PG_GETARG_POINTER(4);
    key  *key = getKey(entry->key);
    bool        retval;

    if (key == NULL || query == NULL)
        PG_RETURN_BOOL(FALSE);

    switch(strategy){
    	case distOp:
    		F *o;
			transform(query,o);
			I t1=calcTam(o);
			if (t1 != key->dim)
				retval=false;
			else
				retval=true;
    		break;
    	default:
    		elog(ERROR, "número incorrecto: %d", strategy);
    		retval = FALSE;
    }

}

Datum mytree_distance(PG_FUNCTION_ARGS){
	GISTENTRY  *entry = (GISTENTRY *) PG_GETARG_POINTER(0);
    text  *query = PG_GETARG_DATA_TYPE_P(1);
    StrategyNumber strategy = (StrategyNumber) PG_GETARG_UINT16(2);
    /* Oid subtype = PG_GETARG_OID(3); */
    /* bool *recheck = (bool *) PG_GETARG_POINTER(4); */
    key  *key = getKey(entry->key);
    double      retval;

    F *o, *i;
	transform(query,o);
	transform(key->center, i);
	retval=dist(o,i);
	free(o);
	free(i);
	PG_RETURN_FLOAT8(retval);
}