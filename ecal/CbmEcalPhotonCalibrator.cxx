/* $Id: CbmEcalPhotonCalibrator.cxx,v 1.6 2006/07/30 13:10:35 prokudin Exp $ */
/*
 * $Log: CbmEcalPhotonCalibrator.cxx,v $
 * Revision 1.6  2006/07/30 13:10:35  prokudin
 * Bug hunting
 *
 * Revision 1.5  2006/07/25 20:23:31  prokudin
 * Thresholds and noise added
 *
 * Revision 1.4  2006/07/23 16:13:05  prokudin
 * Bug hunting
 *
 * Revision 1.3  2006/07/23 14:27:41  prokudin
 * Some optimization.
 *
 * Revision 1.2  2006/07/22 14:12:48  prokudin
 * Some bug hunting
 *
 * Revision 1.1  2006/07/19 09:34:45  prokudin
 * First version
 *
 */

#include "CbmEcalPhotonCalibrator.h"

#include "CbmEcalInf.h"
#include "CbmEcalStructure.h"
#include "CbmEcalPointLite.h"
#include "CbmEcalPoint.h"

#include "FairRootManager.h"
#include "CbmMCTrack.h"

#include "TClonesArray.h"
#include "TTree.h"
#include "TRandom.h"

#include <iostream>

using std::cout;
using std::cerr;
using std::endl;
using std::flush;
using std::list;
using std::vector;

CbmEcalPhotonCalibrator::CbmEcalPhotonCalibrator(const char* name, const Int_t iVerbose, const char* fileGeo) : FairTask(name, iVerbose)
{
  fGeoFile=fileGeo;
  fStr=NULL;
  fNoise=0;
  fNoisePS=0;
  fThreshold=0;
  fThresholdPS=0;
  fInf=CbmEcalInf::GetInstance(fGeoFile);
  fAlgo=0;
}

void CbmEcalPhotonCalibrator::InitArrays(UInt_t size, UInt_t start)
{
  fLowTheta.resize(size+1);
  fHighTheta.resize(size+1);
  fMaxX.resize(size+1);
  fMaxY.resize(size+1);
  for(UInt_t i=start;i<size+1;i++)
  {
    fLowTheta[i]=1111;
    fHighTheta[i]=-1111;
    fMaxX[i]=-1111;
    fMaxY[i]=-1111;
  }
}

void CbmEcalPhotonCalibrator::InitForGenerator()
{
  Float_t angle;
  Char_t maxtype=0;
  InitStructure();

  CbmEcalInf* fInf=fStr->GetEcalInf();
  if (fInf==NULL) 
    return;

  vector<CbmEcalModule*> modules;
  fStr->GetStructure(modules);
  for(UInt_t i=0;i<modules.size();i++)
    if (modules[i]!=NULL&&modules[i]->GetType()>maxtype)
      maxtype=modules[i]->GetType();

  InitArrays(maxtype,0);
  vector<CbmEcalCell*> cells;

  for(UInt_t i=0;i<modules.size();i++)
    if (modules[i])
    {
      Char_t moduleType=modules[i]->GetType();
      if (moduleType==0) continue;
      CbmEcalModule* module=modules[i];
      cells=module->GetCells();
      for(UInt_t j=0;j<cells.size();j++)
      {
	angle=GetAngle(cells[j]->GetX1(), cells[j]->GetY1(), fInf->GetZPos());
	if (angle<fLowTheta[moduleType])
	  fLowTheta[moduleType]=angle;
	if (angle>fHighTheta[moduleType])
	  fHighTheta[moduleType]=angle;
	angle=GetAngle(cells[j]->GetX1(), cells[j]->GetY2(), fInf->GetZPos());
	if (angle<fLowTheta[moduleType])
	  fLowTheta[moduleType]=angle;
	if (angle>fHighTheta[moduleType])
	  fHighTheta[moduleType]=angle;
	angle=GetAngle(cells[j]->GetX2(), cells[j]->GetY1(), fInf->GetZPos());
	if (angle<fLowTheta[moduleType])
	  fLowTheta[moduleType]=angle;
	if (angle>fHighTheta[moduleType])
	  fHighTheta[moduleType]=angle;
	angle=GetAngle(cells[j]->GetX2(), cells[j]->GetY2(), fInf->GetZPos());
	if (angle<fLowTheta[moduleType])
	  fLowTheta[moduleType]=angle;
	if (angle>fHighTheta[moduleType])
	  fHighTheta[moduleType]=angle;

	if (cells[j]->GetX2()>fMaxX[moduleType])
	  fMaxX[moduleType]=cells[j]->GetX2();
	if (cells[j]->GetY2()>fMaxY[moduleType])
	  fMaxY[moduleType]=cells[j]->GetY2();

      }
    }

}
void CbmEcalPhotonCalibrator::InitStructure()
{
  if (CbmEcalInf::GetInstance(NULL)==NULL)
    if (CbmEcalInf::GetInstance(fGeoFile)==NULL)
    {
      cerr << "Error: Can't read file " << fGeoFile << endl;
      return;
    }
  fStr=new CbmEcalStructure(CbmEcalInf::GetInstance(NULL));
  fStr->Construct();
}

Float_t CbmEcalPhotonCalibrator::GetAngle(Float_t x, Float_t y, Float_t z)
{
  Float_t dx=x-fBirthX;
  Float_t dy=y-fBirthY;
  Float_t dz=z-fBirthZ;
  return TMath::ACos(TMath::Abs(dz)/TMath::Sqrt(dx*dx+dy*dy+dz*dz));

}

InitStatus CbmEcalPhotonCalibrator::Init()
{
  FairRootManager* fManager=FairRootManager::Instance();

  if (fInf==NULL)
  {
    cerr << "Can't get information from " << fGeoFile << endl;
    return kFATAL;
  }
  fLitePoints=(TClonesArray*)fManager->ActivateBranch("ECALPointLite");
  if (fLitePoints==NULL)
  {
    cerr << "There are no ECALPointLite branch in the file!!!" << endl;
    return kFATAL; 
  }
  fEcalPoints=(TClonesArray*)fManager->ActivateBranch("ECALPoint");
  if (fEcalPoints==NULL)
  {
    cerr << "There are no ECALPoint branch in the file!!!" << endl;
    return kFATAL; 
  }
  fMCTracks=(TClonesArray*)fManager->ActivateBranch("MCTrack");
  if (fMCTracks==NULL)
  {
    cerr << "There are no MCTrack branch in the file!!!" << endl;
    return kFATAL;
  }

  fInf->CheckVariables();
  fStr=new CbmEcalStructure(fInf);
  fStr->Construct();
  fEvent=0;

  fTree=NULL;
  return kSUCCESS;
}

void CbmEcalPhotonCalibrator::Exec(Option_t* option)
{
  CbmEcalPointLite* pt=NULL;
  CbmEcalPoint* p=NULL;
  CbmEcalCell* cell;
  CbmEcalCell* oldcell;
  CbmMCTrack* track;
  TVector3 mom;
  UInt_t n;
  Int_t ten;
  Bool_t isPS;
  UInt_t points=0;
  Float_t EcalE=0;
  Float_t EcalPSE=0;
  Float_t EcalFullE=0;
  CbmEcalInf* fInf=CbmEcalInf::GetInstance(NULL);
  Float_t dz=fInf->GetPSLead()+fInf->GetPSScin()+fInf->GetPSGap();
  Float_t x;
  Float_t y;
  Float_t z;

  Float_t energy;
  Float_t psenergy;

  list<CbmEcalCell*> cells;
  list<CbmEcalCell*>::const_iterator pc;

  fEvent++;
  if (fTree==NULL)
  {
    fTree=new TTree("ECALcal","ECAL calibration tree");
    fTree->Branch("fe",&fFullEnergy,"fe/F");
    fTree->Branch("ecale",&fEcalEnergy,"ecale/F");
    fTree->Branch("pse",&fPSEnergy,"pse/F");

    fTree->Branch("ecale2x2",&f2x2EcalEnergy,"ecale2x2/F");
    fTree->Branch("pse2x2",&f2x2PSEnergy,"pse2x2/F");

    fTree->Branch("ecale3x3",&f3x3EcalEnergy,"ecale3x3/F");
    fTree->Branch("pse3x3",&f3x3PSEnergy,"pse3x3/F");

    fTree->Branch("ecale5x5",&f5x5EcalEnergy,"ecale5x5/F");
    fTree->Branch("pse5x5",&f5x5PSEnergy,"pse5x5/F");

    fTree->Branch("e",&fE,"e/F");
    fTree->Branch("px",&fPx,"px/F");
    fTree->Branch("py",&fPy,"py/F");
    fTree->Branch("pz",&fPz,"pz/F");
    fTree->Branch("x",&fX,"x/F");
    fTree->Branch("y",&fY,"y/F");
    fTree->Branch("z",&fZ,"z/F");
    fTree->Branch("steps",&fSteps,"steps/I");
    fTree->Branch("type",&fType,"type/I");
  }
  cout << "--> Event no." << fEvent << endl;
  
  fStr->ResetModulesFast();

  // Filling structure
  n=fLitePoints->GetEntriesFast();
  cout << "Number of input MC points: " << n << flush;
  for(UInt_t i=0;i<n;i++)
  {
    pt=(CbmEcalPointLite*)fLitePoints->At(i);
    cell=fStr->GetCell(pt->GetDetectorID(), ten, isPS);
    if (ten==0)
    {
//      EcalFullE+=pt->GetEnergyLoss();
      if (isPS)
      {
	cell->AddPSEnergy(pt->GetEnergyLoss());
//	EcalPSE+=pt->GetEnergyLoss();
      }
      else
      {
	cell->AddEnergy(pt->GetEnergyLoss());
//	EcalE+=pt->GetEnergyLoss();
      }
    }
  }
  cells.clear();
  fStr->GetCells(cells);
  for(pc=cells.begin();pc!=cells.end();++pc)
  {
    cell=(*pc);
    if (fNoise>0)
      energy=cell->GetEnergy()+gRandom->Gaus(0,fNoise);
    else
      energy=cell->GetEnergy();
    if (fNoisePS>0)
      psenergy=cell->GetPSEnergy()+gRandom->Gaus(0,fNoisePS);
    else
      psenergy=cell->GetPSEnergy();

    if (energy<fThreshold) energy=0;
    if (psenergy<fThresholdPS) psenergy=0;
    cell->SetEnergy(energy);
    cell->SetPSEnergy(psenergy);
    EcalE+=energy;
    EcalPSE+=psenergy;
    EcalFullE+=energy+psenergy;
  }

  fFullEnergy=EcalFullE;
  fPSEnergy=EcalPSE;
  fEcalEnergy=EcalE;

  n=fEcalPoints->GetEntriesFast();
  cout << ". Number of incoming particles: " << n << flush;

  for(UInt_t i=0;i<n;i++)
  {
    p=(CbmEcalPoint*)fEcalPoints->At(i);
    track=(CbmMCTrack*)fMCTracks->At(p->GetTrackID());
    p->Momentum(mom);
    if (TMath::Abs(mom.Mag()-fEnergy)<fEnergy*1e-3&&track->GetPdgCode()==fPDGCode)
    {
      fX=p->GetX();
      fY=p->GetY();
      fZ=p->GetZ();
      fPx=p->GetPx();
      fPy=p->GetPy();
      fPz=p->GetPz();
      fE=fEnergy;
      points++;
    }
  }
  if (points!=1)
  {
    cout << endl;
    cout << "Some thing strange. Number of";
    cout << "suitable MC points: " << points << endl;
    return;
  }

  z=fZ+dz;
  x=fX+fPx/fPz*dz;
  y=fY+fPy/fPz*dz;

  cell=fStr->GetCell(x,y);
  if (cell==NULL)
  {
    cout << "Error: Can't find cell with (" << x << ",";
    cout << y << ")" << endl;
    return;
  }
  fSteps=0;
  oldcell=NULL;
  list<CbmEcalCell*> lst;
  list<CbmEcalCell*>::const_iterator lp=lst.begin();
  while(oldcell!=cell)
  {
    oldcell=cell;
    cell->GetNeighborsList(0,lst);
    fSteps++;
    for(lp=lst.begin();lp!=lst.end();++lp)
      if ((*lp)->GetTotalEnergy()>cell->GetTotalEnergy())
      {
	cell=(*lp);
      }
  }

  fType=fStr->GetModule(cell->GetCenterX(), cell->GetCenterY())->GetType();

  cell->GetClusterEnergy(0,f3x3EcalEnergy, f3x3PSEnergy);
  cell->Get5x5ClusterEnergy(f5x5EcalEnergy, f5x5PSEnergy);
  f2x2EcalEnergy=0;
  f2x2PSEnergy=0;
  if (fAlgo==0)
    for(Int_t j=1;j<5;j++)
    {
      cell->GetClusterEnergy(j,EcalE, EcalPSE);
      if (EcalE+EcalPSE>f2x2EcalEnergy+f2x2PSEnergy)
      {
        f2x2EcalEnergy=EcalE;
        f2x2PSEnergy=EcalPSE;
      }
    }
  else
  {
    Double_t mine;
    Double_t maxe;
    CbmEcalCell* min;
    Int_t max;
    Int_t j;
    Double_t e;

    mine=cell->GetTotalEnergy(); min=cell;
    maxe=-1.0;
    cells.clear();
    /** Find minimum cell **/
    cell->GetNeighborsList(0, cells);
    for(pc=cells.begin();pc!=cells.end();++pc)
      if ((*pc)->GetTotalEnergy()<mine)
      {
	mine=(*pc)->GetTotalEnergy();
	min=(*pc);
      }
    for(j=1;j<5;j++)
    {
      cell->GetNeighborsList(j, cells);
      e=0;
      for(pc=cells.begin();pc!=cells.end();++pc)
	e+=(*pc)->GetTotalEnergy();
      if (e>maxe)
      {
	maxe=e;
	max=j;
      }
    }
    maxe+=cell->GetTotalEnergy();
    cell->GetNeighborsList(max, cells);
    cells.push_back(cell);
    if (find(cells.begin(), cells.end(), min)==cells.end())
      cells.push_back(min);
    for(pc=cells.begin();pc!=cells.end();++pc)
    {
      f2x2EcalEnergy+=(*pc)->GetEnergy();
      f2x2PSEnergy+=(*pc)->GetPSEnergy();
    }
  }
  cout << endl;
  fTree->Fill();
}

void CbmEcalPhotonCalibrator::Finish()
{
  ;
}

CbmEcalPhotonCalibrator::~CbmEcalPhotonCalibrator()
{
  fTree->Write();
}

ClassImp(CbmEcalPhotonCalibrator)
