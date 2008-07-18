# Makefile.am for module STS
# For use of automake
#


INCLUDES		= $(ROOTCFLAGS)
INCLUDES               += -I$(top_srcdir)/geobase
INCLUDES               += -I$(top_srcdir)/parbase
INCLUDES               += -I$(top_srcdir)/base 
INCLUDES               += -I$(top_srcdir)/cbmbase
INCLUDES               += -I$(top_srcdir)/field
INCLUDES               += -I$(top_srcdir)/passive

lib_LTLIBRARIES		= libSts.la

libSts_la_LDFLAGS 	= -version-info 0:0:0

libSts_la_LIBADD	= -L$(ROOTLIBDIR) $(ROOTLIBS)
libSts_la_LIBADD       += -L$(top_srcdir)/parbase
libSts_la_LIBADD       += -L$(top_srcdir)/base

sources                 = CbmGeoSts.cxx \
			  CbmGeoStsPar.cxx \
	 		  CbmSts.cxx \
			  CbmStsContFact.cxx \
			  CbmStsDigi.cxx \
			  CbmStsDigiMatch.cxx \
			  CbmStsDigiPar.cxx \
			  CbmStsDigiScheme.cxx \
			  CbmStsDigitize.cxx \
			  CbmStsFindHits.cxx \
			  CbmStsFindTracks.cxx \
			  CbmStsFindTracksQa.cxx \
			  CbmStsFitTracks.cxx \
			  CbmStsHit.cxx \
			  CbmStsHitProducerIdeal.cxx \
			  CbmStsMatchHits.cxx \
			  CbmStsMatchTracks.cxx \
			  CbmStsParAsciiFileIo.cxx \
			  CbmStsParRootFileIo.cxx \
			  CbmStsPoint.cxx \
			  CbmStsSector.cxx \
			  CbmStsSectorDigiPar.cxx \
			  CbmStsStation.cxx \
			  CbmStsStationDigiPar.cxx \
			  CbmStsTrack.cxx \
			  CbmStsTrackFinder.cxx \
			  CbmStsTrackFinderIdeal.cxx \
			  CbmStsTrackFitter.cxx \
			  CbmStsTrackFitterIdeal.cxx \
			  CbmStsTrackMatch.cxx \
			  CbmStsSimulationQa.cxx \
			  CbmStsReconstructionQa.cxx

include_HEADERS		= $(patsubst %.cxx,%.h,$(sources))

noinst_HEADERS		= StsLinkDef.h

libSts_la_SOURCES	= $(sources) CbmStsDict.cxx

CLEANFILES		= CbmStsDict.cxx CbmStsDict.h *~ libSts.la

CbmStsDict.cxx: $(include_HEADERS) StsLinkDef.h
	$(ROOTCINT) -f $@ -c -DHAVE_CONFIG_H $(INCLUDES) $^
