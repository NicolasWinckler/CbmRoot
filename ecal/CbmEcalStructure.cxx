// -------------------------------------------------------------------------
// -----                  CbmEcalStructure source file                 -----
// -----                  Created  27/01/06  by M.Prokudin             -----
// -------------------------------------------------------------------------

/**  CbmEcalStructure.cxx
 *@author Mikhail Prokudin
 **
 ** ECAL structure, consisting of modules
 **/

/* $Id: CbmEcalStructure.cxx,v 1.11 2006/07/19 09:33:34 prokudin Exp $ */

/* History of cvs commits:
 *
 * $Log: CbmEcalStructure.cxx,v $
 * Revision 1.11  2006/07/19 09:33:34  prokudin
 * Modifing for CbmEcalPhotonCalibrator
 *
 * Revision 1.10  2006/07/15 18:31:10  kharlov
 * Compilation warnings and coding conventions are corrected
 *
 * Revision 1.9  2006/07/14 15:09:32  prokudin
 * New functionality added
 *
 * Revision 1.8  2006/07/12 14:22:56  kharlov
 * Adding ClusterFinder
 *
 * Revision 1.7  2006/07/10 07:43:25  kharlov
 * Conversion of cell type from string to a number
 *
 * Revision 1.6  2006/06/30 07:00:15  kharlov
 * Corrections for hit numbering
 *
 * Revision 1.5  2006/06/30 06:17:17  kharlov
 * Optimization, commenting
 *
 * Revision 1.4  2006/06/22 06:52:13  kharlov
 * Compilation warnings removed
 *
 * Revision 1.3  2006/04/02 16:55:23  kharlov
 * Merging full and fast MC
 *
 * Revision 1.2  2006/01/31 17:07:17  kharlov
 * Correction for CbmEcalv1
 *
 * Revision 1.1  2006/01/27 17:26:30  kharlov
 * First adaptation of ECAL detailed geometry to cbmroot2
 *
 */

#include "CbmEcalStructure.h"

#include "CbmEcal.h"
#include "CbmEcalDetailed.h"

#include <iostream>
#include <algorithm>
#include <cmath>
using namespace std;

class __CbmEcalCellWrapper
{
public:
  CbmEcalCell* cell;
  Char_t isPsTen;
};

CbmEcalCell* CbmEcalStructure::GetCell(Int_t volId, Int_t& ten, Bool_t& isPS)
{
  UInt_t i;
  Int_t volidmax = CbmEcal::GetVolIdMax()*2;

  if ((Int_t)fHash.size()<volidmax)
  {
    fHash.resize(volidmax);
    for(i=0;i<fHash.size();i++)
      fHash[i]=NULL;
  }
  if (volId>volidmax)
    return NULL;
  if (fHash[volId]==NULL)
  {
    Bool_t lisPS;
    Int_t iten;
    Float_t x;
    Float_t y;
    fHash[volId]=new __CbmEcalCellWrapper();
    if (fEcalVersion==0)
      lisPS=CbmEcal::GetCellCoord(volId,x,y,iten);
    if (fEcalVersion==1)
      lisPS=CbmEcalDetailed::GetCellCoordInf(volId, x, y, iten);
    fHash[volId]->cell=GetCell(x+0.025,y+0.025);
    fHash[volId]->isPsTen=iten*2;
    if (lisPS) fHash[volId]->isPsTen+=1;
  }
  ten=fHash[volId]->isPsTen/2;
  isPS=fHash[volId]->isPsTen%2;
  return fHash[volId]->cell;
}

void CbmEcalStructure::Serialize()
{
  fCells.clear();
  for(UInt_t i=0;i<fStructure.size();i++)
  if (fStructure[i])
  {
    vector<CbmEcalCell*> cells=fStructure[i]->GetCells();
    copy(cells.begin(),cells.end(), back_inserter(fCells));
  }
}

//-----------------------------------------------------------------------------
CbmEcalStructure::CbmEcalStructure(CbmEcalInf* ecalinf)
  : TNamed("CbmEcalStructure", "Calorimeter structure"), fEcalInf(ecalinf)
{
  fX1=fEcalInf->GetXPos()-\
    fEcalInf->GetModuleSize()*fEcalInf->GetXSize()/2.0;
  fY1=fEcalInf->GetYPos()-\
    fEcalInf->GetModuleSize()*fEcalInf->GetYSize()/2.0;
  fEcalVersion=(Int_t)fEcalInf->GetVariable("ecalversion");
  if (fEcalVersion<0)
    fEcalVersion=0;
}

//-----------------------------------------------------------------------------
void CbmEcalStructure::Construct()
{
  if (!fEcalInf) return;

  Float_t x1=GetX1();
  Float_t y1=GetY1();
  Float_t x;
  Float_t y;
  Float_t dx;
  Float_t dy;
  Int_t i;
  Int_t j;
  Int_t k;
  Int_t number;
  char type;

  fStructure.resize(fEcalInf->GetXSize()*fEcalInf->GetYSize(), NULL);
	
  dx=fEcalInf->GetModuleSize();
  dy=fEcalInf->GetModuleSize();
  //Creating ECAL Matrix
  for(i=0;i<fEcalInf->GetXSize();i++)
    for(j=0;j<fEcalInf->GetYSize();j++) {
      type=fEcalInf->GetType(i,j);
      if (type) {
	x=x1+i*dx;
	y=y1+j*dy;
	number=(i*100+j)*100;
	fStructure[GetNum(i,j)]=new CbmEcalModule(type,number,x,y,x+dx,y+dy);
      }
      else
	fStructure[GetNum(i,j)]=NULL;
    }
#ifdef _DECALSTRUCT
  cerr << "-I- CbmEcalStructure::Construct(): calorimeter matrix created." << endl;
#endif
  //Now ECAL matrix created
  list<CbmEcalCell*> neib[5];
  vector<CbmEcalCell*> cl;
  vector<CbmEcalCell*>::const_iterator pcl;
  
  Int_t num;
  //We want neighbors for EcalModules be EcalModules
  for(i=0;i<fEcalInf->GetXSize();i++)
    for(j=0;j<fEcalInf->GetYSize();j++)
      if (fStructure[GetNum(i,j)]) {
	for(k=0;k<5;k++) neib[k].clear();
	
	num=GetNumber(i-1,j);
	if (-1!=num) {
	  neib[0].push_back(fStructure[num]);
	  neib[1].push_back(fStructure[num]);
	  neib[4].push_back(fStructure[num]);
	}
			
	num=GetNumber(i-1,j+1);
	if (-1!=num) {
	  neib[0].push_back(fStructure[num]);
	  neib[4].push_back(fStructure[num]);
	}
	
	num=GetNumber(i,j+1);
	if (-1!=num) {
	  neib[0].push_back(fStructure[num]);
	  neib[3].push_back(fStructure[num]);
	  neib[4].push_back(fStructure[num]);
	}
	
	num=GetNumber(i+1,j+1);
	if (-1!=num) {
	  neib[0].push_back(fStructure[num]);
	  neib[3].push_back(fStructure[num]);
	}
	
	num=GetNumber(i+1,j);
	if (-1!=num) {
	  neib[0].push_back(fStructure[num]);
	  neib[2].push_back(fStructure[num]);
	  neib[3].push_back(fStructure[num]);
	}
			
	num=GetNumber(i+1,j-1);
	if (-1!=num) {
	  neib[0].push_back(fStructure[num]);
	  neib[2].push_back(fStructure[num]);
	}
	
	num=GetNumber(i,j-1);
	if (-1!=num) {
	  neib[0].push_back(fStructure[num]);
	  neib[1].push_back(fStructure[num]);
	  neib[2].push_back(fStructure[num]);
	}
	
	num=GetNumber(i-1,j-1);
	if (-1!=num) {
	  neib[0].push_back(fStructure[num]);
	  neib[1].push_back(fStructure[num]);
	}
	
	num=GetNumber(i,j);
	for(k=0;k<5;k++)
	  fStructure[num]->SetNeighborsList(k,neib[k]);
	cl=fStructure[num]->GetCells();
	for(pcl=cl.begin();pcl!=cl.end();++pcl)
	  CreateNLists(*pcl);
      }
  Serialize();
}

//-----------------------------------------------------------------------------
void _add_not_null(CbmEcalModule* mod, list<CbmEcalModule*>& lst)
{
  if (mod)
    if (find(lst.begin(),lst.end(),mod)==lst.end())	
      lst.push_back(mod);
}

//-----------------------------------------------------------------------------
void _add_not_null(list<CbmEcalCell*> from, list<CbmEcalCell*>& where)
{
  list<CbmEcalCell*>::const_iterator p;
  for(p=from.begin();p!=from.end();++p)
    if (find(where.begin(),where.end(),(*p))==where.end())
      //this p uniq!
      where.push_back(*p);
}

//-----------------------------------------------------------------------------
void _add_not_null(CbmEcalCell* cell, list<CbmEcalCell*>& lst)
{
  if (find(lst.begin(),lst.end(),cell)==lst.end())	
    lst.push_back(cell);
}

//-----------------------------------------------------------------------------
void CbmEcalStructure::CreateNLists(CbmEcalCell* cell)
{
  Float_t x=cell->GetCenterX();
  Float_t y=cell->GetCenterY();
  Float_t dx=(cell->GetX2()-cell->GetX1())/2.0;
  Float_t dy=(cell->GetX2()-cell->GetX1())/2.0;
  Float_t x1;
  Float_t x2;
  Float_t y1;
  Float_t y2;
  Float_t mx1=cell->GetX1();
  Float_t mx2=cell->GetX2();
  Float_t my1=cell->GetY1();
  Float_t my2=cell->GetY2();
  Float_t cx;
  Float_t cy;
  Float_t d=0.1;
  Float_t dd=1e-6;
  list<CbmEcalCell*> neib[5];
  list<CbmEcalCell*> tl;
  list<CbmEcalModule*> tml; 
  list<CbmEcalModule*>::const_iterator ptml;
  list<CbmEcalCell*>::const_iterator ptl;
  Int_t i;

  //1 - lu, 2 - ru, 3 - rd, 4 - ld
  for(i=0;i<5;i++) neib[i].clear();

  tml.clear();
  _add_not_null(GetModule(x-2*dx,y-2*dy),tml);
  _add_not_null(GetModule(x-2*dx,y     ),tml);
  _add_not_null(GetModule(x-2*dx,y+2*dy),tml);
  _add_not_null(GetModule(x     ,y-2*dy),tml);
  _add_not_null(GetModule(x     ,y     ),tml);
  _add_not_null(GetModule(x     ,y+2*dy),tml);
  _add_not_null(GetModule(x+2*dx,y-2*dy),tml);
  _add_not_null(GetModule(x+2*dx,y     ),tml);
  _add_not_null(GetModule(x+2*dx,y+2*dy),tml);
  if (tml.empty()) {
    cerr << "Error during creating neighbors lists." << endl;
    cerr << "Can't' find any modules neighbors to cell." << endl;
    cerr << "Cell: CenterX=" << x << ", CenterY=" << y << "." << endl; 
    return;
  }
  tl.empty();
  for(ptml=tml.begin();ptml!=tml.end();++ptml) {
    _add_not_null((*ptml)->GetCellsY(y-dy-d),tl);
    _add_not_null((*ptml)->GetCellsY(y+dy+d),tl);
    _add_not_null((*ptml)->GetCellsX(x-dx-d),tl);
    _add_not_null((*ptml)->GetCellsX(x+dx+d),tl);
  }
  if (tl.empty()) {
    cerr << "Error during creating neighbors lists." << endl;
    cerr << "Can't' find any cells neighbors to cell." << endl;
    cerr << "Cell: CenterX=" << x << ", CenterY=" << y << "." << endl; 
    return;
  }
  for(ptl=tl.begin();ptl!=tl.end();++ptl) {
    x1=(*ptl)->GetX1();
    x2=(*ptl)->GetX2();
    y1=(*ptl)->GetY1();
    y2=(*ptl)->GetY2();
    cx=(*ptl)->GetCenterX();
    cy=(*ptl)->GetCenterY();
    if (fabs(mx1-x2)<dd) {
      if ((cy-y+2*dy>-dd&&cy-y-dy/2<dd)||fabs(y-dy-y1)<dd||fabs(y-dy-y2)<dd) {
	_add_not_null((*ptl),neib[0]);
	_add_not_null((*ptl),neib[1]);
      }
      if ((cy-y-2*dy<dd&&cy-y+dy/2>-dd)||fabs(y+dy-y1)<dd||fabs(y+dy-y2)<dd) {
	_add_not_null((*ptl),neib[0]);
	_add_not_null((*ptl),neib[4]);
      }
    }
    if (fabs(my1-y2)<dd) {
      if ((cx-x+2*dx>-dd&&cx-x-dx/2<dd)||fabs(x-dx-x1)<dd||fabs(x-dx-x2)<dd) {
	_add_not_null((*ptl),neib[0]);
	_add_not_null((*ptl),neib[1]);
      }
      if ((cx-x-2*dx<dd&&cx-x+dx/2>-dd)||fabs(x+dx-x1)<dd||fabs(x+dx-x2)<dd) {
	_add_not_null((*ptl),neib[0]);
	_add_not_null((*ptl),neib[2]);
      }
    }
    if (fabs(mx2-x1)<dd) {
      if ((cy-y+2*dy>-dd&&cy-y-dy/2<dd)||fabs(y-dy-y1)<dd||fabs(y-dy-y2)<dd) {
	_add_not_null((*ptl),neib[0]);
	_add_not_null((*ptl),neib[2]);
      }
      if ((cy-y-2*dy<dd&&cy-y+dy/2>-dd)||fabs(y+dy-y1)<dd||fabs(y+dy-y2)<dd) {
	_add_not_null((*ptl),neib[0]);
	_add_not_null((*ptl),neib[3]);
      }
    }
    if (fabs(my2-y1)<dd) {
      if ((cx-x+2*dx>-dd&&cx-x-dx/2<dd)||fabs(x-dx-x1)<dd||fabs(x-dx-x2)<dd) {
	_add_not_null((*ptl),neib[0]);
	_add_not_null((*ptl),neib[4]);
      }
      if ((cx-x-2*dx<dd&&cx-x+dx/2>-dd)||fabs(x+dx-x1)<dd||fabs(x+dx-x2)<dd) {
	_add_not_null((*ptl),neib[0]);
	_add_not_null((*ptl),neib[3]);
      }
    }
    
  }
  for(i=0;i<5;i++) cell->SetNeighborsList(i,neib[i]);
}

//-----------------------------------------------------------------------------
void CbmEcalStructure::ResetModules()
{
  vector<CbmEcalModule*>::const_iterator p;
  for(p=fStructure.begin();p!=fStructure.end();++p)
    if (*p) (*p)->ResetModule();
}

//-----------------------------------------------------------------------------
void CbmEcalStructure::GetHitXY(const Int_t hitId, Float_t& x, Float_t& y) const
{
  /** Hit Id -> (x,y) **/

  // Some translation from x*100+y  to y*sizex+x coding...
 
  Int_t mnum=hitId/100;
  Int_t cellx = mnum/100;
  Int_t celly = mnum%100;
  mnum = GetNum(cellx, celly);
  
  // end translation

  CbmEcalModule* module=fStructure[mnum];
  CbmEcalCell* cell;

  Int_t cellnum=hitId%100;
  // change place
  Int_t cx=cellnum%10-1;
  Int_t cy=cellnum/10-1;

  if (module==NULL||cx<0||cy<0||cx>=module->GetType()||cy>=module->GetType()) {x=0; y=0; return;}
  cell=module->At(cx,cy);
  x=cell->GetCenterX();
  y=cell->GetCenterY();
}

//-----------------------------------------------------------------------------
CbmEcalCell* CbmEcalStructure::GetHitCell(const Int_t hitId) const
{
  /** Hit Id -> Cell **/
 
  // Some translation from x*100+y  to y*sizex+x coding...
 
  Int_t mnum=hitId/100;
  Int_t cellx = mnum/100;
  Int_t celly = mnum%100;
  mnum = GetNum(cellx, celly);
  
  // end translation

  CbmEcalModule* module=fStructure[mnum];

  Int_t cellnum=hitId%100;
  Int_t cx=cellnum%10-1;
  Int_t cy=cellnum/10-1;

  if (module==NULL||cx<0||cy<0||cx>=module->GetType()||cy>=module->GetType()) 
    return NULL;
//  cout << hitId << " --- " << module->At(cx,cy)->GetCellNumber() << endl;
  return module->At(cx,cy);
}

//-----------------------------------------------------------------------------

void CbmEcalStructure::GetGlobalCellXY(const Int_t hitId, Int_t& x, Int_t& y) const
{


  Int_t modulenum = hitId/100;
  Int_t cellx = modulenum/100;
  Int_t celly = modulenum%100; 

  Int_t innernum =  hitId%100;
  Int_t iny = innernum/10;
  Int_t inx = innernum%10;
  Int_t msize = fEcalInf->GetType(cellx,celly);

   
  x = (cellx-1)* msize + inx;
  y = (celly-1)* msize + iny;

}
//
Int_t CbmEcalStructure::GetType(const Int_t hitId) const
{
  Int_t modulenum = hitId/100;
  Int_t cellx = modulenum/100;
  Int_t celly = modulenum%100; 

  Int_t msize = fEcalInf->GetType(cellx,celly);
  return msize;
}
