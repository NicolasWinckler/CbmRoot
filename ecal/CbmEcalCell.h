// -------------------------------------------------------------------------
// -----                  CbmEcalCell header file                      -----
// -----                  Created  27/01/06  by M.Prokudin             -----
// -------------------------------------------------------------------------

/**  CbmEcalCell.h
 *@author Mikhail Prokudin
 **
 ** ECAL cell structure, a part of ECAL module
 **/

#ifndef CBMECALCELL_H
#define CBMECALCELL_H

/* $Id: CbmEcalCell.h,v 1.9 2006/09/12 18:15:23 prokudin Exp $ */

/* History of cvs commits:
 *
 * $Log: CbmEcalCell.h,v $
 * Revision 1.9  2006/09/12 18:15:23  prokudin
 * GetType moved to CbmEcalCell
 *
 * Revision 1.8  2006/07/19 11:49:39  prokudin
 * Commenting, optimizing hitproducer
 *
 * Revision 1.7  2006/07/19 09:33:34  prokudin
 * Modifing for CbmEcalPhotonCalibrator
 *
 * Revision 1.6  2006/07/13 14:46:00  prokudin
 * New version of hit producer
 *
 * Revision 1.5  2006/06/30 06:17:17  kharlov
 * Optimization, commenting
 *
 * Revision 1.4  2006/04/02 16:55:23  kharlov
 * Merging full and fast MC
 *
 * Revision 1.3  2006/02/03 14:53:40  kharlov
 * CbmEcalAnalysisDS.cxx
 *
 * Revision 1.2  2006/01/31 17:07:17  kharlov
 * Correction for CbmEcalv1
 *
 * Revision 1.1  2006/01/27 17:26:29  kharlov
 * First adaptation of ECAL detailed geometry to cbmroot2
 *
 */

#include "TObject.h"

#include <list>
#include <map>

class CbmEcalCell : public TObject
{
public:
  CbmEcalCell(Int_t cellnumber, Float_t x1=0, Float_t y1=0, Float_t x2=0, Float_t y2=0, Char_t type=0, Float_t energy=0) :
    fNumber(cellnumber), fX1(x1), fY1(y1), fX2(x2)
    , fY2(y2), fType(type), fEnergy(energy)
  {f5x5List.clear();fPSEnergy=0;};

  inline Bool_t IsInside(Float_t x, Float_t y) {return x>GetX1()&&x<GetX2()&&y>GetY1()&&y<GetY2();}
  //getters
  inline Char_t GetType() const {return fType;}
  inline Float_t X1() const {return fX1;}
  inline Float_t Y1() const {return fY1;}
  inline Float_t X2() const {return fX2;}
  inline Float_t Y2() const {return fY2;}
  inline Float_t GetX1() const {return fX1;}
  inline Float_t GetY1() const {return fY1;}
  inline Float_t GetX2() const {return fX2;}
  inline Float_t GetY2() const {return fY2;}
  inline Float_t GetCenterX() const {return (fX1+fX2)/2.0;}
  inline Float_t GetCenterY() const {return (fY1+fY2)/2.0;}

  inline Int_t   GetCellNumber() const {return fNumber;}
	
  inline Float_t GetEnergy() const {return fEnergy;}
  inline Float_t GetTotalEnergy() const {return fEnergy+fPSEnergy;}
  inline Float_t GetPSEnergy() const {return fPSEnergy;}
	
  Float_t GetTrackEnergy(Int_t num) const;
  Float_t GetTrackPSEnergy(Int_t num) const;
  inline Float_t GetTrackTotalEnergy(Int_t num) const
  {
    return GetTrackEnergy(num)+GetTrackPSEnergy(num);
  }
	
  inline void GetNeighborsList(Int_t num, std::list<CbmEcalCell*> &neib) const
  {
    if (num<0||num>4) return;
    neib=fNeighbors[num];
  }
  inline void SetNeighborsList(Int_t num, std::list<CbmEcalCell*> &neib)
  {
    if (num<0||num>4) return;
    fNeighbors[num]=neib;
  }
  inline void SetEnergy(Float_t energy) {fEnergy=energy;}
  inline void SetPSEnergy(Float_t energy) {fPSEnergy=energy;}
  /** Reset all energies in cell **/
  void ResetEnergy();
  /** Reset only PS and ECAL energy, not tracks energies **/
  void ResetEnergyFast();
  inline void AddEnergy(Float_t energy) {fEnergy+=energy;}
  inline void AddPSEnergy(Float_t energy) {fPSEnergy+=energy;}
	
  inline void SetTrackEnergy(Int_t num, Float_t energy)
  {fTrackEnergy[num]=energy;}
  inline void SetTrackPSEnergy(Int_t num, Float_t energy)
  {fTrackPSEnergy[num]=energy;}
  inline void AddTrackEnergy(Int_t num, Float_t energy)
  {fTrackEnergy[num]+=energy;}
  inline void AddTrackPSEnergy(Int_t num, Float_t energy)
  {fTrackPSEnergy[num]+=energy;}
	
  // code=0 for "3x3" cluster
  // code=1-4 for other clusters for Ivan's procedure
  void GetClusterEnergy(Int_t code, Float_t& EcalEnergy, Float_t& PSEnergy);

  void Get5x5ClusterEnergy(Float_t& EcalEnergy, Float_t& PSEnergy);
  /** Get 2x2 subcluster with maximum energy deposition of 3x3 cluster**/
  void IvansProcedure(Float_t &clusterE, Float_t &ivansE, Float_t &shape);
  // same for tracks
  Float_t GetTrackClusterEnergy(Int_t num, Int_t code);
  Float_t GetTrack5x5ClusterEnergy(Int_t num);
  void TrackIvansProcedure(Int_t num, Float_t &clusterE, Float_t &ivansE, Float_t &shape);

  inline std::map<Int_t, Float_t>::const_iterator GetTrackEnergyBegin() const
	 {return fTrackEnergy.begin();}
  inline std::map<Int_t, Float_t>::const_iterator GetTrackEnergyEnd() const
	 {return fTrackEnergy.end();}

  inline std::map<Int_t, Float_t>::const_iterator GetTrackPSEnergyBegin() const
	 {return fTrackPSEnergy.begin();}
  inline std::map<Int_t, Float_t>::const_iterator GetTrackPSEnergyEnd() const
	 {return fTrackPSEnergy.end();}

  inline void SetCoord(Float_t x1, Float_t y1, Float_t x2, Float_t y2)
    { fX1=x1; fY1=y1; fX2=x2; fY2=y2; }
  inline void SetType(Char_t type) {fType=type;}
  /** returns number of neighbors in lst with cell **/
  Int_t CountNeighbors(const std::list<CbmEcalCell*>& lst) const; 
private:
  /**  cell number within the module **/
  Int_t fNumber;
  /** left edge of the cell **/
  Float_t fX1;
  /** bottom edge of the cell **/
  Float_t fY1;
  /** right edge of the cell **/
  Float_t fX2;
  /** upper edge of the cell **/
  Float_t fY2;
  /** type of cell **/
  Char_t fType;
  /** energy in the calorimeter cell **/
  Float_t fEnergy;
  /** energy in the preshower cell **/
  Float_t fPSEnergy;


  /** list of neighbor cells **/
  std::list<CbmEcalCell*> fNeighbors[5];
  /** list of neighbors of neighbors cells **/
  std::list<CbmEcalCell*> f5x5List;
  /**  map<TrackId, Energy in ECAL> **/
  std::map<Int_t, Float_t> fTrackEnergy;
  /**  map<TrackIf, Energy in PS> **/
  std::map<Int_t, Float_t> fTrackPSEnergy;
  ClassDef(CbmEcalCell,1);
};
  
inline void CbmEcalCell::ResetEnergyFast()
{
  fEnergy=0;
  fPSEnergy=0;
}

#endif
