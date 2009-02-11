/**  CbmEcalDetailed.cxx
 *@author Mikhail Prokudin
 **
 ** Defines the active detector ECAL with geometry coded here.
 ** Layers, holes, fibers,steel tapes implemented 
 **/

#include "CbmEcalDetailed.h"

#include "CbmEcalPoint.h"
#include "CbmGeoEcal.h"
#include "CbmGeoEcalPar.h"

#include "CbmDetectorList.h"
#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairGeoRootBuilder.h"
#include "FairRuntimeDb.h"
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRunAna.h"
#include "CbmMCTrack.h"
#include "CbmStack.h"
#include "CbmDetectorList.h"
#include "FairVolume.h"
#include "FairGeoMedium.h"
#include "FairGeoMedia.h"

#include "TClonesArray.h"
#include "TGeoMCGeometry.h"
#include "TGeoManager.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TGeoBBox.h"
#include "TGeoPgon.h"
#include "TGeoTube.h"
#include "TGeoMatrix.h"
#include "TList.h"

#include <iostream>
#include <stdlib.h>

using namespace std;

#define kN kNumberOfECALSensitiveVolumes

// -----   Default constructor   -------------------------------------------
CbmEcalDetailed::CbmEcalDetailed() : FairDetector("ECAL", kTRUE, kECAL) {
//  CbmEcalPoint::Class()    ->IgnoreTObjectStreamer();
//  CbmEcalPointLite::Class()->IgnoreTObjectStreamer();
//  CbmMCTrack::Class()      ->IgnoreTObjectStreamer();
  fEcalCollection = new TClonesArray("CbmEcalPoint");
  fLiteCollection = new TClonesArray("CbmEcalPointLite");
  fPosIndex = 0;
  fVerboseLevel = 1;

  Int_t i;

  for(i=kN-1;i>-1;i--)
    fVolArr[i]=-1111;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmEcalDetailed::CbmEcalDetailed(const char* name, Bool_t active, const char* fileGeo)
  : FairDetector(name, active, kECAL)
{
  /** CbmEcalDetailed constructor:
   ** reads geometry parameters from the ascii file <fileGeo>,
   ** creates the ECAL geometry container CbmEcalInf
   ** and initializes basic geometry parameters needed to construct
   ** TGeo geometry
   **/

  Int_t i;
  Int_t j;
  TString nm;
  Info("CbmEcalDetailed","Geometry is read from file %s.", fileGeo);
  fInf=CbmEcalInf::GetInstance(fileGeo);
  if (fInf==NULL)
  {
    Fatal("CbmEcalDetailed"," Can't read geometry from %s.", fileGeo);
    return;
  }
  fGeoScale=1.;
  fEcalSize[0]=fInf->GetEcalSize(0);
  fEcalSize[1]=fInf->GetEcalSize(1);
  fEcalSize[2]=fInf->GetEcalSize(2);

  fCellSize=fInf->GetCellSize();
  fZEcal=fInf->GetZPos();

  fThicknessLead=fInf->GetLead();
  fThicknessScin=fInf->GetScin();
  fThicknessTyvk=fInf->GetTyveec();
  fNLayers=fInf->GetNLayers();

  fThicknessPSlead=fInf->GetPSLead();
  fThicknessPSscin=fInf->GetPSScin();
  fEcalPSgap=fInf->GetPSGap();

  fXSize=fInf->GetXSize();
  fYSize=fInf->GetYSize();
  
  fPosIndex=0;
  fDebug="";
  fEcalCollection=new TClonesArray("CbmEcalPoint");
  fLiteCollection=NULL;
  fVerboseLevel = 1;

  fHoleRad=fInf->GetVariableStrict("holeradius");
  fFiberRad=fInf->GetVariableStrict("fiberradius");
  fThicknessSteel=fInf->GetVariableStrict("steel");
  fEdging=fInf->GetVariableStrict("tileedging");
  fModuleSize=fInf->GetVariableStrict("modulesize");

  fInf->AddVariable("ecalversion", "1"); 
  for(i=kN-1;i>-1;i--)
    fVolArr[i]=-1111;

  if (fInf->GetFastMC()==1) return;
  fLiteCollection=new TClonesArray("CbmEcalPointLite");

  for(i=0;i<cMaxModuleType;i++)
  {
    fModules[i]=NULL;
    fCells[i]=NULL;
    fScTiles[i]=NULL;
    fPbTiles[i]=NULL;
    fTvTiles[i]=NULL;
    fHolePos[i]=NULL;
    fTileEdging[i]=NULL;
    fModulesWithType[i]=0;
  }
  for(i=0;i<2;i++)
  {
    fSteelTapes[i]=NULL;
  }
  for(i=0;i<3;i++)
  {
    fHoleVol[i]=NULL;
    fFiberVol[i]=NULL;
  }
  /** Counting modules **/
  for(i=0;i<fInf->GetXSize();i++)
  for(j=0;j<fInf->GetYSize();j++)
    fModulesWithType[fInf->GetType(i,j)]++;

  for(i=1;i<cMaxModuleType;i++)
  {
    if (fModulesWithType[i]==0) continue;
    nm="nh[";nm+=i; nm+="]";
    fNH[i]=(Int_t)fInf->GetVariableStrict(nm);
    Info("CbmEcalDetailed", "Number of modules of type %d is %d (%d channels)", i, fModulesWithType[i], fModulesWithType[i]*i*i);
    fXCell[i]=(fModuleSize-2.0*fThicknessSteel)/i-2.0*fEdging;
    fYCell[i]=(fModuleSize-2.0*fThicknessSteel)/i-2.0*fEdging;
    Info("CbmEcalDetailed", "Size of cell of type %d is %f cm.", i, fXCell[i]);
  }
}

// -------------------------------------------------------------------------

void CbmEcalDetailed::Initialize()
{
  FairDetector::Initialize();
  FairRun* sim = FairRun::Instance();
  FairRuntimeDb* rtdb=sim->GetRuntimeDb();
  CbmGeoEcalPar *par=new CbmGeoEcalPar();
  fInf->FillGeoPar(par,0);
  rtdb->addContainer(par);
}

// -----   Destructor   ----------------------------------------------------
CbmEcalDetailed::~CbmEcalDetailed()
{
  if (fEcalCollection) {
    fEcalCollection->Delete(); 
    delete fEcalCollection;
    fEcalCollection=NULL;
  }
  if (fLiteCollection) {
    fLiteCollection->Delete();
    delete fLiteCollection;
    fLiteCollection=NULL;
  }
}
// -------------------------------------------------------------------------

// -----   Private method SetEcalCuts   ------------------------------------
void CbmEcalDetailed::SetEcalCuts(Int_t medium)
{
  /** Set GEANT3 tracking energy cuts for selected medium **/
  if (fInf->GetElectronCut() > 0) {
    gMC->Gstpar(medium,"CUTGAM",fInf->GetElectronCut());
    gMC->Gstpar(medium,"CUTELE",fInf->GetElectronCut());
    gMC->Gstpar(medium,"BCUTE" ,fInf->GetElectronCut());
    gMC->Gstpar(medium,"BCUTM" ,fInf->GetElectronCut());
  }

  if (fInf->GetHadronCut() > 0) {
    gMC->Gstpar(medium,"CUTNEU",fInf->GetHadronCut());
    gMC->Gstpar(medium,"CUTHAD",fInf->GetHadronCut());
    gMC->Gstpar(medium,"CUTMUO",fInf->GetHadronCut());
    gMC->Gstpar(medium,"PPCUTM",fInf->GetHadronCut());
  }
}
// -------------------------------------------------------------------------

void CbmEcalDetailed::FinishPrimary()
{
  if (fInf->GetFastMC()==1) return;
  fFirstNumber=fLiteCollection->GetEntriesFast();
}

//_____________________________________________________________________________
void CbmEcalDetailed::ChangeHit(CbmEcalPointLite* oldHit)
{
  Double_t edep = gMC->Edep();
  Double_t el=oldHit->GetEnergyLoss();
  Double_t ttime=gMC->TrackTime()*1.0e9;
  oldHit->SetEnergyLoss(el+edep);
  if(ttime<oldHit->GetTime())
    oldHit->SetTime(ttime);
}

//_____________________________________________________________________________
void CbmEcalDetailed::SetSpecialPhysicsCuts()
{
  /** Change the special tracking cuts for
   ** two ECAL media, Scintillator and Lead
   **/

  if (fInf->GetFastMC()==1) return;

  FairRun* fRun = FairRun::Instance();
  if (strcmp(fRun->GetName(),"TGeant3") == 0) {
    Int_t mediumID;
    mediumID = gGeoManager->GetMedium("Scintillator")->GetId();
    SetEcalCuts(mediumID);
    if (fInf->GetFastMC()==1) return;
    mediumID = gGeoManager->GetMedium("Lead")->GetId();
    SetEcalCuts(mediumID);
    mediumID = gGeoManager->GetMedium("Tyvek")->GetId();
    SetEcalCuts(mediumID);
    mediumID = gGeoManager->GetMedium("SensVacuum")->GetId();
    SetEcalCuts(mediumID);
    mediumID = gGeoManager->GetMedium("ECALAir")->GetId();
    SetEcalCuts(mediumID);
    mediumID = gGeoManager->GetMedium("ECALFiber")->GetId();
    SetEcalCuts(mediumID);
    mediumID = gGeoManager->GetMedium("ECALTileEdging")->GetId();
    SetEcalCuts(mediumID);
    mediumID = gGeoManager->GetMedium("ECALSteel")->GetId();
    SetEcalCuts(mediumID);
  }
}

// -----   Public method ProcessHits  --------------------------------------
Bool_t  CbmEcalDetailed::ProcessHits(FairVolume* vol)
{
  /** Fill MC point for sensitive ECAL volumes **/

  fELoss   = gMC->Edep();
  fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
  fTime    = gMC->TrackTime()*1.0e09;
  fLength  = gMC->TrackLength();

  if (vol->getVolumeId()==fStructureId)
    if (gMC->IsTrackEntering())
    {
      FillWallPoint();
      ((CbmStack*)gMC->GetStack())->AddPoint(kECAL, fTrackID);
  
      ResetParameters();

      return kTRUE;
    }
    else
      return kFALSE;

  if (fELoss>0)
  {
    Int_t i;
    TParticle* p=gMC->GetStack()->GetCurrentTrack();
    Double_t x, y, z;
    Double_t px;
    Double_t py;
    Double_t dx;
    Int_t mx;
    Int_t my;
    Int_t cell;
    Int_t type;
    Int_t cx;
    Int_t cy;
    gMC->TrackPosition(x, y, z);
//    cout << "Id: " << p->GetPdgCode() << " (" << x << ", " << y << ", ";
//    cout << z << "): ";
//    cout << endl;
    gMC->CurrentVolOffID(3, mx); mx--;
    gMC->CurrentVolOffID(4, my); my--;
    gMC->CurrentVolOffID(2, cell); cell--;
    Int_t id=(my*100+mx)*100+cell+1;
    fVolumeID=id;
    FillLitePoint(0);
//    type=fInf->GetType(mx, my);
//    cx=cell%type;
//    cy=cell/type;
//    px=mx*fModuleSize-fEcalSize[0]/2.0+cx*fModuleSize/type+1.0;
//    py=my*fModuleSize-fEcalSize[1]/2.0+cy*fModuleSize/type+1.0;
//    cout << "(" << px << ", " << py << "|" << type << "): ";
//    for(i=0;i<8;i++)
//    {
//      Int_t t;
//      
//      gMC->CurrentVolOffID(i, t);
//      cout << i << ": " << gMC->CurrentVolOffName(i) << " " << t << "; ";
//   }
//    cout << endl;
  }
  ((CbmStack*)gMC->GetStack())->AddPoint(kECAL, fTrackID);
  
  ResetParameters();

  return kTRUE;

}

/** returns type of volume **/
Int_t CbmEcalDetailed::GetVolType(Int_t volnum)
{
	Int_t i;
	for(i=kN-1;i>-1;i--) {
	  if (fVolArr[i]==volnum) break;
	}
        
	return i;
}

//-----------------------------------------------------------------------------
void CbmEcalDetailed::FillWallPoint()
{
  /** Fill MC points on the ECAL front wall **/

  gMC->TrackPosition(fPos);
  gMC->TrackMomentum(fMom);
  fVolumeID = -1;
  Double_t mass = gMC->TrackMass();
  // Calculate kinetic energy
  Double_t ekin = TMath::Sqrt( fMom.Px()*fMom.Px() +
			       fMom.Py()*fMom.Py() +
			       fMom.Pz()*fMom.Pz() +
			       mass * mass ) - mass;
  fELoss = ekin;
  // Create CbmEcalPoint at the entrance of calorimeter
  // for particles with pz>0 coming through the front wall
  if (fMom.Pz() > 0 && fPos.Z() < fZEcal+0.01)
    AddHit(fTrackID, fVolumeID, TVector3(fPos.X(),  fPos.Y(),  fPos.Z()),
	   TVector3(fMom.Px(), fMom.Py(), fMom.Pz()), fTime, fLength, 
	   fELoss);
  fTrackID=gMC->GetStack()->GetCurrentTrackNumber();
}

CbmEcalPointLite* CbmEcalDetailed::FindHit(Int_t VolId, Int_t TrackId)
{
  for(Int_t i=fFirstNumber;i<fLiteCollection->GetEntriesFast();i++)
  {
    CbmEcalPointLite* point=(CbmEcalPointLite*)fLiteCollection->At(i);
    if (point->GetTrackID()==TrackId&&point->GetDetectorID()==VolId)
      return point;
  }
  return NULL;
}
//-----------------------------------------------------------------------------
Bool_t CbmEcalDetailed::FillLitePoint(Int_t volnum)
{
  /** Fill MC points inside the ECAL for non-zero deposited energy **/

  //Search for input track
  
  static Float_t zmin=fZEcal-0.0001;
  static Float_t zmax=fZEcal+fEcalSize[2];
  static Float_t xecal=fEcalSize[0]/2;
  static Float_t yecal=fEcalSize[1]/2;
  TParticle* part=gMC->GetStack()->GetCurrentTrack();
  fTrackID=gMC->GetStack()->GetCurrentTrackNumber();

  /** Need to rewrite this part **/
  while (part->GetFirstMother()>=0&&\
      part->Vz()>=zmin&&part->Vz()<=zmax&& \
      TMath::Abs(part->Vx())<=xecal&&\
      TMath::Abs(part->Vy())<=yecal)
    {
      fTrackID=part->GetFirstMother();
      part =((CbmStack*)gMC->GetStack())->GetParticle(fTrackID);
    }
#ifdef _DECAL
  if (fTrackID<0) cout<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!fTrackID="<<fTrackID<<endl;
#endif
  CbmEcalPointLite* oldHit;
  CbmEcalPointLite* newHit;
  
  if ((oldHit=FindHit(fVolumeID,fTrackID))!=NULL)
    ChangeHit(oldHit);
  else
    // Create CbmEcalPoint for scintillator volumes
    newHit = AddLiteHit(fTrackID, fVolumeID, fTime, fELoss);
      

  return kTRUE;
}

// -----   Public method EndOfEvent   --------------------------------------
void CbmEcalDetailed::EndOfEvent() {
  if (fVerboseLevel) Print();
  fEcalCollection->Clear();

  if (fInf->GetFastMC()==0)
    fLiteCollection->Clear();
  fPosIndex = 0;
  fFirstNumber=0;
}
// -------------------------------------------------------------------------

// -----   Public method GetCollection   -----------------------------------
TClonesArray* CbmEcalDetailed::GetCollection(Int_t iColl) const
{
  if (iColl == 0) return fEcalCollection;
  if (fInf->GetFastMC()==1) return NULL;
  if (iColl == 1) return fLiteCollection;
  else return NULL;
}
// -------------------------------------------------------------------------

// -----   Public method Reset   -------------------------------------------
void CbmEcalDetailed::Reset()
{
  fEcalCollection->Clear();
  if (fInf->GetFastMC()==0)
    fLiteCollection->Clear();
  ResetParameters();
  fFirstNumber=0;
}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void CbmEcalDetailed::Print() const 
{
  Int_t nHits = fEcalCollection->GetEntriesFast();
  Int_t nLiteHits;
  Int_t i;

  cout << "-I- CbmEcalDetailed: " << nHits << " points registered in this event.";
  cout << endl;

  if (fInf->GetFastMC()==0) {
    nLiteHits = fLiteCollection->GetEntriesFast();
    cout << "-I- CbmEcalDetailed: " << nLiteHits << " lite points registered in this event.";
    cout << endl;
  }

  if (fVerboseLevel>1) {
    for (i=0; i<nHits; i++) (*fEcalCollection)[i]->Print();
    if (fInf->GetFastMC()==0)
      for (i=0; i<nLiteHits; i++) (*fLiteCollection)[i]->Print();
  }
}
// -------------------------------------------------------------------------

// -----   Public method CopyClones   --------------------------------------
void CbmEcalDetailed::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset)
{   
  Int_t nEntries = cl1->GetEntriesFast();
  Int_t i;
  Int_t index;
  cout << "-I- CbmEcalDetailed: " << nEntries << " entries to add." << endl;
  TClonesArray& clref = *cl2;
  if (cl1->GetClass()==CbmEcalPoint::Class()) {
    CbmEcalPoint* oldpoint = NULL;
    for (i=0; i<nEntries; i++) {
      oldpoint = (CbmEcalPoint*) cl1->At(i);
      index = oldpoint->GetTrackID()+offset;
      oldpoint->SetTrackID(index);
      new (clref[fPosIndex]) CbmEcalPoint(*oldpoint);
      fPosIndex++;
    }
    cout << "-I- CbmEcalDetailed: " << cl2->GetEntriesFast() << " merged entries."
         << endl;
  }
  else if (cl1->GetClass()==CbmEcalPointLite::Class()) {
    CbmEcalPointLite* oldpoint = NULL;
    for (i=0; i<nEntries; i++) {
      oldpoint = (CbmEcalPointLite*) cl1->At(i);
      index = oldpoint->GetTrackID()+offset;
      oldpoint->SetTrackID(index);
      new (clref[fPosIndex]) CbmEcalPointLite(*oldpoint);
      fPosIndex++;
    }
    cout << "-I- CbmEcalDetailed: " << cl2->GetEntriesFast() << " merged entries."
         << endl;
  }
}
// -------------------------------------------------------------------------

// -----   Public method Register   ----------------------------------------
void CbmEcalDetailed::Register()
{
  FairRootManager::Instance()->Register("ECALPoint","Ecal",fEcalCollection,kTRUE);
  if (fInf->GetFastMC()==0)
    FairRootManager::Instance()->Register("ECALPointLite","EcalLite",fLiteCollection,kTRUE);
}
// -------------------------------------------------------------------------

// -----   Public method ConstructGeometry   -------------------------------
void CbmEcalDetailed::ConstructGeometry()
{
  FairGeoLoader*geoLoad = FairGeoLoader::Instance();
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  FairGeoMedia *Media =  geoFace->getMedia();
  FairGeoBuilder *geobuild=geoLoad->getGeoBuilder();

  TGeoVolume *volume;
  FairGeoMedium *CbmMedium;
  TGeoPgon *spl;

  Float_t *buf = 0;
  Int_t i;
  Double_t par[10];
  Float_t y;
  TString nm;
  Double_t thickness=fThicknessLead+fThicknessScin+fThicknessTyvk*2;
  Double_t moduleth=thickness*fNLayers+fThicknessPSlead+fThicknessPSscin+fEcalPSgap;
//  Float_t sumWeight;
//  Int_t i;

  // create SensVacuum which is defined in the media file

  /** Initialize all media **/
  InitMedia();
  par[0]=fEcalSize[0]/2.0+0.1;
  par[1]=fEcalSize[1]/2.0+0.1;
  par[2]=moduleth/2.0+0.1;
  volume=gGeoManager->Volume("Ecal", "BOX",  gGeoManager->GetMedium("SensVacuum")->GetId(), par, 3);
  gGeoManager->Node("Ecal", 1, "cave", 0.0,0.0, fZEcal+par[2]-0.05, 0, kTRUE, buf, 0);
  // An ugly way!!!
  // Need to make a two volumes for each calorimeter arm 
  AddSensitiveVolume(volume);
  fStructureId=volume->GetNumber();

  for(i=1;i<cMaxModuleType;i++)
    if (fModulesWithType[i]>0)
      ConstructModule(i);
  
  TGeoVolume* vol=new TGeoVolumeAssembly("EcalStructure");
  for(i=0;i<fYSize;i++)
  {
    cout << i << "	" << flush;
    volume=ConstructRaw(i);
    if (volume==NULL) 
    {
      cout << endl;
      continue;
    }
    nm=volume->GetName();
    y=(i-fYSize/2.0+0.5)*fModuleSize;
    cout << volume->GetName() << flush;
    gGeoManager->Node(nm.Data(), i+1, "EcalStructure", 0.0, y, 0.0, 0, kTRUE, buf, 0);
    cout << endl << flush;
  }
  gGeoManager->Node("EcalStructure", 1, "Ecal", 0.0, 0.0, 0.0, 0, kTRUE, buf, 0);
}
// -------------------------------------------------------------------------

// ----- Public method ConstructRaw ----------------------------------------
TGeoVolume* CbmEcalDetailed::ConstructRaw(Int_t num)
{
  Int_t i;
  list<pair<Int_t, TGeoVolume*> >::const_iterator p=fRawNumber.begin();
  pair<Int_t, TGeoVolume*> out;
  Float_t x;
  Float_t* buf=NULL;
  for(i=0;i<fXSize;i++)
    if ((Int_t)fInf->GetType(i, num)!=0) break;
  if (i==fXSize)
    return NULL;
  for(;p!=fRawNumber.end();++p)
  {
    for(i=0;i<fXSize;i++)
      if (fInf->GetType(i, num)!=fInf->GetType(i, (*p).first))
	break;
    if (i==fXSize)
      break;
  }
  if (p!=fRawNumber.end())
    return (*p).second;
  TString nm="ECALRaw"; nm+=num;
  TString md;
  TGeoVolume* vol=new TGeoVolumeAssembly(nm);
  for(i=0;i<fXSize;i++)
  {
    x=(i-fXSize/2.0+0.5)*fModuleSize;
    md="EcalModule"; md+=(Int_t)fInf->GetType(i, num);
    gGeoManager->Node(md.Data(),i+1, nm.Data(), x, 0.0, 0.0, 0, kTRUE, buf, 0);
  }

  out.first=num;
  out.second=vol;
  fRawNumber.push_back(out);
  return out.second;
}
// -------------------------------------------------------------------------


// ----- Public method BeginEvent  -----------------------------------------
void CbmEcalDetailed::BeginEvent()
{
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------

// -----   Private method AddHit   -----------------------------------------    
CbmEcalPoint* CbmEcalDetailed::AddHit(Int_t trackID, Int_t detID, TVector3 pos,         
			      TVector3 mom, Double_t time, Double_t length,       
			      Double_t eLoss)
{
  TClonesArray& clref = *fEcalCollection;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) CbmEcalPoint(trackID, detID, pos, mom,
                                      time, length, eLoss);
}                                                                               
// -------------------------------------------------------------------------

// -----   Private method AddHit   -----------------------------------------    
CbmEcalPointLite* CbmEcalDetailed::AddLiteHit(Int_t trackID, Int_t detID, Double32_t time, Double32_t eLoss)
{
  TClonesArray& clref = *fLiteCollection;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) CbmEcalPointLite(trackID, detID, time, eLoss);
}
// -------------------------------------------------------------------------

// -----   Private method ConstructModule ----------------------------------    
void CbmEcalDetailed::ConstructModule(Int_t type)
{
  if (fModules[type]!=NULL) return;
  ConstructCell(type);

  TString nm="EcalModule"; nm+=type;
  TString cellname="EcalCell"; cellname+=type;
  Int_t i;
  Int_t j;
  Int_t n;
  Float_t x;
  Float_t y;
  Float_t* buf=NULL;
  Double_t thickness=fThicknessLead+fThicknessScin+fThicknessTyvk*2;
  Double_t moduleth=thickness*fNLayers;
  Double_t par[3]={fModuleSize/2.0, fModuleSize/2.0, moduleth/2.0};
  if (fSteelTapes[0]==NULL)
  {
    TGeoBBox* st1=new TGeoBBox(fThicknessSteel/2.0, fModuleSize/2.0-fThicknessSteel, moduleth/2.0);
    fSteelTapes[0]=new TGeoVolume("EcalModuleSteelTape1", st1, gGeoManager->GetMedium("ECALSteel"));
  }
  if (fSteelTapes[1]==NULL)
  {
    TGeoBBox* st2=new TGeoBBox(fModuleSize/2.0-fThicknessSteel, fThicknessSteel/2.0, moduleth/2.0);
    fSteelTapes[1]=new TGeoVolume("EcalModuleSteelTape2", st2, gGeoManager->GetMedium("ECALSteel"));
  }


//  TGeoVolume* modulev=new TGeoVolumeAssembly(nm);
  TGeoVolume* modulev=gGeoManager->Volume(nm.Data(), "BOX",  gGeoManager->GetMedium("ECALAir")->GetId(), par, 3);

  //Adding cells into module
  for(i=0;i<type;i++)
  for(j=0;j<type;j++)
  {
    x=(i-type/2.0+0.5)*(fXCell[type]+2.0*fEdging);
    y=(j-type/2.0+0.5)*(fYCell[type]+2.0*fEdging);
    n=i+j*type+1;
    gGeoManager->Node(cellname.Data(), n, nm.Data(), x, y, 0.0, 0, kTRUE, buf, 0);
  }
  gGeoManager->Node("EcalModuleSteelTape1", 1, nm.Data(), -fThicknessSteel/2.0+fModuleSize/2.0, 0.0, 0.0, 0, kTRUE, buf, 0);
  gGeoManager->Node("EcalModuleSteelTape1", 2, nm.Data(), +fThicknessSteel/2.0-fModuleSize/2.0, 0.0, 0.0, 0, kTRUE, buf, 0);
  gGeoManager->Node("EcalModuleSteelTape2", 1, nm.Data(), 0.0, -fThicknessSteel/2.0+fModuleSize/2.0, 0.0, 0, kTRUE, buf, 0);
  gGeoManager->Node("EcalModuleSteelTape2", 2, nm.Data(), 0.0, +fThicknessSteel/2.0-fModuleSize/2.0, 0.0, 0, kTRUE, buf, 0);
  fModuleLenght=moduleth;
}
// -------------------------------------------------------------------------

// -----   Private method ConstructCell ------------------------------------    
void CbmEcalDetailed::ConstructCell(Int_t type)
{
  if (fCells[type]!=NULL) return;

  ConstructTile(type, 0);
  ConstructTile(type, 1);
  if (fThicknessTyvk>0)
    ConstructTile(type, 2);

  Double_t thickness=fThicknessLead+fThicknessScin+fThicknessTyvk*2;
  Int_t i;
  TString nm="EcalCell"; nm+=type;
  TString scin="ScTile"; scin+=type; scin+="_edging";
  TString lead="LeadTile"; lead+=type;
  TString tyvek="TvTile"; tyvek+=type;
  Double_t* buf=NULL;
  Double_t moduleth=thickness*fNLayers;
  Double_t par[3]={fXCell[type]/2.0+fEdging, fYCell[type]/2.0+fEdging, moduleth/2.0};
//  TGeoVolume* cellv=new TGeoVolumeAssembly(nm);
  TGeoVolume* cellv=gGeoManager->Volume(nm.Data(),"BOX", gGeoManager->GetMedium("ECALAir")->GetId(), par, 3);
  for(i=0;i<fNLayers;i++)
  {
    gGeoManager->Node(scin.Data(), i+1, nm.Data(), 0.0, 0.0, -thickness*fNLayers/2.0+fThicknessScin/2.0+i*thickness, 0, kTRUE, buf, 0);
    gGeoManager->Node(lead.Data(), i+1, nm.Data(), 0.0, 0.0, -thickness*fNLayers/2.0+fThicknessScin+i*thickness+fThicknessTyvk+fThicknessLead/2.0, 0, kTRUE, buf, 0);
    gGeoManager->Node(tyvek.Data(), 2*i+1, nm.Data(), 0.0, 0.0, -thickness*fNLayers/2.0+fThicknessScin+i*thickness+1.5*fThicknessTyvk+fThicknessLead, 0, kTRUE, buf, 0);
    gGeoManager->Node(tyvek.Data(), 2*i+2, nm.Data(), 0.0, 0.0, -thickness*fNLayers/2.0+fThicknessScin+i*thickness+0.5*fThicknessTyvk, 0, kTRUE, buf, 0);
  }
  fCells[type]=cellv;
}
// -------------------------------------------------------------------------

// -----   Private method InitMedium ---------------------------------------    
Int_t CbmEcalDetailed::InitMedium(const char* name)
{
  static FairGeoLoader *geoLoad=FairGeoLoader::Instance();
  static FairGeoInterface *geoFace=geoLoad->getGeoInterface();
  static FairGeoMedia *media=geoFace->getMedia();
  static FairGeoBuilder *geoBuild=geoLoad->getGeoBuilder();

  FairGeoMedium *CbmMedium=media->getMedium(name);

  if (!CbmMedium)
  {
    Fatal("InitMedium","Material %d not defined in media file.", name);
    return -1111;
  }
  return geoBuild->createMedium(CbmMedium);
}
// -------------------------------------------------------------------------

// -----   Private method InitMedia ----------------------------------------    
void CbmEcalDetailed::InitMedia()
{
  Info("InitMedia", "Initializing media.");
  InitMedium("SensVacuum");
  InitMedium("ECALVacuum");
  InitMedium("Lead");
  InitMedium("Scintillator");
  InitMedium("Tyvek");
  InitMedium("ECALAir");
  InitMedium("ECALFiber");
  InitMedium("ECALSteel");
  InitMedium("ECALTileEdging");
}
// -------------------------------------------------------------------------

// -----   Private method ConstructTile ------------------------------------    
void CbmEcalDetailed::ConstructTile(Int_t type, Int_t material)
{
  switch (material)
  {
    case 0: if (fScTiles[type]!=NULL) return; break;
    case 1: if (fPbTiles[type]!=NULL) return; break;
    case 2: if (fTvTiles[type]!=NULL) return; break;
    default: Error("ConstructTile", "Can't construct a tile of type %d.", material);
  }
  Double_t thickness;
  TGeoVolume* hole;
  TGeoVolume* fiber;
  TGeoTranslation** tr;
  TGeoTranslation* tm;
  Int_t nh=fNH[type];
  Int_t i;
  Int_t j;
  TString nm;
  TString medium;
  Double_t x;
  Double_t y;
  TGeoBBox* tile;
  TGeoVolume* tilev;
  TGeoBBox* edging;
  TGeoVolume* edgingv;

  switch (material)
  {
    case 0: thickness=fThicknessScin/2.0; break;
    case 1: thickness=fThicknessLead/2.0; break;
    case 2: thickness=fThicknessTyvk/2.0; break;
    default: Error("ConstructTile", "Can't construct a tile of type %d.", material);
  }

  // Holes in the tiles
  if (fHoleRad>0)
  {
    if (fHoleVol[material]==NULL)
    {
      TGeoTube* holetube=new TGeoTube(0, fHoleRad, thickness);
      fHoleVol[material]=new TGeoVolume("ECALHole", holetube,  gGeoManager->GetMedium("ECALAir"));
    }
    hole=fHoleVol[material];
    // Fibers in holes 
    if (fFiberRad>0)
    {
      if (fFiberVol[material]==NULL)
      {
        TGeoTube* fibertube=new TGeoTube(0, fFiberRad, thickness);
        fFiberVol[material]=new TGeoVolume("ECALFiber", fibertube,  gGeoManager->GetMedium("ECALFiber"));
        hole->AddNode(fiber, 1);
      }
      fiber=fFiberVol[material];
      // TODO: Cerenkoff !!!
      //AddSensitiveVolume(fiber);
    }
  }

  if (fHolePos[type]==NULL)
  {
    tr=new TGeoTranslation*[nh*nh];
    for(i=0;i<nh;i++)
    for(j=0;j<nh;j++)
    {
      nm="sh"; nm+=type; nm+="_"; nm+=j*nh+i;
      x=(i-nh/2+0.5)*fXCell[type]/nh;
      y=(j-nh/2+0.5)*fYCell[type]/nh;


      tm=new TGeoTranslation(nm, x, y, 0);
      gGeoManager->AddTransformation(tm);
      tr[j*nh+i]=tm;
    }
    fHolePos[type]=tr;
  }
  tr=fHolePos[type];

  /** Building tile **/
  switch (material)
  {
    case 0: nm="ScTile"; medium="Scintillator"; break;
    case 1: nm="LeadTile"; medium="Lead"; break;
    case 2: nm="TvTile"; medium="Tyvek"; break;
    default: Error("ConstructTile", "Can't construct a tile of type %d.", material);
  }

  nm+=type;
  if (material==0)
    tile=new TGeoBBox(fXCell[type]/2.0, fYCell[type]/2.0, thickness);
  else
    tile=new TGeoBBox(fXCell[type]/2.0+fEdging, fYCell[type]/2.0+fEdging, thickness);
  tilev=new TGeoVolume(nm, tile, gGeoManager->GetMedium(medium));
  if (fHoleRad>0)
  {
    for(i=0;i<nh;i++)
    for(j=0;j<nh;j++)
      tilev->AddNode(hole, j*nh+i+1, tr[j*nh+i]);
    /** Clear Fiber**/
    if (nh%2==0)
      tilev->AddNode(hole, j*nh+i+1);
  }
  /** Adding edging to scintillator **/
  if (material==0)
  {
    AddSensitiveVolume(tilev);
    edging=new TGeoBBox(fXCell[type]/2.0+fEdging, fYCell[type]/2.0+fEdging, thickness);
     
    edgingv=new TGeoVolume(nm+"_edging", edging, gGeoManager->GetMedium("ECALTileEdging"));
    edgingv->AddNode(tilev, 1);
    fScTiles[cMaxModuleType]=tilev;
    fTileEdging[cMaxModuleType]=edgingv;
  }
  else
  {
    if (material==1) //Lead
      fPbTiles[type]=tilev;
    else
      fTvTiles[type]=tilev;
    return;
  }
}
// -------------------------------------------------------------------------

// ----- Public method GetCellCoordInf ----------------------------------------
Bool_t CbmEcalDetailed::GetCellCoordInf(Int_t fVolID, Float_t &x, Float_t &y, Int_t& tenergy)
{
  static CbmEcalInf* inf=NULL;
  if (inf==NULL)
  {
    inf=CbmEcalInf::GetInstance(NULL);
    if (inf==NULL)
    {
      cerr << "CbmEcalDetailed::GetCellCoordInf(): Can't get geometry information." << endl;
      return kFALSE;
    }
  }
  Int_t volid=fVolID;
  Int_t cell=volid%100-1; volid=volid-cell-1; volid/=100;
  Int_t mx=volid%100; volid-=mx; volid/=100;
  Int_t my=volid%100; volid-=my; volid/=100;
  Int_t type=inf->GetType(mx, my);
  Int_t cx=cell%type;
  Int_t cy=cell/type;
  static Float_t modulesize=inf->GetVariableStrict("modulesize");
  static Float_t xcalosize=inf->GetEcalSize(0);
  static Float_t ycalosize=inf->GetEcalSize(1);
  x=mx*modulesize-xcalosize/2.0+cx*modulesize/type+1.0;
  y=my*modulesize-ycalosize/2.0+cy*modulesize/type+1.0;
  tenergy=0;

  return kFALSE;
}

// ------------------------------------------------------------------------------

Bool_t CbmEcalDetailed::GetCellCoord(Int_t fVolID, Float_t &x, Float_t &y, Int_t& tenergy)
{
  return GetCellCoordInf(fVolID, x, y, tenergy);
}


ClassImp(CbmEcalDetailed)
