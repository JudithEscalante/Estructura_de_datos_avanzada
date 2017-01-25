\echo Use "CREATE EXTENSION my_gin" to load this file. \quit

CREATE FUNCTION my_ginarrayextract(float8[], internal, internal) 
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION my_ginqueryarrayextract(internal, internal, int2, internal, internal, internal, internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION my_ginarrayconsistent(internal, int2, float8[], int4, internal, internal, internal, internal)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;



CREATE OPERATOR CLASS vec_ops
FOR TYPE float8[] USING gin
AS
	OPERATOR	1	&&  ,
	OPERATOR	2	@>	,
	OPERATOR	3	<@	,
	OPERATOR	4	=	,
	FUNCTION 	1	my_ginarrayextract(float8[], internal, internal) ,
	FUNCTION 	2   my_ginqueryarrayextract(internal, internal, int2, internal, internal, internal, internal),
	FUNCTION 	3   my_ginarrayconsistent(internal, int2, float8[], int4, internal, internal, internal, internal);