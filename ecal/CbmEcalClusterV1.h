#ifndef CBMECALCLUSTERV1_H
#define CBMECALCLUSTERV1_H

#include "TObject.h"
#include "TArrayI.h"
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
friend class CbmEcalRecoSlow2;
friend class CbmEcalClusterFinderV1;
friend class CbmEcalClusterFinderV2;
friend class CbmEcalClusterFinderV3;
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

  /** Getters for cells and peaks **/
  inline Int_t CellNum(Int_t i) const {return fCellNums[i];}
  inline Int_t PeakNum(Int_t i) const {return fPeakNums[i];}
  /** An virtual destructor **/
  virtual ~CbmEcalClusterV1();
private:
  /** An initialization **/
  void Init(std::list<CbmEcalCell*>& cls);
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
  
  /** Serial numbers of cells in cluster **/
  TArrayI fCellNums;
  /** Serial numbers of peaks in cluster **/
  TArrayI fPeakNums;

  ClassDef(CbmEcalClusterV1, 1)
};

#endif
