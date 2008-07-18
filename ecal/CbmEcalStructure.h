// -------------------------------------------------------------------------
// -----                  CbmEcalStructure header file                 -----
// -----                  Created  27/01/06  by M.Prokudin             -----
// -------------------------------------------------------------------------

/**  CbmEcalStructure.h
 *@author Mikhail Prokudin
 **
 ** ECAL structure, consisting of modules
 **/

#ifndef CBMECALSTRUCTURE_H
#define CBMECALSTRUCTURE_H

/* $Id: CbmEcalStructure.h,v 1.8 2006/07/19 11:49:39 prokudin Exp $ */

/* History of cvs commits:
 *
 * $Log: CbmEcalStructure.h,v $
 * Revision 1.8  2006/07/19 11:49:39  prokudin
 * Commenting, optimizing hitproducer
 *
 * Revision 1.7  2006/07/14 15:09:32  prokudin
 * New functionality added
 *
 * Revision 1.6  2006/07/12 14:22:56  kharlov
 * Adding ClusterFinder
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
 * Revision 1.1  2006/01/27 17:26:30  kharlov
 * First adaptation of ECAL detailed geometry to cbmroot2
 *
 */

#include "CbmEcalInf.h"
#include "CbmEcalModule.h"
#include "CbmEcalCell.h"

#include "TMath.h"
#include "TNamed.h"

#include <vector>

#define _DECALSTRUCT

class __CbmEcalCellWrapper;

class CbmEcalStructure : public TNamed
{
public:
  CbmEcalStructure(CbmEcalInf* ecalinf);
  void Construct();
  Int_t GetNumber(Int_t x, Int_t y) const;
  
  Bool_t AddEnergy(Float_t x, Float_t y, Float_t energy, Bool_t isPS=kFALSE);
  Bool_t AddTrackEnergy(Int_t  num, Float_t x, Float_t y, Float_t energy, Bool_t isPS=kFALSE);
  Float_t GetEnergy(Float_t x, Float_t y, Bool_t isPS=kFALSE) const;
  Float_t GetTrackEnergy(Int_t num, Float_t x, Float_t y, Bool_t isPS=kFALSE) const;
  CbmEcalCell* GetCell(Float_t x, Float_t y) const;
  CbmEcalModule* GetModule(Float_t x, Float_t y) const;
  Int_t GetModuleNumber(Float_t x, Float_t y) const;
  
  Float_t GetX1() const {return fX1;};
  Float_t GetY1() const {return fY1;};
  Float_t GetX2() const;
  Float_t GetY2() const;
  inline CbmEcalInf* GetEcalInf() const {return fEcalInf;}
  inline void GetStructure(std::vector<CbmEcalModule*>& stru) const {stru=fStructure;}
  inline void GetCells(std::list<CbmEcalCell*>& cells) const {cells=fCells;}
  //Create neighbors lists
  void CreateNLists(CbmEcalCell* cell);
  void ResetModules();
  /** Reset only PS and ECAL energy, not energy per track **/
  void ResetModulesFast();
  
  //Some usefull procedures for hit processing
  
  //Converts (x,y) to hit Id
  Int_t GetHitId(Float_t x, Float_t y) const;
  //Hit Id -> (x,y)
  void GetHitXY(const Int_t hitId, Float_t& x, Float_t& y) const;

  // HitId -> in global cell coordinate 
  void GetGlobalCellXY(const Int_t hitId, Int_t& x, Int_t& y) const;

  // HitId -> cell type
  Int_t GetType(const Int_t hitId) const;

  CbmEcalCell* GetCell(Int_t fVolId, Int_t& ten, Bool_t& isPS);
  //Hit It -> Cell
  CbmEcalCell* GetHitCell(const Int_t hitId) const;

private:
  Int_t GetNum(Int_t x, Int_t y) const;
  
private:
  /** Creates fCells lists **/
  void Serialize();
  /** X coordibate of left bottom angle of ECAL **/
  Float_t fX1;
  /** Y coordibate of left bottom angle of ECAL **/
  Float_t fY1;
  /** ECAL geometry container **/
  CbmEcalInf* fEcalInf;
  /** total list of ECAL modules **/
  std::vector<CbmEcalModule*> fStructure;
  /** All ECAL cells **/
  std::list<CbmEcalCell*> fCells;
  /** MCPoint id -> ECAL cell**/
  std::vector<__CbmEcalCellWrapper*> fHash;

  ClassDef(CbmEcalStructure,1);
};

inline CbmEcalCell* CbmEcalStructure::GetCell(Float_t x, Float_t y) const
{
  /** get ECAL cell by known cell center coordinate (x,y) **/
  CbmEcalModule* module=GetModule(x,y);
  if (module) return module->FindCell(x,y);
  return NULL;
}

inline CbmEcalModule* CbmEcalStructure::GetModule(Float_t x, Float_t y) const
{
  /** get ECAL module by known module center coordinate (x,y) **/
  Int_t num=GetModuleNumber(x,y);
  if (-1==num) return NULL; else return fStructure[num]; 
}

inline Int_t  CbmEcalStructure::GetModuleNumber(Float_t x, Float_t y) const
{
  /** get ECAL module by known module center coordinate (x,y) **/
  Int_t ix=(Int_t)TMath::Floor((x-GetX1())/fEcalInf->GetModuleSize());
  Int_t iy=(Int_t)TMath::Floor((y-GetY1())/fEcalInf->GetModuleSize());
  return GetNumber(ix,iy);
}

inline Int_t CbmEcalStructure::GetNumber(Int_t x, Int_t y) const
{
  /** get ECAL absolute module number by known module relative number (x,y)
   ** with check for the ECAL boundaries **/
  if (x>-1&&y>-1)
    if (x<fEcalInf->GetXSize()&&y<fEcalInf->GetYSize())
      return GetNum(x,y);
  return -1;
}

/** We rely that no energy in modules, only in cells **/
inline void CbmEcalStructure::ResetModulesFast()
{
  for(std::list<CbmEcalCell*>::const_iterator p=fCells.begin(); p!=fCells.end(); ++p)
    (*p)->ResetEnergyFast();
}
inline Int_t CbmEcalStructure::GetNum(Int_t x, Int_t y) const
{
  /** get ECAL absolute module number by known module relative number (x,y) **/
  return y*fEcalInf->GetXSize()+x;
}

inline Float_t CbmEcalStructure::GetX2() const
{
  /** get ECAL right edge coordinate in cm **/
  return fEcalInf->GetXPos() +
         fEcalInf->GetModuleSize()*fEcalInf->GetXSize()/2.0;
}

inline Float_t CbmEcalStructure::GetY2() const
{
  /** get ECAL upper edge coordinate in cm **/
  return fEcalInf->GetYPos() +
         fEcalInf->GetModuleSize()*fEcalInf->GetYSize()/2.0;
}

inline Bool_t CbmEcalStructure::AddEnergy(Float_t x, Float_t y, Float_t energy, Bool_t isPS)
{
  /** Add preshower or calorimeter energy to a cell with coordinate (x,y) **/
  CbmEcalCell* cell=GetCell(x,y);
  if (cell)
    if (isPS) cell->AddPSEnergy(energy);
    else      cell->AddEnergy(energy);
  else
    return kFALSE;
  return kTRUE;
}

inline Bool_t CbmEcalStructure::AddTrackEnergy(Int_t num, Float_t x, Float_t y, Float_t energy, Bool_t isPS)
{
  /** Add preshower or calorimeter energy to a cell with coordinate (x,y)
   ** for a particular track <num> **/
  CbmEcalCell* cell=GetCell(x,y);
  if (cell)
    if (isPS)    cell->AddTrackPSEnergy(num, energy);
    else cell->AddTrackEnergy(num, energy);
  else
    return kFALSE;
  return kTRUE;
}

inline Float_t CbmEcalStructure::GetEnergy(Float_t x, Float_t y, Bool_t isPS) const
{
  CbmEcalCell* cell=GetCell(x,y);
  if (cell)
    if (isPS)    return cell->GetPSEnergy();
    else return cell->GetEnergy();
  return -1;
}

inline Float_t CbmEcalStructure::GetTrackEnergy(Int_t num, Float_t x, Float_t y, Bool_t isPS) const
{
  CbmEcalCell* cell=GetCell(x,y);
  if (cell)
    if (isPS)    return cell->GetTrackPSEnergy(num);
    else return cell->GetTrackEnergy(num);
  return -1;
}


//Converts (x,y) to hit Id
inline Int_t CbmEcalStructure::GetHitId(Float_t x, Float_t y) const
{
  CbmEcalCell* cell=GetCell(x,y);
  if (cell) return cell->GetCellNumber();
  else return -1;
}

#endif
