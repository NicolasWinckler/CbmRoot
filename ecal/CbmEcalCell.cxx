// -------------------------------------------------------------------------
// -----                  CbmEcalCell source file                      -----
// -----                  Created  27/01/06  by M.Prokudin             -----
// -------------------------------------------------------------------------

/**  CbmEcalCell.cxx
 *@author Mikhail Prokudin
 **
 ** ECAL cell structure, a part of ECAL module
 **/

/* $Id: CbmEcalCell.cxx,v 1.6 2006/07/19 09:33:34 prokudin Exp $ */

/* History of cvs commits:
 *
 * $Log: CbmEcalCell.cxx,v $
 * Revision 1.6  2006/07/19 09:33:34  prokudin
 * Modifing for CbmEcalPhotonCalibrator
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

#include "CbmEcalCell.h"

#include <algorithm>
#include <iostream>

using std::cout;
using std::endl;
using std::map;
using std::list;

//-----------------------------------------------------------------------------
Int_t CbmEcalCell::CountNeighbors(const std::list<CbmEcalCell*>& lst) const
{
  Int_t c=0;
  list<CbmEcalCell*>::const_iterator p=lst.begin();
  for(;p!=lst.end();++p)
    if (find(fNeighbors[0].begin(), fNeighbors[0].end(), *p)!=fNeighbors[0].end())
      ++c;

  return c;
}
//-----------------------------------------------------------------------------
Float_t CbmEcalCell::GetTrackTime(Int_t num) const
{
  map<Int_t, Float_t>::const_iterator p=fTrackTime.find(num);
  if (p==fTrackTime.end()) return 0; else return p->second;
}


//-----------------------------------------------------------------------------
Float_t CbmEcalCell::GetTrackEnergy(Int_t num) const
{
  map<Int_t, Float_t>::const_iterator p=fTrackEnergy.find(num);
  if (p==fTrackEnergy.end()) return 0; else return p->second;
}

//-----------------------------------------------------------------------------
Float_t CbmEcalCell::GetTrackPSEnergy(Int_t num) const
{
  map<Int_t, Float_t>::const_iterator p=fTrackPSEnergy.find(num);
  if (p==fTrackPSEnergy.end()) return 0; else return p->second;
}

//-----------------------------------------------------------------------------
void CbmEcalCell::ResetEnergy()
{
  fEnergy=0;
  fPSEnergy=0;
  fTrackEnergy.clear();
  fTrackPSEnergy.clear();
  fTime=-1111;
  fTrackTime.clear();
}

//-----------------------------------------------------------------------------
void CbmEcalCell::GetClusterEnergy(Int_t code, Float_t& EcalEnergy, Float_t& PSEnergy)
{
  EcalEnergy=-1;
  PSEnergy=-1;
  if (code>4||code<0)
    return;
  EcalEnergy=GetEnergy();
  PSEnergy=GetPSEnergy();
  list<CbmEcalCell*>::const_iterator p;
  for(p=fNeighbors[code].begin();p!=fNeighbors[code].end();++p)
  {
    EcalEnergy+=(*p)->GetEnergy();
    PSEnergy+=(*p)->GetPSEnergy();
  }
}

//-----------------------------------------------------------------------------
void CbmEcalCell::IvansProcedure(Float_t &clusterE, Float_t &ivansE, Float_t &shape)
{
  Float_t e, ps;
  GetClusterEnergy(0,e,ps);
  clusterE=e+ps;
  ivansE=0;
  for(Int_t i=1;i<5;i++)
  {
    GetClusterEnergy(i,e,ps);
    if (e+ps>ivansE) ivansE=e+ps;
  }
  shape=ivansE/clusterE;
}

//-----------------------------------------------------------------------------
void CbmEcalCell::Get5x5ClusterEnergy(Float_t& EcalEnergy, Float_t& PSEnergy)
{
  if (f5x5List.empty()) {
    list<CbmEcalCell*> tmp_list;
    list<CbmEcalCell*> lst;
    list<CbmEcalCell*>::const_iterator i;
    list<CbmEcalCell*>::const_iterator j;
    list<CbmEcalCell*>::const_iterator res;
    
    GetNeighborsList(0,tmp_list);
    for(i=tmp_list.begin();i!=tmp_list.end();++i) {
      (*i)->GetNeighborsList(0,lst);
      for(j=lst.begin();j!=lst.end();++j)
	if (f5x5List.empty()||(res=find(f5x5List.begin(),f5x5List.end(),(*j)))==f5x5List.end())
	  f5x5List.push_back(*j);
    }
    //cout << f5x5List.size() << endl;
  }
  EcalEnergy=0;
  PSEnergy=0;
  list<CbmEcalCell*>::const_iterator p;
  for(p=f5x5List.begin();p!=f5x5List.end();++p)
  {
    EcalEnergy+=(*p)->GetEnergy();
    PSEnergy+=(*p)->GetPSEnergy();
  }
}

//-----------------------------------------------------------------------------
Float_t CbmEcalCell::GetTrackClusterEnergy(Int_t num, Int_t code)
{
  if (code>4||code<0) return -1;
  Float_t energy=GetTrackTotalEnergy(num);
  list<CbmEcalCell*>::const_iterator p;
  for(p=fNeighbors[code].begin();p!=fNeighbors[code].end();++p)
    energy+=(*p)->GetTrackTotalEnergy(num);
  return energy;
}

//-----------------------------------------------------------------------------
void CbmEcalCell::TrackIvansProcedure(Int_t num, Float_t &clusterE, Float_t &ivansE, Float_t &shape)
{
  clusterE=GetTrackClusterEnergy(num,0);
  ivansE=0;
  Float_t en;
  for(Int_t i=1;i<5;i++)
    if ((en=GetTrackClusterEnergy(num,i))>ivansE) ivansE=en;
  shape=ivansE/clusterE;
}

//-----------------------------------------------------------------------------
Float_t CbmEcalCell::GetTrack5x5ClusterEnergy(Int_t num)
{
  if (f5x5List.empty())	{
    list<CbmEcalCell*> tmp_list;
    list<CbmEcalCell*> lst;
    list<CbmEcalCell*>::const_iterator i;
    list<CbmEcalCell*>::const_iterator j;
    list<CbmEcalCell*>::const_iterator res;
    
    GetNeighborsList(0,tmp_list);
    for(i=tmp_list.begin();i!=tmp_list.end();++i) {
      (*i)->GetNeighborsList(0,lst);
      for(j=lst.begin();j!=lst.end();++j)
	if (f5x5List.empty()||(res=find(f5x5List.begin(),f5x5List.end(),(*j)))==f5x5List.end())
	  f5x5List.push_back(*j);
    }
    cout << f5x5List.size() << endl;
  }
  Float_t energy=0;
  list<CbmEcalCell*>::const_iterator p;
  for(p=f5x5List.begin();p!=f5x5List.end();++p)
    energy+=(*p)->GetTrackTotalEnergy(num);
  return energy;
}
