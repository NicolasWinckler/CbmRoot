/**
 * \file CbmTrdCluster.h
 * \author Florian Uhlig <f.uhlig@gsi.de>
 * \brief Data Container for TRD clusters.
 *
 * TRD clusters are collection of neighbouring pads above a defined threshold.
 *
 * TODO: It has to be defined if the threshold is defined here
 *       or in the digi class. It has also to be defined if
 *       neighbouring also includes pads which are only touched
 *       at the edges of the pad (diagonal)
 **/

#ifndef CBMTRDCLUSTER_H
#define CBMTRDCLUSTER_H 

#include "CbmCluster.h"

/**
 * \class CbmTrdCluster
 * \author Florian Uhlig <f.uhlig@gsi.de>
 * \brief Data Container for TRD clusters.
 */
class CbmTrdCluster : public CbmCluster
{
public:

  /**
   * \brief Default constructor.
   */
  CbmTrdCluster();

  /**
   * \brief Destructor.
   */
  virtual ~CbmTrdCluster();

  /**
   * \brief Set summary charge for cluster.
   * \param[in] charge Summary charge.
   */
  void SetCharge(Float_t charge) { fCharge = charge; }

  /**
   * \brief Set maximum charge collected on a single digi.
   * \param[in] charge Maximum charge.
   */
  void SetMaxCharge(Float_t maxCharge) { fMaxCharge = maxCharge; }

  /**
   * \brief Return total charge in the cluster.
   * \return Total charge in the cluster.
   */
  Float_t GetCharge() const { return fCharge; }

  /**
   * \brief Return maximum charge collected on a single digi.
   * \return Maximum charge collected on a single digi.
   */
  Float_t GetMaxCharge() const { return fMaxCharge; }

private:
  Float_t fCharge; // Summary charge of the cluster
  Float_t fMaxCharge;  // Maximal charge collected on a single pad

  ClassDef(CbmTrdCluster, 2)
};
#endif
