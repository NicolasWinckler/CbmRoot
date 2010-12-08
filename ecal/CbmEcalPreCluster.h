/** A helper class for cluster finder **/
#ifndef CBMECALPRECLUSTER_H
#define CBMECALPRECLUSTER_H

#include "CbmEcalCell.h"

#include <list>

class CbmEcalPreCluster
{
public:
  CbmEcalPreCluster(const std::list<CbmEcalCell*> cells, CbmEcalCell* max, CbmEcalCell* min) 
    : fCells(cells), fMaximum(max), fMinimum(min), fMark(0)
  {
    ;
  }

  CbmEcalPreCluster(CbmEcalCell** cells, Int_t size, CbmEcalCell* max, CbmEcalCell* min)
  {
    fCells.clear();
    for(Int_t i=0;i<size;i++)
      fCells.push_back(cells[i]);
    fMaximum=NULL;
    fMinimum=NULL;
    fMark=0;
  }
  std::list<CbmEcalCell*> fCells;
  CbmEcalCell* fMaximum;
  CbmEcalCell* fMinimum;
  Int_t fMark;
};

#endif
