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

lib_LTLIBRARIES		= libTrdCaTrack.la

libTrdCaTrack_la_LDFLAGS 	= -version-info 1:0:1

libTrdCaTrack_la_LIBADD	= -L$(ROOTLIBDIR) $(ROOTLIBS) -L$(top_srcdir)/base

include_HEADERS		= CbmL1CATrdTrackFinderSA.h \
			  CbmL1TrdTracklet4.h \
			  CbmL1TrdTracklet.h \
			  CbmTrdMatchTracks.h \
			  CbmTrdTestEventsWithMom.h


noinst_HEADERS		= TrdCaTrackLinkDef.h 

libTrdCaTrack_la_SOURCES	= CbmL1CATrdTrackFinderSA.cxx \
			  CbmL1TrdTracklet4.cxx \
			  CbmL1TrdTracklet.cxx \
			  CbmTrdMatchTracks.cxx \
			  CbmTrdTestEventsWithMom.cxx

CLEANFILES		= CbmTrdTrackDict.cxx CbmTrdTrackDict.h *~ libTrdCaTrack.la

CbmTrdCaTrackDict.cxx: $(include_HEADERS) TrdCaTrackLinkDef.h
	$(ROOTCINT) -f $@ -c -DHAVE_CONFIG_H $(INCLUDES) $^
