\echo Use "CREATE EXTENSION my_kd" to load this file. \quit

CREATE FUNCTION distance(float8[], float8[]) 
RETURNS float8
AS 'MODULE_PATHNAME' 
LANGUAGE C IMMUTABLE STRICT;


CREATE FUNCTION equalto(float8[], float8[]) 
RETURNS bool
AS 'MODULE_PATHNAME' 
LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR ~= (
   LEFTARG = float8[], RIGHTARG = float8[], PROCEDURE = equalto
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
	OPERATOR	6	~=  ,
	
	FUNCTION 	1	spg_mykd_config(internal, internal) ,
	FUNCTION 	2   spg_mykd_choose(internal, internal),
	FUNCTION 	3   spg_mykd_picksplit(internal, internal),
	FUNCTION 	4   spg_mykd_inner_consistent(internal, internal),
	FUNCTION 	5   spg_mykd_leaf_consistent(internal, internal);
