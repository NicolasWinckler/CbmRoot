# Copyright (C) 2005 Florian Uhlig <f.uhlig@gsi.de>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

INCLUDES		= $(ROOTCFLAGS)
INCLUDES               += -I$(top_srcdir)/geobase 
INCLUDES               += -I$(top_srcdir)/parbase 
INCLUDES               += -I$(top_srcdir)/field
INCLUDES               += -I$(top_srcdir)/base 
INCLUDES               += -I$(top_srcdir)/passive
INCLUDES               += -I$(top_srcdir)/sts
INCLUDES               += -I$(top_srcdir)/mcstack

lib_LTLIBRARIES		= libLHETrack.la

libLHETrack_la_LDFLAGS 	= -version-info 0:0:0

libLHETrack_la_LIBADD	= -L$(ROOTLIBDIR) $(ROOTLIBS) -L$(top_srcdir)/base/ 

include_HEADERS		= CbmArrayI.h  CbmStsHit.h   CbmStsTrackCuts.h \
				CbmStsCandidTracker.h  CbmStsJointTrack.h  \
				CbmStsTrackletCuts.h CbmStsCMPoint.h \
				CbmStsMassHypothesis.h  CbmStsVertex.h \
				CbmStsCMTrackFinder.h  CbmStsPolyFitter.h \
				CbmStsCMTrack.h   CbmStsSegments.h \
				CbmStsMapsStripTracker.h 




noinst_HEADERS		= LHETrackLinkDef.h

libLHETrack_la_SOURCES	= CbmArrayI.cxx CbmStsCMTrackFinder.cxx \
				CbmStsSegments.cxx CbmStsCandidTracker.cxx\
				CbmStsHit.cxx            CbmStsTrackCuts.cxx\
				CbmStsCMPoint.cxx        CbmStsJointTrack.cxx \
				CbmStsTrackletCuts.cxx  CbmStsCMTrack.cxx\
				CbmStsPolyFitter.cxx     CbmStsVertex.cxx \
				CbmStsMapsStripTracker.cxx  LHETrackDict.cxx

CLEANFILES		= LHETrackDict.cxx CbmLHETrackDict.h *~ libLHETrack.la

LHETrackDict.cxx: $(include_HEADERS) LHETrackLinkDef.h
	$(ROOTCINT) -f $@ -c -DHAVE_CONFIG_H $(INCLUDES) $^
