#ifndef CBMECALCLUSTERV1_H
#define CBMECALCLUSTERV1_H

#include "TObject.h"
#include <list>

#include "CbmEcalCell.h"

class CbmEcalRecParticle;

struct CbmEcalClusterSortProcess : public std::binary_function<CbmEcalCell*, CbmEcalCell*, Bool_t>
{
  inline Bool_t operator()(const CbmEcalCell* left, const CbmEcalCell* right) const
  {
    if (left->GetTotalEnergy()<right->GetTotalEnergy())
      return kTRUE;
    return kFALSE;
  }
};

/** A temporary cluster needed for debugging of cluster finder procedure **/
class CbmEcalClusterV1 : public TObject
{
friend class CbmEcalRecoSimple;
friend class CbmEcalRecoSlow;
friend class CbmEcalClusterFinderV1;
public:
  /** An empty constructor **/
  CbmEcalClusterV1();
  /** A standard constructor **/
  CbmEcalClusterV1(Int_t num, const std::list<CbmEcalCell*>& cluster);
  /** Number of cluster in event **/
  inline Int_t Number() const {return fNum;}
  /** Size of cluster **/
  inline Int_t Size() const {return fSize;}
  /** Number of maximums in cluster **/
  inline Int_t Maxs() const {return fMaxs;}
  /** Energy of cluster **/
  inline Double_t Energy() const {return fEnergy;}
  /** Average type of cells in cluster **/
  inline Double_t Type() const {return fType;}
  /** Second moment **/
  inline Double_t Moment() const {return fMoment;}
  /** Moment over X axis **/
  inline Double_t MomentX() const {return fMomentX;}
  /** Moment over Y axis **/
  inline Double_t MomentY() const {return fMomentY;}
  /** Coordinates of cluster centre of gravity **/
  inline Double_t X() const {return fX;}
  inline Double_t Y() const {return fY;}
  /** \chi^2 of cluster after fitting **/
  inline Double_t Chi2() const {return fChi2;}

  /** fCells and peaks are sorted. First entry with maximum energy **/
  inline std::list<CbmEcalCell*>::const_iterator Begin() const {return fCells.begin();}
  inline std::list<CbmEcalCell*>::const_iterator End() const {return fCells.end();}
  inline std::list<CbmEcalCell*>::const_iterator PeaksBegin() const {return fPeaks.begin();}
  inline std::list<CbmEcalCell*>::const_iterator PeaksEnd() const {return fPeaks.end();}

  inline std::list<CbmEcalRecParticle*>::const_iterator PhotonsBegin() const {return fPhotons.begin();}
  inline std::list<CbmEcalRecParticle*>::const_iterator PhotonsEnd() const {return fPhotons.end();}
  /** An virtual destructor **/
  virtual ~CbmEcalClusterV1();
private:
  /** An initialization **/
  void Init();
  /** Cluster number **/
  Int_t fNum;
  /** Cluster size in cells
   ** A separate variable. fCells not stored **/
  Int_t fSize;
  /** Number of maximums in cluster **/
  Int_t fMaxs;
  /** Energy of cluster **/
  Double_t fEnergy;
  /** Type is an average of type of cells in cluster **/
  Double_t fType;
  /** Second moment **/
  Double_t fMoment;
  /** Moment over X axis **/
  Double_t fMomentX;
  /** Moment over Y axis **/
  Double_t fMomentY;
  /** Coordinates of cluster centre of gravity **/
  Double_t fX;
  Double_t fY;
  /** \chi^2 after fitting **/
  Double_t fChi2;

  /** Cluster it self **/
  std::list<CbmEcalCell*> fCells;		//!		
  /** Peaks in cluster **/
  std::list<CbmEcalCell*> fPeaks;		//!
  /** List of reconstructed particles **/
  std::list<CbmEcalRecParticle*> fPhotons;	//!

  ClassDef(CbmEcalClusterV1, 1)
};

#endif
