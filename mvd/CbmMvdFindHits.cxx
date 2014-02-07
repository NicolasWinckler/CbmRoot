// --------------------------------------------------------------------------------------------
// -----                    CbmMvdFindHits source file                 --------
// -----                  Created 06/05/08  by C. Dritsa               -----
// -----   Code tuning and maintainance M.Deveaux 01/07/2010     -----
// ---------------------------------------------------------------------------------------------
// Includes from MVD
#include "CbmMvdGeoPar.h"
#include "CbmMvdHit.h"
#include "CbmMvdCluster.h"
#include "CbmMvdHitMatch.h"
#include "CbmMvdPileupManager.h"
#include "CbmMvdPoint.h"
#include "CbmMvdDigitize.h"
#include "CbmMvdFindHits.h"
#include "CbmMvdStation.h"

// Includes from base
#include "FairGeoNode.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "CbmMCTrack.h"

// Includes from ROOT
#include "TGeoManager.h"
#include "TGeoTube.h"
#include "TArrayD.h"
#include "TObjArray.h"
#include "TRefArray.h"
#include "TCanvas.h"

#include "TClonesArray.h"

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
CbmMvdFindHits::CbmMvdFindHits()
  : FairTask("MVDHitFinder"),
    fAdcDynamic(150),
    fAdcOffset(0),
    fAdcBits(1),
    fAdcSteps(0),
    fAdcStepSize(0.),
    fDigis(NULL),
    fHits(new TClonesArray("CbmMvdHit")),
    fClusters(new TClonesArray("CbmMvdCluster")),
    fMatches(new TClonesArray("CbmMvdHitMatch")),
    fPixelChargeHistos(NULL),
    fTotalChargeInNpixelsArray(NULL),
    fResolutionHistoX(NULL),
    fResolutionHistoY(NULL),
    fResolutionHistoCleanX(NULL),
    fResolutionHistoCleanY(NULL),
    fResolutionHistoMergedX(NULL),
    fResolutionHistoMergedY(NULL),
    fBadHitHisto(NULL),
    fGausArray(NULL),
    fGausArrayIt(0),
    fGausArrayLimit(500000),
    fDigiMap(),
    fDigiMapIt(),
    h(NULL),
    h3(NULL),
    h1(NULL),
    h2(NULL),
    Qseed(NULL),
    c1(NULL),
    fNEvent(0),
    fMode(0),
    fCounter(0),
    fSigmaNoise(15.),
    fSeedThreshold(1.),
    fNeighThreshold(1.),
    fShowDebugHistos(kFALSE),
    fLayerRadius(0.),
    fLayerRadiusInner(0.),
    fLayerPosZ(0.),
    fHitPosX(0.),
    fHitPosY(0.),
    fHitPosZ(0.),
    fHitPosErrX(0.0005),
    fHitPosErrY(0.0005),
    fHitPosErrZ(0.0),
    fBranchName("MvdDigi"),
    fAddNoise(kFALSE),
    fStationMap() 
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmMvdFindHits::CbmMvdFindHits(const char* name, Int_t iMode,
			       Int_t iVerbose)
  : FairTask(name, iVerbose),
    fAdcDynamic(150),
    fAdcOffset(0),
    fAdcBits(1),
    fAdcSteps(0),
    fAdcStepSize(0.),
    fDigis(NULL),
    fHits(new TClonesArray("CbmMvdHit")),
    fClusters(new TClonesArray("CbmMvdCluster")),
    fMatches(new TClonesArray("CbmMvdHitMatch")),
    fPixelChargeHistos(NULL),
    fTotalChargeInNpixelsArray(NULL),
    fResolutionHistoX(NULL),
    fResolutionHistoY(NULL),
    fResolutionHistoCleanX(NULL),
    fResolutionHistoCleanY(NULL),
    fResolutionHistoMergedX(NULL),
    fResolutionHistoMergedY(NULL),
    fBadHitHisto(NULL),
    fGausArray(NULL),
    fGausArrayIt(0),
    fGausArrayLimit(500000),
    fDigiMap(),
    fDigiMapIt(),
    h(NULL),
    h3(NULL),
    h1(NULL),
    h2(NULL),
    Qseed(NULL),
    c1(NULL),
    fNEvent(0),
    fMode(iMode),
    fCounter(0),
    fSigmaNoise(15.),
    fSeedThreshold(1.),
    fNeighThreshold(1.),
    fShowDebugHistos(kFALSE),
    fLayerRadius(0.),
    fLayerRadiusInner(0.),
    fLayerPosZ(0.),
    fHitPosX(0.),
    fHitPosY(0.),
    fHitPosZ(0.),
    fHitPosErrX(0.0005),
    fHitPosErrY(0.0005),
    fHitPosErrZ(0.0),
    fBranchName("MvdDigi"),
    fAddNoise(kFALSE),
    fStationMap() 
{
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmMvdFindHits::~CbmMvdFindHits() {

    if ( fHits ) {
	fHits->Delete();
	delete fHits;
    }

    if ( fClusters ) {
	fClusters->Delete();
	delete fClusters;
    }

    if ( fDigis ) {
	fDigis->Delete();
	delete fDigis;
    }
    if ( fMatches ) {
	fMatches->Delete();
	delete fMatches;
    }

}
// -------------------------------------------------------------------------

// -----    Virtual private method Init   ----------------------------------
InitStatus CbmMvdFindHits::Init() {

  cout << endl;
  cout << "---------------------------------------------" << endl;
  cout << "-I- Initialising " << GetName() << " ...." << endl; 

    //11.12.08 cdritsa: start: copy Volker's code to parametrise mvd-geometry
    // *****  Get MVD geometry
    Int_t nStations = GetMvdGeometry();
    if ( ! nStations ) {
	cout << "-W- " << GetName() << "::Init: No MVD stations in geometry!"
	    << endl << "   Task will be inactive!" << endl;
	fActive = kFALSE;
    }
    //11.12.08 cdritsa: end-------------------------


    // Get input array
    FairRootManager* ioman = FairRootManager::Instance();
    if ( ! ioman ) {
	cout << "-E- " << GetName() << "::Init: No FairRootManager!" << endl;
	return kFATAL;
    }

    fDigis = (TClonesArray*) ioman->GetObject(fBranchName);
    if ( ! fDigis ) {
	cout << "-E- " << GetName() << "::Init: No FairRootManager!" << endl;
	return kFATAL;
    }
    Register();
    //if(fShowDebugHistos){
    fResolutionHistoX=new TH1F("SinglePointResolution_X","SinglePointResolution_X",10000,-0.0100,0.0100);
    fResolutionHistoY=new TH1F("SinglePointResolution_Y","SinglePointResolution_Y",10000,-0.0100,0.0100);
    fResolutionHistoCleanX=new TH1F("SinglePointResolution_X_Clean","SinglePointResolution_X_Clean",10000,-0.0100,0.0100);
    fResolutionHistoCleanY=new TH1F("SinglePointResolution_Y_Clean","SinglePointResolution_Y_Clean",10000,-0.0100,0.0100);
    fResolutionHistoMergedX=new TH1F("SinglePointResolution_X_Merged","SinglePointResolution_X_Merged",10000,-0.0100,0.0100);
    fResolutionHistoMergedY=new TH1F("SinglePointResolution_Y_Merged","SinglePointResolution_Y_Merged",10000,-0.0100,0.0100);
    fBadHitHisto            = new TH2F("BadHits","Hits above 0.003cm",1000,-2.5,2.5,1000,-2.5,2.5);
    //}

    TH1F* histo;
    TH1F* histoTotalCharge;
    char* histoName= new char[20];
    char* histoTotalChargeName= new char[50];

    //Add charge collection histograms
    fPixelChargeHistos=new TObjArray();
    for (Int_t i=0; i<49; i++)
    {
	sprintf(histoName,"ChargePixel%i",i+1);
	histo=new TH1F(histoName,histoName,100,0,12000);
	fPixelChargeHistos->AddLast(histo);
    };

     fTotalChargeInNpixelsArray = new TObjArray();
    for (Int_t i=0; i<49; i++)
    {
	sprintf(histoTotalChargeName,"totalChargeInNPixels%i",i+1);
	histoTotalCharge=new TH1F(histoTotalChargeName,histoTotalChargeName,200,0,14000);
	fTotalChargeInNpixelsArray->AddLast(histoTotalCharge);
    };

    //Number 49
    histo = new TH1F("ChargePixelSeed","ChargePixelSeed",200,0,14000);
    fPixelChargeHistos->AddLast(histo);
    //Number 50
    histo = new TH1F("ChargePixel9of49","ChargePixel 9 Of 49",200,0,14000);
    fPixelChargeHistos->AddLast(histo);
    //Number 51
    histo = new TH1F("ChargePixel25of49","ChargePixel 25 Of 49",200,0,14000);
    fPixelChargeHistos->AddLast(histo);
    //Number 52
    histo = new TH1F("ChargePixel49of49","ChargePixel 49 Of 49",200,0,14000);
    fPixelChargeHistos->AddLast(histo);
    //Number 53
    histo = new TH1F("ChargePixel9of49Sorted","ChargePixel 9 Of 49 Sorted",200,0,14000);
    fPixelChargeHistos->AddLast(histo);
    //Number 54
    histo = new TH1F("ChargePixel25of49Sorted","ChargePixel 25 Of 49 Sorted",200,0,14000);
    fPixelChargeHistos->AddLast(histo);
    //Number 55
    histo = new TH1F("ChargePixel49of49Sorted","ChargePixel 49 Of 49 Sorted",49,0.5,49.5);
    fPixelChargeHistos->AddLast(histo);
    //Number 56
    //histo = new TH1F("ChargePixel49Of49Sorted","ChargePixel 49 Of 49 Sorted",49,0.5,49.5);
    //fPixelChargeHistos->AddLast(histo);

    Qseed = new TH1F("Qseed","ChargeOnSeedPixel",1000,0,12000);

    cout << "------- CbmMvdFindHits::Init completed ------" << endl;
    cout << "---------------------------------------------" << endl;


   
    Int_t adcMax = fAdcOffset + fAdcDynamic;
    fAdcSteps= (Int_t)TMath::Power(2,fAdcBits);
    fAdcStepSize  = fAdcDynamic/fAdcSteps;

    fGausArray=new Float_t[fGausArrayLimit];
    for(Int_t i=0;i<fGausArrayLimit;i++){fGausArray[i]=gRandom->Gaus(0, fSigmaNoise);};
    fGausArrayIt=0;

    delete histoName;
    delete histoTotalChargeName;

    return kSUCCESS;

}
// -------------------------------------------------------------------------

// -----   Virtual public method Reinit   ----------------------------------
InitStatus CbmMvdFindHits::ReInit() {
    cout << "-I- " <<"CbmMvdFindHits::ReInt---------------"<<endl;
}
// -------------------------------------------------------------------------


// -----   Virtual public method Exec   --------------
void CbmMvdFindHits::Exec(Option_t* opt) {


    if( fDigis->GetEntriesFast() == 0 ){
	cout << "-I- " << GetName() << "::Exec: No digis found, event skipped. " << endl;
        fCounter++;
	return;
    }

    Reset(); //clear arrays

    //cdritsa: 15/12/08 start
    map<Int_t, CbmMvdStation*>::iterator stationIt;
    for (stationIt=fStationMap.begin(); stationIt!=fStationMap.end();
	 stationIt++) (*stationIt).second->Clear();
    //cdritsa: 15/12/08 end

    fNEvent++;

    vector<CbmMvdDigi*>* clusterArray=new vector<CbmMvdDigi*>;
    CbmMvdDigi* digi = NULL;

    Int_t iDigi=0;
    digi = (CbmMvdDigi*) fDigis->At(iDigi);

    if(!digi){
	cout << "-E- : CbmMvdFindHits - Fatal: No Digits found in this event."<< endl;
    }


    Int_t nDigis = fDigis->GetEntriesFast();


    //----------------------------------------------------------------------------------
    // Generate random number and call it noise
    // add the noise to the charge of the digis
    //----------------------------------------------------------------------------------
    if( fAddNoise == kTRUE ){

	cout << "-I- " << "CbmMvdFindHits: Calling method AddNoiseToDigis()...\n" << endl;

	for ( iDigi=0; iDigi<nDigis; iDigi++ ) {

	    digi  = (CbmMvdDigi*) fDigis->At(iDigi);
	    AddNoiseToDigis(digi);
	}
    }
    //----------------------------------------------------------------------------------
    //----------------------------------------------------------------------------------

    Double_t pixelSizeX = digi->GetPixelSizeX();;
    Double_t pixelSizeY = digi->GetPixelSizeY();;

    //----------------------------------------------------------------------------------
    // -------- Create Fake Digis ------------------------------------------------------
    //----------------------------------------------------------------------------------
    if( fMode == 1 ){

    	for (stationIt=fStationMap.begin(); stationIt!=fStationMap.end(); stationIt++) {

	    CbmMvdStation* station = (*stationIt).second;

	    Double_t layerRadius = station->GetRmax();

    	    if( gDebug>0 ){
		cout << "-I- " << GetName() << ": Fakes: stationIt: " << station->GetVolumeId() << " , layerRadius " << layerRadius << endl;
		cout << "-I- " << GetName() << ": Calling method GenerateFakeDigis()..." << endl;
	    }

	    GenerateFakeDigis(station,pixelSizeX, pixelSizeY);

	}

    }

    //----------------------------------------------------------------------------------


    nDigis = fDigis->GetEntriesFast();
    TArrayS* pixelUsed  = new TArrayS(nDigis);

    for ( iDigi=0; iDigi<nDigis; iDigi++) {
	pixelUsed->AddAt(0,iDigi);
    }


    if( gDebug>0 ){
	cout << GetName() << ": NDigis now " << nDigis << endl;
	cout << GetName() << ": CbmMvdFindHits: Entering loop on Mvd stations ..." << endl;
    }

    //----------------------------------------------------------------------------------
    //-------------- Loop over MVD stations  -------------------------------------------
    //----------------------------------------------------------------------------------

    for (stationIt=fStationMap.begin(); stationIt!=fStationMap.end(); stationIt++) {

	CbmMvdStation* station = (*stationIt).second;

        Double_t layerRadius = station->GetRmax();

	if( gDebug>0 ){
	    cout << "-I- " << GetName() << ": station->GetVolumeId(): " << station->GetVolumeId() << " , layerRadius " << layerRadius << endl;
	}


	// DigiMap MDX
	fDigiMap.clear();

	for(Int_t k=0;k<nDigis;k++){

	    digi = (CbmMvdDigi*) fDigis->At(k);

	    // test if station is correct and apply fNeighThreshold
	    if( digi->GetStationNr() != station->GetStationNr() ){ continue; }
	    if(GetAdcCharge(digi->GetCharge()) < fNeighThreshold ) continue;

	    pair<Int_t, Int_t> a (digi->GetPixelX(),digi->GetPixelY());
	    fDigiMap[a]=k;
	};


	if( gDebug>0 ){cout << "\n-I- " << GetName() << ": VolumeId " << station->GetVolumeId() << endl;}

	for ( iDigi=0; iDigi<nDigis; iDigi++) {

	    if( gDebug>0 && iDigi%10000==0 ){ cout << "-I- " << GetName() << " Digi:" << iDigi << endl; };

	    digi = (CbmMvdDigi*) fDigis->At(iDigi);

	    if ( digi->GetStationNr() != station->GetStationNr() ) { continue; }


	    /*
	     ---------------------------------------------------------
	     check if digi is above threshold (define seed pixel)
	     then check for neighbours.
	     Once the cluster is created (seed and neighbours)
	     calculate the position of the hit
	     using center of gravity (CoG) method.
	     ---------------------------------------------------------
	     */

	    if( gDebug>0 ){
		cout << "-I- " << "CbmMvdFindHits: Checking for seed pixels..." << endl;
	    }

	    if( ( GetAdcCharge(digi->GetCharge())>=fSeedThreshold ) && ( pixelUsed->At(iDigi)==kFALSE ) ){

		clusterArray->clear();
		clusterArray->push_back(digi);

		pixelUsed->AddAt(1,iDigi);

		pair<Int_t, Int_t> a(digi->GetPixelX(), digi->GetPixelY());
    		fDigiMapIt = fDigiMap.find(a);
    		fDigiMap.erase(fDigiMapIt);

		for ( Int_t iCluster=0; iCluster<clusterArray->size(); iCluster++ ){

		    if( gDebug>0 ){
			cout << "-I- " << " CbmMvdFindHits: Calling method CheckForNeighbours()..." << endl;
		    }

		    CheckForNeighbours(clusterArray, iCluster, pixelUsed);

		}

		//Calculate the center of gravity of all pixels in the cluster.
		TVector3 pos(0,0,0);
                TVector3 dpos(0,0,0);

		if( gDebug>0 ){ cout << "-I- " << " CbmMvdFindHits: Calling method CreateHit()..." << endl; }
		
		CreateHit(clusterArray, station ,pos, dpos); // Add cluster to array. Return pointer for filling the CbmMvdHitMatch
		
		
		
		
	    }// if AdcCharge>threshold
	}// loop on digis
    }// loop on detectors

    //----------------------------------------------------------------------------------
     //------------- End of Detector Loops ----------------------------------------------
    //----------------------------------------------------------------------------------

    cout << "-I-  End of task " << GetName() << ": Event Nr: " << fNEvent << ", nDIGIS: "<<nDigis << ", nHits:"<<fHits->GetEntriesFast()<<endl;

    delete pixelUsed;
    delete clusterArray;



}


//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
// -----  Private method GetMvdGeometry ------------------------------------

/**  The method assumes the following convention:
 **  --- The MVD stations are of the shape TGeoTube.
 **  --- The name of a station is mvdstationxx, where xx is the number
 **      of the station.
 **  --- The stations are numbered from 1 on consecutively.
 **  V. Friese, 4 December 2008
 **/


Int_t CbmMvdFindHits::GetMvdGeometry() {
   
  cout << "-I- " << GetName() << " : Reading MVD geometry..." << endl;
  Int_t iStation =  1;
  Int_t volId    = -1;
  
  do {

    // Volume name according to convention
    TString volName  = Form("mvdstation%02i", iStation);
    volId = gGeoManager->GetUID(volName);
    if ( volId > -1 ) {
      
      // Get shape parameters
      TGeoVolume* volume = gGeoManager->GetVolume(volName.Data());
      TGeoTube* tube = (TGeoTube*) volume->GetShape();
      Double_t rmin = tube->GetRmin();
      Double_t rmax = tube->GetRmax();
      Double_t d    = 2. * tube->GetDz();

      // Full path to node 
      // Since there are now different pipe and mvd geomteries  it is more 
      // difficult to get the full path
      //TString nodeName = "/cave_1/pipevac1_0/" + volName + "_0";
      TString nodeName;
      TGeoNode* node1 = gGeoManager->GetTopVolume()->FindNode("pipevac1_0");
      if (node1) { // old ascii geometry for pipe
	TObjArray* nodes = node1->GetNodes();
	for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {
	  TGeoNode* node = (TGeoNode*) nodes->At(iNode);
          TString tmpnodeName = node->GetName();
	  if (tmpnodeName.Contains(volName)) {
            nodeName = "/cave_1/pipevac1_0/" + volName + "_0";
            break;
	  } else if (tmpnodeName.Contains("mvd_")) {
            nodeName = "/cave_1/pipevac1_0/" + tmpnodeName 
	      + "/" + volName + "_0";
            break;
	  }
	}
      } else {
	// Find Pipe top node
	nodeName = "/cave_1/";
	TObjArray* nodes = gGeoManager->GetTopNode()->GetNodes();
	for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {
	  TGeoNode* node = (TGeoNode*) nodes->At(iNode);
	  TString nodeName1 = node->GetName();
	  cout << "Node: "<< nodeName1 <<endl;
	  nodeName1.ToLower();
	  if (nodeName1.Contains("pipe")) {
            nodeName +=  node->GetName();
            nodeName += "/";	  
	    // find pipevac1
	    TObjArray* nodes2 = node->GetNodes();
	    for (Int_t iiNode = 0; iiNode < nodes2->GetEntriesFast(); iiNode++) {
	      TGeoNode* node2 = (TGeoNode*) nodes2->At(iiNode);
	      TString nodeName2 = node2->GetName();
	      cout << "Node: "<< nodeName2 <<endl;
	      nodeName2.ToLower();
	      if (nodeName2.Contains("pipevac1")) {
		nodeName +=  node2->GetName();
		nodeName += "/";	  
		cout << "I am here " <<endl;
		// check if there is a volume with mvd in name in the pipevac
		TObjArray* nodes3 = node2->GetNodes();
		for (Int_t iiiNode = 0; iiiNode < nodes3->GetEntriesFast(); iiiNode++) {
		  TGeoNode* node3 = (TGeoNode*) nodes3->At(iiiNode);
		  TString nodeName3 = node3->GetName();
		  cout << "Node: "<< nodeName3 <<endl;
		  if (nodeName3.Contains(volName)) {
		    nodeName += volName;
		    nodeName +=  "_0";	  
                    goto Node_found;
		  } else if ( nodeName3.Contains("mvd") ) {
		    nodeName +=  node3->GetName();
		    nodeName += "/";	  
		    nodeName += volName;
		    nodeName +=  "_0";	  
                    goto Node_found;
		  }
		}
	      }
	    }
	  }
	}
      }
      
      Node_found:
      
      // Get z position of node
      Bool_t nodeFound = gGeoManager->cd(nodeName.Data());
      if ( ! nodeFound ) {
	cout << "-E- " << GetName() << "::SetMvdGeometry: Node " << nodeName
	     << " not found in geometry!" << endl;
	Fatal("SetMvdGeometry", "Node not found");
      }
      Double_t local[3] = {0., 0., 0.};  // Local centre of volume
      Double_t global[3];                // Global centre of volume
      gGeoManager->LocalToMaster(local, global);
      Double_t z = global[2];


      // Check for already existing station with the same ID
      // (Just in case, one never knows...)
      if ( fStationMap.find(iStation) != fStationMap.end() ) {
	cout << "-E- " << GetName() << "::GetMvdGeometry: " 
	     << "Station " << volId << " already in map!" << endl;
	Fatal("GetMvdGeometry", "Double volume ID in TGeoManager!");
      }

      // Create new CbmMvdStation and add it to the map
      fStationMap[iStation] = new CbmMvdStation(volName.Data(), iStation, volId, 
			 		        z, d, rmin, rmax);
      fStationMap[iStation]->Print();
      
      iStation++;

    }     // Volume found

  } while ( volId > -1 );

   
  return iStation - 1;
}


//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

void CbmMvdFindHits::AddNoiseToDigis(CbmMvdDigi* digi){
    Double_t noise  = fGausArray[fGausArrayIt++]; // noise is simulated by a gauss
    if (fGausArrayIt-2>fGausArrayLimit){fGausArrayIt=0;};
    Double_t charge = digi->GetCharge() + noise;
    digi->SetCharge((int)charge);
}


//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

void CbmMvdFindHits::GenerateFakeDigis(CbmMvdStation* station, Double_t pixelSizeX, Double_t pixelSizeY){

    //max index of pixels
    //Int_t nx = TMath::Nint(2*fLayerRadius/pixelSizeX);
    //Int_t ny = TMath::Nint(2*fLayerRadius/pixelSizeY);

    //cdritsa: parametrise geometry: 15/12/08
    Double_t layerRadius = station->GetRmax();
    Double_t layerRadiusInner = station->GetRmin();

    Int_t nx = int(2*layerRadius/pixelSizeX);
    Int_t ny = int(2*layerRadius/pixelSizeY);

    Double_t x;
    Double_t y;
    Double_t distance2;
    Double_t noise;
    Double_t r2       = layerRadius*layerRadius;
    Double_t r2_inner = layerRadiusInner*layerRadiusInner;

    for( Int_t i=0; i<nx; i++){

	x = (i+0.5)*pixelSizeX - layerRadius;

	for( Int_t j=0; j<ny; j++){

	    y = (j+0.5)*pixelSizeY - layerRadius;

	    distance2 =  x*x + y*y ;


	    if(  distance2>r2 || distance2<r2_inner )  continue;

	   noise  = fGausArray[fGausArrayIt++]; // noise is simulated by a gauss
    	   if (fGausArrayIt-2>fGausArrayLimit){fGausArrayIt=0;}; 

	    if ( noise>fSeedThreshold /*&& pixel is not used ???*/){
                 Int_t nDigis = fDigis->GetEntriesFast();
		 CbmMvdDigi* fakeDigi= 
		       new ((*fDigis)[nDigis]) CbmMvdDigi(station->GetVolumeId(),i,j,(int)noise,
		       					  pixelSizeX,pixelSizeY,0,0,0,0,-4,-4);
		}
	}
    }



}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

void CbmMvdFindHits::CheckForNeighbours(vector<CbmMvdDigi*>* clusterArray, Int_t clusterDigi, TArrayS* pixelUsed)
{
    CbmMvdDigi* seed = clusterArray->at(clusterDigi);
    CbmMvdDigi* digiOfInterest;
	
    // Remove Seed Pixel from list of non-used pixels
    Int_t channelX=seed->GetPixelX();
    Int_t channelY=seed->GetPixelY();
    pair<Int_t, Int_t> a(channelX, channelY);
    
    // Find first neighbour
    
    a=std::make_pair(channelX+1, channelY);
    fDigiMapIt=fDigiMap.find(a);

    if (!(fDigiMapIt == fDigiMap.end()))
    	{ 
	  Int_t i=fDigiMap[a];
	  digiOfInterest=(CbmMvdDigi*)fDigis->At(i);
	  
	  // Only digis depassing fNeighThreshold are in the map, no cut required
	  clusterArray->push_back(digiOfInterest);
	  pixelUsed->AddAt(1,i); // block pixel for the seed pixel scanner
	  fDigiMap.erase(fDigiMapIt); // block pixel for the neighbour pixel scanner
	 } 
	  
    a = std::make_pair(channelX-1, channelY);
    fDigiMapIt = fDigiMap.find(a);

    if (!(fDigiMapIt == fDigiMap.end()))
    {
	Int_t i=fDigiMap[a];
	digiOfInterest=(CbmMvdDigi*)fDigis->At(i);

	// Only digits depassing fNeighThreshold are in the map, no cut required
	clusterArray->push_back(digiOfInterest);
	pixelUsed->AddAt(1,i); // block pixel for the seed pixel scanner
	fDigiMap.erase(fDigiMapIt); // block pixel for the neighbour pixel scanner
    }

    a = std::make_pair(channelX, channelY-1);
    fDigiMapIt=fDigiMap.find(a);
    if (!(fDigiMapIt == fDigiMap.end()))
    {
	Int_t i=fDigiMap[a];
	digiOfInterest=(CbmMvdDigi*)fDigis->At(i);

	// Only digits depassing fNeighThreshold are in the map, no cut required
	clusterArray->push_back(digiOfInterest);
	pixelUsed->AddAt(1,i); // block pixel for the seed pixel scanner
	fDigiMap.erase(fDigiMapIt); // block pixel for the neighbour pixel scanner
    }

    a = std::make_pair (channelX, channelY+1);
    fDigiMapIt=fDigiMap.find(a);

    if (!(fDigiMapIt == fDigiMap.end()))
    {
	Int_t i=fDigiMap[a];
	digiOfInterest=(CbmMvdDigi*)fDigis->At(i);
	  
	// Only digis depassing fNeighThreshold are in the map, no cut required
	clusterArray->push_back(digiOfInterest);
	pixelUsed->AddAt(1,i); // block pixel for the seed pixel scanner
	fDigiMap.erase(fDigiMapIt); // block pixel for the neighbour pixel scanner
    }




}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
void CbmMvdFindHits::CreateHit(vector<CbmMvdDigi*>* clusterArray, CbmMvdStation* station, TVector3 &pos, TVector3 &dpos)
{

    //loop on cluster array elements
    //calculate the CoG for this cluster

    Int_t    clusterSize = clusterArray->size();
    Double_t numeratorX  = 0;
    Double_t numeratorY  = 0;
    Double_t denominator = 0;
    Double_t pixelSizeX  = 0;
    Double_t pixelSizeY  = 0;
    Int_t charge;
    Int_t xIndex;
    Int_t yIndex;
    
    Double_t layerRadius = station->GetRmax();
    Double_t layerPosZ = station->GetZ();

    CbmMvdDigi* pixelInCluster = clusterArray->at(0);
    Int_t detId = pixelInCluster->GetDetectorId();

    // Calculate the center of gravity of the charge of a cluster
    
    for( Int_t iCluster = 0; iCluster<clusterSize; iCluster++ ){
         
    	pixelInCluster = clusterArray->at(iCluster);

	charge      = GetAdcCharge(pixelInCluster->GetCharge());
	xIndex      = pixelInCluster->GetPixelX();
	yIndex      = pixelInCluster->GetPixelY();
	pixelSizeX  = pixelInCluster->GetPixelSizeX();
	pixelSizeY  = pixelInCluster->GetPixelSizeY();

	if(gDebug>0){
	    cout << "-I- " << "CbmMvdFindHits:: iCluster= "<<iCluster << " , clusterSize= " << clusterSize << endl;
	    cout << "-I- " << "CbmMvdFindHits::xIndex " << xIndex << " , yIndex " << yIndex << " , charge = " << pixelInCluster->GetAdcCharge(fAdcDynamic, fAdcOffset, fAdcBits) << endl;
	}


	//Calculate x,y coordinates of the pixel in the laboratory ref frame
	Double_t x = ( 0.5+double(xIndex) )*pixelSizeX - layerRadius;
	Double_t y = ( 0.5+double(yIndex) )*pixelSizeY - layerRadius;

	//Calculate x,y coordinates of the pixel in the detector ref frame
	//Double_t x = ( 0.5+double(xIndex) )*pixelSizeX;
	//Double_t y = ( 0.5+double(yIndex) )*pixelSizeY;

	Double_t xc = x*charge;
	Double_t yc = y*charge;

	if(gDebug>0){
	    cout << "-I- " << " CbmMvdFindHits::detId "<< detId << " layerRadius =" << layerRadius << ", layerPosZ "<<layerPosZ << endl;
	    cout << "-I- " << " CbmMvdFindHits::x*charge = " << x <<"*" <<charge<< " = " <<x*charge << ",  xc= " << xc << endl;
	    cout << "-I- " << " CbmMvdFindHits::y*charge = " << y <<"*" <<charge<< " = " <<y*charge << ",  yc= " << yc << endl;
	}
	numeratorX   += xc;
	numeratorY   += yc;
	denominator  += charge;

    }

    if(gDebug>0){
	cout << "-I- " << "CbmMvdFindHits::=========================\n " << endl;
	cout << "-I- " << "CbmMvdFindHits::numeratorX: " <<numeratorX<<" , numeratorY: " <<numeratorY << ", denominator: " << denominator << endl;
    }

    //Calculate x,y coordinates of the pixel in the laboratory ref frame
    if(denominator!=0) {
	fHitPosX = (numeratorX/denominator);
	fHitPosY = (numeratorY/denominator);
	fHitPosZ = layerPosZ;
    }else{
	fHitPosX = 0;
	fHitPosY = 0;
	fHitPosZ = 0;
    }
    if(gDebug>0){
	cout << "-I- " << "CbmMvdFindHits::-----------------------------------" << endl;
	cout << "-I- " << "CbmMvdFindHits::X hit= " << fHitPosX << " Y hit= "<<fHitPosY << " Z hit= "<<fHitPosZ <<endl;
	cout << "-I- " << "CbmMvdFindHits::X seed " <<int( (fHitPosX+layerRadius)/pixelSizeX )<< " , Y Seed = " << int( (fHitPosY+layerRadius)/pixelSizeY ) << " , charge = " << pixelInCluster->GetAdcCharge(fAdcDynamic, fAdcOffset, fAdcBits)<< endl;
	cout << "-I- " << "CbmMvdFindHits::-----------------------------------\n" << endl;
    }

    // pos = center of gravity (labframe), dpos uncertainty
    pos.SetXYZ(fHitPosX,fHitPosY,fHitPosZ);
    dpos.SetXYZ(fHitPosErrX, fHitPosErrY, fHitPosErrZ);




    /************************************************************

    Algorithm for cluster shapes

    /************************************************************/

    Int_t xSeed;
    Int_t ySeed;
    Bool_t digiFound;
    
    Short_t chargeArray[49], dominatorArray[49];
    //    long long orderArray[49];
   
    Float_t xPos[5],yPos[5];
    Short_t contributors=-2;

    for (Int_t k=0;k<49;k++){
	chargeArray[k]= 0; 
	dominatorArray[k]=-1;
    }

    for (Int_t k=0;k<5;k++){
	xPos[k]=0;
	yPos[k]=0;
    }

    Int_t k=0;

   

    // fill the charge array indicating the shape of the cluster
    // the central pixel of the charge array contains the center of gravity
    
    Float_t xCentralTrack=0, yCentralTrack=0; 
    CbmMvdDigi* centralDigi;
    // to be used to identify the position of the track, which contributes most charge
    // to the digi in the center of the cluster -> Check for hit resolution
    
    for( Int_t i=0; i<7; i++ ){

	for( Int_t j=0; j<7; j++ ){

	    digiFound = kFALSE;

	    for( k=0; k<clusterSize; k++ ){

		CbmMvdDigi* digi = (CbmMvdDigi*) clusterArray->at(k);
		pixelSizeX = digi->GetPixelSizeX();
		pixelSizeY = digi->GetPixelSizeY();
		xSeed = int( (fHitPosX+layerRadius)/pixelSizeX );  // find index of seed pixel in the detector frame
		ySeed = int( (fHitPosY+layerRadius)/pixelSizeY );


		if( (digi->GetPixelX() - xSeed == i-3) && (digi->GetPixelY() - ySeed == j-3) ){

		    if(gDebug>0){
			cout << "-I- " << GetName() << "X " << digi->GetPixelX() << " ,  Y= " << digi->GetPixelY() << endl;
			cout << "-I- " << GetName() << "xSeed " << xSeed << " ,  ySeed " << ySeed << endl;
			cout << "-I- " << GetName() << "charge = " << digi->GetCharge() << " ,  AdcCharge = " << digi->GetAdcCharge(fAdcDynamic, fAdcOffset, fAdcBits) << endl;
		    }

		
		    chargeArray[7*i+j] = GetAdcCharge(digi->GetCharge());


                     // Fill Quality control Histograms
		    if (fShowDebugHistos)
		         {TH1F* histo = (TH1F*)fPixelChargeHistos->At(7*i+j);
		          histo->Fill( chargeArray[7*i+j] );}


		    if(i==3 && j==3){xCentralTrack=digi->GetDominatorX(); yCentralTrack=digi->GetDominatorY();
		    	            centralDigi=digi;}
		    //check for dominating digi

		    Short_t l=-1,trackNr=-1;

		    do {
			l=l+1;
			if( xPos[l]==digi->GetDominatorX() && yPos[l]==digi->GetDominatorY() )
			{
			    trackNr=l;
			};
		    }while ( xPos[l]!=0 && l<4 );  //4 is  the size of xPos[] array -> save up to 4 mc points

		    if (trackNr>=0) { dominatorArray[7*i+j]=trackNr; }

		    else if( trackNr == -1 && l == 4 ) {
			dominatorArray[7*i+j] = l;
			if (l>contributors) {contributors=l;}
		    }

		    else {
			xPos[l] = digi->GetDominatorX();
			yPos[l]=digi->GetDominatorY();
			dominatorArray[7*i+j]=l;
			if(l>contributors){contributors=l;}
		    }

		    digiFound = kTRUE;


		} // if

		if( !digiFound ){
		    
    	   	    if (fGausArrayIt-2>fGausArrayLimit){fGausArrayIt=0;}; 
		    chargeArray   [7*i+j] = (Short_t)fGausArray[fGausArrayIt++]; //insert noise
		    dominatorArray[7*i+j] = 0;
		}
	    } // for k
	} // for j
    } // for i


    Int_t nClusters = fClusters->GetEntriesFast();
    CbmMvdCluster* clusterNew=new ((*fClusters)[nClusters]) CbmMvdCluster(detId, pos, dpos, 0,  chargeArray, pixelSizeX, pixelSizeY);
    clusterNew->SetDebuggingInfo(dominatorArray,xPos,yPos);
    clusterNew->SetContributors(contributors);

	// Save hit into array
    Int_t nHits = fHits->GetEntriesFast();
    new ((*fHits)[nHits]) CbmMvdHit(station->GetStationNr(), pos, dpos, 0);
    new ((*fMatches)[nHits]) CbmMvdHitMatch(0, 0, centralDigi->GetTrackID(), centralDigi->GetPointID(), clusterNew->GetContributors());
    
    if(fShowDebugHistos)
	{ 

	Float_t qSeed = chargeArray[24];
        Float_t q9    = chargeArray[16] + chargeArray[17] + chargeArray[18]+
	            chargeArray[23] + chargeArray[24] + chargeArray[25]+
	            chargeArray[30] + chargeArray[31] + chargeArray[32];
    	Float_t q25= q9 +
	chargeArray[8]  + chargeArray[9]  + chargeArray[10] + chargeArray[11] + chargeArray[12] +
	chargeArray[15] + chargeArray[22] + chargeArray[29] +
	chargeArray[19] + chargeArray[26] + chargeArray[33] +
	chargeArray[36] + chargeArray[37] + chargeArray[38] + chargeArray[39] + chargeArray[40];
    	Float_t q49=0;
    	for(Int_t i=0; i<49; i++){q49+=chargeArray[i];};

    	((TH1F*) fPixelChargeHistos->At(49))->Fill(qSeed);
    	((TH1F*) fPixelChargeHistos->At(50))->Fill(q9);
    	((TH1F*) fPixelChargeHistos->At(51))->Fill(q25);
    	((TH1F*) fPixelChargeHistos->At(52))->Fill(q49);

	if( fHitPosX-xCentralTrack>0.003 && fHitPosZ<6 ) { fBadHitHisto->Fill(fHitPosX,fHitPosY); }

	fResolutionHistoX->Fill(fHitPosX-xCentralTrack);
	fResolutionHistoY->Fill(fHitPosY-yCentralTrack);
	if(contributors==0) {
	    fResolutionHistoCleanX->Fill(fHitPosX-xCentralTrack);
	    fResolutionHistoCleanY->Fill(fHitPosY-yCentralTrack);
	}
	else {
	    fResolutionHistoMergedX->Fill(fHitPosX-xCentralTrack);
	    fResolutionHistoMergedY->Fill(fHitPosY-yCentralTrack);
	};
	
	//Prepare selection of crowns for charge bow histograms
	
	const Int_t arrayMembers=25;
	//25 and 49 allowed. 
	Float_t tempArray[arrayMembers];
	Int_t orderArray[arrayMembers];
	Int_t pixelsOfInterest[arrayMembers]={8,9,10,11,12,15,16,17,18,19,22,23,24,25,26,29,30,31,32,33,36,37,38,39,40};
		
	for(Int_t i=0;i<arrayMembers;i++){
	  if (arrayMembers==25){tempArray[i]=chargeArray[pixelsOfInterest[i]];}
	  else {tempArray[i]=chargeArray[i];};
	};
	
	TMath::Sort(arrayMembers,tempArray,orderArray,kTRUE);
	
	
	
	Float_t qSort=0;
	for (Int_t i=0; i<9; i++){ qSort+=tempArray[orderArray[i]]; };
    	((TH1F*) fPixelChargeHistos->At(53))->Fill(qSort);

	for (Int_t i=9; i<25; i++) { qSort+=tempArray[orderArray[i]]; };
	((TH1F*) fPixelChargeHistos->At(54))->Fill(qSort);
		
	TH1F* histoTotalCharge;
    	qSort=0;
    	for (Int_t i=0; i<arrayMembers; i++){
		qSort+=tempArray[orderArray[i]];
		((TH1F*) fPixelChargeHistos->At(55))->Fill(i+1,qSort);
		histoTotalCharge =(TH1F*) fTotalChargeInNpixelsArray->At(i) ;
	    	histoTotalCharge->Fill(qSort);
    		};

  
   	}

               	

}

//--------------------------------------------------------------------------

// -----   Private method Register   ---------------------------------------
void CbmMvdFindHits::Register() {
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman) Fatal("Register",
		      "No FairRootManager");
  ioman->Register("MvdHit", "MVD", fHits, kTRUE);
  ioman->Register("MvdCluster", "MVD", fClusters, kTRUE);
  ioman->Register("MvdHitMatch", "MVD Hit Match", fMatches, kTRUE);


}
//--------------------------------------------------------------------------
void CbmMvdFindHits::Finish() {
    cout << "\n============================================================" << endl;
    cout << "-I- " << GetName() << "::Finish: Total events skipped: " << fCounter << endl;
    cout << "============================================================" << endl;
    if(fShowDebugHistos){
	TCanvas* canvas=new TCanvas("HitFinderCanvas","HitFinderCanvas");
	canvas->Divide (2,3);
	canvas->cd(1);
	fResolutionHistoX->Draw();
	fResolutionHistoX->Write();
	canvas->cd(2);
	fResolutionHistoY->Draw();
	fResolutionHistoY->Write();
	canvas->cd(3);
	fResolutionHistoCleanX->Draw();
	fResolutionHistoCleanX->Write();
	canvas->cd(4);
	fResolutionHistoCleanY->Draw();
	fResolutionHistoCleanY->Write();
	canvas->cd(5);
	((TH1F*)fTotalChargeInNpixelsArray->At(24))->Draw();
	((TH1F*)fTotalChargeInNpixelsArray->At(24))->Fit("landau");
	fResolutionHistoMergedX->Write();
	canvas->cd(6);
	fResolutionHistoMergedY->Draw();
	fResolutionHistoMergedY->Write();
	
	for(Int_t i=0;i<fPixelChargeHistos->GetEntries();i++)
	{((TH1F*)fPixelChargeHistos->At(i))->Write();}
	for(Int_t i=0;i<fTotalChargeInNpixelsArray->GetEntries();i++)
	{((TH1F*)fTotalChargeInNpixelsArray->At(i))->Write();}
	new TCanvas();
	((TH1F*)fPixelChargeHistos->At(51))->Draw();
	((TH1F*)fPixelChargeHistos->At(51))->Fit("landau");
	new TCanvas();
	((TH1F*)fPixelChargeHistos->At(49))->Draw();
	((TH1F*)fPixelChargeHistos->At(49))->Fit("landau");
    }
}
//--------------------------------------------------------------------------

Int_t CbmMvdFindHits::GetAdcCharge(Float_t charge)
{

    Int_t adcCharge;

    if(charge<fAdcOffset){return 0;};

    adcCharge = int( (charge-fAdcOffset)/fAdcStepSize );
    if ( adcCharge>fAdcSteps-1 ) {adcCharge = fAdcSteps-1;}

    return adcCharge;

}


// -----   Private method Reset   ------------------------------------------
void CbmMvdFindHits::Reset() {
    fHits->Clear("C");
    fClusters->Clear("C");
    fMatches->Clear("C");
}

// -------------------------------------------------------------------------



ClassImp(CbmMvdFindHits);

