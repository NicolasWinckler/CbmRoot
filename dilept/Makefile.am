# $Id: Makefile.am,v 1.6 2006/01/19 12:21:22 hoehne Exp $

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
					-I$(top_srcdir)/cbmbase \
                                        -I$(top_srcdir)/parbase\
                                        -I$(top_srcdir)/geobase \
                                        -I$(top_srcdir)/field \
                                        -I$(top_srcdir)/passive \
                                        -I$(top_srcdir)/global \
					-I$(top_srcdir)/sts \
                                        -I$(top_srcdir)/rich \
                                        -I$(top_srcdir)/trd \
                                        -I$(top_srcdir)/tof \
                                        -I$(top_srcdir)/KF \
                                        -I$(top_srcdir)/KF/Interface \
                                        -I$(top_srcdir)/mcstack

lib_LTLIBRARIES		= libDilepton.la

libDilepton_la_LDFLAGS 	= -version-info 0:0:0

libDilepton_la_LIBADD	= -L$(ROOTLIBDIR) $(ROOTLIBS) -L$(top_srcdir)/base/ 

sources         	= CbmDileptonTrack.cxx \
                          CbmDileptonTrackReal.cxx \
                          CbmDileptonTrackRealCollection.cxx \
			  CbmDileptonAssignPid.cxx \
			  CbmDileptonTrackSim.cxx \
			  CbmDileptonTrackSimCollection.cxx \
			  CbmDileptonAssignMCid.cxx 
			  

include_HEADERS		= $(patsubst %.cxx,%.h,$(sources))	

noinst_HEADERS		= DileptonLinkDef.h 


libDilepton_la_SOURCES  = $(sources) CbmDileptonDict.cxx 

CLEANFILES		= CbmDileptonDict.cxx CbmDileptonDict.h *~ libDilepton.la

CbmDileptonDict.cxx: $(include_HEADERS) DileptonLinkDef.h
	$(ROOTCINT) -f $@ -c -DHAVE_CONFIG_H $(INCLUDES) $^
