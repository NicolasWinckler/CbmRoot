# Copyright (C) 2005 Florian Uhlig <f.uhlig@gsi.de>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

# $Id: Makefile.am,v 1.14 2006/09/18 07:58:04 prokudin Exp $

INCLUDES		= $(ROOTCFLAGS) $(GEANT3INC)
INCLUDES               += -I$(top_srcdir)/geobase
INCLUDES               += -I$(top_srcdir)/parbase
INCLUDES               += -I$(top_srcdir)/base
INCLUDES               += -I$(top_srcdir)/cbmbase
INCLUDES               += -I$(top_srcdir)/field

lib_LTLIBRARIES		= libEcal.la

libEcal_la_LDFLAGS 	= -version-info 0:0:0

libEcal_la_LIBADD	= -L$(ROOTLIBDIR) $(ROOTLIBS) -L$(top_srcdir)/base/ 

include_HEADERS		= CbmEcal.h CbmEcalHit.h CbmEcalPoint.h	\
			  CbmEcalPointLite.h CbmEcalHitProducer.h \
			  CbmGeoEcalPar.h CbmEcalDrawer.h \
                          CbmEcalContFact.h CbmGeoEcal.h CbmEcalInf.h \
			  CbmEcalAnalysisDS.h CbmEcalAnalysis.h \
			  CbmEcalCell.h CbmEcalModule.h CbmEcalStructure.h \
			  CbmEcalHitFastMC.h CbmEcalHitProducerFastMC.h \
			  CbmEcalRecParticle.h CbmEcalReconstructionFastMC.h \
			  CbmEcalCluster.h CbmEcalClusterFinder.h \
			  CbmEcalPhotonCalibrator.h CbmEcalShowerCalibrator.h \
			  CbmEcalUrqmdCalibrator.h CbmEcalSCurveLib.h \
			  CbmEcalSCurveLibRecord.h CbmEcalOShowerCalibrator.h \
			  CbmEcalShowerLib.h
			  

noinst_HEADERS		= EcalLinkDef.h 

libEcal_la_SOURCES	= CbmEcal.cxx CbmEcalHit.cxx CbmEcalPoint.cxx \
			  CbmEcalPointLite.cxx CbmEcalHitProducer.cxx \
			  CbmGeoEcalPar.cxx CbmEcalDrawer.cxx \
			  CbmEcalContFact.cxx CbmGeoEcal.cxx \
			  CbmEcalInf.cxx CbmEcalStructure.cxx \
			  CbmEcalAnalysisDS.cxx CbmEcalAnalysis.cxx \
			  CbmEcalCell.cxx CbmEcalModule.cxx CbmEcalDict.cxx \
			  CbmEcalHitFastMC.cxx CbmEcalHitProducerFastMC.cxx \
			  CbmEcalRecParticle.cxx CbmEcalReconstructionFastMC.cxx \
			  CbmEcalCluster.cxx CbmEcalClusterFinder.cxx \
			  CbmEcalPhotonCalibrator.cxx CbmEcalShowerCalibrator.cxx \
			  CbmEcalUrqmdCalibrator.cxx CbmEcalSCurveLib.cxx \
			  CbmEcalSCurveLibRecord.cxx CbmEcalOShowerCalibrator.cxx \
			  CbmEcalShowerLib.cxx


CLEANFILES		= CbmEcalDict.cxx CbmEcalDict.h *~ libEcal.la

CbmEcalDict.cxx: $(include_HEADERS) EcalLinkDef.h
	$(ROOTCINT) -f $@ -c -DHAVE_CONFIG_H $(INCLUDES) $^
