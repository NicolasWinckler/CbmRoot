#include "CbmEcalIdentification.h"

#include "FairRootManager.h"
#include "FairTrackParam.h"

#include "CbmEcalStructure.h"
#include "CbmEcalCalibration.h"
#include "CbmEcalIdParticle.h"
#include "CbmEcalTrackExtrapolation.h"
#include "CbmEcalCell.h"
#include "CbmEcalParam.h"
#include "CbmMCTrack.h"
#include "CbmEcalInf.h"
#include "CbmEcalPoint.h"

#include "CbmGlobalTrack.h"
#include "CbmTrdTrack.h"
#include "CbmTrackMatch.h"

#include "TTree.h"
#include "TClonesArray.h"
#include "TMath.h"

#include <iostream>
#include <map>

using namespace std;

/** Loop procedure **/
void CbmEcalIdentification::Exec(Option_t* option)
{
  fEvent++;
  if (fTreeOut) InitTree();
  fId->Clear();

  Int_t n=fTracks->GetEntriesFast();
  Int_t i;
  Int_t oldn;
  fN=0;
  FairTrackParam* tr;
  if (fVerbose>9)
    Info("Exec", "Total %d tracks before the calorimeter", n);
  for(i=0;i<n;i++)
  {
    if (fTreeOut) InitVar();
    tr=(FairTrackParam*)fTracks->At(i);
    fExtraNum=i;
    oldn=fN;
    Identify(tr);
    if (oldn!=fN&&fTreeOut)
      WriteTree();
  }
}

/** Identify a track **/
void CbmEcalIdentification::Identify(FairTrackParam* tr)
{
  CbmEcalCell* cell;
  CbmEcalCell* cell2;
  CbmEcalCell* max=NULL;
  list<CbmEcalCell*> cells;
  list<CbmEcalCell*>::const_iterator p;
  list<CbmEcalCell*> ocells;
  list<CbmEcalCell*>::const_iterator op;
  Double_t e;
  Double_t tracke;
  Float_t x;
  Float_t y;
  Float_t dst;
  Float_t t;
  CbmEcalInf* fInf=fStr->GetEcalInf();
  TVector3 trackmom;
  Double_t te;
  Double_t me;
  Double_t me1;
  Int_t i;
  Int_t mc;


  tr->Momentum(trackmom);
  fX=tr->GetX();
  fY=tr->GetY();
  fZ=tr->GetZ();
  cell=fStr->GetCell(fX, fY);
  fTrackNum=fExtra->Map()[fExtraNum];
  if (cell==NULL)
  {
    if (fTreeOut) WriteTreeLight();
    return;
  }
  cell->GetNeighborsList(0, cells);
  fCellType=cell->GetType();
  /** Check cell corresponds to track position for maximum **/
  e=cell->GetTotalEnergy();
  for(p=cells.begin();p!=cells.end();++p)
    if ((*p)->GetTotalEnergy()>e)
      break;
  if (p==cells.end())
    max=cell;
  cell->GetNeighborsList(0, cells);
  e=cell->GetTotalEnergy();
  if (max==NULL)
  /** Check cells near given for maximums **/
  for(p=cells.begin();p!=cells.end();++p)
  {
    x=(*p)->GetCenterX()-tr->GetX();
    y=(*p)->GetCenterY()-tr->GetY();
    dst=TMath::Sqrt(x*x+y*y);
//      if (dst>fInf->GetModuleSize()*TMath::Sqrt(2.0)+0.001) continue;
    x=tr->GetX(); x*=x; y=tr->GetY(); y*=y;
    t=TMath::Sqrt(x+y);
    x=(*p)->GetCenterX()-cell->GetCenterX();
    y=(*p)->GetCenterY()-cell->GetCenterY();
//    if (x*tr->GetTx()/t+y*tr->GetTy()/t<TMath::Sqrt(2.0)*fInf->GetModuleSize()/cell->GetType()+0.001)
//    if (x*tr->GetTx()/t+y*tr->GetTy()/t<-0.01&&dst>fInf->GetModuleSize()/cell->GetType()+0.001) continue;
    (*p)->GetNeighborsList(0, ocells);
    e=(*p)->GetTotalEnergy();
    for(op=ocells.begin(); op!=ocells.end();++op)
      if ((*op)->GetTotalEnergy()>e) break;
    if (op==ocells.end())
      if (max==NULL||max->GetTotalEnergy()<(*p)->GetTotalEnergy())
        max=*p;
  }

  if (max==NULL)
  {
    if (fTreeOut) WriteTreeLight();
    return;
  }
  max->GetNeighborsList(0, cells); te=0;
  fCellType=max->GetType();
  for(p=cells.begin();p!=cells.end();++p)
    te+=GetEnergy(*p)+GetPSEnergy(*p);
  me=0; fPSEAll=0;
  for(i=1;i<5;i++)
  {
    e=0;
    max->GetNeighborsList(i, cells);
    for(p=cells.begin();p!=cells.end();++p)
    {
      fPSEAll+=GetPSEnergy(*p);
      e+=GetEnergy(*p)+GetPSEnergy(*p);
    }
    if (e>me)
    {
      me=e;
      mc=i;
    }
  }
  me1=GetEnergy(max)+GetPSEnergy(max);
  me+=me1;
  te+=me1;
  fShape=me/te;
  fE=fCal->GetEnergy(me, max);
  x=fX; x+=trackmom.X()/trackmom.Z()*(fInf->GetPSLead()+fInf->GetPSScin()/2.0);
  y=fY; y+=trackmom.Y()/trackmom.Z()*(fInf->GetPSLead()+fInf->GetPSScin()/2.0);
  cell2=fStr->GetCell(x, y);
  if (cell2)
    fPSE3=GetPSEnergy(cell2);
  else
    fPSE3=-1111;
  fPSE=GetPSEnergy(max);
  fPSE2=GetPSEnergy(cell);
  fPSEAll+=fPSE;
  fMaxX=max->GetCenterX();
  fMaxY=max->GetCenterY();
  fCellX=cell->GetCenterX();
  fCellY=cell->GetCenterY();
  //TODO: Calculate probability
  fEProb=1;
  tracke=trackmom.Mag();
  fTrackP=tracke;
  if (fE<tracke)
    fEProb*=TMath::Erfc((tracke-fE)/TMath::Sqrt(tracke));

  fMCTrackNum=-1111;
  if (fUseMC==1)
  {
    CbmGlobalTrack* gtr=(CbmGlobalTrack*)fGlobal->At(fTrackNum);
    Int_t stsn=gtr->GetStsTrackIndex();
    if (stsn>=0)
    {
      CbmTrackMatch* ststr=(CbmTrackMatch*)fStsTracksMatch->At(stsn);
      if (ststr)
	fMCTrackNum=ststr->GetMCTrackId();
    }
  }
  //fEProb=-1111;
  new((*fId)[fN++]) CbmEcalIdParticle(max, fX, fY, fZ, fE/tracke, TMath::Max(fPSE, fPSE3), fShape, fTrackNum, fEProb, fMCTrackNum);
}

/** If identification failed :-( **/
void CbmEcalIdentification::WriteTreeLight()
{
  fPDG=-1111;
  fMCPDG=-1111;
  fTrdELoss=-1111;
  fMotherMCPDG=-1111;
  fMCM=-1111;
  if (fUseMC==1)
  {
    CbmGlobalTrack* gtr=(CbmGlobalTrack*)fGlobal->At(fTrackNum);
    Int_t stsn=gtr->GetStsTrackIndex();
    if (stsn>=0)
    {
      CbmTrackMatch* ststr=(CbmTrackMatch*)fStsTracksMatch->At(stsn);
      if (ststr)
	fMCTrackNum=ststr->GetMCTrackId();
    }
  }
  if (fUseMC&&fMCTrackNum>=0)
  {
    CbmMCTrack* mctr=(CbmMCTrack*)fMC->At(fMCTrackNum);
    CbmMCTrack* mother;
    if (mctr->GetMotherId()>=0)
    {
      mother=(CbmMCTrack*)fMC->At(mctr->GetMotherId());
      fMotherMCPDG=mother->GetPdgCode();
    }
    fMCPDG=mctr->GetPdgCode();
    fMCP=mctr->GetP();

    CbmEcalPoint* pt=NULL;
    CbmMCTrack* tr;
    CbmMCTrack* mtr;
    Int_t n=fMCPoints->GetEntriesFast();
    Int_t i=0;
    Int_t j;
    Double_t x1;
    Double_t y1;
    Double_t dx;
    Double_t dy;
    Double_t r;
    TVector3 mom;

    for(i=0;i<n;i++)
    {
      pt=(CbmEcalPoint*)fMCPoints->At(i);
      if (pt->GetTrackID()==fMCTrackNum)
	break;
    }
    if (i!=n)
    {
      fMCM=0;
      dx=pt->GetX()-fX;
      dy=pt->GetY()-fY;
      x1=pt->GetX();
      y1=pt->GetY();
      fR=TMath::Sqrt(dx*dx+dy*dy);
      pt->Momentum(mom);
      fMCSurfE=mom.Mag();
      fMCCircE=fMCSurfE;
      for(j=0;j<n;j++)
      if (j!=i)
      {
	pt=(CbmEcalPoint*)fMCPoints->At(j);
	if (pt->GetTrackID()>=0)
	  tr=(CbmMCTrack*)fMC->At(pt->GetTrackID());
	else
	  tr=NULL;
	while(tr!=NULL)
	{
	  if (tr->GetMotherId()<0)
	  {
	    tr=NULL; break;
	  }
	  if (tr->GetMotherId()==fMCTrackNum)
	    break;
	  tr=(CbmMCTrack*)fMC->At(tr->GetMotherId());
	}
	if (tr)
        {
          pt->Momentum(mom);
	  dx=x1-pt->GetX();
	  dy=y1-pt->GetY();
	  r=TMath::Sqrt(dx*dx+dy*dy);
	  fMCM+=r*mom.Mag();
          if (r<fCircRad)
  	    fMCCircE+=mom.Mag();
	}
      }
    }
    else
    {
      fR=-1111;
      fMCSurfE=-1111;
    }
  }
  fTree->Fill();
}
/** Write information to the tree **/
void CbmEcalIdentification::WriteTree()
{
  fPDG=-1111;
  fMCPDG=-1111;
  fTrdELoss=-1111;
  fMotherMCPDG=-1111;
  fMCM=-1111;
  if (fTrackNum>=0)
  {
    CbmGlobalTrack* gtr=(CbmGlobalTrack*)fGlobal->At(fTrackNum);
    fPDG=gtr->GetPidHypo();
    fTrChi2=gtr->GetChi2();
    fNTRD=gtr->GetTrdTrackIndex();
    fNTOF=gtr->GetTofHitIndex();
    fNRICH=gtr->GetRichRingIndex();
    if (fNTRD>=0)
    {
      CbmTrdTrack* trdtr=(CbmTrdTrack*)fTrdTracks->At(fNTRD);
      fTrdELoss=trdtr->GetELoss();
    }
  }
  else
  {
    fPDG=-1111;
    fTrChi2=-1111;
    fNTRD=-1111;
    fNTOF=-1111;
    fNRICH=-1111;
  }
  if (fMCTrackNum>=0&&fUseMC==1)
  {
    CbmMCTrack* mctr=(CbmMCTrack*)fMC->At(fMCTrackNum);
    CbmMCTrack* mother;
    if (mctr->GetMotherId()>=0)
    {
      mother=(CbmMCTrack*)fMC->At(mctr->GetMotherId());
      fMotherMCPDG=mother->GetPdgCode();
    }
    fMCPDG=mctr->GetPdgCode();
    fMCP=mctr->GetP();

    CbmEcalPoint* pt=NULL;
    CbmMCTrack* tr;
    CbmMCTrack* mtr;
    Int_t n=fMCPoints->GetEntriesFast();
    Int_t i=0;
    Int_t j;
    Double_t x1;
    Double_t y1;
    Double_t dx;
    Double_t dy;
    Double_t r;
    TVector3 mom;

    for(i=0;i<n;i++)
    {
      pt=(CbmEcalPoint*)fMCPoints->At(i);
      if (pt->GetTrackID()==fMCTrackNum)
	break;
    }
    if (i!=n)
    {
      fMCM=0;
      dx=pt->GetX()-fX;
      dy=pt->GetY()-fY;
      x1=pt->GetX();
      y1=pt->GetY();
      fR=TMath::Sqrt(dx*dx+dy*dy);
      pt->Momentum(mom);
      fMCSurfE=mom.Mag();
      fMCCircE=fMCSurfE;
      for(j=0;j<n;j++)
      if (j!=i)
      {
	pt=(CbmEcalPoint*)fMCPoints->At(j);
	if (pt->GetTrackID()>=0)
	  tr=(CbmMCTrack*)fMC->At(pt->GetTrackID());
	else
	  tr=NULL;
	while(tr!=NULL)
	{
	  if (tr->GetMotherId()<0)
	  {
	    tr=NULL; break;
	  }
	  if (tr->GetMotherId()==fMCTrackNum)
	    break;
	  tr=(CbmMCTrack*)fMC->At(tr->GetMotherId());
	}
	if (tr)
        {
          pt->Momentum(mom);
	  dx=x1-pt->GetX();
	  dy=y1-pt->GetY();
	  r=TMath::Sqrt(dx*dx+dy*dy);
	  fMCM+=r*mom.Mag();
          if (r<fCircRad)
  	    fMCCircE+=mom.Mag();
	}
      }
    }
    else
    {
      fR=-1111;
      fMCSurfE=-1111;
    }
  }
  fTree->Fill();
}

/** Get energy of the cell **/
Double_t CbmEcalIdentification::GetEnergy(CbmEcalCell* cell)
{
  if (fUseHack==0)
    return cell->GetEnergy();

  map<Int_t, Float_t>::const_iterator q;
  Double_t e=0;
  CbmMCTrack* tr;
  Int_t pdg;

  for(q=cell->GetTrackEnergyBegin();q!=cell->GetTrackEnergyEnd();++q)
  {
    tr=(CbmMCTrack*)fMC->At((*q).first);
    if (tr==NULL)
    {
      Info("GetEnergy","Can't find MC track with number %d.",(*q).first);
      continue;
    }
    pdg=tr->GetPdgCode();
    if (pdg==22||pdg==-11||pdg==11||pdg==13||pdg==-13)
      e+=(*q).second;
    else
      e+=(*q).second*fHack;
  }
  return e;
}

/** Get energy in PS  **/
Double_t CbmEcalIdentification::GetPSEnergy(CbmEcalCell* cell)
{
  if (fUseHack==0)
    return cell->GetPSEnergy();

  map<Int_t, Float_t>::const_iterator q;
  Double_t e=0;
  CbmMCTrack* tr;
  Int_t pdg;

  for(q=cell->GetTrackPSEnergyBegin();q!=cell->GetTrackPSEnergyEnd();++q)
  {
    tr=(CbmMCTrack*)fMC->At((*q).first);
    if (tr==NULL)
    {
      Info("GetEnergy","Can't find MC track with number %d.",(*q).first);
      continue;
    }
    pdg=tr->GetPdgCode();
    if (pdg==22||pdg==-11||pdg==11||pdg==13||pdg==-13)
      e+=(*q).second;
    else
      e+=(*q).second*fHack;
  }
  return e;
}

/** Initializes tree **/
void CbmEcalIdentification::InitTree()
{
  if (fTree) return;
  fTree=new TTree("ecalid","Identification in calorimeter");
  fTree->Branch("ev", &fEvent, "ev/I");
  fTree->Branch("cellx", &fCellX, "cellx/D");
  fTree->Branch("celly", &fCellY, "celly/D");
  fTree->Branch("maxx", &fMaxX, "maxx/D");
  fTree->Branch("maxy", &fMaxY, "maxy/D");
  fTree->Branch("x", &fX, "x/D");
  fTree->Branch("y", &fY, "y/D");
  fTree->Branch("z", &fZ, "z/D");
  fTree->Branch("e", &fE, "e/D");
  fTree->Branch("pse", &fPSE, "pse/D");
  fTree->Branch("pse2", &fPSE2, "pse2/D");
  fTree->Branch("pse3", &fPSE3, "pse3/D");
  fTree->Branch("pseall", &fPSEAll, "pseall/D");
  fTree->Branch("shape", &fShape, "shape/D");
  fTree->Branch("eprob", &fEProb, "eprob/D");
  fTree->Branch("n", &fN, "n/I");
  fTree->Branch("tr", &fTrackNum, "tr/I");
  fTree->Branch("mctr", &fMCTrackNum, "mctr/I");
  fTree->Branch("pdg", &fPDG, "pdg/I");
  fTree->Branch("tcell", &fCellType, "pdg/I");
  fTree->Branch("mcpdg", &fMCPDG, "mcpdg/I");
  fTree->Branch("mmcpdg", &fMotherMCPDG, "mmcpdg/I");
  fTree->Branch("mcp", &fMCP, "mcp/D");
  fTree->Branch("mcm", &fMCM, "mcm/D");
  fTree->Branch("trp", &fTrackP, "trp/D");
  fTree->Branch("r", &fR, "r/D");
  fTree->Branch("mcsurfe", &fMCSurfE, "mcsurfe/D");
  fTree->Branch("mccirce", &fMCCircE, "mccirce/D");
  fTree->Branch("ntdr", &fNTRD, "ntrd/I");
  fTree->Branch("nrich", &fNRICH, "nrich/I");
  fTree->Branch("ntof", &fNTOF, "ntof/I");
  fTree->Branch("trchi2", &fTrChi2, "trchi2/D");
  fTree->Branch("trdeloss", &fTrdELoss, "trdeloss/D");
}

/** Initializes tree **/
void CbmEcalIdentification::InitVar()
{
  fCellX=-1111;
  fCellY=-1111;
  fMaxX=-1111;
  fMaxY=-1111;
  fX=-1111;
  fY=-1111;
  fZ=-1111;
  fE=-1111;
  fPSE=-1111;
  fPSE2=-1111;
  fPSE3=-1111;
  fPSEAll=-1111;
  fShape=-1111;
  fEProb=-1111;
  fTrackNum=-1111;
  fMCTrackNum=-1111;
  fPDG=-1111;
  fCellType=-1111;
  fMCPDG=-1111;
  fMotherMCPDG=-1111;
  fMCP=-1111;
  fMCM=-1111;
  fTrackP=-1111;
  fR=-1111;
  fMCSurfE=-1111;
  fMCCircE=-1111;
  fNTRD=-1111;
  fNRICH=-1111;
  fNTOF=-1111;
  fTrChi2=-1111;
  fTrdELoss=-1111;
}


CbmEcalIdentification::CbmEcalIdentification(const char* name, const Int_t iVerbose, const char* config)
  : FairTask(name, iVerbose)
{
  fTree=NULL;
  fCfgName=config;
}

/** Initing routine **/
InitStatus CbmEcalIdentification::Init()
{
  FairRootManager* fManager=FairRootManager::Instance();
  if (!fManager)
  {
    Fatal("Init", "Can't find a Root Manager.");
    return kFATAL;
  }
  fTracks=(TClonesArray*)fManager->GetObject("EcalTrackParam");
  if (!fTracks)
  {
    Fatal("Init", "Can't find an array of reconstructed tracks.");
    return kFATAL;
  }
  fStr=(CbmEcalStructure*)fManager->GetObject("EcalStructure");
  if (!fStr)
  {
    Fatal("Init()", "Can't find calorimeter structure in the system.");
    return kFATAL;
  }
  fCal=(CbmEcalCalibration*)fManager->GetObject("EcalCalibration");
  if (!fCal)
  {
    Fatal("Init", "Can't find EcalCalibration");
    return kFATAL;
  }
  fExtra=(CbmEcalTrackExtrapolation*)fManager->GetObject("EcalTrackExtrapolation");
  if (!fExtra)
  {
    Fatal("Init", "Can't find EcalTrackExtrapolation");
    return kFATAL;
  }
  fGlobal=(TClonesArray*)fManager->GetObject("GlobalTrack");
  if (!fGlobal)
  {
    Fatal("Init()","Can't find global tracks array");
    return kFATAL;
  }
  fId=new TClonesArray("CbmEcalIdParticle", 2000);
  fManager->Register("EcalId", "ECAL", fId, kTRUE);
  fEvent=0;
  CbmEcalParam* p=new CbmEcalParam("IdentificationParam", fCfgName);
  fTreeOut=p->GetInteger("treeout");
  fUseHack=p->GetInteger("usehacks");
  fUseMC=p->GetInteger("usemcinfo");
  fEResolution=p->GetDouble("eresolution");
  if (fUseHack>0)
  {
    Info("Init", "Hack option given in configuration file.");
    fHack=p->GetDouble("hack");
    fMC=(TClonesArray*)fManager->GetObject("MCTrack");
    if (!fMC)
    {
      Fatal("Init", "Can't find array of MC tracks in system.");
      return kFATAL;
    }
    fMCPoints=(TClonesArray*)fManager->GetObject("ECALPoint");
    if (!fMCPoints)
    {
      Fatal("Init","Can't find array of ECALPoint in the system");
      return kFATAL;
    }
  }
  if (fUseMC)
  {
    fCircRad=p->GetDouble("circrad");
/*
    fStsTracks=(TClonesArray*)fManager->GetObject("STSTrack");
    if (!fStsTracks)
    {
      Fatal("Init()","Can't find STS tracking information");
      return kFATAL;
    }
*/
    fStsTracksMatch=(TClonesArray*)fManager->GetObject("STSTrackMatch");
    if (!fStsTracksMatch)
    {
      Fatal("Init()","Can't find STS track matching information");
      return kFATAL;
    }
    fTrdTracks=(TClonesArray*)fManager->GetObject("TRDTrack");
    if (!fTrdTracks)
    {
      Fatal("Init()","Can't find TRD track information in run");
    }
  }
  delete p;

  fTree=NULL;

  return kSUCCESS;
}

/** Finishing routine **/
void CbmEcalIdentification::Finish()
{
  if (fTree)
    fTree->Write();
}

ClassImp(CbmEcalIdentification)
