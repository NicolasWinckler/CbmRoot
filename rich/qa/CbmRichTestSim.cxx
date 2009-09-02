
/******************************************************************************
*  $Id: CbmRichTestSim.cxx,v 1.2 2006/02/09 16:32:02 hoehne Exp $
*
*  Class  : CbmRichTestSim
*  Description: Implementation of Class CbmRichTestSim:
*               This class fills some histograms for a fast check of simulated data
*               Use macro macro/rich/CbmRichTestSim.C
*
*  Author : Claudia Hoehne
*  E-mail : c.hoehne@gsi.de
*
*******************************************************************************
*  $Log: CbmRichTestSim.cxx,v $
*  Revision 1.2  2006/02/09 16:32:02  hoehne
*  improve
*
*  Revision 1.1  2006/02/03 08:59:25  hoehne
*  initial version: class and header for performing a fast test of the simulation output for RICH
*
*
*
*******************************************************************************/
#include "CbmRichTestSim.h"

#include "CbmRichPoint.h"
#include "CbmGeoRichPar.h"

#include "FairTask.h"
#include "FairRootManager.h"
#include "CbmMCTrack.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"
#include "FairGeoVolume.h"
#include "FairGeoTransform.h"
#include "FairGeoVector.h"
#include "FairGeoMedium.h"
#include "FairGeoNode.h"

#include "TClonesArray.h"
#include "TGraph.h"
#include "TVector3.h"
#include "TH2D.h"
#include "TH1D.h"

#include <map>
#include <iostream>

using std::cout;
using std::endl;
using std::map;


// -----   Default constructor   -------------------------------------------
CbmRichTestSim::CbmRichTestSim() :FairTask("RichTestSim")
{
}
// -------------------------------------------------------------------------

//------------  standard constructor (with verbosity level)  ---------------------------------
CbmRichTestSim::CbmRichTestSim(const char *name, const char *title, Int_t verbose)
  :FairTask(name)
{

  // verbosity level
  fVerbose = verbose;

  // count events
  nevents = 0;

  // define some histograms
  fh_Det1ev = new TH2D("fh_Det1ev","points in detector plane (1 event)",170,-170,170,250,-250,250);
  fh_Det1ev_zoom = new TH2D("fh_Det1ev_zoom","points in detector plane (1 event, zoom in)",100,10,60,100,100,150);
  fh_Detall = new TH2D("fh_Detall","points in detector plane (all events)",170,-170,170,250,-250,250);
  fh_n_vs_p  = new TH2D("fh_n_vs_p","Npoints versus momentum",150,0,15,100,0,400);
  fh_v_el = new TH2D("fh_v_el","(y,z) of production vertex of electrons",230,0,460,300,-300,300);

  fh_Nall = new TH1D("fh_Nall","Number of all rings in RICH",150,0,150);
  fh_Nel = new TH1D("fh_Nel","Number of electron rings in RICH",150,0,150);
  fh_Nelprim = new TH1D("fh_Nelprim","Number of electron (STS>6) rings in RICH",150,0,150);
  fh_Npi = new TH1D("fh_Npi","Number of pi rings in RICH",150,0,150);
  fh_Nk = new TH1D("fh_Nk","Number of K rings in RICH",150,0,150);
  fh_Nhad = new TH1D("fh_Nhad","Number of hadrons crossing PMT plane",50,0,50);

  }

  // -----   Destructor   ----------------------------------------------------
CbmRichTestSim::~CbmRichTestSim()
{
  // write histograms
  fh_Det1ev    ->Write();
  fh_Det1ev_zoom    ->Write();
  fh_Detall    ->Write();
  fh_n_vs_p    ->Write();
  fh_v_el      ->Write();

  fh_Nall     ->Write();
  fh_Nel      ->Write();
  fh_Nelprim  ->Write();
  fh_Npi      ->Write();
  fh_Nk       ->Write();
  fh_Nhad     ->Write();

}
// -------------------------------------------------------------------------

// -----   Initialization of parameter Containers  ------------------------------------------------
void CbmRichTestSim::SetParContainers()
{
  // Get Base Container
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  fPar=(CbmGeoRichPar*)(rtdb->getContainer("CbmGeoRichPar"));

//  fPar->setStatic();    // setting the parameters on static mode: <explanation>
}

// -----   Initialization   -----------------------------------------------

InitStatus CbmRichTestSim::Init()
{

  fSensNodes = fPar->GetGeoSensitiveNodes();

  // get detector position (z):
  FairGeoNode *det= dynamic_cast<FairGeoNode*> (fSensNodes->FindObject("rich1d#1"));
  if (! det) cout << " -I no RICH Geo Node  found !!!!!  " << endl;
  FairGeoTransform* detTr=det->getLabTransform();  // detector position in labsystem
  FairGeoVector detPosLab=detTr->getTranslation(); // ... in cm
  FairGeoTransform detCen=det->getCenterPosition();  // center in Detector system
  FairGeoVector detPosCen=detCen.getTranslation();
  fDetZ = detPosLab.Z() + detPosCen.Z();   /* z coordinate of photodetector (Labsystem, cm) */


  // Get and check FairRootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if (! ioman) {
    cout << "-E- CbmRichRingFinderIdeal::Init: "
	 << "RootManager not instantised!" << endl;
    return kERROR;
  }

  // Get MCTrack array
  fMCTrackArray  = (TClonesArray*) ioman->GetObject("MCTrack");
  if ( ! fMCTrackArray) {
    cout << "-E- CbmRichRingFinderIdeal::Init: No MCTrack array!"
	 << endl;
    return kERROR;
  }

  // Get MCPoint array
  fMCRichPointArray  = (TClonesArray*) ioman->GetObject("RichPoint");
  if ( ! fMCRichPointArray) {
    cout << "-E- CbmRichRingFinderIdeal::Init: No MCPoint array!"
	 << endl;
    return kERROR;
  }

  return kSUCCESS;

  }

  //-----------------------------------------------------------------------------

// -----   Execution of Task   ---------------------------------------------

// -------------------------------------------------------------------------
void CbmRichTestSim::Exec(Option_t* option)
{

  nevents++;

  // define some variables
  TVector3 position;
  TVector3 momentum;
  TVector3 vertex;

  Int_t Nhad, Nring, Nel, Nel_prim, Npi, Nk;
  Nhad = Nring = Nel = Nel_prim = Npi = Nk = 0;

  CbmRichPoint *pPoint = NULL;
  CbmMCTrack *pTrack = NULL;

  map<Int_t,Int_t> pointMap;

    Int_t nPoints = fMCRichPointArray->GetEntriesFast();
    Int_t nMCTracks = fMCTrackArray->GetEntriesFast();
    

    for(Int_t iPoint=0; iPoint<nPoints; iPoint++){

      // Get the pointer to the Rich Point

      pPoint= (CbmRichPoint*)fMCRichPointArray->At(iPoint);
      if( !pPoint ){
	cout<<" - Error -  Could not get the pointer"<<endl;
	continue;
      }

      pPoint->Position(position);
      fh_Detall->Fill(position.X(),position.Y());
      if (nevents == 1) {
        fh_Det1ev->Fill(position.X(),position.Y());
	fh_Det1ev_zoom->Fill(position.X(),position.Y());
      }
      if (pPoint->GetTrackID() < 0 ) {
         cout << " - Error - RichPoint with trackID " << pPoint->GetTrackID() << endl;
         continue;   // trackID = -2 for rejected particles (in CbmStack)
	 }
      pTrack = (CbmMCTrack*)fMCTrackArray->At(pPoint->GetTrackID());
      Int_t gcode = pTrack->GetPdgCode();
      if (gcode != 50000050) {
        Nhad++;
	if (fVerbose > 1 ) cout << " Hadron in RICH: " << gcode << " with Npoints "
	                        << pTrack->GetNPoints(kRICH) << endl;
	}
      if (gcode == 50000050){
        pTrack->GetStartVertex(vertex);
//	if (vertex.Z() > (fDetZ-8.) && vertex.Z() < (fDetZ+1.)) continue;
        Int_t motherID = pTrack->GetMotherId();
	if (motherID == -1) {
	   cout << "-E- MotherID of Cherenkov photon = -1" << endl;
	   continue;
	 }
        pTrack = (CbmMCTrack*)fMCTrackArray->At(motherID);
	pointMap[motherID]++;
       } //select cherenkov photons

       } // point loop

      for (Int_t iMCTrack=0; iMCTrack<nMCTracks; iMCTrack++) {
        pTrack = (CbmMCTrack*) fMCTrackArray->At(iMCTrack);
        if ( ! pTrack ) continue;
	Int_t Npoints = pointMap[iMCTrack];
	if (fVerbose>1 && Npoints>0) cout << "Track " << iMCTrack << "(PdgCode) " <<
	             pTrack->GetPdgCode()  <<   " has " << Npoints << " Rich points" <<
		     " -- and GetRichPoints " << pTrack->GetNPoints(kRICH) << endl;
	if (Npoints){
	  Nring++;
          pTrack->GetMomentum(momentum);
	  fh_n_vs_p->Fill(momentum.Mag(),Npoints);
	  pTrack->GetStartVertex(vertex);
	  Int_t gcode = pTrack->GetPdgCode();
	  if (TMath::Abs(gcode) == 11) {
	    Nel++;
	    fh_v_el->Fill(vertex.Z(),vertex.Y());
	    if (pTrack->GetNPoints(kSTS) > 5) Nel_prim++;
           } //electrons
          if (TMath::Abs(gcode) == 211) Npi++;
	  if (TMath::Abs(gcode) == 321) Nk++;
	} // tracks with points in Rich

      } // track loop

     fh_Nall->Fill(Nring);
     fh_Nel->Fill(Nel);
     fh_Nelprim->Fill(Nel_prim);
     fh_Npi->Fill(Npi);
     fh_Nk->Fill(Nk);
     fh_Nhad->Fill(Nhad);

  if (fVerbose){
  cout << "--------------------------------------------------------------------------" << endl;
  cout << "-----------               Test Rich Simulation                 -----------" << endl;
  cout << endl;
  cout << " Number of particles in RICH detector  ----- event number  " << nevents << endl;
  cout << " hadrons in RICH (no Cherenkov photons) = " << Nhad << endl;
  cout << " total rings        = " << Nring << endl;
  cout << " electrons          = " << Nel << endl;
  cout << " electrons (STS>=6) = " << Nel_prim << endl;
  cout << " pions              = " << Npi << endl;
  cout << " Kaons              = " << Nk << endl;
  cout << endl;
  cout << "--------------------------------------------------------------------------" << endl;
  }


}

// -----   Finish Task   ---------------------------------------------------
void CbmRichTestSim::Finish()
{
 fMCTrackArray->Clear();
 fMCRichPointArray->Clear();
}
// -------------------------------------------------------------------------

ClassImp(CbmRichTestSim)

