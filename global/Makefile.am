# Copyright (C) 2005 Florian Uhlig <f.uhlig@gsi.de>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

INCLUDES		= $(ROOTCFLAGS) -I$(top_srcdir)/base \
					-I$(top_srcdir)/geobase \
					-I$(top_srcdir)/parbase \
					-I$(top_srcdir)/cbmbase \
					-I$(top_srcdir)/field \
			                -I$(top_srcdir)/sts \
					-I$(top_srcdir)/rich \
					-I$(top_srcdir)/trd \
					-I$(top_srcdir)/tof

lib_LTLIBRARIES		= libGlobal.la

libGlobal_la_LDFLAGS 	= -version-info 0:0:0

libGlobal_la_LIBADD	= -L$(ROOTLIBDIR) $(ROOTLIBS) -L$(top_srcdir)/base/

sources                 = CbmFindGlobalTracks.cxx    \
			  CbmFindGlobalTracksQa.cxx  \
			  CbmFindPrimaryVertex.cxx   \
			  CbmFitGlobalTracks.cxx \
			  CbmFitGlobalTracksQa.cxx \
			  CbmGlobalTrackFitter.cxx \
			  CbmGlobalTrackFitterIdeal.cxx \
			  CbmGlobalTrack.cxx         \
			  CbmPVFinderIdeal.cxx       \
			  CbmPrimaryVertexFinder.cxx \
			  CbmRichMerger.cxx          \
			  CbmTrackMerger.cxx         \
			  CbmTrackMergerIdeal.cxx    \
			  CbmTofMerger.cxx           \
			  CbmTofMergerIdeal.cxx \
			  CbmVertex.cxx

include_HEADERS		= $(patsubst %.cxx,%.h,$(sources))

noinst_HEADERS		= GlobalLinkDef.h 

libGlobal_la_SOURCES	= $(sources) GlobalDict.cxx

CLEANFILES		= GlobalDict.cxx GlobalDict.h *~ libGlobal.la

GlobalDict.cxx: $(include_HEADERS) GlobalLinkDef.h
	$(ROOTCINT) -f $@ -c -DHAVE_CONFIG_H $(INCLUDES) $^
