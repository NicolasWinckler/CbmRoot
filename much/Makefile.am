
INCLUDES		= $(ROOTCFLAGS)
INCLUDES               += -I$(top_srcdir)/geobase
INCLUDES               += -I$(top_srcdir)/parbase
INCLUDES               += -I$(top_srcdir)/field
INCLUDES               += -I$(top_srcdir)/base
INCLUDES               += -I$(top_srcdir)/cbmbase
INCLUDES               += -I$(top_srcdir)/passive

lib_LTLIBRARIES		= libMuch.la

libMuch_la_LDFLAGS 	= -version-info 0:0:0

libMuch_la_LIBADD	= -L$(ROOTLIBDIR) $(ROOTLIBS) -L$(top_srcdir)/base
libMuch_la_LIBADD       += -L$(top_srcdir)/parbase
libMuch_la_LIBADD       += -L$(top_srcdir)/base

sources                 = 	CbmGeoMuch.cxx \  
				CbmGeoMuchPar.cxx \
				CbmMuch.cxx  \
				CbmMuchCluster.cxx \
				CbmMuchContFact.cxx \  
				CbmMuchDigi.cxx \
				CbmMuchDigiMatch.cxx \
				CbmMuchDigitize.cxx \
				CbmMuchFindClusters.cxx \
				CbmMuchFindHits.cxx \
				CbmMuchFindHitsSimple.cxx \
				CbmMuchFindTracks.cxx \
				CbmMuchGeoScheme.cxx \
				CbmMuchHit.cxx  \
				CbmMuchHitFinderQa.cxx \
				CbmMuchHitProducerIdeal.cxx \
				CbmMuchLayer.cxx \
				CbmMuchLayerSide.cxx \
				CbmMuchMatchTracks.cxx \
				CbmMuchModule.cxx \
				CbmMuchPad.cxx \
				CbmMuchPoint.cxx \
				CbmMuchPointInfo.cxx \
				CbmMuchSector.cxx \
				CbmMuchSegmentation.cxx \
				CbmMuchStation.cxx \
				CbmMuchTask.cxx \
				CbmMuchTrack.cxx  \
				CbmMuchTrackFinder.cxx \
				CbmMuchTrackMatch.cxx


include_HEADERS		= $(patsubst %.cxx,%.h,$(sources))

noinst_HEADERS		= MuchLinkDef.h

libMuch_la_SOURCES	= $(sources) CbmMuchDict.cxx

CLEANFILES		= CbmMuchDict.cxx CbmMuchDict.h *~ libMuch.la

CbmMuchDict.cxx: $(include_HEADERS) MuchLinkDef.h
	$(ROOTCINT) -f $@ -c -DHAVE_CONFIG_H $(INCLUDES) $^
