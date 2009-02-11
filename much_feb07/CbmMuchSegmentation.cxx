/** CbmMuchSegmentation.cxx
 *@author Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since 20.06.07
 *@version 1.0
 **
 ** class for making parameter file for MUCH digitizer
 ** 
 **/

#include "CbmGeoMuchPar.h"
#include "CbmMuchSegmentation.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairRootManager.h"
#include "CbmMuchPoint.h"

#include "TCanvas.h"
#include "TStyle.h"
#include "TString.h"
#include "TArrow.h"
#include "TEllipse.h"
#include "TMath.h"
#include "TObjArray.h"
#include "TClonesArray.h"
#include "TArrayD.h"
#include "TH1.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <map>

using std::cout;
using std::endl;
using std::ios;
using std::vector;
using std::sqrt;
using std::pow;
using std::fabs;
using std::log;
using std::map;
using std::pair;


// -----   Default constructor   ------------------------------------------
CbmMuchSegmentation::CbmMuchSegmentation(){ }
// -------------------------------------------------------------------------

// -----   Default constructor   -------------------------------------------
CbmMuchSegmentation::CbmMuchSegmentation(char* digiFileName){
  fDigiFileName = digiFileName;
  fNStations = 0;

  Double_t hdArray[] = {0.4, 0.2, 0.1, 0.05, 0.025, 0.01, 0.005, 0.0025, 0.001}; 
  Double_t hdSigmaX[] = {400, 800, 800,  1600, 1600, 3200, 3200, 6400, 6400}; 
  Double_t hdSigmaY[] = {800, 800, 1600, 1600, 3200, 3200, 6400, 6400, 12800};
  fHitDensities.assign(hdArray, hdArray+sizeof(hdArray)/sizeof(Double_t)); // Hit density boundaries
  fSigmasX.assign(hdSigmaX, hdSigmaX+sizeof(hdSigmaX)/sizeof(Double_t));
  fSigmasY.assign(hdSigmaY, hdSigmaY+sizeof(hdSigmaY)/sizeof(Double_t));
}

// -----   Destructor   ----------------------------------------------------
CbmMuchSegmentation::~CbmMuchSegmentation() { 
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------

// -----   Private method SetParContainers  --------------------------------
void CbmMuchSegmentation::SetParContainers() {

  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

  // Get MUCH geometry parameter container
  fGeoPar = (CbmGeoMuchPar*) db->getContainer("CbmGeoMuchPar");
}
// -------------------------------------------------------------------------

// -----   Private method Init  --------------------------------------------
InitStatus CbmMuchSegmentation::Init() {
  // Get arrays of sensitive and passive nodes
  cout << "Digi file: " << fDigiFileName << endl;
  TObjArray* sensNodes = fGeoPar->GetGeoSensitiveNodes();
  TObjArray* passNodes = fGeoPar->GetGeoPassiveNodes();
  Int_t nSensNodes = sensNodes->GetEntries();
  Int_t nPassNodes = passNodes->GetEntries();
 
  // Sort geo nodes by z coordinate
  map<Double_t, pair<GeoNodeType, FairGeoNode*> > geoNodesMap;
  for(Int_t iPassNode = 0; iPassNode < nPassNodes; iPassNode++){
    FairGeoNode* node = (FairGeoNode*)(passNodes->At(iPassNode));
    // Search for mother volume
    FairGeoNode* motherVolume = (FairGeoNode*)(passNodes->FindObject(node->getMother().Data()));
    if(!motherVolume) continue;      
    Double_t z = node->getParameters()->At(3);
    if(geoNodesMap.find(z) == geoNodesMap.end()){
      pair<GeoNodeType, FairGeoNode*> p(kPassive, node);
      geoNodesMap[z] = p;
    }
  }
  for(Int_t iSensNode = 0; iSensNode < nSensNodes; iSensNode++){
    FairGeoNode* node = (FairGeoNode*)(sensNodes->At(iSensNode));
    Double_t z = node->getParameters()->At(3);
    if(geoNodesMap.find(z) == geoNodesMap.end()){
      pair<GeoNodeType, FairGeoNode*> p(kSensitive, node);
      geoNodesMap[z] = p;
    }
  }
  
  // Calculate region radia 
  FillRadia(geoNodesMap);

  // Fill vectors with region radia
  Int_t incAbsorbers = 0; // number of absorbers
  Int_t incStations = 0;  // number of layers
  for(map<Double_t, pair<GeoNodeType, FairGeoNode*> >::iterator it=geoNodesMap.begin();
      it!=geoNodesMap.end(); it++){
    pair<GeoNodeType, FairGeoNode*> p = (*it).second;
    GeoNodeType nodeType = p.first;
    FairGeoNode* node = p.second;

    if(nodeType == kPassive){
      incAbsorbers++;
      continue;
    }
    else if(nodeType == kSensitive){
      incStations++;
      TArrayD* params = node->getParameters();
      
      vector<Double_t> regionRadia;
      Double_t rInt = params->At(7);
      Double_t rExt = params->At(8);
      vector<Double_t> radia;
      vector<Double_t> regions;

      // Fill min resolution vectors with default values if needed
      regions = fRadiaInt[incAbsorbers-1];
      if(incStations > fMinSigmaX.size()) SetNStations(incStations);
      if(fMinSigmaX[incStations-1] < 50.) SetMinSigmaX(incStations, fSigmasX[fHDIndices[incAbsorbers - 1]]);
      if(fMinSigmaY[incStations-1] < 50.) SetMinSigmaY(incStations, fSigmasY[fHDIndices[incAbsorbers - 1]]);

      // Fill radia vectors for each station
      radia.push_back(rInt);
      for(vector<Double_t>::iterator iter=regions.begin(); iter!=regions.end(); iter++){
	Double_t r = (*iter);
	if(rInt < r && r < rExt) 
	  //if(TMath::Abs(rInt-r) > 5 && TMath::Abs(rExt-r) > 5) radia.push_back(r);
	  if(TMath::Abs(rExt-r) > 7) 
          radia.push_back(r);
      }
      radia.push_back(rExt);
      fRadia.push_back(radia);
    }
  }

//   cout << "Number of absorbers = " << incAbsorbers << endl;
//   cout << "fRadiaInt.size() = " << fRadiaInt.size() << endl;
//   cout << "fHDIndices.size() = " << fHDIndices.size() << endl;
  cout << "final incStations = " << incStations << endl;
  SetNStations(incStations);

  // Make segmentation for each station
  SegmentMuch();

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method FillRadia  ---------------------------------------
void CbmMuchSegmentation::FillRadia(map<Double_t, pair<GeoNodeType, FairGeoNode*> > geoNodesMap){
  gStyle->SetOptStat(0);
  Bool_t afterAbsorber = kFALSE; // auxilary flag showing whether a station comes after an absorber  
  vector<pair<Double_t, Double_t> > stationBorders; // borders of a station coming after an absorber
  

  // Fill stationBorders vector
  Int_t absorberInc = 0;
  vector<TH1D*> hitDensityHist;
  Int_t binSize = 2;
  for(map<Double_t, pair<GeoNodeType, FairGeoNode*> >::iterator it=geoNodesMap.begin();
      it!=geoNodesMap.end(); it++){

    pair<GeoNodeType, FairGeoNode*> p = (*it).second;
    GeoNodeType nodeType = p.first;
    FairGeoNode* node = p.second;
    FairGeoTransform* transform = node->getLabTransform();
    FairGeoVector translation = transform->getTranslation();

    if(nodeType == kPassive){
      absorberInc++;
      afterAbsorber = kTRUE;
      continue;
    }
    else if(nodeType == kSensitive){
      TArrayD* params = node->getParameters();
      Double_t z1 = translation.Z() + params->At(3);
      Double_t z2 = translation.Z() + params->At(6);
      Double_t rExt = params->At(8);

      if(afterAbsorber){
		pair<Double_t, Double_t> zPair(z1, z2);
		stationBorders.push_back(zPair);

		// Initialize histograms
		char title[256];
		char name[256];
		sprintf(title, "After %d absorber",absorberInc);
		sprintf(name, "Absorber%d", absorberInc);
		TH1D* hist = new TH1D(name, title, 100, 0, 200);
		hist->GetXaxis()->SetTitle("r, cm");
		hist->GetYaxis()->SetTitle("hits/(event#timescm^{2})");
		hitDensityHist.push_back(hist);
		
		afterAbsorber = kFALSE;
      }
    }
  }

  // Get MC points array
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");
  TClonesArray* mcPoints = (TClonesArray*) ioman->GetObject("MuchPoint");

  // Loop over all events
  Int_t nEvents = ioman->GetInTree()->GetEntries();  
  for(Int_t iEvent = 0; iEvent < nEvents; iEvent++){
    ioman->ReadEvent(iEvent);
    for(Int_t iPoint = 0; iPoint < mcPoints->GetEntriesFast(); iPoint++){
      CbmMuchPoint* point = (CbmMuchPoint*)mcPoints->At(iPoint);
      TVector3 position;
      point->Position(position);     
      for(Int_t l=0; l<100; l++){
		if(position.Pt() >= binSize*(l-1) && position.Pt() < binSize*l){
		  Double_t s = TMath::Pi()*binSize*binSize*(2*l - 1);	  
		  Int_t inc = 0;
		  for(vector<pair<Double_t, Double_t> >::iterator iter = stationBorders.begin();
			  iter!=stationBorders.end(); iter++){
			pair<Double_t, Double_t> zPair = *iter;
			Double_t z1 = zPair.first - 0.1;
			Double_t z2 = zPair.second + 0.1;
			Double_t zPoint = point->GetZIn();
			if(zPoint > z1 && zPoint < z2) {	      
			  hitDensityHist[inc]->Fill(position.Pt(), 1./(nEvents*s));
			  break;
			}	    
			inc++;
		  }
		}
      }
    }
  }

  // Analyze hit density histograms
  Int_t nGaps = hitDensityHist.size();
  for(Int_t iGap = 0; iGap < nGaps; iGap++){
    TH1D* h = hitDensityHist[iGap];
    // Calculate internal radia
    fRadiaInt.push_back(CalculateInternalRadia(h));
    // Draw and print histograms
    TCanvas* c1 = new TCanvas("c1","c1", 10,10,500,500);
    c1->SetLogy();
    c1->SetLeftMargin(0.12);
    c1->SetRightMargin(0.08);
    h->SetMaximum(1.1);
    h->SetMinimum(9e-6);
    h->SetLineColor(4);
    h->SetLineWidth(5);
    h->GetYaxis()->SetTitleOffset(1.27);
    h->Draw();
    char name[256];
    sprintf(name, "absorber%d.eps", iGap+1);
    c1->Print(name);
  }
}
// -------------------------------------------------------------------------

// -----   Private method CalculateInternalRadia method  -------------------
vector<Double_t> CbmMuchSegmentation::CalculateInternalRadia(TH1D* hist){
  vector<Double_t> radiaInt;                      // List of ring radia
  Int_t maxNRegions = fHitDensities.size();       // Maximal number of ring regions
  Double_t maxHD = fHitDensities[0];              // Hit density corresponding to minimal allowed pad size
  Double_t minHD = fHitDensities[maxNRegions-1];  // Hit density corresponding to maximal allowed pad size
  Int_t maxBin = hist->GetMaximumBin();
  Int_t nBins = hist->GetNbinsX();
  Double_t maxBinHD = hist->GetBinContent(maxBin);

  Double_t hd = 0;
  Double_t rad = 0;
  Int_t hdIndex = -1;
  for(Int_t iHD = 0; iHD < maxNRegions; iHD++){
    Double_t hdBorder1  = fHitDensities[iHD];
    Double_t hdBorder2 = iHD + 1 < maxNRegions ? fHitDensities[iHD+1] : 0;
    for(Int_t iBin = maxBin; iBin < nBins; iBin++){
      hd = hist->GetBinContent(iBin);
      if(hd < minHD) break;
      if(hd > hdBorder1) continue;
      else{
		maxBin = iBin+1 < nBins ? iBin+1 : iBin;
		if(hd < hdBorder2) break;
		if(iBin*2 - rad < 7) continue;
		if(hdIndex < 0) {
		  fHDIndices.push_back(hdIndex = iHD);
		}
		rad = iBin*2;
	//  	cout << "rad = " << rad << "\thd_prev = " << hist->GetBinContent(iBin-1)
	//  	     << "\thd_curr = " << hd << "\tresol = " << fSigmasX[hdIndex] << "x" 
	// 	     << fSigmasY[hdIndex] << endl;	
		radiaInt.push_back(rad);
		break;
      }      
    }
    if(hd < minHD) break;
  }

  if(hdIndex < 0) fHDIndices.push_back(maxNRegions-1);

  return radiaInt;
}
// -------------------------------------------------------------------------


// -----   SetAngle method    ----------------------------------------------
void CbmMuchSegmentation::SetAngle(Int_t iStation, Double_t angle){
    if(iStation > fNStations){
      SetNStations(iStation);
    }
  if(iStation < 1 || iStation > fNStations) {
    cout << "The number of the station is out of available range." << endl;
    return;
  }

  fAngle[iStation-1] = angle;
}
// -------------------------------------------------------------------------

// -----   SetMinSigmaX method    ------------------------------------------
void CbmMuchSegmentation::SetMinSigmaX(Int_t iStation, Double_t sigmaX){
    if(iStation > fNStations){
      SetNStations(iStation);
    }
  if(iStation < 1 || iStation > fNStations) {
    cout << "The number of the station is out of available range." << endl;
    return;
  }
  // Lower limit for resolution is 100 microns
  if(sigmaX < 100.) {
    cout << "Too low resolution on X is set." << endl;
    return;
  }
  fMinSigmaX[iStation-1] = sigmaX;
  
  // Calculate corresponding pad size [cm]
  fMinPadWidth[iStation-1] = fMinSigmaX[iStation-1]*3.464101*1e-4;
}
// -------------------------------------------------------------------------

// -----   SetMinSigmaY method    ------------------------------------------
void CbmMuchSegmentation::SetMinSigmaY(Int_t iStation, Double_t sigmaY){
    if(iStation > fNStations){
      SetNStations(iStation);
    }
  if(iStation < 1 || iStation > fNStations) {
    cout << "\nThe number of the station is out of available range." << endl;
    return;
  }
  // Upper limit for resolution is 100 microns
  if(sigmaY < 100.) {
    cout << "Too low resolution on Y is set." << endl;
    return;
  }
  fMinSigmaY[iStation-1] = sigmaY;
  
  // Calculate corresponding pad size [cm]
  fMinPadLength[iStation-1] = fMinSigmaY[iStation-1]*3.464101*1e-4;
}
// -------------------------------------------------------------------------

// -----   SegmentMuch method    -------------------------------------------
void CbmMuchSegmentation::SegmentMuch(){
  ofstream digiFile(fDigiFileName, ios::out);
  digiFile << "#################################################################\n"
	   << "# Digitisation parameters for MuCh\n"
	   << "# Adjust this info according to your needs. For geometry file ???.geo\n"
	   << "# How many station of what type\n"
	   << "# \n"
	   << "# Format:\n"
	   << "# StationNr global_rot nSectors\n" 
	   << "# sectNr \t iType \t x0 \t y0 \t lx \t ly \t nCols \t nRows \n"
	   << "#################################################################\n"
	   << "[CbmMuchDigiPar]" << endl;
  digiFile.close();
  
  // Make segmentation for each station
  for(Int_t iStation = 0; iStation<fNStations; iStation++){
    CalculateMinimalSizes(iStation);
    MakeSegmentation(iStation);    
  }
  cout << "Parameter for the digitizer are stored in " << fDigiFileName << endl;
}
// -------------------------------------------------------------------------

// -----   CalculateMinimalSizes method   ----------------------------------
void CbmMuchSegmentation::CalculateMinimalSizes(int iStation){
  double dx = fMinPadWidth[iStation];
  double dy = fMinPadLength[iStation];
  double lx, ly; // minimal sector sizes
  double ratio;
  int k, n;  

  if(fabs(dx-dy)<1e-5){
    lx = 8*dx;
    ly = 16*dy;
  }
  else if(dx > dy){
    ratio = dx/dy;
    n = (int)(log(ratio)/0.693147);
    dx = pow(2.,n)*dy;
    k = (int)(n/2);
    lx = 8/pow(2.,k)*dx;
    ly = 16*pow(2.,k)*dy;
  }
  else if(dx < dy){
    ratio = dy/dx;
    n = (int)(log(ratio)/0.693147);
    dy = pow(2.,n)*dx;
    k = (int)((n+1)/2);
    lx = 8*pow(2.,k)*dx;
    ly = 16/pow(2.,k)*dy;
  }

  fMinPadWidth[iStation] = dx;
  fMinPadLength[iStation] = dy;
  fMinSectorWidth[iStation] = lx;
  fMinSectorLength[iStation] = ly;
}
// -------------------------------------------------------------------------

// -----   MakeSegmentation method   ---------------------------------------
void CbmMuchSegmentation::MakeSegmentation(Int_t iStation){
  Double_t minArea = fMinSectorWidth[0]*fMinSectorLength[0];
  TString name("station");
  name += iStation + 1;
  TCanvas* canvas = new TCanvas(name, name, 900,900);
  Int_t nRadia = fRadia[iStation].size();

  //************** Fill regions with sectors (begin) *******************//
  vector<TPave*> paves;                // objects representing sectors
  vector<double> secLx(nRadia - 1);    // width of sectors in each region
  vector<double> secLy(nRadia - 1);    // length of sectors in each region

  // Define sector size for each region
  SetSectorSizes(nRadia - 1, fMinSectorWidth[iStation], fMinSectorLength[iStation], secLx, secLy);

  // Fill regions with sectors
  for(int i=0; i<fRadia[iStation].size() - 1; i++){
      Double_t area = secLx[i]*secLy[i];
      Int_t ratio = (Int_t)((area + 1e-5)/minArea);
      Int_t iColor = (Int_t)log2(ratio) + 2;
      //      Int_t iColor = (Int_t)((area + 1e-5)/minArea) + 1;
      if(iColor==10) iColor = 11;
//      cout << "iColor = " << iColor << endl;
    DrawPaves(fRadia[iStation][i], fRadia[iStation][i+1], secLx[i], secLy[i], paves, iColor, kMain, 0.0);
  }
  // If there are gaps fill gaps
  for(int id = 0; id < 4; id++){
    for(int i = nRadia - 2; i>=0; i--){
      if(id!=0 && i==nRadia - 2) continue;
      Double_t fraction = 0.0;
      if(i == nRadia-2) fraction = 0.1;
      Double_t delta = id*fMinSectorLength[iStation];
      Double_t rExt = fRadia[iStation][i+1]+delta;
      Double_t rInt = fRadia[iStation][i];
      Double_t area = secLx[i]*secLy[i];
      Int_t ratio = (Int_t)((area + 1e-5)/minArea);
      Int_t iColor = (Int_t)log2(ratio) + 2;
//      Int_t iColor = (Int_t)((area + 1e-5)/minArea) + 1;
      if(iColor==10) iColor = 11;

      DrawPaves(rInt, rExt > fRadia[iStation][nRadia-1] ? fRadia[iStation][nRadia-1] : rExt, 
		secLx[i], secLy[i], paves, iColor, kGaps, fraction);
    }
  }

  Double_t lx = secLx[0];
  Double_t ly = secLy[0];
  for(int j=0;j<5;j++){
	if(TMath::Abs(ly/lx - 1) < 1e-5) lx = lx/2;
	else ly = ly/2;
	if(lx < fMinSectorWidth[0] || ly < fMinSectorLength[0]) break;
	Double_t area = lx*ly;
	Double_t rExt = 2*fRadia[iStation][nRadia - 1]/3.;
	Double_t rInt = fRadia[iStation][0];
	Int_t ratio = (Int_t)((area + 1e-5)/minArea);
	Int_t iColor = (Int_t)log2(ratio) + 2;
	if(iColor==10) iColor = 11;
	DrawPaves(rInt, rExt, lx, ly, paves, iColor, kGaps, 0.0);
  }
  //************** Fill regions with sectors (end) *********************//

  //************** Draw regions and coordinates (begin) ***************//
  Int_t xMax = 250;//(Int_t)(fRadia[iStation][nRadia - 1]*1.5);
  Int_t xMin = -xMax;
  Int_t yMin = xMin;
  Int_t yMax = xMax;
  gPad->Range(xMin,yMin,xMax,yMax);
  gPad->SetFillColor(0);

  // Draw circled regions
  for(Int_t i = 0; i<=nRadia - 1;i++){
    // Draw regions
    TEllipse* el = new TEllipse(0,0,fRadia[iStation][i], fRadia[iStation][i]);
    el->SetFillColor(0);
    el->Draw();
  }

  // Draw coordinate system
  TArrow* arrowX = new TArrow(xMin,0,xMax,0,0.03,"|>");
  TArrow* arrowY = new TArrow(0,yMin,0,yMax,0.03,"|>");
  arrowX->SetAngle(30);
  arrowY->SetAngle(30);
  arrowX->Draw();
  arrowY->Draw();
  //************** Draw regions and coordinates (end) ******************//

  //************** Write parameters in ASCII file (begin) **************//
  CreateDigiFile(iStation, 1, paves);
  //************** Write parameters in ASCII file (end) ****************//

  //************** Save graphics (begin) *******************************//
  TString epsFileName(name);
  epsFileName += ".eps";
  canvas->Print(epsFileName.Data());
  canvas->Close();
  //************** Save graphics (end) *********************************//
}
// -------------------------------------------------------------------------

// -----   SetSectorSizes method   -----------------------------------------
void CbmMuchSegmentation::SetSectorSizes(int nRegions, double minSecLx, double minSecLy, 
					 vector<double>& secLx, vector<double>& secLy){
  for(Int_t i=0;i<nRegions;i++){
    double width, height;
    if(fabs(minSecLy - minSecLx) < 1e-5){
      if(i%2==0){
	width = height = pow(2.,i/2.)*minSecLx;      
      }
      else{
	width = pow(2.,(i-1)/2.)*minSecLx;
	height = 2*width;
      }
    }
    else{
      if(i%2==0){
	assert(fabs(minSecLy - 2.*minSecLx) < 1e-5);
	height = pow(2.,i/2.)*minSecLy;
	width = pow(2.,i/2.)*minSecLx;
      }
      else{
	width = pow(2.,(i+1)/2.)*minSecLx;
	height = pow(2.,(i-1)/2.)*minSecLy;
      }
    }
    secLx[i] = width;
    secLy[i] = height;
  }    
}
// -------------------------------------------------------------------------

// ------   DrawPaves method   ---------------------------------------------
void CbmMuchSegmentation::DrawPaves(Double_t rad1, Double_t rad2, Double_t width, Double_t height, 
				    vector<TPave*>& paves, Int_t iColor, PlainPart iPart, Double_t fraction){
  // Define number of rows and columns for the region
  Int_t nRows = (Int_t)((rad2 + 1e-5)/height);
  Int_t nColumns;
  ++nRows *= 2;
  if(fabs(2.*width - height) < 1e-5)
    nColumns  = 2*nRows;
  else if(fabs(width - height) < 1e-5)
    nColumns = nRows;
  else{
    cout << width << "\t" << height << endl;
    cout << "Error occured. Exiting...\n"; 
    return;
  }

  // Fill the region
  Double_t x1,x2,y1,y2; // current coordianates of the card
  Double_t regionSize = nRows*height; // size of the region side [cm]

  assert(nRows%2 == 0);
  Int_t n = nRows/2;
  Int_t k = 0;
  for(int i = 0; i<nColumns/2; i++){    
    if(iPart == kMain && i>0) k = i-1;
    for(int j = k; j<n; j++){
      x1 = -regionSize/2. + i*width;
      y1 = -regionSize/2. + j*height;
      x2 = x1 + width;
      y2 = y1 + height;
      

      // Verify if the new card does not overlap existing cards
      Bool_t result = kFALSE;
      if(iPart == kMain)
		result = IntersectsFraction(rad2,x1,y1,x2,y2, 0.5) &&
		  !IntersectsFraction(rad1, x1,y1, x2,y2, 0.01);
      else 
		result = IntersectsFraction(rad2, x1,y1,x2,y2, fraction) &&
		  !IntersectsFraction(rad1, x1, y1, x2, y2, 0.0);
      if(result){
		if((x1==y1 && x2!= y2 && iPart==kMain) ||
		   ((x1!=y1 && x2== y2 && iPart==kMain))){
	      continue; // no axial symmetry
	}
	// Draw card
	DrawPave(x1,y1,x2,y2,paves, iColor);
	if((y1 + y2)==0){
	  DrawPave(-x2,y1,-x1,y2,paves,iColor);
	  DrawPave(y1,-x2,y2,-x1,paves,iColor);
	  DrawPave(y1,x1,y2,x2,paves,iColor);
	}
	else{
	  DrawPave(-x2,y1,-x1,y2,paves, iColor);
	  DrawPave(x1,-y2,x2,-y1,paves, iColor);
	  DrawPave(-x2,-y2,-x1,-y1,paves, iColor);
	  if(x1!=y1 && x2!=y2){
	    DrawPave(y1,x1,y2,x2,paves, iColor);
	    DrawPave(y1,-x2,y2,-x1,paves, iColor);
	    DrawPave(-y2,x1,-y1,x2,paves, iColor);
	    DrawPave(-y2,-x2,-y1,-x1,paves, iColor);
	  }
	}	
      }
    }
  }  
}
// -------------------------------------------------------------------------

// -----    DrawPave method   ----------------------------------------------
void CbmMuchSegmentation::DrawPave(Double_t x1, Double_t y1, Double_t x2, Double_t y2, 
				   vector<TPave*>& paves, Int_t iColor){
  TVector2 p1(x1+0.2,y1+0.2);
  TVector2 p2(x2-0.2,y1+0.2);
  TVector2 p3(x1+0.2,y2-0.2);
  TVector2 p4(x2-0.2,y2-0.2);
  if(IntersectsPaves(paves, p1,p2,p3,p4)) return;

  TPave* p = new TPave(x1,y1,x2,y2);
  p->SetBorderSize(1);
  p->SetFillColor(iColor);
  p->Draw();
  paves.push_back(p);
}
// -------------------------------------------------------------------------

// -----    IntersectsFraction method   ------------------------------------
Bool_t CbmMuchSegmentation::IntersectsFraction(Double_t iRadius, Double_t x1, Double_t y1, 
					    Double_t x2, Double_t y2, Double_t fraction){
  if(fraction > 1. + 1e-7) return kFALSE;
  Bool_t ul = kFALSE;
  Bool_t ur = kFALSE;
  Bool_t bl = kFALSE;
  Bool_t br = kFALSE;
  Int_t i = 0;
  Double_t Scard = (x2-x1)*(y2-y1);
  Double_t a,b,A,B,C,S;
  if(sqrt(x1*x1 + y1*y1) < iRadius) {
    bl = kTRUE;
    i++;
  }
  if(sqrt(x1*x1 + y2*y2) < iRadius){
    ul = kTRUE;
    i++;
  }
  if(sqrt(x2*x2 + y2*y2) < iRadius) {
    ur = kTRUE;
    i++;
  }
  if(sqrt(x2*x2 + y1*y1) < iRadius) {
    br = kTRUE;  
    i++;
  }
  
  switch(i){
  case 0:
    return kFALSE;
    break;
  case 1:
  case 3:
    Double_t xp1, yp1, xp2, yp2;
    if((br && i==1) || (!br && i==3)){
      xp1 = x2; 
      yp1 = i==1 ? sqrt(iRadius*iRadius - xp1*xp1) : -sqrt(iRadius*iRadius - xp1*xp1);
      yp2 = y1; 
      xp2 = i==1 ? -sqrt(iRadius*iRadius - yp2*yp2) : sqrt(iRadius*iRadius - yp2*yp2);      
    }
    else if((bl && i==1) || (!bl && i==3)){
      yp1= y1; 
      xp1 = i==1 ? sqrt(iRadius*iRadius - yp1*yp1) : -sqrt(iRadius*iRadius - yp1*yp1);
      xp2 = x1; 
      yp2 = i==1 ? sqrt(iRadius*iRadius - xp2*xp2) : -sqrt(iRadius*iRadius - xp2*xp2);
    }
    else if((ul && i==1) || (!ul && i==3)){
      xp1 = x1; 
      yp1 = i==1 ? -sqrt(iRadius*iRadius - xp1*xp1) : sqrt(iRadius*iRadius - xp1*xp1);
      yp2 = y2; 
      xp2 = i==1 ? sqrt(iRadius*iRadius - yp2*yp2) : -sqrt(iRadius*iRadius - yp2*yp2);
    }
    else if(ur){
      yp1 = y2; 
      xp1 = i==1 ? -sqrt(iRadius*iRadius - yp1*yp1) : sqrt(iRadius*iRadius - yp1*yp1);
      xp2 = x2; 
      yp2 = i==1 ? -sqrt(iRadius*iRadius - xp2*xp2) : sqrt(iRadius*iRadius - xp2*xp2);
    }
    S = 0.5*(fabs(xp1-xp2)*fabs(yp1-yp2));
    if(i==1){
      if(S/Scard > fraction) return kTRUE;
    }
    else if(i==3){
      if((Scard - S)/S > fraction) return kTRUE;
    }
    break;
  case 2:
    if(ur && br){
      B = y2; C = y1; A = x2;
    }
    else if(ul && ur){
      B = x2; C = x1; A = y2;     
    }
    else if(bl && ul){
      B = y2; C = y1; A = x1;
    }
    else if(bl && br){
      B = x2; C = x1; A = y1;
    }
    a = sqrt(iRadius*iRadius - B*B) - fabs(A);
    b = sqrt(iRadius*iRadius - C*C) - fabs(A);
    S = (a+b)/2.*(B-C);
    if(S/Scard > fraction) return kTRUE;
    break;
  case 4:
    return kTRUE;
    break;    
  }
  return kFALSE;
}
// -------------------------------------------------------------------------

// ------   IntersectsPaves method   ---------------------------------------
Bool_t CbmMuchSegmentation::IntersectsPaves(vector<TPave*> paves, TVector2 p1, TVector2 p2, 
					    TVector2 p3, TVector2 p4){
  for(int l = 0;l<(int)paves.size();l++){
    if(IntersectsPave(paves.at(l),p1,p2,p3,p4)){
      return kTRUE;
      break;
    }
  }
  return kFALSE;
}
// -------------------------------------------------------------------------

// ------   IntersectsPave method   ----------------------------------------
Bool_t CbmMuchSegmentation::IntersectsPave(TPave* pave, TVector2 p1, TVector2 p2, 
					   TVector2 p3, TVector2 p4){
  Double_t xPave1 = pave->GetX1();
  Double_t yPave1 = pave->GetY1();
  Double_t xPave2 = pave->GetX2();
  Double_t yPave2 = pave->GetY2();

  Double_t x1 = p1.Px();
  Double_t x2 = p2.Px();
  Double_t x3 = p3.Px();
  Double_t x4 = p4.Px();
  Double_t y1 = p1.Py();
  Double_t y2 = p2.Py();
  Double_t y3 = p3.Py();
  Double_t y4 = p4.Py();

  if((x1<xPave2 && x1>xPave1 && y1 < yPave2 && y1 > yPave1) ||
     (x2<xPave2 && x2>xPave1 && y2 < yPave2 && y2 > yPave1) ||
     (x3<xPave2 && x3>xPave1 && y3 < yPave2 && y3 > yPave1) ||
     (x4<xPave2 && x4>xPave1 && y4 < yPave2 && y4 > yPave1))
    return kTRUE;
  return kFALSE;  
}
// -------------------------------------------------------------------------

// ------   CreateDigiFile method  -----------------------------------------
void CbmMuchSegmentation::CreateDigiFile(int iStation, int iType, vector<TPave*> paves){
  ofstream digiFile(fDigiFileName, ios::app);
  Double_t minLx = fMinSectorWidth[0];//iStation];
  Double_t minLy = fMinSectorLength[0];//iStation];
  Double_t minDx = fMinPadWidth[0];//iStation];
  Double_t minDy = fMinPadLength[0];//iStation];
  Double_t minSecArea = minLx*minLy;
  Double_t angle = fAngle[iStation];
  
    digiFile << iStation + 1 << "\t" << angle << "\t" << paves.size() << endl;
    for(int i=0; i<(int)paves.size();i++){
      TPave* pave = paves.at(i);
      Double_t lx = pave->GetX2() - pave->GetX1();
      Double_t ly = pave->GetY2() - pave->GetY1();
      Double_t x0 = pave->GetX1() + lx/2.;
      Double_t y0 = pave->GetY1() + ly/2.;

      Double_t secArea = lx*ly;

      Int_t ratio = (int)((secArea + 1e-7)/minSecArea);
      Double_t dx;
      Double_t dy;

      // number of the region for the sector
      Int_t iRegion = (Int_t)(log((Double_t)ratio)/0.693147) + 1;

      Int_t k = (iRegion-1)/2;
      Int_t l = iRegion/2;

      // Calculate pad sizes
      if(fabs(minLx-minLy)<1e-7){
	dx = pow(2.,k)*minDx;
	dy = pow(2.,l)*minDy;
      }
      else {
	dx = pow(2.,l)*minDx;
	dy = pow(2.,k)*minDy;
	if(minLx > minLy){
	  Double_t temp = dx;
	  dx = dy;
	  dy = temp;
	}
      }

      Int_t nCols = (Int_t)((lx+dx/2.)/dx);
      Int_t nRows = (Int_t)((ly+dy/2.)/dy);

      digiFile << i+1 << "\t" << iType << "\t" 
	       << x0 << "\t" << y0 << "\t" 
	       << lx << "\t" << ly << "\t" 
	       << nCols << "\t" << nRows << endl;
    }
  digiFile.close();
}
// -------------------------------------------------------------------------

ClassImp(CbmMuchSegmentation)
