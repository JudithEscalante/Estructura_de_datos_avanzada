\echo Use "CREATE EXTENSION my_kd" to load this file. \quit

CREATE FUNCTION distance(float8[], float8[]) 
RETURNS float8
AS 'MODULE_PATHNAME' 
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION lesserThan(float8[], float8[]) 
RETURNS bool
AS 'MODULE_PATHNAME' 
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION greaterThan(float8[], float8[]) 
RETURNS bool
AS 'MODULE_PATHNAME' 
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION equalTo(float8[], float8[]) 
RETURNS bool
AS 'MODULE_PATHNAME' 
LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR <-> (
   LEFTARG = float8[], RIGHTARG = float8[], PROCEDURE = distance
);

CREATE OPERATOR < (
   LEFTARG = float8[], RIGHTARG = float8[], PROCEDURE = lesserThan
);

CREATE OPERATOR > (
   LEFTARG = float8[], RIGHTARG = float8[], PROCEDURE = greaterThan
);

CREATE OPERATOR = (
   LEFTARG = float8[], RIGHTARG = float8[], PROCEDURE = equalTo
);

CREATE FUNCTION spg_mykd_config(internal, internal) 
RETURNS void
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION spg_mykd_choose(internal, internal)
RETURNS void
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION spg_mykd_picksplit(internal, internal) 
RETURNS void
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION spg_mykd_inner_consistent(internal, internal)
RETURNS void
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION spg_mykd_leaf_consistent(internal, internal)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR CLASS spg_array_ops
DEFAULT FOR TYPE float8[] USING spgist
AS
	OPERATOR	1	<  ,
	OPERATOR	3	=  ,
	OPERATOR	5	>  ,
	OPERATOR	15	<-> (float8[], float8[]) FOR ORDER BY pg_catalog.float_ops,
	FUNCTION 	1	spg_mykd_config(internal, internal) ,
	FUNCTION 	2   g_mykd_choose(internal, internal),
	FUNCTION 	3   spg_mykd_picksplit(internal, internal),
	FUNCTION 	4   spg_mykd_inner_consistent(internal, internal),
	FUNCTION 	5   spg_mykd_leaf_consistent(internal, internal);
