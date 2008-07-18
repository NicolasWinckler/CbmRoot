# Copyright (C) 2005 Florian Uhlig <f.uhlig@gsi.de>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

INCLUDES   = -I$(top_srcdir)/L1
INCLUDES  += -I$(top_srcdir)/L1/OffLineInterface
INCLUDES  += $(ROOTCFLAGS) -I$(top_srcdir)/KF \
	     -I$(top_srcdir)/base -I$(top_srcdir)/geobase \
             -I$(top_srcdir)/passive -I$(top_srcdir)/mvd -I$(top_srcdir)/sts \
             -I$(top_srcdir)/magnet -I$(top_srcdir)/trd -I$(top_srcdir)/rich \
	     -I$(top_srcdir)/tof -I$(top_srcdir)/global
INCLUDES  += -I$(top_srcdir)/KF/Interface
INCLUDES  += -I$(top_srcdir)/parbase -I$(top_srcdir)/field
INCLUDES  += -I$(top_srcdir)/much
INCLUDES  += -I$(top_srcdir)/cbmbase

# C++ Headers
#

# C++ Sources
#
#L1SRC	:= $(wildcard */src/*.cxx) 
#L1SRC	+= $(wildcard src/*.cxx) 
#L1SRC	+= $(wildcard *.cxx) 

L1SRC	= L1Algo/L1CATrackFinder.cxx \
          CbmL1Performance.cxx \
          CbmL1ReadEvent.cxx \
	  CbmL1CATrdTrackFinderSA.cxx \
	  CbmL1TrdTracklet.cxx \
	  CbmL1TrdTracklet4.cxx \
          CbmL1.cxx \
	  CbmL1TrdTrackFinderSts.cxx \
	  CbmL1TrackMerger.cxx \
	  CbmL1TofMerger.cxx \
	  OffLineInterface/CbmL1RichENNRingFinder.cxx \
          OffLineInterface/CbmL1StsTrackFinder.cxx \
	  OffLineInterface/CbmL1MuchFinder.cxx \
	  OffLineInterface/CbmL1MuchHit.cxx \
	  OffLineInterface/CbmL1MuchTrack.cxx \
	  OffLineInterface/CbmL1MuchFinderQa.cxx \
	  OffLineInterface/CbmL1RichRingQa.cxx 
 
L1HDRS    := $(wildcard $(top_srcdir)/L1/Cbm*.h ) 
L1HDRS    += $(wildcard $(top_srcdir)/L1/OffLineInterface/*.h ) 


AM_CXXFLAGS  = -O3 -Wall -Wno-deprecated

# Use CPU SSE instruction set
#
AM_CXXFLAGS += -msse -DHAVE_SSE

lib_LTLIBRARIES		= libL1.la

libL1_la_LDFLAGS 	= -version-info 0:0:0

libL1_la_LIBADD	= -L$(ROOTLIBDIR) $(ROOTLIBS) -L$(top_srcdir)/base/ 

include_HEADERS		= $(L1HDRS)

noinst_HEADERS		= L1LinkDef.h

libL1_la_SOURCES	= $(L1SRC)  L1Dict.cxx

CLEANFILES		= L1Dict.cxx L1Dict.h *~ libL1.la */*~ */*/*~ 

L1Dict.cxx: $(include_HEADERS) L1LinkDef.h
	$(ROOTCINT) -f $@ -c -DHAVE_CONFIG_H $(INCLUDES) $^
