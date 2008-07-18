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
INCLUDES               += -I$(top_srcdir)/field
INCLUDES               += -I$(top_srcdir)/passive
INCLUDES               += -I$(top_srcdir)/sts
INCLUDES               += -I$(top_srcdir)/global

lib_LTLIBRARIES		= libTrd.la

libTrd_la_LDFLAGS 	= -version-info 1:0:1

libTrd_la_LIBADD	= -L$(ROOTLIBDIR) $(ROOTLIBS) -L$(top_srcdir)/base

include_HEADERS		= CbmGeoTrd.h CbmGeoTrdPar.h CbmTrdContFact.h  \
			  CbmTrd.h 		\
			  CbmTrdHit.h		\
			  CbmTrdTrack.h		\
			  CbmTrdHitProducerIdeal.h   \
			  CbmTrdHitProducer.h	\
			  CbmTrdHitProducerQa.h \
			  CbmTrdPoint.h	CbmTrdRadiator.h \
			  CbmTrdFindTracks.h \
			  CbmTrdFindTracksQa.h \
			  CbmTrdTrackFinder.h \
			  CbmTrdTrackFinderIdeal.h \
			  CbmTrdFitTracks.h \
			  CbmTrdFitTracksQa.h \
			  CbmTrdTrackFitter.h \
			  CbmTrdTrackFitterIdeal.h \
			  CbmTrdTrackMatch.h \
			  CbmTrdMatchTracks.h	\
			  CbmTrdSetTracksPidWkn.h	\
			  CbmTrdSetTracksPidLike.h	\
			  CbmTrdSetTracksPidANN.h	\
			  CbmTrdCreatePidLike.h	\
			  CbmTrdGas.h	\
			  CbmTrdTracksPidQa.h


noinst_HEADERS		= TrdLinkDef.h 

libTrd_la_SOURCES	= CbmGeoTrd.cxx  CbmGeoTrdPar.cxx CbmTrdContFact.cxx  \
			  CbmTrd.cxx 		\
			  CbmTrdHit.cxx		\
			  CbmTrdTrack.cxx	\
			  CbmTrdHitProducerIdeal.cxx   \
			  CbmTrdHitProducer.cxx \
			  CbmTrdHitProducerQa.cxx \
			  CbmTrdPoint.cxx	\
			  CbmTrdDict.cxx CbmTrdRadiator.cxx \
			  CbmTrdFindTracks.cxx \
			  CbmTrdFindTracksQa.cxx \
			  CbmTrdTrackFinder.cxx \
			  CbmTrdTrackFinderIdeal.cxx \
			  CbmTrdFitTracks.cxx \
			  CbmTrdFitTracksQa.cxx \
			  CbmTrdTrackFitter.cxx \
			  CbmTrdTrackFitterIdeal.cxx \
			  CbmTrdTrackMatch.cxx \
			  CbmTrdMatchTracks.cxx	\
			  CbmTrdSetTracksPidWkn.cxx	\
			  CbmTrdSetTracksPidLike.cxx	\
			  CbmTrdSetTracksPidANN.cxx	\
			  CbmTrdCreatePidLike.cxx	\
			  CbmTrdGas.cxx	\
			  CbmTrdTracksPidQa.cxx

CLEANFILES		= CbmTrdDict.cxx CbmTrdDict.h *~ libTrd.la

CbmTrdDict.cxx: $(include_HEADERS) TrdLinkDef.h
	$(ROOTCINT) -f $@ -c -DHAVE_CONFIG_H $(INCLUDES) $^
