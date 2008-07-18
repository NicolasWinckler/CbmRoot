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

lib_LTLIBRARIES		= libZdc.la

libZdc_la_LDFLAGS 	= -version-info 0:0:0

libZdc_la_LIBADD	= -L$(ROOTLIBDIR) $(ROOTLIBS) -L$(top_srcdir)/base/ 

sources                 = CbmGeoZdc.cxx \
                          CbmGeoZdcPar.cxx \
                          CbmZdcContFact.cxx \
                          CbmZdc.cxx \
                          CbmZdcEvent.cxx \
                          CbmZdcPoint.cxx \
                          CbmZdcv1.cxx

include_HEADERS		= $(patsubst %.cxx,%.h,$(sources))

noinst_HEADERS		= ZdcLinkDef.h 

libZdc_la_SOURCES	= $(sources) CbmZdcDict.cxx

CLEANFILES		= CbmZdcDict.cxx CbmZdcDict.h *~ libZdc.la

CbmZdcDict.cxx: $(include_HEADERS) ZdcLinkDef.h
	$(ROOTCINT) -f $@ -c -DHAVE_CONFIG_H $(INCLUDES) $^
