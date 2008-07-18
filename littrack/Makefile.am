
INCLUDES		= $(ROOTCFLAGS)
INCLUDES               += -I$(top_srcdir)/geobase
INCLUDES               += -I$(top_srcdir)/parbase
INCLUDES               += -I$(top_srcdir)/field
INCLUDES               += -I$(top_srcdir)/base
INCLUDES               += -I$(top_srcdir)/cbmbase
INCLUDES               += -I$(top_srcdir)/passive
INCLUDES               += -I$(top_srcdir)/trd
INCLUDES               += -I$(top_srcdir)/sts
INCLUDES               += -I$(top_srcdir)/rich

lib_LTLIBRARIES		= libLittrack.la

libLittrack_la_LDFLAGS 	= -version-info 0:0:0

libLittrack_la_LIBADD	= -L$(ROOTLIBDIR) $(ROOTLIBS) -L$(top_srcdir)/base
libLittrack_la_LIBADD       += -L$(top_srcdir)/parbase
libLittrack_la_LIBADD       += -L$(top_srcdir)/base

sources                 =  CbmLitTrdTrackFinder.cxx \
                           CbmLitTrdTrackFinderSts.cxx \
                           CbmLitTrdTrackFinderS.cxx \
                           CbmLitMaterial.cxx \
                           CbmLitEnvironment.cxx  \
                           CbmTrackFitter.cxx   \
                           CbmTrackPropagator.cxx \
                           CbmTrackFilter.cxx    \
                           CbmTrackExtrapolator.cxx \
                           CbmLitTrackPropagator.cxx   \
                           CbmLitRK4TrackExtrapolator.cxx \
                           CbmLitKalmanFilter.cxx \
                           CbmLitLineTrackExtrapolator.cxx \
                           CbmLitCircleMaterial.cxx \
                           CbmLitRectMaterial.cxx





include_HEADERS		= $(patsubst %.cxx,%.h,$(sources))

noinst_HEADERS		= LittrackLinkDef.h

libLittrack_la_SOURCES	= $(sources) CbmLittrackDict.cxx

CLEANFILES		= CbmLittrackDict.cxx CbmLittrackDict.h *~ libLittrack.la

CbmLittrackDict.cxx: $(include_HEADERS) LittrackLinkDef.h
	$(ROOTCINT) -f $@ -c -DHAVE_CONFIG_H $(INCLUDES) $^
