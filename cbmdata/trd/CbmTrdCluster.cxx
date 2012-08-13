/**
 * \file CbmTrdCluster.cxx
 * \author Florian Uhlig <f.uhlig@gsi.de>
 * \brief Data Container for TRD clusters.
 */
#include "CbmTrdCluster.h"

CbmTrdCluster::CbmTrdCluster()
  :CbmCluster(),
   fCharge(0.),
   fMaxCharge(0.)
{
}

CbmTrdCluster::~CbmTrdCluster(){
}

ClassImp(CbmTrdCluster)
