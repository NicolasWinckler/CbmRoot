/******************************************************************************
*  $Id: CbmRichRingQa.cxx,v 1.3 2006/06/19 11:19:52 turany Exp $
*
*  Class  : CbmRichRingQa
*  Description : Quality checks or ring finders:
*                efficiency calculation etc.
*
*  Author : Simeon Lebedev
*
*******************************************************************************
*  $Log: CbmRichRingQa.cxx,v $
*  Revision 1.3  2006/06/19 11:19:52  turany
*  map<Int_t, Int_t>iterator it do not need to be set to zero, it will be reseted by calling it.begin(), putting it=0 gives a compiler error with the new compilers
*
*  Revision 1.2  2006/04/17 22:11:04  sgorboun
*  changes in L1 ENN Ring Finder
*
*  Revision 1.1  2006/04/10 08:21:03  hoehne
*  routine for checking the ring finders (quality, efficiency)
*  initial version
*
*
*
*******************************************************************************/
 
#include "CbmRichRingQa.h"

#include "CbmRichPoint.h"
#include "CbmRichHit.h"
#include "CbmRichRing.h"
#include "CbmRichRingMatch.h"
#include "CbmRichRingFitterCOP.h"

#include "CbmTask.h"
#include "CbmRootManager.h"
#include "CbmMCTrack.h"
#include "CbmTrackParam.h"
#include "CbmGlobalTrack.h"
#include "CbmMCTrack.h"
#include "CbmStsTrackMatch.h"

#include <map>
#include <vector>
#include <iostream>

using std::cout;
using std::endl;
using std::map;
using std::vector;

// -----   Default constructor   -------------------------------------------
CbmRichRingQa::CbmRichRingQa() :CbmTask("RichRingQa")
{
  fNormType = 0;
}
// -------------------------------------------------------------------------

//------------  standard constructor (with verbosity level)  ---------------------------------
CbmRichRingQa::CbmRichRingQa(const char *name, const char *title, Int_t verbose, Int_t normType)
  :CbmTask(name)
{
    
    // verbosity level
    fVerbose = verbose;
    fNormType = normType;
    
    fNofHitsInRingCut = 5; /// minumum number of hits in ring
    
    // count events
    fEventNumber = 1;
    
    fNofAllRings = 0;   
    fNofElRings = 0; 
    fNofPiRings = 0;  
    fNofElRingsProj = 0; 
    fNofPiRingsProj = 0; 
    fNofElRingsProjHitCut = 0;
    fNofPiRingsProjHitCut = 0;
    
    fNofTrueFoundElRingsProjHitCut = 0;
    fNofTrueFoundPiRingsProjHitCut = 0;
    fNofCloneRings = 0;
    fNofFakeRings = 0;  
    
    fNofTrueElBadFit = 0;

    fh_TrueFoundElRingsProjHitCutMom = new TH1D("fh_TrueFoundElRingsProjHitCutMom","fh_TrueFoundElRingsProjHitCutMom",40,0,10);
    fh_MCElRingsProjHitCutMom = new TH1D("fh_MCElRingsProjHitCutMom","fh_MCElRingsProjHitCutMom",40,0,10);
    
    fh_TrueFoundElRingsProjHitCutRadPos = new TH1D("fh_TrueFoundElRingsProjHitCutRadPos","fh_TrueFoundElRingsProjHitCutRadPos",40,0,150);
    fh_MCElRingsProjHitCutRadPos = new TH1D("fh_MCElRingsProjHitCutRadPos","fh_MCElRingsProjHitCutRadPos",40,0,150);
    
    // (x,y) of fake rings
    fh_FakeFoundRingsXYAll = new TH2D("fh_FakeFoundRingsXYAll","(x,y) fake rings",100,-200,200,125,-250,250);

/// Difference Fake and True rings histogramms BEGIN      
    fh_FakeNofHits = new TH1D("fh_FakeNofHits","Number of hits in ring",50,0,50);
    fh_TrueElNofHits= new TH1D("fh_TrueElNofHits","Number of hits in ring",50,0,50);
    
    fh_FakeDistance = new TH1D("fh_FakeDistance","Distance between track and ring center",50,0,5);
    fh_TrueElDistance = new TH1D("fh_TrueElDistance","Distance between track and ring center",50,0,5);
    
    fh_FakeAngle = new TH1D("fh_FakeAngle","Biggest angle in ring",50,0,6.5);
    fh_TrueElAngle = new TH1D("fh_TrueElAngle","Biggest angle in ring",50,0,6.5);
    
    fh_FakeNofHitsOnRing= new TH1D("fh_FakeNofHitsOnRing","# of hits on ring",50,0,50);
    fh_TrueElNofHitsOnRing= new TH1D("fh_TrueElNofHitsOnRing","# of hits on ring",50,0,50);
    
    fh_FakeChi2 = new TH1D("fh_FakeChi2","Chi2",50,0.,1.);
    fh_TrueElChi2 = new TH1D("fh_TrueElChi2","Chi2",50,0.,1.);
    
    fh_FakeRadPos = new TH1D("fh_FakeRadPos","Radial Position",200,0,200);;
    fh_TrueElRadPos = new TH1D("fh_TrueElRadPos","Radial Position",200,0,200);;
    
    fh_FakeRadius = new TH1D("fh_FakeRadius","Radius",90,0,9);;
    fh_TrueElRadius = new TH1D("fh_TrueElRadius","Radius",90,0,9);;
    
    fh_FakeA = new TH1D("fh_FakeA","A",90,0, 9.);
    fh_TrueElA = new TH1D("fh_TrueElA","A",90,0, 9.);
    
    fh_FakeB = new TH1D("fh_FakeB","B", 90, 0., 9.);
    fh_TrueElB = new TH1D("fh_TrueElB","B", 90, 0. ,9.);
    
    fh_FakePhi = new TH1D("fh_FakePhi","Phi",50, -2.,2.);
    fh_TrueElPhi = new TH1D("fh_TrueElPhi","Phi",50, -2. ,2.);  
    
    fh_FakeStsMom = new TH1D("fh_FakeStsMom","Sts Mom",50, 0., 15.);
    fh_TrueElStsMom = new TH1D("fh_TrueElStsMom","Sts Mom",50, 0. , 15.);  
    
    fh_TrueElPhiVsRadAngle = new TH2D("fh_TrueElPhiVsRadAngle","Phi vs RadAngle", 50, -2. ,2.,50, 0. , 6.3);     
    fh_FakePhiVsRadAngle = new TH2D("fh_FakePhiVsRadAngle","Phi vs RadAngle", 50, -2. ,2.,50, 0. , 6.3);     

    fh_TrueElRadiusVsMom = new TH2D("fh_TrueElRadiusVsMom","Radius Vs Momentum", 50,0.,15.,50, 0. ,9.);     
    fh_FakeRadiusVsMom = new TH2D("fh_FakeRadiusVsMom","Radius Vs Momentum", 50, 0.,15.,50, 0.,9.);     
         
/// Difference Fake and True rings histogramms END
    
    fh_WrongMatchElDistance = new TH1D("fh_WrongMatchElDistance","Distance between track and ring center",50,0,5);
    fh_TrueMatchElDistance = new TH1D("fh_TrueMatchElDistance","Distance between track and ring center",50,0,5);
    fh_TrueMatchElMom = new TH1D("fh_TrueMatchElMom","fh_TrueMatchElMom",40,0,10);    

/// Difference primary electrons and NOT prymary electrons histogramms BEGIN      
    fh_NotTrueElNofHits = new TH1D("fh_NotTrueElNofHits","Number of hits in ring",50,0,50);    
    fh_NotTrueElDistance = new TH1D("fh_NotTrueElDistance","Distance between track and ring center",50,0,5);    
    fh_NotTrueElAngle = new TH1D("fh_NotTrueElAngle","Biggest angle in ring",50,0,6.5);    
    fh_NotTrueElNofHitsOnRing= new TH1D("fh_NotTrueElNofHitsOnRing","# of hits on ring",50,0,50);    
    fh_NotTrueElChi2 = new TH1D("fh_NotTrueElChi2","Chi2",50,0.,1.);    
    fh_NotTrueElRadPos = new TH1D("fh_NotTrueElRadPos","Radial Position",50,0,150);;    
    fh_NotTrueElRadius = new TH1D("fh_NotTrueElRadius","Radius",70,0,7);;
    fh_NotTrueElA = new TH1D("fh_NotTrueElA","A",90,0, 9.);
    fh_NotTrueElB = new TH1D("fh_NotTrueElB","B", 90, 0. ,9.);
    fh_NotTrueElPhi = new TH1D("fh_NotTrueElPhi","Phi",50, -2. ,2.);  
    fh_NotTrueElStsMom = new TH1D("fh_NotTrueElStsMom","Sts Mom",50, 0. , 15.);       
    fh_NotTrueElPhiVsRadAngle = new TH2D("fh_NotTrueElPhiVsRadAngle","Phi vs RadAngle", 50, -2. ,2.,50, 0. , 6.3);
    fh_NotTrueElRadiusVsMom = new TH2D("fh_NotTrueElRadiusVsMom","Radius Vs Momentum", 50,0.,15.,50, 0. ,9.);             
/// Difference primary electrons and NOT prymary electrons histogramms END    

    fh_StartVertexXZ = new TH2D("fh_StartVertexXZ","StartVertex XZ", 200, -2. ,200.,300, -150. , 150.);
    fh_StartVertexYZ = new TH2D("fh_StartVertexYZ","StartVertex YZ", 200, -2.,200.,300, -150. ,150.);  
           
  //hits distribution (x,y)
  fh_HitsXY = new TH2D("fh_HitsXY","Hits distribution (x,y)",100,-200,200,125,-250,250);
  fh_HitsRadPos = new TH1D("fh_HitsRadPos","Hits distribution rad position",200, 0, 200);  
  // (x,y) of true primary electron rings
  fh_TrueFoundRingsXYE = new TH2D("fh_TrueFoundRingsXYE","(x,y) true electrons rings. >=10 Hits. Proj",100,-200,200,125,-250,250);
  // (x,y) of true primary pions rings
  fh_TrueFoundRingsXYPi = new TH2D("fh_TrueFoundRingsXYPi","(x,y) true pions rings. >=10 Hits. Proj",100,-200,200,125,-250,250);
  //number of hits per event
  fh_NhitsPerEvent = new TH1D("fh_NhitsPerEvent","Number of hits per event",100,1000,3000);
  //number of projections per event
  fh_NprojPerEvent = new TH1D("fh_NprojPerEvent","Number of projections per event",50,200,450);
  
  foutFakeAndTrue.open("ann_fake_and_true.txt",std::ios_base::app);
  foutElAndNotEl.open("ann_el_notel.txt",std::ios_base::app);  

}

  // -----   Destructor   ----------------------------------------------------
CbmRichRingQa::~CbmRichRingQa()
{


}
// -------------------------------------------------------------------------


// -----   Initialization   -----------------------------------------------

InitStatus CbmRichRingQa::Init()
{
    cout << "InitStatus CbmRichRingQa::Init()"<<endl;
 // Get and check CbmRootManager
  CbmRootManager* ioman = CbmRootManager::Instance();
  if (! ioman) {
    cout << "-E- CbmRichRingQa::Init: "
     << "RootManager not instantised!" << endl;
    return kERROR;
  }

  // Get hit Array
  fHits = (TClonesArray*) ioman->GetObject("RichHit");
  if ( ! fHits) {
    cout << "-W- CbmRichRingQa::Init: No RichHit array!"
     << endl;
  }

  // Get RichRing Array
  fRings = (TClonesArray*) ioman->GetObject("RichRing");
  if ( ! fRings ) {
    cout << "-E- CbmRichRingQa::Init: No RichRing array!" << endl;
    return kERROR;
  }

  // Get MC Point array
  fPoints = (TClonesArray*) ioman->GetObject("RichPoint");
  if ( ! fPoints ) {
    cout << "-E- CbmRichRingQa::Init: No RichPoint array!" << endl;
    return kERROR;
  }

  // Get MC Point array
  fTracks = (TClonesArray*) ioman->GetObject("MCTrack");
  if ( ! fTracks ) {
    cout << "-E- CbmRichRingQa::Init: No MCTrack array!" << endl;
    return kERROR;
  }

  // Get RichRingMatch array
  fMatches = (TClonesArray*) ioman->GetObject("RichRingMatch");
  if ( ! fMatches ) {
    cout << "-E- CbmRichRingQa::Init: No RichRingMatch array!" << endl;
    return kERROR;
  }

  // Get RichProjection array
  fProj = (TClonesArray*) ioman->GetObject("RichProjection");
  if ( ! fProj ) {
    cout << "-E- CbmRichRingQa::Init: No RichProjection array!" << endl;
    return kERROR;
  }


  // get TrackMatch array
  fTrackMatch = (TClonesArray*) ioman->GetObject("STSTrackMatch");
  if ( ! fTrackMatch) {
    cout << "-E- CbmRichRingQa::Init: No track match array!"<< endl;
    return kERROR;
  }
  
  // Get global track array
    gTrackArray = (TClonesArray*) ioman->GetObject("GlobalTrack");
    if ( ! gTrackArray) {
        cout << "-W- CbmRichRingQa::Init: No global track array!"
        << endl;
    return kERROR;
     }
     
  return kSUCCESS;

}

  //-----------------------------------------------------------------------------

// -----   Execution of Task   ---------------------------------------------

// -------------------------------------------------------------------------
void CbmRichRingQa::Exec(Option_t* option)
{
    cout<<"CbmRichRingQa Event No. "<< fEventNumber++ << endl;
    
    // Create some pointers and variables
    CbmRichHit* hit    = NULL;
    CbmMCPoint* point  = NULL;
    CbmMCTrack* track  = NULL;
    
    Int_t iPoint       = 0;
    Int_t iMCTrack     = 0;
    Int_t iMother      = 0;
    
    map<Int_t, Int_t>::iterator it;//iterator for acass to fRingMap
    map<Int_t, CbmRichRing >::iterator itMapWithHits;
    
    //calculate number of hits in MC ring to
    //compare its with number of hits in found ring
    fRingMap.clear();
    fRingMapWithHits.clear();
    
    Int_t nRichHits = fHits->GetEntriesFast();
    fh_NhitsPerEvent->Fill(nRichHits);
    fh_NprojPerEvent->Fill(fProj->GetEntriesFast());
    
    /// Loop over Rich hits
    for (Int_t iHit=0; iHit < nRichHits; iHit++) {
        hit = (CbmRichHit*) fHits->At(iHit);
        if ( ! hit ) {
        cout << "-E- CbmRichMatchRings::Exec: "
            << "No Hit " << iHit << endl;
        continue;
        }
        Double_t radPos;
        if (hit->Y() > 0){
            radPos = sqrt((hit->X() - 0)*(hit->X() - 0) +
                          (hit->Y() - 110)*(hit->Y() - 110));
        } else {
            radPos = sqrt((hit->X() - 0)*(hit->X() - 0) +
                          (hit->Y() + 110)* (hit->Y() + 110));
        }
        fh_HitsRadPos->Fill(radPos);
        
        fh_HitsXY->Fill(hit->X(), hit->Y());
        iPoint = hit->GetRefIndex();
    
        if ( iPoint < 0 )continue; // Fake or background hit
        //Get the MC Point corresponding to the hit
    
        point = (CbmMCPoint*) fPoints->At(iPoint);
        if ( ! point ) {
        cout << "-E- CbmRichRingQa::Exec() "
            << "Empty MCPoint " << iPoint << " from Hit " << iHit
            << endl;
        continue;
        }
        //Get the MC Track ID corresponding to the MC Point
        iMCTrack = point->GetTrackID();
        // Get the MC Track corresponding to the ID
        track   = (CbmMCTrack*)fTracks->At(iMCTrack);
        if ( ! track ) {
        cout << "-E- CbmRichRingQa::Exec "
            << "Empty MCtrack " << iMCTrack
            << endl;
        continue;
        }
        iMother = track->GetMotherId();
        if (iMother == -1) {
            cout << "-E- MotherID of Cherenkov photon = -1" << endl;
            continue;
        }
        fRingMap[iMother]++;
        fRingMapWithHits[iMother].AddHit(iHit);
    }  /// Loop over Rich hits
    
   
    ///Loop for all MC rings
    for (it=fRingMap.begin(); it!=fRingMap.end(); it++) {
        track = (CbmMCTrack*) fTracks->At(it->first);
        if (!track){
            cout << "-E- CbmRichRingQa::Exec() no track"<<
            it->first<<endl;
            continue;
        }
        
        Double_t momentum = track->GetP();
        Int_t trackID = it->first;
        
        Bool_t isProj = DoesRingHaveProjection(trackID);
        
        Int_t gcode = track->GetPdgCode();    
        Int_t motherId = track->GetMotherId();
                
        fNofAllRings++;
    
        if ((TMath::Abs(gcode) == 11) && motherId == -1) {///primary electron rings
            fNofElRings++;
            if (isProj) fNofElRingsProj++;
            if (isProj && it->second >= fNofHitsInRingCut){
                fNofElRingsProjHitCut++;
                fh_MCElRingsProjHitCutMom->Fill(momentum);
            }
        }
    
        if ((TMath::Abs(gcode) == 211)) {///pion rings
            fNofPiRings++;
            if (isProj) fNofPiRingsProj++;
            if (isProj && it->second >= fNofHitsInRingCut) fNofPiRingsProjHitCut++;
        }
    }  ///Loop for all MC rings
    
   
    CbmRichRingFitterCOP* fitCOP = new CbmRichRingFitterCOP(0,0);
    fitCOP->Init();
    for (itMapWithHits=fRingMapWithHits.begin(); itMapWithHits!=fRingMapWithHits.end(); itMapWithHits++) {    
        track = (CbmMCTrack*) fTracks->At(itMapWithHits->first);
        if (!track){
            cout << "-E- CbmRichRingQa::Exec() no track"<< itMapWithHits->first<<endl;
            continue;
        }
        Int_t trackID = itMapWithHits->first;
        Bool_t isProj = DoesRingHaveProjection(trackID);        
        Int_t gcode = track->GetPdgCode();
        Int_t motherId = track->GetMotherId();        
        if ((TMath::Abs(gcode) == 11)&& motherId == -1) {///primary electron rings
            if (isProj && itMapWithHits->second.GetNofHits() >= fNofHitsInRingCut){
                fitCOP->DoFit(&(itMapWithHits->second));
                fh_MCElRingsProjHitCutRadPos->Fill(itMapWithHits->second.GetRadialPosition());
                //cout << "MC "<<itMapWithHits->second.GetRadialPosition() << endl;
            }
        }
    }
    delete fitCOP;
    
    EfficiencyCalc();   
    DiffFakeTrue();
    RingTrackMatchEff();
    DiffPrimElectronsAndNotPrimElectrons();

   cout <<  "fNofAllRings="<< fNofAllRings << 
           "  per Event = " << (Double_t)fNofAllRings/fEventNumber<< endl << 
  
           "fNofElRings="<< fNofElRings<< 
           "  per Event = " << (Double_t)fNofElRings/fEventNumber<< endl <<
           
           "fNofPiRings="<< fNofPiRings << 
           "  per Event = " << (Double_t)fNofPiRings/fEventNumber<< endl << 
           
           "fNofElRingsProj="<<fNofElRingsProj << 
           "  per Event = " << (Double_t)fNofElRingsProj/fEventNumber<< endl << 
           
           "fNofPiRingsProj="<<fNofPiRingsProj << 
           "  per Event = " << (Double_t)fNofPiRingsProj/fEventNumber<< endl << 
           
           "fNofElRingsProjHitCut="<<fNofElRingsProjHitCut << 
           "  per Event = " << (Double_t)fNofElRingsProjHitCut/fEventNumber<< endl << 
           
           "fNofPiRingsProjHitCut="<<fNofPiRingsProjHitCut << 
           "  per Event = " << (Double_t)fNofPiRingsProjHitCut/fEventNumber<< endl << 
           
           "fNofTrueFoundElRingsProjHitCut = " << fNofTrueFoundElRingsProjHitCut << 
           "  per Event = " << (Double_t)fNofTrueFoundElRingsProjHitCut/fEventNumber<< endl <<
           
           "fNofTrueFoundPiRingsProjHitCut = " << fNofTrueFoundPiRingsProjHitCut << 
           "  per Event = " << (Double_t)fNofTrueFoundPiRingsProjHitCut/fEventNumber<< endl <<
           
           "fNofCloneRings = " << fNofCloneRings << 
           "  per Event = " << (Double_t)fNofCloneRings/fEventNumber<< endl <<
           
           "fNofFakeRings = " << fNofFakeRings << 
           "  per Event = " << (Double_t)fNofFakeRings/fEventNumber<< endl;
           
     Double_t elEff = 0.;
     if (fNofElRingsProjHitCut != 0.) 
        elEff = (Double_t)fNofTrueFoundElRingsProjHitCut/(Double_t)fNofElRingsProjHitCut; 
     cout << "El efficiency = "<< elEff << endl;
                                 
     Double_t piEff = 0.;
     if (fNofPiRingsProjHitCut != 0.)
        piEff = (Double_t)fNofTrueFoundPiRingsProjHitCut/(Double_t)fNofPiRingsProjHitCut;
     cout << "Pi efficiency = "<<piEff << endl;
                                 
     cout << "NofTrueMatchEl = "<<fh_TrueMatchElDistance->GetEntries() << 
            "  per Event = " << (Double_t)fh_TrueMatchElDistance->GetEntries()/fEventNumber<< endl;
            
     cout << "NofWrongMatchEl = "<<fh_WrongMatchElDistance->GetEntries() << 
            "  per Event = " << (Double_t)fh_WrongMatchElDistance->GetEntries()/fEventNumber<< endl;
            
     Double_t matchRatio = 0.;
     if (fh_TrueMatchElDistance->GetEntries()!=0.)
         matchRatio = (Double_t)fh_WrongMatchElDistance->GetEntries() /
                      (Double_t)fh_TrueMatchElDistance->GetEntries();
     cout <<"Ratio (NofWrongMatchEl / NofTrueMatchEl) = "<< matchRatio<< endl;
            
     cout << "fNofTrueElBadFit = " << fNofTrueElBadFit << endl;
           
     cout << "fNofNOTTrueEl = " << fh_NotTrueElNofHits->GetEntries() << 
           "  per Event = " << (Double_t)fh_NotTrueElNofHits->GetEntries() / fEventNumber << endl;
}

Bool_t CbmRichRingQa::DoesRingHaveProjection(Int_t trackId){
    //search for projection with such TrackID
    Bool_t isProj = false;
    Int_t nProj = fProj->GetEntriesFast();
    CbmTrackParam* proj = NULL;
    CbmGlobalTrack* gtrack = NULL;
      CbmStsTrackMatch* trackMatch = NULL;
        
    for (Int_t iProj = 0; iProj < nProj; iProj++){
        proj   = (CbmTrackParam*)fProj->At(iProj);
        if (!proj){
            cout << "-E- no projection"<< iProj<<endl;
            continue;

        }
        gtrack = (CbmGlobalTrack*)gTrackArray->At(iProj);
        if (gtrack->GetStsTrackIndex() == -1) continue;
        trackMatch = (CbmStsTrackMatch*)fTrackMatch->At(gtrack->GetStsTrackIndex());

        if (!trackMatch) cout << "-E- no matched track!: fake?"<< endl;
        if (!trackMatch) continue;;
        if (trackMatch->GetMCTrackId() == trackId){
            isProj = true;
            break;
        }
    }
    return isProj;
}

Double_t CbmRichRingQa::GetStsMomentum(CbmRichRing * ring)
{
    Int_t ringTrackId = ring->GetTrackID();
    
    if (ringTrackId == -1) return 0.0;
        
    CbmGlobalTrack* gtrack = (CbmGlobalTrack*)gTrackArray->At(ringTrackId);
    if (gtrack->GetStsTrackIndex() == -1) return 0.0;
    CbmStsTrackMatch* stsMatch  =
            (CbmStsTrackMatch*) fTrackMatch->At(gtrack->GetStsTrackIndex());
    if (!stsMatch){
        cout << "-E- CbmRichRingQa::GetStsMomentum() no matched track!: fake?"<< endl;
        return 0.0;
    }    

    CbmMCTrack* track = (CbmMCTrack*) fTracks->At(stsMatch->GetMCTrackId());
    if (!track) {
        cout << "-E- CbmRichRingQa::GetStsMomentum() No track " << endl;
        return 0.0;
    }      

    return track->GetP();
}

void CbmRichRingQa::EfficiencyCalc()
{
    CbmRichRingMatch *match;
    CbmMCTrack* track;
    CbmRichRing* ring;
    
    Int_t nMatches = fMatches->GetEntriesFast();
    cout<<"Number of found rings = "<<nMatches<<endl;
    
    vector<Int_t> clone;
    clone.clear();

    for (Int_t iMatches = 0; iMatches < nMatches; iMatches++){
        match   = (CbmRichRingMatch*)fMatches->At(iMatches);
        if (!match){
            cout << "-E- CbmRichRingQa::EfficiencyCalc() no match"<<
            iMatches<<endl;
            continue;
        }
        ring = (CbmRichRing*)fRings->At(iMatches);
        if (!ring){
            cout << "-E- CbmRichRingQa::EfficiencyCalc() no ring"<<
            iMatches<<endl;
            continue;
        }  
        
        Int_t lTrueHits = match->GetNofTrueHits();
        Int_t lWrongHits = match->GetNofWrongHits();
        Int_t lFakeHits = match->GetNofFakeHits();
        Int_t lMCHits = match->GetNofMCHits();
        Int_t lFoundHits = lTrueHits + lWrongHits + lFakeHits;
        Double_t lPercTrue = 0;
        if (lFoundHits >= 3){
            lPercTrue = (Double_t)lTrueHits / (Double_t)lFoundHits;
        }
        Double_t lPercMCTrue = 0;
        if (lMCHits >= 3){
            lPercMCTrue = (Double_t)lTrueHits / (Double_t)lMCHits;
        }
        
        Int_t trackID = match->GetMCTrackID();
        track = (CbmMCTrack*) fTracks->At(trackID);
        if (!track) {
            cout << "-E- CbmRichRingQa::EfficiencyCalc(). No track "
            << trackID <<endl;
            continue;
        }
        Int_t gcode = track->GetPdgCode();
        Double_t momentum = track->GetP();
        Int_t motherId = track->GetMotherId();
        Bool_t isProj = DoesRingHaveProjection(trackID);
    
        //choose the normalisation type!!!!!!!
        Double_t PercOfTrueHits = -1.;
        if (fNormType == 0) //normalize by Number of MC hits
            PercOfTrueHits = lPercMCTrue;
        if (fNormType == 1) //normalize by Number of found hits
            PercOfTrueHits = lPercTrue;
    
        if (PercOfTrueHits == -1){
            cout << "-E- CbmRichRingQa::EfficiencyCalc() "<<
            "PercOfTrueHits == -1"<< endl;
            continue;
        }
        
        ring->SetRecFlag(-1);
            
        ///reject the fake rings
        if (ring->GetSelectionNN() < -0.5) continue;


        ///fake rings
        if (PercOfTrueHits < 0.5){
            ring->SetRecFlag(1);
        }else{///true rings
            if ( TMath::Abs(gcode) == 11 && 
                 motherId == -1 &&
                 lMCHits >= fNofHitsInRingCut && 
                 isProj ){ ///primary electron rings
                
                ring->SetRecFlag(3);
            }
            if ( TMath::Abs(gcode) == 211 && 
                lMCHits >= fNofHitsInRingCut && 
                isProj ){ ///pion rings
                
                ring->SetRecFlag(4);
            }
            /// search for clone rings
            for (UInt_t ci = 0; ci < clone.size(); ci++)
                if (trackID == clone[ci]) ring->SetRecFlag(2);
            clone.push_back(trackID);
            
        }///true rings
        

    
        Int_t recFlag = ring->GetRecFlag();
    
    
        ///clone ring
        if (recFlag ==2){
            fNofCloneRings++;
        }
        
        ///fake ring
        if (recFlag == 1){
            fh_FakeFoundRingsXYAll->Fill(ring->GetCenterX(), ring->GetCenterY());
            fNofFakeRings++;
        }
        
        ///primary electrons
        if (recFlag == 3){
            fNofTrueFoundElRingsProjHitCut++;
            if (ring->GetAaxis() > 10 || ring->GetAaxis() < 0) fNofTrueElBadFit++;
            fh_TrueFoundElRingsProjHitCutMom->Fill(momentum);
            fh_TrueFoundRingsXYE->Fill(ring->GetCenterX(),ring->GetCenterY());
            fh_TrueFoundElRingsProjHitCutRadPos->Fill(fRingMapWithHits[trackID].GetRadialPosition());
            //cout << "True Found"<<fRingMapWithHits[trackID].GetRadialPosition() << endl;
        }
        
        ///pions
        if (recFlag == 4){
            fNofTrueFoundPiRingsProjHitCut++;
            fh_TrueFoundRingsXYPi->Fill(ring->GetCenterX(),ring->GetCenterY());
        }
    

    }///loop over all found rings
    
}

void CbmRichRingQa::DiffFakeTrue()
{
  CbmRichRingMatch *match;
  CbmRichRing* ring;
  Int_t nMatches = fMatches->GetEntriesFast();
  
  for (Int_t iMatches = 0; iMatches < nMatches; iMatches++){
    match   = (CbmRichRingMatch*)fMatches->At(iMatches);
    if (!match){
       cout << "-E- CbmRichRingQa::DiffFakeTrue() no match"<<
       iMatches<<endl;
       continue;
    }
    ring = (CbmRichRing*)fRings->At(iMatches);
    if (!ring){
       cout << "-E- CbmRichRingQa::DiffFakeTrue() no ring"<<
       iMatches<<endl;
       continue;
    }

    Int_t recFlag = ring->GetRecFlag();
    Double_t angle = ring->GetAngle();
    Int_t hitsOnRing = ring->GetNofHitsOnRing();
    Double_t chi2 = ring->GetChi2();
    Double_t distance = ring->GetDistance();
    Int_t nHits = ring->GetNofHits();
    Double_t radPos = ring->GetRadialPosition();
    Double_t radius = ring->GetRadius();
    Double_t axisA = ring->GetAaxis();
    Double_t axisB = ring->GetBaxis();
    Double_t phi = ring->GetPhi();
    Double_t radAngle = ring->GetRadialAngle();
    Double_t stsMomentum = GetStsMomentum(ring);  
    Double_t centerX = ring->GetCenterX();
    Double_t centerY = ring->GetCenterY();
    
    if (ring->GetAaxis() >= 10. || ring->GetAaxis() <= 0.){
        continue;
    }
              
    if (recFlag == 1) fh_FakeNofHits->Fill(nHits);
    if (recFlag == 3) fh_TrueElNofHits->Fill(nHits);    
    
    if (recFlag == 1) fh_FakeDistance->Fill(distance);
    if (recFlag == 3) fh_TrueElDistance->Fill(distance);

    if (recFlag == 1) fh_FakeAngle->Fill(angle);
    if (recFlag == 3)  fh_TrueElAngle->Fill(angle);

    if (recFlag == 1) fh_FakeNofHitsOnRing->Fill(hitsOnRing);
    if (recFlag == 3) fh_TrueElNofHitsOnRing->Fill(hitsOnRing);

    if (recFlag == 1) fh_FakeRadPos->Fill(radPos);
    if (recFlag == 3) fh_TrueElRadPos->Fill(radPos);

    if (recFlag == 1) fh_FakeChi2->Fill(chi2);
    if (recFlag == 3) fh_TrueElChi2->Fill(chi2);

    if (recFlag == 1) fh_FakeRadius->Fill(0.5*(axisB+axisA));
    if (recFlag == 3) fh_TrueElRadius->Fill(0.5*(axisB+axisA));
    
    if (recFlag == 1) fh_FakeA->Fill(axisA);
    if (recFlag == 3) fh_TrueElA->Fill(axisA);

    if (recFlag == 1) fh_FakeB->Fill(axisB);
    if (recFlag == 3) fh_TrueElB->Fill(axisB);

    if (recFlag == 1) fh_FakePhi->Fill(phi);
    if (recFlag == 3) fh_TrueElPhi->Fill(phi); 
    
    if (recFlag == 1) fh_FakeStsMom->Fill(GetStsMomentum(ring));
    if (recFlag == 3) fh_TrueElStsMom->Fill(GetStsMomentum(ring));     
       
    if (recFlag == 3) fh_TrueElPhiVsRadAngle->Fill(phi, radAngle); 
    if (recFlag == 1) fh_FakePhiVsRadAngle->Fill(phi, radAngle); 
    
    if (recFlag == 3) fh_TrueElRadiusVsMom->Fill(stsMomentum, axisB); 
    if (recFlag == 1) fh_FakeRadiusVsMom->Fill(stsMomentum, axisB);     
    
      
    if (recFlag == 1) foutFakeAndTrue << nHits << " "
                            << distance << " "
                            << angle << " "
                            << hitsOnRing << " "
                            << radPos << " "
                            << axisA << " "
                            << axisB << " "
                            << phi << " "
                            << radAngle << " "
                            << chi2 << " "
                            << -1 << endl;
                            
    if (recFlag == 3) foutFakeAndTrue << nHits << " "
                            << distance << " "
                            << angle << " "
                            << hitsOnRing << " "
                            << radPos << " "
                            << axisA << " "
                            << axisB << " "
                            << phi << " "
                            << radAngle << " "
                            << chi2 << " "
                            << 1 << endl;

  }///loop over matches

}

void CbmRichRingQa::RingTrackMatchEff()
{
    CbmRichRingMatch *match;
    CbmRichRing* ring;
    CbmMCTrack* track;
    Int_t nMatches = fMatches->GetEntriesFast();
    Int_t nTraksMatch = fTrackMatch->GetEntriesFast();
    cout << "nTraksMatch = " << nTraksMatch<< endl;
    
    for (Int_t iMatches = 0; iMatches < nMatches; iMatches++){
    
        match   = (CbmRichRingMatch*)fMatches->At(iMatches);
        if (!match){
            cout << "-E- CbmRichRingQa::RingTrackMatchEff() no match"<<
            iMatches<<endl;
            continue;
        }
        
        ring = (CbmRichRing*)fRings->At(iMatches);
        if (!ring){
            cout << "-E- CbmRichRingQa::RingTrackMatchEff() no ring"<<
            iMatches<<endl;
            continue;
        }
        Int_t recFlag = ring->GetRecFlag();
        Double_t ringTrackDist = ring->GetDistance();

        Int_t trackID = match->GetMCTrackID();
        
        ///get momentum
        track = (CbmMCTrack*) fTracks->At(trackID);
        if (!track) {
            cout << "-E- CbmRichRingQa::RingTrackMatchEff(). No track "
            << trackID <<endl;
            continue;
        }
        Double_t momentum = track->GetP();        
        
 
        Int_t ringTrackId = ring->GetTrackID();
        if (ringTrackId == -1) continue;
        
        CbmGlobalTrack* gtrack = (CbmGlobalTrack*)gTrackArray->At(ringTrackId);
        if (gtrack->GetStsTrackIndex() == -1) continue;
        CbmStsTrackMatch* stsMatch  =
                (CbmStsTrackMatch*) fTrackMatch->At(gtrack->GetStsTrackIndex());
        if (!stsMatch) cout << "-E- no matched track!: fake?"<< endl;
        if (!stsMatch) continue;
        Int_t ringMCTrackId = stsMatch->GetMCTrackId();

        if (recFlag == 3){///true primary electron
            //fh_AllMatchDistTrackE->Fill(ringTrackDist);
            if (trackID == ringMCTrackId){///true match
                fh_TrueMatchElDistance->Fill(ringTrackDist);
                fh_TrueMatchElMom->Fill(momentum);
            }
            if (trackID != ringMCTrackId){///wrong match
                fh_WrongMatchElDistance->Fill(ringTrackDist);
            }
        }
    }///loop over matches
}


void CbmRichRingQa::DiffPrimElectronsAndNotPrimElectrons()
{
    CbmRichRingMatch *match;
    CbmRichRing* ring;
    Int_t nMatches = fMatches->GetEntriesFast();
    
    for (Int_t iMatches = 0; iMatches < nMatches; iMatches++){
        match   = (CbmRichRingMatch*)fMatches->At(iMatches);
        if (!match){
            cout << "-E- CbmRichRingQa::DiffPrimElectronsAndNotPrimElectrons no match"<<
            iMatches<<endl;
            continue;
        }
        ring = (CbmRichRing*)fRings->At(iMatches);
        if (!ring){
            cout << "-E- CbmRichRingQa::DiffPrimElectronsAndNotPrimElectrons() no ring"<<
            iMatches<<endl;
            continue;
        }
        
        Int_t trackID = match->GetMCTrackID();
        CbmMCTrack *track = (CbmMCTrack*) fTracks->At(trackID);
        if (!track) {
            cout << "-E- CbmRichRingQa::DiffPrimElectronsAndNotPrimElectrons(). No track "
            << trackID <<endl;
            continue;
        }
        Int_t gcode = TMath::Abs(track->GetPdgCode());
        Double_t momentum = track->GetP();
        Int_t motherId = track->GetMotherId();
        Bool_t isProj = DoesRingHaveProjection(trackID);    
        Double_t startX = track->GetStartX();
        Double_t startY = track->GetStartY();
        Double_t startZ = track->GetStartZ();
        
        if (gcode == 11 )fh_StartVertexXZ->Fill(startZ, startX);
        if (gcode == 11 )fh_StartVertexYZ->Fill(startZ, startX); 
         
        Int_t recFlag = ring->GetRecFlag(); 
        
        Bool_t isReject = false;
       // if (recFlag == 1) continue;   
        
       // if (momentum > 5.) continue;
        if (gcode != 211 || recFlag == 1) isReject = true;
        //if (TMath::Abs(gcode) == 211 || TMath::Abs(gcode) == 11) continue;
        //if (TMath::Abs(gcode) != 11) isReject = true;
        //if ( !( startZ > 50.  && TMath::Abs(gcode) == 11) ) isReject = true;
       // if ( !(  (startX > 40. || startY > 40.) && TMath::Abs(gcode) == 11)  ) isReject = true;        
        ///reject the fake rings
       // if (ring->GetSelectionNN() < -0.5) isReject = true;
    
       
       
        Double_t angle = ring->GetAngle();
        Int_t hitsOnRing = ring->GetNofHitsOnRing();
        Double_t chi2 = ring->GetChi2();
        Double_t distance = ring->GetDistance();
        Int_t nHits = ring->GetNofHits();
        Double_t radPos = ring->GetRadialPosition();
        Double_t radius = ring->GetRadius();
        Double_t axisA = ring->GetAaxis();    
        Double_t axisB = ring->GetBaxis();
        Double_t phi = ring->GetPhi();
        Double_t radAngle = ring->GetRadialAngle();  
        Double_t stsMomentum = GetStsMomentum(ring);
    
        if (recFlag != 3 && !isReject) fh_NotTrueElNofHits->Fill(nHits);
        if (recFlag != 3 && !isReject) fh_NotTrueElDistance->Fill(distance);
        if (recFlag != 3 && !isReject) fh_NotTrueElAngle->Fill(angle);
        if (recFlag != 3 && !isReject) fh_NotTrueElNofHitsOnRing->Fill(hitsOnRing);
        if (recFlag != 3 && !isReject) fh_NotTrueElRadPos->Fill(radPos);
        if (recFlag != 3 && !isReject) fh_NotTrueElChi2->Fill(chi2);
        if (recFlag != 3 && !isReject) fh_NotTrueElRadius->Fill(radius);
        if (recFlag != 3 && !isReject) fh_NotTrueElA->Fill(axisA);
        if (recFlag != 3 && !isReject) fh_NotTrueElB->Fill(axisB);
        if (recFlag != 3 && !isReject) fh_NotTrueElPhi->Fill(phi);
        if (recFlag != 3 && !isReject) fh_NotTrueElStsMom->Fill(stsMomentum);        
        if (recFlag != 3 && !isReject) fh_NotTrueElPhiVsRadAngle->Fill(phi, radAngle);
        if (recFlag != 3 && !isReject) fh_NotTrueElRadiusVsMom->Fill(stsMomentum, axisB);    
        

    
        if (recFlag != 3 && !isReject) foutElAndNotEl << radPos << " "
                                << axisA << " "
                                << axisB << " "
                                << phi << " "
                                << radAngle << " "
                                << chi2 << " "
                                << stsMomentum << " "
                                << -1 << endl;
                                
        if (recFlag == 3) foutElAndNotEl << radPos << " "
                                << axisA << " "
                                << axisB << " "
                                << phi << " "
                                << radAngle << " "
                                << chi2 << " "
                                << stsMomentum << " "
                                << 1 << endl;        
            
            
    }///loop over matches

}

// -----   Finish Task   ---------------------------------------------------
void CbmRichRingQa::Finish()
{

    fRings->Clear();
    fPoints->Clear();
    fTracks->Clear();
    fHits->Clear();
    fMatches->Clear();
    gTrackArray->Clear();
    
    TDirectory *current = gDirectory;
    TDirectory *rich = current->mkdir("RichRingQaHist");
    rich->cd();

    fh_TrueFoundElRingsProjHitCutMom->Write();
      fh_MCElRingsProjHitCutMom->Write();
    
    fh_TrueFoundElRingsProjHitCutRadPos->Write();
      fh_MCElRingsProjHitCutRadPos->Write();
    
    /// Difference Fake and True rings histogramms BEGIN
    fh_FakeNofHits->Write();
    fh_TrueElNofHits->Write();
    
    fh_FakeDistance->Write();
    fh_TrueElDistance->Write();
    
    fh_FakeAngle->Write();
    fh_TrueElAngle->Write();

    fh_FakeNofHitsOnRing->Write();
    fh_TrueElNofHitsOnRing->Write();
    
    fh_FakeRadPos->Write();
    fh_TrueElRadPos->Write();
    
    fh_FakeChi2->Write();
    fh_TrueElChi2->Write();
    
    fh_FakeRadius->Write();
    fh_TrueElRadius->Write();    
    
    fh_FakeA->Write();
    fh_TrueElA->Write();
    
    fh_FakeB->Write();
    fh_TrueElB->Write();
    
    fh_FakePhi->Write();
    fh_TrueElPhi->Write(); 
    
    fh_TrueElStsMom->Write();  
    fh_FakeStsMom->Write();    
    
    fh_TrueElPhiVsRadAngle->Write();  
    fh_FakePhiVsRadAngle->Write();  
    
    fh_TrueElRadiusVsMom->Write();  
    fh_FakeRadiusVsMom->Write();                      
    /// Difference Fake and True rings histogramms END
    
    fh_WrongMatchElDistance->Write();
    fh_TrueMatchElDistance->Write();
    fh_TrueMatchElMom->Write();
    
/// Difference Fake and True rings histogramms BEGIN      
    fh_NotTrueElNofHits->Write();
    fh_NotTrueElDistance->Write();     
    fh_NotTrueElAngle->Write(); 
    fh_NotTrueElNofHitsOnRing->Write();    
    fh_NotTrueElChi2->Write(); 
    fh_NotTrueElRadPos->Write();
    fh_NotTrueElRadius->Write();
    fh_NotTrueElA->Write(); 
    fh_NotTrueElB->Write();
    fh_NotTrueElPhi->Write();   
    fh_NotTrueElStsMom->Write();       
    fh_NotTrueElPhiVsRadAngle->Write(); 
    fh_NotTrueElRadiusVsMom->Write();     
/// Difference Fake and True rings histogramms END        
             
    fh_StartVertexXZ->Write();
    fh_StartVertexYZ->Write();  

   
    fh_HitsXY->Write();
    fh_HitsRadPos->Write();
    fh_TrueFoundRingsXYE->Write();
    fh_TrueFoundRingsXYPi->Write();
    fh_FakeFoundRingsXYAll->Write();
    
    fh_NhitsPerEvent->Write();
    fh_NprojPerEvent->Write();
    
    rich->cd();
    current->cd();
}

// -------------------------------------------------------------------------

ClassImp(CbmRichRingQa)

