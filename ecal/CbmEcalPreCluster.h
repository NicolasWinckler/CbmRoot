/** A helper class for cluster finder
 ** Need refactoring. April 2011 //Dr.Sys **/
#ifndef CBMECALPRECLUSTER_H
#define CBMECALPRECLUSTER_H

#include "CbmEcalCell.h"
#include "CbmEcalMaximum.h"

#include <list>
//#include <iostream>

class CbmEcalPreCluster
{
public:
  CbmEcalPreCluster(const std::list<CbmEcalCell*> cells, CbmEcalCell* max, CbmEcalCell* min, Double_t energy=-1111.0) 
    : fCells(cells), fMaximum(max), fMinimum(min), fMark(0), fEnergy(energy)
  {
    fMax=NULL;
  }

  CbmEcalPreCluster(const std::list<CbmEcalCell*> cells, CbmEcalMaximum* max, CbmEcalCell* min, Double_t energy=-1111.0) 
    : fCells(cells), fMaximum(max->Cell()), fMinimum(min), fMark(0), fEnergy(energy)
  {
    fMax=max;
//    std::cout << "list: " << fEnergy << std::endl;
  }

  CbmEcalPreCluster(CbmEcalCell** cells, Int_t size, CbmEcalCell* max, CbmEcalCell* min, Double_t energy=-1111.0)
  {
    fCells.clear();
    for(Int_t i=0;i<size;i++)
      fCells.push_back(cells[i]);
    fMaximum=max;
    fMinimum=min;
    fMark=0;
    fEnergy=energy;
    fMax=NULL;
  }

  CbmEcalPreCluster(CbmEcalCell** cells, Int_t size, CbmEcalMaximum* max, CbmEcalCell* min, Double_t energy=-1111.0)
  {
    fCells.clear();
    for(Int_t i=0;i<size;i++)
      fCells.push_back(cells[i]);
    fMax=max;
    fMaximum=max->Cell();
    fMinimum=min;
    fMark=0;
    fEnergy=energy;
//    std::cout << "array: "<< fEnergy << std::endl;
  }

  std::list<CbmEcalCell*> fCells;
  CbmEcalCell* fMaximum;
  CbmEcalCell* fMinimum;
  Double_t fEnergy;
  CbmEcalMaximum* fMax;		//!
  Int_t fMark;
};

#endif
