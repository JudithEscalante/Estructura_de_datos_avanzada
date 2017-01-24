MODULE_big = my_gin

OBJS =  my_gin.o

EXTENSION = my_gin
DATA = my_gin--1.0.sql
PGFILEDESC = "My gin"

REGRESS = float8[]

ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/my_gin
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
