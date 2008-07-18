# Makefile.am for module MVD
# For use of automake
# V. Friese, 06/11/06


INCLUDES		= $(ROOTCFLAGS)
INCLUDES               += -I$(top_srcdir)/geobase
INCLUDES               += -I$(top_srcdir)/parbase
INCLUDES               += -I$(top_srcdir)/base 
INCLUDES               += -I$(top_srcdir)/cbmbase
INCLUDES               += -I$(top_srcdir)/field
INCLUDES               += -I$(top_srcdir)/passive

lib_LTLIBRARIES		= libMvd.la

libMvd_la_LDFLAGS 	= -version-info 0:0:0

libMvd_la_LIBADD	= -L$(ROOTLIBDIR) $(ROOTLIBS) -L$(top_srcdir)/base
libMvd_la_LIBADD       += -L$(top_srcdir)/parbase
libMvd_la_LIBADD       += -L$(top_srcdir)/base

sources                 = CbmMvd.cxx \
			  CbmMvdContFact.cxx \
			  CbmMvdGeo.cxx \
			  CbmMvdGeoPar.cxx \
			  CbmMvdHit.cxx \
			  CbmMvdHitMatch.cxx \
			  CbmMvdHitProducer.cxx \
			  CbmMvdPileupManager.cxx \
			  CbmMvdPoint.cxx

include_HEADERS		= $(patsubst %.cxx,%.h,$(sources))

noinst_HEADERS		= MvdLinkDef.h

libMvd_la_SOURCES	= $(sources) CbmMvdDict.cxx

CLEANFILES		= CbmMvdDict.cxx CbmMvdDict.h *~ libMvd.la

CbmMvdDict.cxx: $(include_HEADERS) MvdLinkDef.h
	$(ROOTCINT) -f $@ -c -DHAVE_CONFIG_H $(INCLUDES) $^
