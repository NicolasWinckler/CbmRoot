// -------------------------------------------------------------------------
// -----                    CbmMvdDigitiser source file                -----
// -------------------------------------------------------------------------


/**  Digitizer for Simulated Point in the Vertex Detector. 
 * Digitization follows the procedure adopted in the CMS software package
 * an adapted later on to the ILC vertex detector.
 * For each Simulated MVD Hit the intersection points with 
 * the inner and outer boundaries of sensitive Si layer are calculated. 
 * Track segment within a layer is approximated with the line. 
 * It is divided by n subsegments, where n can be specified by a user 
 * via external Processor parameter.  
 * The charge transfer from the middle point of track subsegment (referred 
 * hereafter to as ionisation point) 
 * to the outer collection plane is performed. 
 * It is assumed that on the collection plane the electron cloud from 
 * each ionisation point is spread according to the Gaussian distribution 
 * whose width  is proportional to the square-root of the drift distance. 
 * The charge on each fired pixel is then calculated as a sum of contributions 
 * from n Gaussians.  
 * The output of the processor is the collection of MVD digis.
 *
 * Input collection 
 * Processor requires collection of simulated mvd points. 
 * Output
 * Processor produces an output collection of Mvd digis
 *
 * @param fCutOnDeltaRays: cut on the energy of delta-electrons (in MeV) 
 * (default parameter value : 0.0016972 ) 
 *
 * @param fDiffusionCoefficient: diffusion coefficient for the nominal active layer thickness (in cm) 
 * (default parameter value : 0.0055) 
 *
 * @param fEpiThickness: thickness of the active Silicon layer (in cm) 
 * (default parameter value : 0.0014) 
 *
 * @param PixelSizeX: pixel size along X direction (in cm)
 * (default value : 0.003) 
 *
 * @param PixelSizeY: pixel size along Y direction (in cm) 
 * (default value : 0.003) 
 *
 * @param fElectronsPerKeV: number of electrons produced per KeV of deposited energy 
 * (default parameter value : 276) 
 *
 * @param fChargeThreshold: threshold on charge deposited on one pixel (in electrons) 
 * (default parameter value : 1 ) 
 *
 * @param fSegmentLength: segment length along track path which is used to subdivide track into segments (in cm).
 * The number of track subsegments is calculated as int(TrackLengthWithinActiveLayer/SegmentLength)+1 
 * (default parameter value : 0.0001) 
 *
 * @param fWidthOfCluster: defines width in Gaussian sigmas to perform charge integration for
 * a given pixel 
 * (default parameter value : 3.0) 
 * 
 * original author: A. Raspereza, MPI Munich
 *
 * ____________________________________________________________________________________________
 * --------------------------------------------------------------------------------------------
 * adaptation for CBM: C.Dritsa
 * Acknowlegments to:
 *	Rita de Masi (IPHC, Strasbourg), M.Deveaux (IKF, Frankfurt), V.Friese (GSI, Darmstadt)
 * ____________________________________________________________________________________________
 * --------------------------------------------------------------------------------------------
 */


// Includes from MVD
#include "CbmMvdDigitiser.h"

#include "CbmMvdGeoPar.h"
#include "CbmMvdHit.h"
#include "CbmMvdHitMatch.h"
#include "CbmMvdPileupManager.h"
#include "CbmMvdPoint.h"

// Includes from base
#include "CbmGeoNode.h"
#include "CbmRootManager.h"
#include "CbmRunAna.h"
#include "CbmRuntimeDb.h"
#include "CbmMCTrack.h"

// Includes from ROOT
#include "TArrayD.h"
#include "TClonesArray.h"
#include "TObjArray.h"
#include "TRandom3.h"
#include "TString.h"
#include "TVector3.h"
#include "TMath.h"
#include "TH1.h"
#include "TH2.h"

// Includes from C++
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>

#include "gsl_sf_erf.h"
#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Random/RandPoisson.h"
#include "CLHEP/Random/RandFlat.h"


using std::cout;
using std::endl;
using std::map;
using std::setw;
using std::left;
using std::right;
using std::fixed;
using std::pair;
using std::setprecision;
using std::ios_base;
using std::vector;


// -----   Default constructor   ------------------------------------------
CbmMvdDigitiser::CbmMvdDigitiser()
    : CbmTask("MVDDigitiser")
{
    fMode          = 0;
    fBranchName    = "MVDPoint";
    fDigis         = new TClonesArray("CbmMvdDigi");
    fPixelCharge   = new TClonesArray("CbmMvdPixelCharge");
    fPileupManager = NULL;
    fDeltaManager  = NULL;
    fRandGen.SetSeed(2736);
    fEvent       = 0;
    fTime        = 0.;
    fSigmaX      = 0.0005;
    fSigmaY      = 0.0005;
    fNPileup     = 0;
    fNDeltaElect = 0;
    fBgFileName    = "";
    fDeltaFileName = "";
    fBgBufferSize    = 1000;
    fDeltaBufferSize = 10000;

    fPoints=new TRefArray();
    fFluctuate = new MyG4UniversalFluctuationForSi();

    fEpiTh         = 0.0014;
    fSegmentLength = 0.0001;
    fDiffusionCoefficient = 0.0055; // correspondes to the sigma of the gauss with the max drift length
    fElectronsPerKeV = 276; //3.62 eV for e-h creation
    fWidthOfCluster  = 3; // in sigmas
    fPixelSizeX = 0.0030; // in cm
    fPixelSizeY = 0.0030;
    fCutOnDeltaRays  = 0.00169720;  //MeV
    fChargeThreshold = 1; //electrons
    fFanoSilicium    = 0.115;
    fEsum            = 0;
    fSegmentDepth    = 0;
    fCurrentTotalCharge      = 0;
    fCurrentParticleMass     = 0;
    fCurrentParticleMomentum = 0;
    fPixelScanAccelerator    = 0;


}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmMvdDigitiser::CbmMvdDigitiser(const char* name, Int_t iMode,
				 Int_t iVerbose)
: CbmTask(name, iVerbose)
{
    fMode          = iMode;
    fBranchName    = "MVDPoint";
    fDigis         = new TClonesArray("CbmMvdDigi");
    fPixelCharge   = new TClonesArray("CbmMvdPixelCharge");
    fPileupManager = NULL;
    fDeltaManager  = NULL;
    fRandGen.SetSeed(2736);
    fEvent       = 0;
    fTime        = 0.;
    fSigmaX      = 0.0005;
    fSigmaY      = 0.0005;
    fNPileup     = 0;
    fNDeltaElect = 0;
    fBgFileName    = "";
    fDeltaFileName = "";
    fBgBufferSize    = 1000;
    fDeltaBufferSize = 10000;

    fPoints    = new TRefArray();
    fFluctuate = new MyG4UniversalFluctuationForSi();

    fEpiTh         = 0.0014;
    fSegmentLength = 0.0001;
    fDiffusionCoefficient = 0.0055; // correspondes to the sigma of the gauss with the largest drift length
    fElectronsPerKeV = 276; //3.62 eV deposited for creation of 1 pair electron/hole
    fWidthOfCluster  = 3; // in sigmas
    fPixelSizeX      = 0.0030; // in cm
    fPixelSizeY      = 0.0030;
    fCutOnDeltaRays  = 0.00169720; //MeV
    fChargeThreshold = 1; //charge threshold above which the digis are filled
    fFanoSilicium    = 0.115; //not used for the moment, 02 june 08
    
    fSegmentDepth        = 0;
    fCurrentTotalCharge  = 0;
    fCurrentParticleMass = 0;
    fCurrentParticleMomentum = 0;
    fPixelScanAccelerator    = 0;



    //h_trackLength   = new TH1F("h_trackLength", "h_trackLength", 2000, -0.03, 0.03);
    h_trackLength   = new TH1F("h_trackLength", "h_trackLength", 2000, 0., 0.05);
    h_LengthVsAngle = new TH2F("h_LengthVsAngle", "test", 1000, -0.3, 0.3, 1000, -0.3, 0.3);

}


// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmMvdDigitiser::~CbmMvdDigitiser() {

    if ( fDigis) {
	fDigis->Delete();
	delete fDigis;
    }
    if ( fPileupManager ) delete fPileupManager;
    if ( fDeltaManager ) delete fDeltaManager;

    delete h_LengthVsAngle;
    delete h_trackLength;

}


// -------------------------------------------------------------------------

void CbmMvdDigitiser::BuildEvent() {


    fPoints->Clear();

    //===============Include standard input into the input array

    Int_t i;
    
    for (i=0; i<fInputPoints->GetEntriesFast();i++)

    { CbmMvdPoint* point=(CbmMvdPoint*)fInputPoints->At(i);

      fPoints->AddLast(point);
      CbmMCTrack* track=(CbmMCTrack*)fMCTracks->At(point->GetTrackID());
      point->SetPdgCode(track->GetPdgCode());
     }

    

    //=============== Include pileup module ==============

    if (fNPileup>0) {
 
      // --- Vector of available background events from pile-up. 
      // --- Each event is used only once.
      Int_t nBuffer = fPileupManager->GetNEvents();
      vector<Int_t> freeEvents(nBuffer);
      for (Int_t i=0; i<nBuffer; i++) freeEvents[i] = i;

      // --- Loop over pile-up events
      for (Int_t iBg=0; iBg<fNPileup; iBg++) {

	// Select random event from vector and remove it after usage
	Int_t index = gRandom->Integer(freeEvents.size());
	Int_t iEvent = freeEvents[index];
	TClonesArray* points = fPileupManager->GetEvent(iEvent);
	freeEvents.erase(freeEvents.begin() + index);

	// Add points from this event to the input array
	for (Int_t iPoint=0; iPoint<points->GetEntriesFast(); iPoint++) 
	  fPoints->AddLast(points->At(iPoint));
      }

    }    // Usage of pile-up

			   


    // ========================== Delta electron module =================
   /*
    if (fNDeltaElect>0)
    {

	//Include Background events (for pileup) into the input array

	Int_t nEvents     = fDeltaManager->GetNEvents();
	Int_t firstEvent;

	// usedEvents containes the list of events in the bg-file
	// which have already been used in this event
	// This is done to avoid using one event twice
	// Volker is kindly asked not to change this concept.

	TArrayI usedEvents(fNDeltaElect);
	for(i=0; i<fNDeltaElect; i++) {usedEvents[i]=-1;}

	//Decide, which events should be used

	do {
    
	    firstEvent  = gRandom->Integer(nEvents);
	    i=0;
	    Bool_t eventFound=0;

	    // Check, if event has already been used. Else use it

	    do {

		if (usedEvents[i]==-1){
		    usedEvents[i]=firstEvent;
		    eventFound=1;}

	     	    else if (usedEvents[i]==firstEvent){eventFound=1;}
		i=i+1;

	    } while ((i<fNDeltaElect) && (eventFound==0));

	} while (usedEvents[fNDeltaElect-1]==-1);

	// Add the events to the input array



	for (i=0; i<fNDeltaElect;i++)
	{
	    // Read event from the Bg-Buffer
	    TClonesArray* points=fDeltaManager->GetEvent(usedEvents[i]);

	    // Fill event into the point-Buffer
	    for (Int_t j=0; j<points->GetEntriesFast();j++)

	    {fPoints->AddLast(points->At(j));}
	}
    };
    */


    // cdritsa: copy from Volker's version on pile-up also for deltas

    if (fNDeltaElect>0) {
 
      // --- Vector of available delta events.
      // --- Each event is used only once.
      Int_t nDeltaBuffer = fDeltaManager->GetNEvents();
      vector<Int_t> freeDeltaEvents(nDeltaBuffer);
      for (Int_t i=0; i<nDeltaBuffer; i++) freeDeltaEvents[i] = i;

      // --- Loop over delta events
      for (Int_t it=0; it<fNDeltaElect; it++) {

	// Select random event from vector and remove it after usage
	Int_t indexD  = gRandom->Integer(freeDeltaEvents.size());
	Int_t iEventD = freeDeltaEvents[indexD];
	TClonesArray* pointsD = fDeltaManager->GetEvent(iEventD);
	freeDeltaEvents.erase(freeDeltaEvents.begin() + indexD);

	// Add points from this event to the input array
	for (Int_t iPoint=0; iPoint<pointsD->GetEntriesFast(); iPoint++)
	  fPoints->AddLast(pointsD->At(iPoint));
      }

    }    // Usage of delta




    // ========================== End of delta electron module =================
    cout << "Event building completed. Treating "<< fPoints->GetEntriesFast() << " MCPoints."<< endl;
}



// -----   Virtual public method Exec   ------------------------------------
void CbmMvdDigitiser::Exec(Option_t* opt) {



    fDigis->Clear("C");

    BuildEvent(); // Composes input array from signal and background files


    fEvent++;
    TVector3 mom;

    // First loop on MCpoints; Define min and max detectorID
    Int_t nPoints = fPoints->GetEntriesFast();
    Int_t minId;
    Int_t maxId;
    Int_t volId;
    Int_t mcTrackIndex;
    Int_t iPoint=0;
    CbmMvdPoint* point = NULL;
    point = (CbmMvdPoint*) fPoints->At(iPoint);

    minId = point->GetDetectorID();
    maxId = point->GetDetectorID();

    for (iPoint=0; iPoint<nPoints; iPoint++) {
	point = (CbmMvdPoint*) fPoints->At(iPoint);
	volId = point->GetDetectorID();
	if ( volId<minId ) minId = volId;
	if ( volId>maxId ) maxId = volId;
    }
    //====================================================


    //loop on mvd stations
    for (Int_t stationOfInterest = minId; stationOfInterest<(maxId+1);  stationOfInterest++)
    {
	fPixelCharge->Clear("C");

	// Clear charge map
	fChargeMap.clear();

	//Loop on MC points

	for (iPoint=0; iPoint<nPoints; iPoint++) {

	    point = (CbmMvdPoint*) fPoints->At(iPoint);
	    Int_t volId   = point->GetDetectorID();

	    if ( volId!=stationOfInterest ) continue;

	    SetMvdGeometry(volId);

	    Int_t trackId = point->GetTrackID();
	    Int_t statNr  = fStationMap[volId];
	    Int_t pdgCode = point->GetPdgCode();

	    //momentum at the entrance point????
	    point->Momentum(mom); 

	    fCurrentParticleMomentum = mom.Mag();

	    // reject temporarily light nuclei. they have to be added in the data base...
	    if ( pdgCode > 100000)   	continue;
	    //--------------------------------------------------------------------------

	    fCurrentParticleMass = TDatabasePDG::Instance()->GetParticle( pdgCode )->Mass();
	    ProduceIonisationPoints( point );
	    ProduceSignalPoints();
	    ProducePixelCharge();

	    CbmMvdPixelCharge* pixelCharge;

	    for(Int_t f=0; f<fPixelCharge->GetEntriesFast(); f++)
	    {
		pixelCharge = (CbmMvdPixelCharge*) fPixelCharge->At(f);
		pixelCharge->DigestCharge( ( (float)( point->GetX()+point->GetXOut() )/2 ) , ( (float)( point->GetY()+point->GetYOut() )/2 )  );
	    };

	  } //loop on MCpoints

	  for (Int_t i=0; i<fPixelCharge->GetEntriesFast(); i++)
	  {
	      CbmMvdPixelCharge* pixel = (CbmMvdPixelCharge*)fPixelCharge->At(i);
	      if ( pixel->GetCharge()>fChargeThreshold )
	      {
		  Int_t nDigis = fDigis->GetEntriesFast();
		  new ((*fDigis)[nDigis])
		      CbmMvdDigi(stationOfInterest,
				 pixel->GetX(), pixel->GetY(),
				 pixel->GetCharge(),
				 fPixelSizeX, fPixelSizeY,
				 pixel->GetPointX(), pixel->GetPointY(),
				 pixel->GetContributors(),
				 pixel->GetMaxChargeContribution());
	      }
	  }
	  //------------------------------------------------------------------------------

    }//loop on mvd stations

    cout << "Event Nr: " << fEvent << " --- nPoints --- " << nPoints << endl;
    cout << " => Number of Digis: " << fDigis->GetEntriesFast() <<"\n"<< endl;



}// end of exec

// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmMvdDigitiser::ProduceIonisationPoints(CbmMvdPoint* point) {
    /** Produces ionisation points along track segment within active Silicon layer.
     */



    Int_t volId   = point->GetDetectorID();
    SetMvdGeometry(volId);
 
    // entry and exit from the det layer ( detector ref frame ) :
    // -------------OK-------------------------------------------//
    Double_t entryZ = -fLayerTh/2;                               //
    Double_t exitZ  =  fLayerTh/2;                               //
                                                                 //
    Double_t entryX = point->GetX()    + fLayerRadius;           //
    Double_t exitX  = point->GetXOut() + fLayerRadius;           //
    Double_t entryY = point->GetY()    + fLayerRadius;           //
    Double_t exitY  = point->GetYOut() + fLayerRadius;           //
                                                                 //
    Double_t lxDet  = TMath::Abs(entryX-exitX);                  //
    Double_t lyDet  = TMath::Abs(entryY-exitY);                  //
    Double_t lzDet  = TMath::Abs(entryZ-exitZ);                  //
    //-----------------------------------------------------------//


    /**

    Create vector entryDet a (x1,y1,z1) = entry in detector
    Create vector exitDet  b (x2,y2,z2) = exit from detector

    Substract   b-a and get the vector "c" giving the direction of the particle.

    Scale the vector c (draw the 3D schema and check the similar triangles)

    Add vector a.

    The result is a vector with starting point [(x,y,z)entry in detector]
    and end point [(x,y,z)entry in the epi layer]

    same for defining exit from epi layer.
    **/


    // entry and exit from the epi layer ( det ref frame ) :
    Double_t entryZepi = -fEpiTh/2;
    Double_t exitZepi  =  fEpiTh/2;

    //Double_t lxzDet = sqrt(lxDet*lxDet + lzDet*lzDet);
    //Double_t lyzDet = sqrt(lyDet*lyDet + lzDet*lzDet);

    TVector3  a( entryX, entryY, entryZ ); // entry in the detector
    TVector3  b( exitX,  exitY,  exitZ  ); // exit from the detector
    TVector3  c;

    c = b-a;  // AB in schema

    TVector3 d;
    TVector3 e;

    Double_t scale1 = (entryZepi-entryZ)/(exitZ-entryZ);
    Double_t scale2 = (exitZepi-entryZ)/(exitZ-entryZ);


    d = c*scale1;
    e = c*scale2;

    TVector3 entryEpiCoord;
    TVector3 exitEpiCoord;

    entryEpiCoord = d+a;
    exitEpiCoord  = e+a;


    //Get x and y coordinates at the ENTRY of the epi layer
    Double_t entryXepi = entryEpiCoord.X();
    Double_t entryYepi = entryEpiCoord.Y();
             entryZepi = entryEpiCoord.Z();

    //Get x and y coordinates at the EXIT of the epi layer
    Double_t exitXepi = exitEpiCoord.X();
    Double_t exitYepi = exitEpiCoord.Y();
             exitZepi = exitEpiCoord.Z();

    Double_t lx        = TMath::Abs(entryXepi-exitXepi); //length of segment x-direction
    Double_t ly        = TMath::Abs(entryYepi-exitYepi);
    Double_t lz        = TMath::Abs(entryZepi-exitZepi);
    //-----------------------------------------------------------


    Double_t rawLength = sqrt( lx*lx + ly*ly + lz*lz );  //length of the track inside the epi-layer, in cm
    Double_t trackLength = 0;

    if(rawLength<1.0e+3) { trackLength = rawLength; }
    else{
	cout << "-W- "<< GetName() << " : rawlength > 1.0e+3 : "<< rawLength << endl;
	trackLength = 1.0e+3;
    }


    //Smear the energy on each track segment
    Double_t dEmean = point->GetEnergyLoss()*fEpiTh/fLayerTh; // dEmean: energy loss corresponds to the epi thickness
    fNumberOfSegments = int(trackLength/fSegmentLength) + 1;
    dEmean = dEmean/((Double_t)fNumberOfSegments); // From this point dEmean corresponds to the E lost per segment.
   
    fSignalPoints.resize(fNumberOfSegments);

    fEsum = 0.0;

    Double_t segmentLength_update = trackLength/((Double_t)fNumberOfSegments);

    if( lz!=0 ){
	/**
	 condition added 05/08/08 because if lz=0 then there is no segment
         projection (=fSegmentDepth)
	 **/
	fSegmentDepth = fEpiTh/((Double_t)fNumberOfSegments);
    }
    else{//condition added 05/08/08
	fSegmentDepth = 0;
        cout << "-W- " << GetName() << " Length of track in detector (z-direction) is 0!!!" << endl;
    }


    Double_t x=0,y=0,z=0;

    for (int i=0; i<fNumberOfSegments; ++i) {
       
	z = -fEpiTh/2 + ((double)(i)+0.5)*fSegmentDepth; //middle position of the segment; zdirection
	x = entryXepi + ((double)(i)+0.5)*( lx/( (Double_t)fNumberOfSegments) ); //middle position of the segment; xdirection
        y = entryYepi + ((double)(i)+0.5)*( ly/( (Double_t)fNumberOfSegments) ); //middle position of the segment; ydirection


	SignalPoint* spoint=&fSignalPoints[i];
	Double_t de;

	if( fCurrentParticleMass!=0 ){
	    de = fFluctuate->SampleFluctuations( double(1000.*fCurrentParticleMomentum), double(1000.*fCurrentParticleMass), fCutOnDeltaRays, segmentLength_update*10, double(1000.*dEmean) ) /1000.;
	}
	else {  de = dEmean;  }



	fEsum = fEsum + de;
	spoint->eloss = de;
	spoint->x = x; //here the coordinates x,y,z are given in the detector reference frame.
	spoint->y = y;
	spoint->z = z;

        // --- debug 05/08/08 start -------

	Int_t pdgCode = point->GetPdgCode();


	x=x-fLayerRadius;
        y=y-fLayerRadius;

	if (sqrt(x*x + y*y )< 0.5) {
	    cout << "point->GetX()= " <<  point->GetX() <<  " , point->GetXOut()= " << point->GetXOut()  << " , pdg code " << pdgCode<< endl;
	    cout << "point->GetY()= " <<  point->GetY() <<  " , point->GetYOut()= " << point->GetYOut() << endl;
	    cout << "point->GetZ()= " <<setprecision(8)<<  point->GetZ() <<  " , point->GetZOut()= " <<setprecision(8)<< point->GetZOut() << endl;
	}

	// --- debug 05/08/08 end -------
      

    }




}


// -------------------------------------------------------------------------
void CbmMvdDigitiser::ProduceSignalPoints() {
    /** Produces signal points on the collection plane.
     */

    

    // loop over ionisation points
    for (Int_t i=0; i<fNumberOfSegments; ++i) {

	SignalPoint* spoint = &fSignalPoints[i];
        Double_t de = spoint->eloss;
	Double_t DriftLength = fEpiTh/2 - spoint->z;
        Double_t SigmaDiff = sqrt(DriftLength/fEpiTh)*fDiffusionCoefficient;

        Double_t charge      = 1.0e+6*de*fElectronsPerKeV;

	//Double_t chargeFluct = gRandom->Gaus( 0, TMath::Sqrt(charge*fFanoSilicium) );
	//charge = charge + chargeFluct;

	spoint->sigmaX = SigmaDiff;
	spoint->sigmaY = SigmaDiff;
	spoint->charge = charge;

    }


}
// -------------------------------------------------------------------------

void CbmMvdDigitiser::ProducePixelCharge() {
    /** Simulation of fired pixels. Each fired pixel is considered
     * as SimTrackerHit
     */

    fCurrentTotalCharge = 0.0;

    for (int i=0; i<fNumberOfSegments; ++i) {
	SignalPoint* spoint = &fSignalPoints[i];

	Double_t xCentre = spoint->x;  //of segment
	Double_t yCentre = spoint->y;  /// idem
	Double_t sigmaX  = spoint->sigmaX;
	Double_t sigmaY  = spoint->sigmaY;
        
	Double_t xLo = spoint->x - fWidthOfCluster*spoint->sigmaX;
	Double_t xUp = spoint->x + fWidthOfCluster*spoint->sigmaX;

	Double_t yLo = spoint->y - fWidthOfCluster*spoint->sigmaY;
	Double_t yUp = spoint->y + fWidthOfCluster*spoint->sigmaY;

	fCurrentTotalCharge += spoint->charge;
	Int_t ixLo, ixUp, iyLo, iyUp;

	TransformXYtoPixelIndex(xLo,yLo,ixLo,iyLo);
	TransformXYtoPixelIndex(xUp,yUp,ixUp,iyUp);
        
	// Loop over all fired pads
	// and calculate deposited charges
	for (int ix = ixLo; ix<ixUp+1; ix++) {

	    for (int iy = iyLo; iy<iyUp+1; iy++) {
	    
			Double_t xCurrent,yCurrent;
			TransformPixelIndexToXY(ix,iy,xCurrent,yCurrent);
			gsl_sf_result result;
			Int_t status = gsl_sf_erf_Q_e(float((xCurrent - 0.5*fPixelSizeX - xCentre)/sigmaX), &result);
			Float_t LowerBound = 1 - result.val;  // substract from 1 because result.val is a probability
			status = gsl_sf_erf_Q_e(float((xCurrent + 0.5*fPixelSizeX - xCentre)/sigmaX), &result);
			Float_t UpperBound = 1 - result.val;
			Float_t integralX = UpperBound - LowerBound;
			status = gsl_sf_erf_Q_e(float((yCurrent - 0.5*fPixelSizeY - yCentre)/sigmaY), &result);
			LowerBound = 1 - result.val;
			status = gsl_sf_erf_Q_e(float((yCurrent + 0.5*fPixelSizeY - yCentre)/sigmaY), &result);
			UpperBound = 1 - result.val;
			Float_t integralY = UpperBound - LowerBound;
			Int_t totCharge = int((spoint->charge)*integralX*integralY);

  			CbmMvdDigi * digi = 0;

			if(totCharge>0){AddChargeToPixel(ix,iy,totCharge);};

	    }//for y
	}// for x
    }// for number of segments

}//end of function

// -------------------------------------------------------------------------
void CbmMvdDigitiser::TransformXYtoPixelIndex(Double_t x, Double_t y,Int_t & ix, Int_t & iy) {
    /**
     * Function calculates position in pixel matrix based on the
     * local coordinates of point in the ladder.
     */
   
    iy = int(y/fPixelSizeY);
    ix = int(x/fPixelSizeX);
}

// ---------------------------------------------------------------------------

void CbmMvdDigitiser:: AddChargeToPixel(Int_t channelX, Int_t channelY, Int_t charge){
    // Adds the charge of a hit to the pixels. Checks if the pixel was hit before.

    CbmMvdPixelCharge* pixel;


    // Look for pixel in charge map
    pair<Int_t, Int_t> a(channelX, channelY);
    fChargeMapIt = fChargeMap.find(a);

    // Pixel not yet in map -> Add new pixel
    if ( fChargeMapIt == fChargeMap.end() ) {
	CbmMvdPixelCharge* pixel= new ((*fPixelCharge)[fPixelCharge->GetEntriesFast()])
	    CbmMvdPixelCharge(charge, channelX, channelY);
	fChargeMap[a] = pixel;
    }

    // Pixel already in map -> Add charge
    else {
	CbmMvdPixelCharge* pixel = fChargeMapIt->second;
	if ( ! pixel ) Fatal("AddChargeToPixel", "Zero pointer in charge map!");
	pixel->AddCharge(charge);
    }

}


// -------------------------------------------------------------------------

void CbmMvdDigitiser::TransformPixelIndexToXY(Int_t ix, Int_t iy, Double_t & x, Double_t & y) {

    /**
     Function calculates position in the local frame
     based on the index of pixel in the ladder.
     */

    //x,y are calculated at the center of the pixel:
    y = ( 0.5+double(iy) )*fPixelSizeY;
    x = ( 0.5+double(ix) )*fPixelSizeX;
}
// -------------------------------------------------------------------------


void CbmMvdDigitiser::SetMvdGeometry(Int_t detId){


    /*

    //*********************************
    if ( detId == 15 ){
	fLayerRadius = 5; //cm
	fLayerTh = 0.015; //cm
	fLayerPosZ   = 10;//cm
	fLayerRadiusInner = 0.55; // cm
    }
    else if( detId == 16 ){
	fLayerRadius = 10; //cm
	fLayerTh = 0.015; //cm
	fLayerPosZ   = 20;//cm
	fLayerRadiusInner = 1.05; //cm
    }
    //*********************************

    */



    //*********** mvd geometry*********
    if ( detId == 57 ){
	fLayerRadius = 2.5; //cm
	fLayerTh = 0.05; //cm
	fLayerPosZ   = 5;//cm
	fLayerRadiusInner = 1; // cm
    }
    else if( detId == 58 ){
	fLayerRadius = 3.5; //cm
	fLayerTh = 0.05; //cm
	fLayerPosZ   = 7;//cm
	fLayerRadiusInner = 1; //cm
    }
    else if( detId == 59 ){
	fLayerRadius = 5; //cm
	fLayerTh = 0.05; //cm
	fLayerPosZ   = 10;//cm
	fLayerRadiusInner = 0.8; //cm
    }
    else if( detId == 60 ){
	fLayerRadius = 6; //cm
	fLayerTh = 0.05; //cm
	fLayerPosZ   = 12;//cm
	fLayerRadiusInner = 0.8; //cm
    }
    else if( detId == 61 ){
	fLayerRadius = 7.5; //cm
	fLayerTh = 0.05; //cm
	fLayerPosZ   = 15;//cm
	fLayerRadiusInner = 0.8; //cm
    }
    else if( detId == 62 ){
	fLayerRadius = 8.5; //cm
	fLayerTh = 0.05; //cm
	fLayerPosZ   = 17;//cm
	fLayerRadiusInner = 0.8; //cm
    }
    else if( detId == 63 ){
	fLayerRadius = 10; //cm
	fLayerTh = 0.05; //cm
	fLayerPosZ   = 20;//cm
	fLayerRadiusInner = 0.8; //cm
    }

    //*********************************



}


// -------------------------------------------------------------------------

//void CbmMvdDigitiser::PoissonSmearer( SimTrackerHitImplVec & simTrkVec ) {
    /**
     * Function that fluctuates charge (in units of electrons)
     * deposited on the fired pixels according to the Poisson
     * distribution...
     */
/*
    for (int ihit=0; ihit<int(simTrkVec.size()); ++ihit) {
	SimTrackerHitImpl * hit = simTrkVec[ihit];
	double charge = hit->getdEdx();
	double rng;
	if (charge > 1000.) { // assume Gaussian
	    double sigma = sqrt(charge);
	    rng = double(RandGauss::shoot(charge,sigma));
	    hit->setdEdx(rng);
	}
	else { // assume Poisson
	    rng = double(RandPoisson::shoot(charge));
	}
	hit->setdEdx(float(rng));
    }
} */
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------



// -----   Virtual private method SetParContainers   -----------------------
void CbmMvdDigitiser::SetParContainers() {
    CbmRunAna*    ana  = CbmRunAna::Instance();
    CbmRuntimeDb* rtdb = ana->GetRuntimeDb();
    fGeoPar  = (CbmMvdGeoPar*)  (rtdb->getContainer("CbmMvdGeoPar"));
}
// -------------------------------------------------------------------------


			      
// -----    Virtual private method Init   ----------------------------------
InitStatus CbmMvdDigitiser::Init() {

  
  // ************* Get input array
  
  CbmRootManager* ioman = CbmRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- " << GetName() << "::Init: No CbmRootManager!" << endl;
    return kFATAL;
  }
  fInputPoints = (TClonesArray*) ioman->GetObject(fBranchName);
  fMCTracks    = (TClonesArray*) ioman->GetObject("MCTrack");
  
  
  Register(); // Register output arrays



  // ************** Check for too many pileup events

  if (fNPileup > 200) {
      cout << "-E- CbmMvdDigitiser::Init:  Pileup of " << fNPileup
	  << " too large; maximum buffer size is 1000 events." << endl;
      return kERROR;
  }

  if (fNDeltaElect > 20000) {
      cout << "-E- CbmMvdDigitiser::Init:  Delta Pileup of " << fNPileup
	  << " too large; maximum buffer size is 100000 events." << endl;
      return kERROR;
  }


  // Create BgManager(s) if necessary

  if (fNPileup >= 1 && !(fPileupManager) && fMode == 0 ) {
      if ( fBgFileName == "" ) {
	  cout << "-E- " << GetName() << "::Init: Pileup events needed, but no "
	      << " background file name given! " << endl;
	  return kERROR;
      }
      fPileupManager = new CbmMvdPileupManager(fBgFileName,
					       fBranchName, fBgBufferSize);
  }

  // Create Manager for Delta electron file

  if (fNDeltaElect >= 1 && !(fDeltaManager) && fMode == 0 ) {
      if ( fDeltaFileName == "" ) {
	  cout << "-E- " << GetName() << "::Init: Pileup events needed, but no "
	      << " background file name given! " << endl;
	  return kERROR;
      }
      fDeltaManager = new CbmMvdPileupManager(fDeltaFileName,
					      fBranchName, fDeltaBufferSize);
  }


  // Get parameters from database
  Bool_t success = GetParameters();
  if ( ! success ) {
      cout << "-E- " << GetName() << "::Init: "
	  << "Error in accessing parameters" << endl;
      return kERROR;
  }



  // Screen output
  cout << endl << "---------------------------------------------" << endl;
  cout << GetName() << " initialised with parameters: " << endl;
  PrintParameters();
  cout << "---------------------------------------------" << endl;



}
// -------------------------------------------------------------------------



// -----   Virtual public method Reinit   ----------------------------------
InitStatus CbmMvdDigitiser::ReInit() {
   return Init();
  /* // Get parameters from database
  Bool_t success = GetParameters();
  if ( ! success ) {
    cout << "-E- " << GetName() << "::Reinit: "
	 << "Error in accessing parameters" << endl;
    return kERROR;
  }

  // Screen output
  cout << endl << "---------------------------------------------" << endl;
  cout << GetName() << " reinitialised with parameters: " << endl;
  PrintParameters();
  cout << "---------------------------------------------" << endl;

  return kSUCCESS; */

}
// -------------------------------------------------------------------------



// -----   Virtual method Finish   -----------------------------------------
void CbmMvdDigitiser::Finish() {
    cout.setf(ios_base::fixed, ios_base::floatfield);
    cout << endl << "---------------------------------------------" << endl;
    cout << "CbmMvdDigitiser:: Parameters used for digitisation: " << endl;
    cout << "Pixel Size X               : " << setw(8) << setprecision(2)
	<< fPixelSizeX * 10000. << " mum" << endl;
    cout << "Pixel Size Y               : " << setw(8) << setprecision(2)
	<< fPixelSizeY * 10000. << " mum" << endl;
    cout << "Epitaxial layer thickness  : " << setw(8) << setprecision(2)
	<< fEpiTh * 10000. << " mum" << endl;
    cout << "Segment Length             : " << setw(8) << setprecision(2)
	<< fSegmentLength * 10000. << " mum" << endl;
    cout << "Diffusion Coefficient      : " << setw(8) << setprecision(2)
	<< fDiffusionCoefficient * 10000. << " mum" << endl;
    cout << "Width of Cluster           : " << setw(8) << setprecision(2)
	<< fWidthOfCluster << " * sigma " << endl;
    cout << "ElectronsPerKeV 3.62 eV/eh : " << setw(8) << setprecision(2)
	<< fElectronsPerKeV  <<  endl;
    cout << "CutOnDeltaRays             : " << setw(8) << setprecision(8)
	<< fCutOnDeltaRays  << " MeV " <<  endl;
    cout << "ChargeThreshold            : " << setw(8) << setprecision(2)
	<< fChargeThreshold  <<  endl;
    cout << "---------------------------------------------" << endl;

	
}					       
// -------------------------------------------------------------------------



// -----   Private method Register   ---------------------------------------
void CbmMvdDigitiser::Register() {
  CbmRootManager* ioman = CbmRootManager::Instance();
  if ( ! ioman) Fatal("Register",
		      "No CbmRootManager");
  ioman->Register("CbmMvdRaw", "MVDRawData", fDigis, kTRUE);

}
// -------------------------------------------------------------------------  



// -----   Private method Reset   ------------------------------------------
void CbmMvdDigitiser::Reset() {
    fDigis->Clear("C");

}
// -------------------------------------------------------------------------  



// -----   Private method PrintParameters   --------------------------------
void CbmMvdDigitiser::PrintParameters() {
    cout.setf(ios_base::fixed, ios_base::floatfield);
    cout << "Pixel Size X               : " << setw(8) << setprecision(2)
	<< fPixelSizeX * 10000. << " mum" << endl;
    cout << "Pixel Size Y               : " << setw(8) << setprecision(2)
	<< fPixelSizeY * 10000. << " mum" << endl;
    cout << "Epitaxial layer thickness  : " << setw(8) << setprecision(2)
	<< fEpiTh * 10000. << " mum" << endl;
    cout << "Segment Length             : " << setw(8) << setprecision(2)
	<< fSegmentLength * 10000. << " mum" << endl;
    cout << "Diffusion Coefficient      : " << setw(8) << setprecision(2)
	<< fDiffusionCoefficient * 10000. << " mum" << endl;
    cout << "Width of Cluster           : " << setw(8) << setprecision(2)
	<< fWidthOfCluster << " * sigma "<< endl;
    cout << "ElectronsPerKeV 3.62 eV/eh : " << setw(8) << setprecision(2)
	<< fElectronsPerKeV  <<  endl;
    cout << "CutOnDeltaRays             : " << setw(8) << setprecision(8)
	<< fCutOnDeltaRays  << " MeV " <<  endl;
    cout << "ChargeThreshold            : " << setw(8) << setprecision(2)
	<< fChargeThreshold  <<  endl;

    cout << "Stations used: " << endl;

    map<Int_t, Int_t>::iterator mapIt;
    for (mapIt=fStationMap.begin(); mapIt!=fStationMap.end(); mapIt++)
	cout << "MVD Station " << (*mapIt).second << ", volume ID "
	    << (*mapIt).first << endl;

}
// -------------------------------------------------------------------------  



// -----   Private method GetParameters   ----------------------------------
Bool_t CbmMvdDigitiser::GetParameters() {

  fStationMap.clear();
  fRadiusMap.clear();
  if ( ! fGeoPar ) return kFALSE;
  TObjArray* sensNodes = fGeoPar->GetGeoSensitiveNodes();
  Int_t nNodes = sensNodes->GetEntriesFast();
  for (Int_t iNode=0; iNode<nNodes; iNode++) {
      CbmGeoNode* node = (CbmGeoNode*) (sensNodes->At(iNode));
      Int_t volId = node->getMCid();
      TString nodeName = node->getName();
      char a[2];
      a[0] = nodeName[10];
      a[1] = nodeName[11];
      Int_t stationNr = atoi(a);
      fStationMap[volId] = stationNr;
      TArrayD* params = node->getParameters();
      Double_t rMin  = params->At(0);
      Double_t rMax  = params->At(1);
      Double_t thick = 2*( params->At(2) ); //cdritsa
      pair<Double_t, Double_t> radii(rMin, rMax);
      fRadiusMap[stationNr] = radii;
  }

  return kTRUE;
}



ClassImp(CbmMvdDigitiser);
