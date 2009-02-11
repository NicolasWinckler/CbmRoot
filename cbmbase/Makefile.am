# Makefile.am for module cbmbase
# For use of automake
#

INCLUDES		= $(ROOTCFLAGS)
INCLUDES               += -I$(top_srcdir)/geobase 
INCLUDES               += -I$(top_srcdir)/base 


lib_LTLIBRARIES		= libCbmBase.la

libCbmBase_la_LDFLAGS 	= -version-info 0:0:0

libCbmBase_la_LIBADD	= -L$(ROOTLIBDIR) $(ROOTLIBS)
libCbmBase_la_LIBADD   += -L$(top_srcdir)/base/ 

sources                 = CbmDigi.cxx \
	                  CbmDigiManager.cxx \
			  CbmLinkMC.cxx \
			  FairMCPoint.cxx \
                          CbmMCTrack.cxx \
                          CbmStack.cxx \
			  CbmTrkHit.cxx

include_HEADERS		= $(patsubst %.cxx,%.h,$(sources)) \
	 		  CbmDetectorList.h

noinst_HEADERS		= CbmBaseLinkDef.h 

libCbmBase_la_SOURCES	= $(sources) CbmBaseDict.cxx

CLEANFILES		= CbmBaseDict.cxx CbmBaseDict.h *~ libCbmBase.la

CbmBaseDict.cxx: $(include_HEADERS) CbmBaseLinkDef.h
	$(ROOTCINT) -f $@ -c -DHAVE_CONFIG_H $(INCLUDES) $^
