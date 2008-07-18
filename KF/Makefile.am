# Copyright (C) 2005 Florian Uhlig <f.uhlig@gsi.de>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

INCLUDES   = -I$(top_srcdir)/KF
INCLUDES  += -I$(top_srcdir)/KF/Interface
INCLUDES  += $(ROOTCFLAGS) 
INCLUDES  += -I$(top_srcdir)/geobase
INCLUDES  += -I$(top_srcdir)/parbase	 
INCLUDES  += -I$(top_srcdir)/base
INCLUDES  += -I$(top_srcdir)/cbmbase
INCLUDES  += -I$(top_srcdir)/field
INCLUDES  += -I$(top_srcdir)/passive 
INCLUDES  += -I$(top_srcdir)/mvd
INCLUDES  += -I$(top_srcdir)/sts
INCLUDES  += -I$(top_srcdir)/trd
INCLUDES  += -I$(top_srcdir)/tof
INCLUDES  += -I$(top_srcdir)/rich
INCLUDES  += -I$(top_srcdir)/much
INCLUDES  += -I$(top_srcdir)/global 

lib_LTLIBRARIES		= libKF.la

libKF_la_LDFLAGS 	= -version-info 0:0:0

libKF_la_LIBADD		= -L$(ROOTLIBDIR) $(ROOTLIBS) -L$(top_srcdir)/base/ 

AM_CXXFLAGS  = -O4

sources                 = CbmKF.cxx \
			  CbmKFFieldMath.cxx \
			  CbmKFHit.cxx \
			  CbmKFMaterial.cxx \
			  CbmKFMath.cxx \
			  CbmKFPixelMeasurement.cxx \
			  CbmKFPrimaryVertexFinder.cxx \
			  CbmKFSecondaryVertexFinder.cxx \
			  CbmKFTrackInterface.cxx \
			  CbmKFUMeasurement.cxx \
			  CbmKFVertexInterface.cxx \
			  Interface/CbmKFStsHit.cxx \
			  Interface/CbmKFTrack.cxx \
			  Interface/CbmKFTrdHit.cxx \
			  Interface/CbmPVFinderKF.cxx \
			  Interface/CbmRichTrackExtrapolationKF.cxx \
                          Interface/CbmStsFitPerformanceTask.cxx \
			  Interface/CbmStsKFTrackFitter.cxx \
	  		  Interface/CbmStsKFSecondaryVertexFinder.cxx \
			  Interface/CbmTrdTrackFitterKF.cxx \
			  Interface/CbmGlobalTrackFitterKF.cxx

include_HEADERS		= $(patsubst %.cxx,%.h,$(sources))

noinst_HEADERS		= KFLinkDef.h

libKF_la_SOURCES	= $(sources)  KFDict.cxx

CLEANFILES		= KFDict.cxx KFDict.h *~ libKF.la */*~ */*/*~ 

KFDict.cxx: $(include_HEADERS) KFLinkDef.h
	$(ROOTCINT) -f $@ -c -DHAVE_CONFIG_H $(INCLUDES) $^


