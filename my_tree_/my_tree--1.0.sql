\echo Use "CREATE EXTENSION my_tree" to load this file. \quit

CREATE FUNCTION distance(text,text)
RETURNS float8
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OPERATOR <-> (
        LEFTARG = text,
        RIGHTARG = text,
        PROCEDURE = distance,
        COMMUTATOR = '<->'
);

CREATE FUNCTION mytree_consistent(internal, data_type, smallint, oid, internal)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT INMUTABLE;

CREATE FUNCTION mytree_decompress(internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION mytree_compress(internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION mytree_penalty(internal, internal, internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION mytree_picksplit(internal, internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION mytree_union(internal, internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION mytree_same(internal, internal, internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION mytree_distance(internal,text,smallint,oid,internal)
RETURNS float8
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR CLASS gist_mytree_ops
FOR TYPE text USING gist
AS
        OPERATOR        15      <-> (text, text) FOR ORDER BY pg_catalog.float_ops,
        FUNCTION        1       mytree_consistent (internal, text, smallint, oid, internal),
        FUNCTION        2       mytree_union (internal, internal),
        FUNCTION        3       mytree_compress (internal),
        FUNCTION        4       mytree_decompress (internal),
        FUNCTION        5       mytree_penalty (internal, internal, internal),
        FUNCTION        6       mytree_picksplit (internal, internal),
        FUNCTION        7       mytree_same (internal, internal, internal),
		FUNCTION        8       mytree_distance(internal,text,smallint,oid,internal);
