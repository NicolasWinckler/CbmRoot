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

lib_LTLIBRARIES		= libTof.la

libTof_la_LDFLAGS 	= -version-info 0:0:0

libTof_la_LIBADD	= -L$(ROOTLIBDIR) $(ROOTLIBS) -L$(top_srcdir)/base/ 

sources                 = CbmGeoTof.cxx \
			  CbmGeoTofPar.cxx \
			  CbmTof.cxx \
			  CbmTofContFact.cxx \
			  CbmTofHit.cxx \
			  CbmTofHitProducer.cxx \
			  CbmTofHitProducerIdeal.cxx \
			  CbmTofPoint.cxx

include_HEADERS		= $(patsubst %.cxx,%.h,$(sources))

noinst_HEADERS		= TofLinkDef.h 

libTof_la_SOURCES	= $(sources) CbmTofDict.cxx

CLEANFILES		= CbmTofDict.cxx CbmTofDict.h *~ libTof.la

CbmTofDict.cxx: $(include_HEADERS) TofLinkDef.h
	$(ROOTCINT) -f $@ -c -DHAVE_CONFIG_H $(INCLUDES) $^
