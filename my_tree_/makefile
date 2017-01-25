# contrib/btree_gist/Makefile

MODULE_big = my_tree

OBJS =  func.o

EXTENSION = my_tree
DATA = my_tree--1.0.sql
PGFILEDESC = "My tree"

REGRESS = text

ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/my_tree_
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
