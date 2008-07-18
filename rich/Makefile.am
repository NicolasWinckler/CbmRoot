# $Id: Makefile.am,v 1.15 2006/09/13 14:55:51 hoehne Exp $

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
INCLUDES               += -I$(top_srcdir)/base
INCLUDES               += -I$(top_srcdir)/cbmbase
INCLUDES               += -I$(top_srcdir)/sts 
INCLUDES               += -I$(top_srcdir)/field
INCLUDES               += -I$(top_srcdir)/global
INCLUDES               += -I$(top_srcdir)/KF

lib_LTLIBRARIES		= libRich.la

libRich_la_LDFLAGS 	= -version-info 0:0:0

libRich_la_LIBADD	= -L$(ROOTLIBDIR) $(ROOTLIBS)

include_HEADERS		= CbmGeoRich.h CbmRichRing.h	\
			  CbmRichRingFinder.h CbmRich.h \
			  CbmRichHit.h CbmRichHitProducer.h   \
			  CbmRichProjectionProducer.h   \
			  CbmGeoRichPar.h CbmRichContFact.h   \
			  CbmRichPoint.h \
			  CbmRichRingFinderHough.h CbmRichRingFinderTrack.h\
                          CbmRichRingFinderIdeal.h CbmRichFindRings.h\
                          CbmRichRingFitter.h CbmRichRingFitterCircle.h CbmRichFitRings.h\
			  CbmRichRingFitterCOP.h CbmRichRingFitterRobustCOP.h CbmRichRingFitterTAU.h\
			  CbmRichTrackExtrapolation.h CbmRichTrackExtrapolationIdeal.h\
			  CbmRichTrackExtrapolationMirrorIdeal.h\
			  CbmRichExtrapolateTracks.h\
			  CbmRichRingMatch.h CbmRichMatchRings.h\
			  CbmRichRingTrackAssign.h CbmRichRingTrackAssignClosestD.h\
			  CbmRichRingTrackAssignIdeal.h CbmRichAssignTrack.h\
			  CbmRichTestSim.h CbmRichTestHits.h\
			  CbmRichRingQa.h\
			  CbmRichRingSelect.h CbmRichRingSelectNeuralNet.h\
        		  CbmRichRingSelect2DCuts.h CbmRichSelectRings.h\
			  CbmRichRingFitterEllipse.h

noinst_HEADERS		= RichLinkDef.h 

libRich_la_SOURCES	= CbmGeoRich.cxx CbmRichRing.cxx \
			  CbmRichRingFinder.cxx CbmRich.cxx  \
			  CbmRichHit.cxx CbmRichHitProducer.cxx   \
			  CbmRichProjectionProducer.cxx \
			  CbmGeoRichPar.cxx CbmRichContFact.cxx   \
			  CbmRichPoint.cxx CbmRichDict.cxx \
			  CbmRichRingFinderHough.cxx CbmRichRingFinderTrack.cxx\
                          CbmRichRingFinderIdeal.cxx CbmRichFindRings.cxx\
                          CbmRichRingFitter.cxx CbmRichRingFitterCircle.cxx CbmRichFitRings.cxx\
			  CbmRichRingFitterCOP.cxx CbmRichRingFitterRobustCOP.cxx CbmRichRingFitterTAU.cxx\
			  CbmRichTrackExtrapolation.cxx CbmRichTrackExtrapolationIdeal.cxx\
			  CbmRichTrackExtrapolationMirrorIdeal.cxx\
			  CbmRichExtrapolateTracks.cxx\
			  CbmRichRingMatch.cxx CbmRichMatchRings.cxx\
			  CbmRichRingTrackAssign.cxx CbmRichRingTrackAssignClosestD.cxx\
			  CbmRichRingTrackAssignIdeal.cxx CbmRichAssignTrack.cxx\
			  CbmRichTestSim.cxx CbmRichTestHits.cxx\
			  CbmRichRingQa.cxx\
			  CbmRichRingSelect.cxx CbmRichRingSelectNeuralNet.cxx\
            		  CbmRichRingSelect2DCuts.cxx CbmRichSelectRings.cxx\
            		  CbmRichRingFitterEllipse.cxx

CLEANFILES		= CbmRichDict.cxx CbmRichDict.h *~ libRich.la

CbmRichDict.cxx: $(include_HEADERS) RichLinkDef.h
	$(ROOTCINT) -f $@ -c -DHAVE_CONFIG_H $(INCLUDES) $^
