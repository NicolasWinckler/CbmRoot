/**
 * \file CbmFiberHodoCluster.h
 * \author Florian Uhlig <f.uhlig@gsi.de>
 * \brief Data Container for TRD clusters.
 **/

#ifndef CBMFIBERHODOCLUSTER_H
#define CBMFIBERHODOCLUSTER_H 

#include "CbmCluster.h"

class CbmFiberHodoCluster : public CbmCluster
{
public:

  /**
   * \brief Default constructor.
   */
  CbmFiberHodoCluster();

  /**
   * \brief Destructor.
   */
  virtual ~CbmFiberHodoCluster();

  ClassDef(CbmFiberHodoCluster, 1)
};
#endif
