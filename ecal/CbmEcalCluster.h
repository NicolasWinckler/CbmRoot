// -------------------------------------------------------------------------
// -----                  CbmEcalCluster header file                   -----
// -----                 Created 12/07/06  by D.Konstantinov           -----
// -------------------------------------------------------------------------

/**  CbmEcalCluster.h
 *@author D.Konstantinov 
 **
 ** ECAL cluster is a group of cells with common edge
 **/

/* $Id: CbmEcalCluster.h,v 1.2 2006/07/19 15:29:03 kharlov Exp $ */

/* History of cvs commits:
 *
 * $Log: CbmEcalCluster.h,v $
 * Revision 1.2  2006/07/19 15:29:03  kharlov
 * Init geometry via constuctor, add cluster hit multiplicity, commenting
 *
 * Revision 1.1  2006/07/12 14:22:56  kharlov
 * Adding ClusterFinder
 *
 */

#ifndef CBMECALCLUSTER_H
#define CBMECALCLUSTER_H

#include "CbmEcalHit.h"

#include "TObject.h"
#include "TClonesArray.h"

#include <iostream>
#include <vector>

class CbmEcalCluster : public TObject
{
public:

  /** Default constructor **/
  CbmEcalCluster():fHitMult(0),fX(0),fY(0),fEnergy(0.){};

  /** Constructor which cleans a vector of hits **/
  CbmEcalCluster( Int_t detId):fHitMult(0),fX(0),fY(0),fEnergy(0.)
  {  
    fPosList.clear();
    fPosList.push_back(detId);
  };

  // Constructor which fills a vector of hits **/
  CbmEcalCluster( std::vector<Int_t> myvec, Double32_t energy ):fHitMult(0),fX(0),fY(0),fEnergy(energy)
  {
    fPosList.clear();
    fPosList = myvec;
  }
  

  /** Destructor **/
  virtual ~CbmEcalCluster();

  /** Output to screen **/
  virtual void Print(const Option_t* opt ="") const
  {
    printf(" Cluster :");
    printf("   %d cells", fPosList.size());
    printf("  Coordinates X,Y = %f,%f ", fX,fY);
    printf("  Moments m2x, m2y = %f, %f \n ",fM2x ,  fM2y); 
  };


  /** Add a hit index to a vector of hits **/
  void AddHitIndex(Int_t hitIndex) {fPosList.push_back(hitIndex);}

  /** Set the cluster energy **/
  void SetEnergy(Double32_t energy) {fEnergy = energy;}

  /** Set the cluster gravity center **/
  void SetXY(Float_t xCenter, Float_t yCenter) {fX = xCenter; fY = yCenter;}

  /** Return the cluster gravity center along X **/
  Float_t GetX() {return fX;}

  /** Return the cluster gravity center along Y **/
  Float_t GetY() {return fY;}

  /** Set the two second moments of the cluster, M2x and M2y **/
  void SetLambdas(Float_t lambda0, Float_t lambda1) {fM2x = lambda0; fM2y = lambda1;}
  
  /** Return the larger second moment of the cluster **/
  Float_t GetM2x() {return fM2x;}

  /** Return the smaller second moment of the cluster **/
  Float_t GetM2y() {return fM2y;}

  /** Return the cluster energy **/
  Double32_t GetEnergy() {return fEnergy;}

  /** Set the hit multiplicity of a cluster **/
  void SetSize(Int_t mult) {fHitMult=mult;}

  /** Return the hit multiplicity of a cluster **/
  Int_t GetSize() {return fPosList.size();}

  /** Return the index in TClonesArray of hits for a hit[pos] of a cluster **/  
  Int_t GetHitIndex(Int_t pos) {return fPosList[pos];}

  /** Clear the vector of hits of a cluster **/
  void Clear() {fPosList.clear();}

  /** Return the vectors of hits of a cluster **/
  std::vector<Int_t> GetVector() {return fPosList;}

private:

  /** Cluster hit multiplicity **/
  Float_t fHitMult;  

  /** X-gravity center of a cluster **/
  Float_t fX;  

  /** Y-gravity center of a cluster **/
  Float_t fY;

  /** Larger second moment of a cluster **/
  Float_t fM2x;

  /** Smaller second moment of a cluster **/
  Float_t fM2y;

  /** Total detected of a cluster **/
  Float_t fEnergy;  

  /** Vector of hit indices in TClonesArray of hits **/
  std::vector<Int_t> fPosList;

  ClassDef(CbmEcalCluster,1)

 };

#endif //
