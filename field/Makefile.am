# Makefile.am for module FIELD
# For use of automake
#
# Volker Friese    13-02-2006


INCLUDES		= $(ROOTCFLAGS)
INCLUDES	       += -I$(top_srcdir)/base
INCLUDES	       += -I$(top_srcdir)/parbase

lib_LTLIBRARIES		= libField.la

libField_la_LDFLAGS 	= -version-info 0:0:0

libField_la_LIBADD	= -L$(ROOTLIBDIR) $(ROOTLIBS) 

sources	               := CbmFieldConst.cxx       \
			  CbmFieldContFact.cxx    \
			  CbmFieldMap.cxx         \
			  CbmFieldMapData.cxx     \
			  CbmFieldMapSym2.cxx     \
			  CbmFieldMapSym3.cxx     \
			  CbmFieldPar.cxx         \
			  CbmBsField.cxx          \
			  CbmFieldCreator.cxx    
#			  CbmMultiField.cxx

include_HEADERS		= $(patsubst %.cxx,%.h,$(sources))

noinst_HEADERS		= FieldLinkDef.h 

libField_la_SOURCES	= $(sources) FieldDict.cxx

CLEANFILES		= FieldDict.cxx FieldDict.h *~ libField.la

FieldDict.cxx: $(include_HEADERS) FieldLinkDef.h
	$(ROOTCINT) -f $@ -c -DHAVE_CONFIG_H $(INCLUDES) $^
