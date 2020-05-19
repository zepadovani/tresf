# Makefile to build class 'helloworld' for Pure Data.
# Needs Makefile.pdlibbuilder as helper makefile for platform-dependent build
# settings and rules.

# library name
lib.name = tresf

##Source file(s) which must be statically linked to each class in the library.

##estrategia para ter uma biblioteca estaticamente linkada a todos as classes da biblioteca
#common.sources = shared/rand.c

# input source file (class name == source file basename)
#class.sources = tresf.c

##estrategia para ter uma biblioteca estaticamente linkada a classes específicas da biblioteca
hrand := shared/rand.c
tresf.class.sources := tresf.c $(hrand)


# all extra files to be included in binary distribution of the library
datafiles = tresf-help.pd tresf-meta.pd

# include Makefile.pdlibbuilder from submodule directory 'pd-lib-builder'
PDLIBBUILDER_DIR=pd-lib-builder/
include $(PDLIBBUILDER_DIR)/Makefile.pdlibbuilder
