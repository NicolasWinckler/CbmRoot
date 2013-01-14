// --------------------------------------------------------------------------
// -----                 Class CbmTofHitProducer                       ------
// -----           Created  by E. Cordier 14/09/05                     ------
// -----           Modified by D. Gonzalez-Diaz 07/09/06               ------
// -----           Modified by D. Gonzalez-Diaz 02/02/07               ------
// -----           modified nh 24/10/2012
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
    fParFileName="";
}


// ---- Constructor ----------------------------------------------------

CbmTofHitProducer::CbmTofHitProducer(const char *name, Int_t verbose)
  :FairTask(TString(name),verbose)
   //  :FairTask()
{
    fVerbose =  verbose;
    fSigmaT  =  0.;
    fSigmaXY =  0.;
    fSigmaY  =  0.;
    fSigmaZ  =  0.;
    fNHits   = -1;
    fParFileName="";
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

  cout << "nh - version of CbmTofHitProducer initializing with file " << fParFileName << endl;
  FairRootManager *fManager = FairRootManager::Instance();

    fTofPoints  = (TClonesArray *) fManager->GetObject("TofPoint");
    fMCTracks   = (TClonesArray *) fManager->GetObject("MCTrack");
    
    FILE *par;

    //Reading the parameter file. In the future this must be done in a different way.
 
    char    header='#';
    int     module, cell, smodule, smtype;
    Int_t   nsmtyp=10, nsm=255, nmodules=10, ncells=255;
    Float_t X_tmp, Y_tmp, Z_tmp, Dx_tmp, Dy_tmp;
    
    //    Initialize the matrixes [make this index visible in all the macro]. FIXME

    for(int t=0;t<nsmtyp;t++){
     for(int i=0;i<nsm;i++){
      for(int j=0;j<nmodules;j++){
	for(int k=0;k<ncells;k++){
	  X[t][i][j][k] = -1;
	  Y[t][i][j][k] = -1;
	  Z[t][i][j][k] = -1;
	  Dx[t][i][j][k]= -1;
	  Dy[t][i][j][k]= -1;
	  Ch[t][i][j][k]= -1;
	}
      }
     }
    }

      TString tofGeoFile = gSystem->Getenv("VMCWORKDIR");
//      tofGeoFile += "/parameters/tof/tof_standard.geom.par";
      if (fParFileName==""){
	tofGeoFile += "/parameters/tof/par_tof.txt";
      }else{
	tofGeoFile += fParFileName;
      }
      cout << "<I> CbmTofHitProducer::Read parameters from "<< fParFileName 
           << "," <<tofGeoFile << endl;
      par=fopen(tofGeoFile,"r");
   
      if(par==NULL) {
      printf("\n ERROR WHILE OPENING THE PARAMETER FILE IN TOF HIT PRODUCER!");
      return kFATAL;
    }
     
// initialize accounting variables    
    ActSMtypMax=0;
    for (Int_t i=0; i<=maxSMtyp; i++){
	ActnSMMax[i]=0;
	ActnModMax[i]=0;
	ActnCellMax[i]=0;
    }
    Int_t iCh=0; // channel identifier 

    //Skip the header. In the future the header structure must be defined. FIXME
    while (fscanf(par,"%c",&header)>=0){
//      cout << "ReadH "<< header << endl;
      if((int)(header-'0')==0) break;
    }

    //Read the first line
//    fscanf(par,"%5d %5d %5d %2d %5.1f %5.1f %5.1f %2.1f %2.1f ", 
    fscanf(par,"%d %d %d %d %f %f %f %f %f ", 
    &smodule, &module, &cell, &smtype, &X_tmp, &Y_tmp, &Z_tmp, &Dx_tmp, &Dy_tmp);
//    cout << "Read " << smodule << " mod " << module << " cell "<< cell << " SMtyp " <<  smtype << " X,Y " << X_tmp <<","<< Y_tmp <<endl;
    if(smtype>ActSMtypMax)             ActSMtypMax=smtype;
    if(smodule>ActnSMMax[ActSMtypMax]) ActnSMMax[ActSMtypMax]=smodule;
    if(module>ActnModMax[ActSMtypMax]) ActnModMax[ActSMtypMax]=module;
    if(cell>ActnCellMax[ActSMtypMax])  ActnCellMax[ActSMtypMax]=cell;
    
    X[smtype][smodule][module][cell]    = X_tmp/10.;
    Y[smtype][smodule][module][cell]    = Y_tmp/10.;
    Z[smtype][smodule][module][cell]    = Z_tmp/10.;
    Dx[smtype][smodule][module][cell]   = Dx_tmp/10.;
    Dy[smtype][smodule][module][cell]   = Dy_tmp/10.;
    Ch[smtype][smodule][module][cell]   = iCh++;
    
    //Read all the lines
     
    while(fscanf(par,"%5d %5d %5d %2d %f %f %f %f %f", 
      &smodule, &module, &cell, &smtype, &X_tmp, &Y_tmp, &Z_tmp, &Dx_tmp, &Dy_tmp)>=0){ 
//      cout << "Read " << smodule << " mod " << module << " cell "<< cell << " SMtyp " <<  smtype << " X,Y " << X_tmp <<","<< Y_tmp <<endl;
  
      if(smtype>ActSMtypMax)             ActSMtypMax=smtype;
      if(smodule>ActnSMMax[ActSMtypMax]) ActnSMMax[ActSMtypMax]=smodule;
      if(module>ActnModMax[ActSMtypMax]) ActnModMax[ActSMtypMax]=module;
      if(cell>ActnCellMax[ActSMtypMax])  ActnCellMax[ActSMtypMax]=cell;

      X[smtype][smodule][module][cell]     = X_tmp/10.;  // units are cm 
      Y[smtype][smodule][module][cell]     = Y_tmp/10.;
      Z[smtype][smodule][module][cell]     = Z_tmp/10.;
      Dx[smtype][smodule][module][cell]    = Dx_tmp/10.;
      Dy[smtype][smodule][module][cell]    = Dy_tmp/10.;      
      Ch[smtype][smodule][module][cell]    = iCh++;
    }

    fclose(par);
    cout << "Filled position array with ActSMtypMax " << ActSMtypMax 
         << " and " << iCh << " active Channels "<< endl;

    for (Int_t i=1; i<=ActSMtypMax; i++){
	cout << " SMtype " << i <<" nsmod " << ActnSMMax[i] 
             << " nmod "<< ActnModMax[i]  << " ncell " << ActnCellMax[i] << endl;
    }
    
    fHitCollection = new TClonesArray("CbmTofHit");
    fManager->Register("TofHit","Tof",fHitCollection, kTRUE);
     
    cout << "-I- CbmTofHitProducer: Intialization successful" << endl;

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

  cout << "-I- CbmTofHitProducer(Exec): " << nTofPoint
       << " points in Tof for this event with " << nMCTracks
       << " MC tracks "<< endl;

  //Some numbers on TOF distributions

  for(Int_t p=0;p<nMCTracks;p++) {
    mc = (CbmMCTrack*) fMCTracks->At(p);
    if(mc->GetNPoints(kTOF)>0) tof_tracks++;
    if(mc->GetNPoints(kTOF)>0 && mc->GetMotherId()==-1) tof_tracks_vert++;
  }
  
  cout << "-I- CbmTofHitProducer : " << tof_tracks << " tracks in Tof " << endl;
  cout << "-I- CbmTofHitProducer : " << tof_tracks_vert << " tracks in Tof from vertex" << endl;
//  cout << "-I- CbmTofHitProducer : " << tof_tracks-tof_tracks_local
//       << " tracks in Tof able to produce a hit" << endl;

  TVector3 pos;

  Int_t    ngaps=8; //FIXME: these parameters must be provided externally
  Double_t xHit, yHit, zHit, tHit, xHitErr, yHitErr, zHitErr;
  Double_t tl_new, tr_new;
  Double_t Dz=2.04;  //FIXME: Introduce also Dz and Z as (constant) parameters 
  Float_t sigma_T=0.07, sigma_Y=0.7, sigma_t_gap, t_o, T_smearing = 0, sigma_el=0.07, 
    vprop = 15., Pgap = 0.75;
  //time[ns], position[cm], velocity[cm/ns]
  //FIXME: these parameters must be provided externally

  Int_t trackID, smtype, smodule, module, cell, gap, flag, ref;
  
  //Here check for the validity of the parameters
  if(fSigmaY>1) cout<<"IRREALISTIC TOF POSITION RESOLUTION!! (HitProducer may crash)"<<endl;
  if((fSigmaT<0.07 && fSigmaT>0)||fSigmaT>0.2) cout<<"IRREALISTIC TOF RESOLUTION!! (HitProducer may crash)"<<endl;

  //Parameterizations. Now they depend on the geometry/algorithm. FIXME

//  if(fSigmaY!=0)    sigma_el = sqrt(2.)*fSigmaY/vprop*1.3;
//  else              sigma_el = sqrt(2.)*sigma_Y/vprop*1.3;

//  if(fSigmaT!=0)  {  
//    sigma_t_gap = sqrt(pow(fSigmaT/(0.5*pow(ngaps,-0.361) + 0.40),2)-1.4/1.3*pow(sigma_el,2));
//    t_o = 1.93*fSigmaT;
//  }
//  else {
//    sigma_t_gap = sqrt(pow(sigma_T/(0.5*pow(ngaps,-0.361) + 0.40),2)-1.4/1.3*pow(sigma_el,2));
//    t_o = 1.93*sigma_T;
//  }

  //Initialization of cell times
  for(Int_t t=1;t<=ActSMtypMax;t++){
   for(Int_t i=1;i<=ActnSMMax[t];i++){
    for(Int_t j=1;j<=ActnModMax[t];j++){
     for(Int_t k=0;k<ActnCellMax[t];k++){
      tl[t][i][j][k]= 1e+6;
      tr[t][i][j][k]= 1e+6;
     }
    }
   }
  }

// Loop over the TOF points
//  nTofPoint=1; //debugging 
  
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
    //if(pt->GetTrackID() < 0) continue;
    mc = (CbmMCTrack*) fMCTracks-> At(pt->GetTrackID()); // pointer to MC - info
    //if((mc->GetStartZ())>996) continue;
    //Get relevant information from the point

    trackID = pt->GetTrackID();
    gap     = pt->GetGap();
    cell    = pt->GetCell();
    module  = pt->GetModule();
    smodule = pt->GetSModule();
    smtype  = pt->GetSMtype();
    pt->Position(pos);

    if(fVerbose >2) {
      pt->Print(""); //FIXME
      cout << endl;
    }

//    T_smearing      = gRandom->Gaus(t_o, sigma_t_gap);
    T_smearing      = gRandom->Gaus(0., sigma_T);

    Float_t X_local = pos.X()-X[smtype][smodule][module][cell];
    Float_t Y_local = pos.Y()-Y[smtype][smodule][module][cell];
    
    tl_new = pt->GetTime() + T_smearing - Y_local/vprop
           + gRandom->Gaus(0,sigma_el);
    tr_new = pt->GetTime() + T_smearing + Y_local/vprop
           + gRandom->Gaus(0,sigma_el);

    if(fVerbose >2 || TMath::Abs(X_local)>1.5) {
      cout << "-W- TofHitProducer " << j <<". Poi," 
         << " TID:" << trackID 
         << " SMtype: " << smtype 
         << " SM: " << smodule 
         << " Mod: " << module 
         << " Str: " << cell 
         << " G: " << gap 
	 << " posX " << pos.X() <<","<<	 X[smtype][smodule][module][cell]
	 << " posY " << pos.Y() <<","<<	 Y[smtype][smodule][module][cell]
         << " tl " << tl_new     << " tr " << tr_new
      //	 << " xl " << X_local	 << " yl " << Y_local
         << endl;
 
      continue; //prevent crashes 
    }
    
    //Take the fastest time from all the points/gaps in this cell
    
    if(tl_new<tl[smtype][smodule][module][cell]) {
      tl[smtype][smodule][module][cell]             = tl_new;
      trackID_left[smtype][smodule][module][cell]   = trackID;
      point_left[smtype][smodule][module][cell]     = j;
    }

    if(tr_new<tr[smtype][smodule][module][cell]) {
      tr[smtype][smodule][module][cell]             = tr_new;
      trackID_right[smtype][smodule][module][cell]  = trackID;
      point_right[smtype][smodule][module][cell]    = j;
    }
    //X and Y depend on the orientation of the cell. FIXME
  } // end of loop on TOF points 

// Extract TOF Hits
  
  Int_t nFl1=0;
  Int_t nFl2=0;
  Int_t ii=0;

  for(Int_t t=1;t<=ActSMtypMax;t++){
   for(Int_t i=1;i<=ActnSMMax[t];i++){
    for(Int_t j=1;j<=ActnModMax[t];j++){
     for(Int_t k=0;k<ActnCellMax[t];k++){
    
//Increase the counter for the TofHit TClonesArray if the first time a hit is attached to this cell
    
      if( tl[t][i][j][k]<1e+6 
       && tr[t][i][j][k]<1e+6) {
       fNHits++;

       xHit    = X[t][i][j][k]; 
       xHitErr = Dx[t][i][j][k]/sqrt(12.);
       zHit    = Z[t][i][j][k];
       zHitErr = Dz/sqrt(12.);
       yHit    =  (tr[t][i][j][k] - tl[t][i][j][k])*vprop*0.5
               + Y[t][i][j][k];
       
    //Reference to the point that contributes to the left side.
       yHitErr = sigma_T*vprop;
       tHit    = 0.5*(tl[t][i][j][k] + tr[t][i][j][k]);
       ref     = point_left[t][i][j][k];
       if(trackID_left[t][i][j][k]==trackID_right[t][i][j][k]){
	   flag = 1; nFl1++;
           // Check consistency
           if(fVerbose >2) {
           pt = (CbmTofPoint*) fTofPoints->At(ref);
           pt->Position(pos);
           cout << " pos check for point "<<ref<<" x:  "<< xHit << " " << pos.X() 
                << " y: " << yHit << " " << pos.Y() << endl;   
	   }
       }
       else {
	   flag = 2; nFl2++;
       }
       
       TVector3 hitPos(xHit, yHit, zHit);
       TVector3 hitPosErr(xHitErr, yHitErr, zHitErr);
       Int_t iCh = Ch[t][i][j][k];

       if(fVerbose >2) {
	 cout << ii++ << " Add hit smt " << t << " sm " << i << " mod " << j << " str " << k
	 <<" Ch " << iCh
         <<" tl " << tl[t][i][j][k] << " tr " << tr[t][i][j][k] 
	 <<" xh " << xHit << " yh " << yHit << " fl "<< flag << " refPoi " << ref 
	 <<" TID "<<  trackID_left[t][i][j][k] <<","<<trackID_right[t][i][j][k]<< endl;
       }
       
       AddHit(pt->GetDetectorID(), hitPos, hitPosErr, ref, tHit, flag, iCh);

      }
     }
    }
   }
  }  
  cout << "-I- CbmTofHitProducer : " << fNHits+1
       << " hits in Tof created, "<< nFl1<< " single, "<< nFl2<< " multiple hits " << endl;

}
// ---- Add Hit to HitCollection --------------------------------------

void CbmTofHitProducer::AddHit(Int_t detID, TVector3 &posHit, TVector3 &posHitErr,
			       Int_t ref, Double_t tHit, Int_t flag, Int_t iChannel)
{
  new((*fHitCollection)[fNHits]) CbmTofHit(detID, posHit, posHitErr,
					   ref, tHit, flag, iChannel);
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


