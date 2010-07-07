// --------------------------------------------------------------------------
// -----                 Class CbmTofHitProducer                       ------
// -----           Created  by E. Cordier 14/09/05                     ------
// -----           Modified by D. Gonzalez-Diaz 07/09/06               ------
// -----           Modified by D. Gonzalez-Diaz 02/02/07               ------
// --------------------------------------------------------------------------

#include "CbmTofHitProducer.h"

#include "CbmTofPoint.h"
#include "CbmTofHit.h"
#include "CbmMCTrack.h"

#include "FairRootManager.h"

#include "TRandom.h"
#include "TString.h"
#include "TVector3.h"
#include "TSystem.h"
#include "TClonesArray.h"

#include <iostream>

using std::cout;
using std::endl;

// ---- Default constructor -------------------------------------------

CbmTofHitProducer::CbmTofHitProducer()
{
    fVerbose = 1;
    fSigmaT  = 0.;
    fSigmaXY = 0.;
    fSigmaY  = 0.;
    fSigmaZ  = 0.;
    fNHits   = -1;
}


// ---- Constructor ----------------------------------------------------

CbmTofHitProducer::CbmTofHitProducer(const char *name, Int_t verbose)
:FairTask(name,verbose)
{
    fVerbose =  verbose;
    fSigmaT  =  0.;
    fSigmaXY =  0.;
    fSigmaY  =  0.;
    fSigmaZ  =  0.;
    fNHits   = -1;
}


// ---- Destructor ----------------------------------------------------

CbmTofHitProducer::~CbmTofHitProducer()
{
//	FairRootManager *fManager =FairRootManager::Instance();
//	fManager->Write();
}


// ---- Init ----------------------------------------------------------

InitStatus CbmTofHitProducer::Init()
{
  FairRootManager *fManager = FairRootManager::Instance();

    fTofPoints  = (TClonesArray *) fManager->ActivateBranch("TofPoint");
    fMCTracks   = (TClonesArray *) fManager->ActivateBranch("MCTrack");
    
    FILE *par;

    //Reading the parameter file. In the future this must be done in a different way.
 
    char header='#', cell_type='#';
    int region, module, cell;
    Int_t   nregions=10, nmodules=500, ncells=500;
    Float_t X_tmp, Y_tmp, Dx_tmp, Dy_tmp;
    
    //    Initialize the matrixes [make this index visible in all the macro]. FIXME

    for(int i=0;i<nregions;i++){
      for(int j=0;j<nmodules;j++){
	for(int k=0;k<ncells;k++){
	  X[i][j][k] = -1;
	  Y[i][j][k] = -1;
	  Dx[i][j][k]= -1;
	  Dy[i][j][k]= -1;
	}
      }
    }

      TString tofGeoFile = gSystem->Getenv("VMCWORKDIR");
      tofGeoFile += "/parameters/tof/tof_standard.geom.par";
      par=fopen(tofGeoFile,"r");
   
      if(par==NULL) {
      printf("\n ERROR WHILE OPENING THE PARAMETER FILE IN TOF HIT PRODUCER!");
      return kFATAL;
    }
     
    //Skip the header. In the future the header structure must be defined. FIXME

    while (fscanf(par,"%c",&header)>=0){
      if((int)(header-'0')==0) break;
    }

    //Read the first line

    region=0;
    fscanf(par,"%d%d%s%f%f%f%f", &module, &cell, &cell_type, &X_tmp, &Y_tmp, &Dx_tmp, &Dy_tmp);
    X[region][module][cell]    = X_tmp;
    Y[region][module][cell]    = Y_tmp;
    Dx[region][module][cell]   = Dx_tmp;
    Dy[region][module][cell]   = Dy_tmp;
    type[region][module][cell] = cell_type;
    
    //Read all the lines
     
    while(fscanf(par,"%d%d%d%s%f%f%f%f", &region, &module, &cell, &cell_type, &X_tmp, &Y_tmp, &Dx_tmp, &Dy_tmp)>=0){
      X[region][module][cell]    = X_tmp;
      Y[region][module][cell]    = Y_tmp;
      Dx[region][module][cell]   = Dx_tmp;
      Dy[region][module][cell]   = Dy_tmp;
      type[region][module][cell] = cell_type;
    }

    fclose(par);

    fHitCollection = new TClonesArray("CbmTofHit");
    fManager->Register("TofHit","Tof",fHitCollection, kTRUE);
     
    cout << "-I- CbmTofHitProducer: Intialization successfull" << endl;

    return kSUCCESS;
}

// ---- Exec ----------------------------------------------------------

void CbmTofHitProducer::Exec(Option_t * option)
{

  fHitCollection->Clear();
  fNHits = -1;              //Must start in -1

  CbmTofPoint *pt;
  CbmMCTrack  *mc;
  
  Int_t nTofPoint = fTofPoints->GetEntries();  
  Int_t nMCTracks = fMCTracks ->GetEntries();
  Int_t tof_tracks = 0, tof_tracks_vert = 0, tof_tracks_local = 0;

  cout << "-I- CbmTofHitProducer : " << nTofPoint
       << " points in Tof for this event" << endl;

  //Some numbers on TOF distributions

  for(Int_t p=0;p<nMCTracks;p++) {
    mc = (CbmMCTrack*) fMCTracks->At(p);
    if(mc->GetNPoints(kTOF)>0) tof_tracks++;
    if(mc->GetNPoints(kTOF)>0 && mc->GetStartZ()>996) tof_tracks_local++;
    if(mc->GetNPoints(kTOF)>0 && mc->GetMotherId()==-1) tof_tracks_vert++;
  }
  
  cout << "-I- CbmTofHitProducer : " << tof_tracks
       << " tracks in Tof " << endl;

  cout << "-I- CbmTofHitProducer : " << tof_tracks_vert
       << " tracks in Tof from vertex" << endl;

  cout << "-I- CbmTofHitProducer : " << tof_tracks_local
       << " tracks in Tof produced locally (Z>996 cm)" << endl;

  cout << "-I- CbmTofHitProducer : " << tof_tracks-tof_tracks_local
       << " tracks in Tof able to produce a hit" << endl;



  TVector3 pos;

  Int_t   nregions=10, nmodules=500, ncells=500, ngaps=8; 
  //FIXME: these parameters must be provided externally
  Double_t xHit, yHit, zHit, tHit;
  Double_t xHitErr, yHitErr, zHitErr;
  Double_t tl_new, tr_new;
  Double_t Z=1000, Dz=2.04;  //FIXME: Introduce also Dz and Z as (constant) parameters 
  Float_t sigma_T=0.08, sigma_Y=0.7, sigma_t_gap, t_o, T_smearing = 0, sigma_el, 
    vprop = 20, Pgap = 0.75;
  //time[ns], position[cm], velocity[cm/ns]
  //FIXME: these parameters must be provided externally

  Int_t trackID, region, module, cell, flag, ref;
  
  //Here check for the validity of the parameters

  if(fSigmaY>1) cout<<"IRREALISTIC TOF POSITION RESOLUTION!! (HitProducer may crash)"<<endl;
  if((fSigmaT<0.07 && fSigmaT>0)||fSigmaT>0.2) cout<<"IRREALISTIC TOF RESOLUTION!! (HitProducer may crash)"<<endl;

  //Parameterizations. Now they depend on the geometry/algorithm. FIXME

  if(fSigmaY!=0)    sigma_el = sqrt(2.)*fSigmaY/vprop*1.3;
  else              sigma_el = sqrt(2.)*sigma_Y/vprop*1.3;

  if(fSigmaT!=0)  {  
    sigma_t_gap = sqrt(pow(fSigmaT/(0.5*pow(ngaps,-0.361) + 0.40),2)-1.4/1.3*pow(sigma_el,2));
    t_o = 1.93*fSigmaT;
  }
  else {
    sigma_t_gap = sqrt(pow(sigma_T/(0.5*pow(ngaps,-0.361) + 0.40),2)-1.4/1.3*pow(sigma_el,2));
    t_o = 1.93*sigma_T;
  }

  //Initialization of cell times

  for(Int_t i=0;i<nregions;i++){
    for(Int_t j=0;j<nmodules;j++){
      for(Int_t k=0;k<ncells;k++){
	tl[i][j][k]= 1e+6;
	tr[i][j][k]= 1e+6;
      }
    }
  }


  // Loop over the TOF points

  for (Int_t j=0; j < nTofPoint; j++ ) {

    // Probability that the avalanche is detected

    if(gRandom->Uniform(1)>Pgap)     continue;
    
    // Get a pointer to the TOF point
      
    pt = (CbmTofPoint*) fTofPoints->At(j);
      
    if(pt == NULL) {
      cout<<"Be careful: hole in the CbmTofPoint TClonesArray!"<<endl;
      continue;
    }

    //Reject particles produced in the last 4 cm. Better job must be done here. For example:
    //it could better to go up to the parent particle and get its trackID, then the
    //secondary is processed. FIXME.

    if(pt->GetTrackID() < 0) continue;
    mc = (CbmMCTrack*) fMCTracks-> At(pt->GetTrackID());
    
    if((mc->GetStartZ())>996) continue; 
      
    //Get relevant information from the point

    trackID = pt->GetTrackID();
    cell    = pt->GetCell();
    module  = pt->GetModule();
    region  = pt->GetRegion();
    pt->Position(pos);

    if(fVerbose > 2) {
      pt->Print(""); //FIXME
      cout << endl;
    }

    T_smearing      = gRandom->Gaus(t_o, sigma_t_gap);

    tl_new = pt->GetTime() + T_smearing - pos.Y()/vprop + gRandom->Gaus(0,sigma_el);   
    tr_new = pt->GetTime() + T_smearing + pos.Y()/vprop + gRandom->Gaus(0,sigma_el);   

    flag = 1; 

    //Increase the counter for the TofHit TClonesArray if the first time a hit is attached to this cell
    
    if(tl[region][module][cell]==1e+6 && tr[region][module][cell]==1e+6)	fNHits++; 
    else {
      if(trackID_left[region][module][cell]!=trackID || trackID_right[region][module][cell]!=trackID) {
	flag   = 2;
	//Worsening of the time response in the presence of 2 (or more) hits is not possible to implement
	//within the present algorith (taking the fastest hit). FIXME
      }
    }

    //Take the fastest time from all the points/gaps in this cell
    
    if(tl_new<tl[region][module][cell]) {
      tl[region][module][cell]             = tl_new;
      trackID_left[region][module][cell]   = trackID;
      point_left[region][module][cell]     = j;
    }

    if(tr_new<tr[region][module][cell]) {
      tr[region][module][cell]             = tr_new;
      trackID_right[region][module][cell]  = trackID;
      point_right[region][module][cell]    = j;
    }

    //X and Y depend on the orientation of the cell. FIXME

    //Parameters of the Hit. Different for pad or strip read-out.

    if (type[region][module][cell]=="s") {
      yHit    = (tr[region][module][cell] - tl[region][module][cell])/2*vprop;
      yHitErr = sigma_el/sqrt(2.)*vprop/1.3; //This must be better done. 
      //1.3 comes from a relatin lines above. FIXME 
      tHit    = (tl[region][module][cell] + tr[region][module][cell])/2;
    }
    else if (type[region][module][cell]=="p") {
      yHit    =  Dy[region][module][cell]/2/10 + Y[region][module][cell]/10;
      yHitErr =  Dy[region][module][cell]/10/sqrt(12.);
      tHit    =  tl[region][module][cell] + pos.Y()/vprop;
    }

    xHit    = Dx[region][module][cell]/2/10 + X[region][module][cell]/10; 
    xHitErr = Dx[region][module][cell]/10/sqrt(12.);
    zHit    = Dz/2 + Z;
    zHitErr = Dz/sqrt(12.);

    //Reference to the point that contributes to the left side.

    ref     = point_left[region][module][cell];

    //Reference to the point that contributes to the right side.

    if(type[region][module][cell]=="s" &&
       (trackID_left[region][module][cell]!=trackID_right[region][module][cell])) {
      flag = 10 + point_right[region][module][cell]; 
      //More than one TRACK contributed to this Hit: store the reference as flag + an offset of 10.
    }

    TVector3 hitPos(xHit, yHit, zHit);
    TVector3 hitPosErr(xHitErr, yHitErr, zHitErr);
    AddHit(pt->GetDetectorID(), hitPos, hitPosErr,
	     j, tHit, flag);
  }
  
  cout << "-I- CbmTofHitProducer : " << fNHits
       << " hits in Tof created" << endl;
}


// ---- Add Hit to HitCollection --------------------------------------

void CbmTofHitProducer::AddHit(Int_t detID, TVector3 &posHit, TVector3 &posHitErr,
			       Int_t ref, Double_t tHit, Int_t flag)
{
  new((*fHitCollection)[fNHits]) CbmTofHit(detID, posHit, posHitErr,
					     ref, tHit, flag);
  if(fVerbose > 1) {
    CbmTofHit* tofHit = (CbmTofHit*) fHitCollection->At(fNHits);
    tofHit->Print();
    cout << endl;
  }
}


// ---- Finish --------------------------------------------------------

void CbmTofHitProducer::Finish()
{
}


// ---- SetSigmaT -----------------------------------------------------

void CbmTofHitProducer::SetSigmaT(Double_t sigma)
{
    fSigmaT = sigma;
}


// ---- SetSigmaXY -----------------------------------------------------

void CbmTofHitProducer::SetSigmaXY(Double_t sigma)
{
    fSigmaXY = sigma;
}

// ---- SetSigmaY -----------------------------------------------------

void CbmTofHitProducer::SetSigmaY(Double_t sigma)
{
    fSigmaY = sigma;
}


// ---- SetSigmaZ -----------------------------------------------------

void CbmTofHitProducer::SetSigmaZ(Double_t sigma)
{
    fSigmaZ = sigma;
}


// ---- GetSigmaXY -----------------------------------------------------

Double_t CbmTofHitProducer::GetSigmaT()
{  
    return  fSigmaT;
}


// ---- GetSigmaXY -----------------------------------------------------

Double_t CbmTofHitProducer::GetSigmaXY()
{  
    return  fSigmaXY;
}

// ---- GetSigmaY -----------------------------------------------------

Double_t CbmTofHitProducer::GetSigmaY()
{  
    return  fSigmaY;
}

// ---- GetSigmaZ -----------------------------------------------------

Double_t CbmTofHitProducer::GetSigmaZ()
{  
    return  fSigmaZ;
}


ClassImp(CbmTofHitProducer)


