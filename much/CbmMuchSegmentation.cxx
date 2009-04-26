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

#include "FairRuntimeDb.h"
#include "FairRootManager.h"
#include "CbmMuchPoint.h"
#include "CbmMuchStationGem.h"
#include "CbmMuchLayer.h"
#include "CbmMuchModule.h"
#include "CbmMuchSector.h"
#include "CbmMuchGeoScheme.h"

#include "TMath.h"
#include "TClonesArray.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TFile.h"
#include "TH1.h"
#include "TF1.h"

#include <iostream>
#include <cassert>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
CbmMuchSegmentation::CbmMuchSegmentation(){
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchSegmentation::CbmMuchSegmentation(char* digiFileName){
    fDigiFileName = digiFileName;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmMuchSegmentation::~CbmMuchSegmentation() {
}
// -------------------------------------------------------------------------


// -----   Private method SetParContainers  --------------------------------
void CbmMuchSegmentation::SetParContainers() {
    // Get runtime database
    FairRuntimeDb* db = FairRuntimeDb::instance();
    if ( ! db ) Fatal("SetParContainers", "No runtime database");
    // Get MUCH geometry param
    fGeoPar = (CbmGeoMuchPar*) db->getContainer("CbmGeoMuchPar");
}
// -------------------------------------------------------------------------


// -----   Private method Init  --------------------------------------------
InitStatus CbmMuchSegmentation::Init(){
    FairRootManager* fManager = FairRootManager::Instance();
    fPoints = (TClonesArray *) fManager->GetObject("MuchPoint");
    fEvents = 0;

    fStations = fGeoPar->GetStations();
    if(!fStations) Fatal("Init", "No input array of MUCH stations.");
    fNStations = fStations->GetEntries();
    fHistHitDensity = new TH1D*[fNStations];

    for (Int_t i=0;i<fNStations;i++) {
        CbmMuchStation* station = (CbmMuchStation*) fStations->At(i);
        fHistHitDensity[i] = new TH1D(Form("hStation%i",i+1),Form("Station %i",i+1), 110, 0, 220);
        fHistHitDensity[i]->GetXaxis()->SetTitle("r, cm");
        fHistHitDensity[i]->GetYaxis()->SetTitle("hits/(event#timescm^{2})");
        fHistHitDensity[i]->SetLineColor(4);
        fHistHitDensity[i]->SetLineWidth(2);
        fHistHitDensity[i]->GetYaxis()->SetTitleOffset(1.27);
    }
    return kSUCCESS;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmMuchSegmentation::Exec(Option_t * option){
    fEvents++;
    printf("Event: %i\n",fEvents);

    gStyle->SetOptStat(0);
    for(Int_t iPoint = 0; iPoint < fPoints->GetEntriesFast(); iPoint++){
        CbmMuchPoint* point = (CbmMuchPoint*)fPoints->At(iPoint);
        if(!point) continue;
        Int_t iStation = CbmMuchGeoScheme::GetStationIndex(point->GetDetectorID());
        CbmMuchStation* station = (CbmMuchStation*) fStations->At(iStation);
        if (station->GetDetectorType()==2) continue;

        Int_t iLayer = CbmMuchGeoScheme::GetLayerIndex(point->GetDetectorID());
        //    printf("iStation = %i\n", iStation);
        //    printf("detId = %qd\n", point->GetDetectorID());
        //    printf("iPoint = %i\n", iPoint);
        assert(iStation >= 0 && iStation < fNStations);
        if(iLayer) continue;
        TVector3 pos;
        point->Position(pos);
        ((TH1D*) fHistHitDensity[iStation])->Fill(pos.Pt());
    }
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmMuchSegmentation::FinishTask(){
    // Create normalization histo
    TH1D* hNorm = new TH1D("hNorm","",110,0,220);
    Double_t binSize = 2.;
    for(Int_t l=0; l<100; l++){
        Double_t R1 = l*binSize;
        Double_t R2 = (l+1)*binSize;
        Double_t s = TMath::Pi()*(R2*R2-R1*R1);
        hNorm->SetBinContent(l+1,s*fEvents);
    }

    for (Int_t i=0;i<fNStations;i++) {
        CbmMuchStation* station = (CbmMuchStation*)fStations->At(i);
        TH1D* h =fHistHitDensity[i];
        h->Divide(hNorm);
        TCanvas* c1 = new TCanvas(Form("cStation%i",i+1),Form("Station %i",i+1),10,10,500,500);
        c1->SetLogy();
        c1->SetLeftMargin(0.12);
        c1->SetRightMargin(0.08);
        TF1* f1 = new TF1("f1", "exp([0] + [1]*x)");
        f1->SetParameter(0, 0.5);
        f1->SetParameter(1, -0.1);
        h->Fit("f1","LL","",station->GetRmin(),station->GetRmax());
        Double_t exp0 = h->GetFunction("f1")->GetParameter(0);
        Double_t exp1 = h->GetFunction("f1")->GetParameter(1);
        fExp0.push_back(exp0);
        fExp1.push_back(exp1);

        h->Draw();
        c1->Print();
    }

    for (Int_t i=0;i<fNStations;i++) {
        CbmMuchStation* station = (CbmMuchStation*)fStations->At(i);
        if (station->GetDetectorType()==2) continue;

        Int_t nLayers = station->GetNLayers();
        for(Int_t iLayer = 0; iLayer < nLayers; iLayer++){
            CbmMuchLayer* layer = station->GetLayer(iLayer);
            if(!layer) Fatal("Init", "Incomplete layers array.");
            // Get layer sides
            InitLayerSide(layer->GetSideF());
            InitLayerSide(layer->GetSideB());
        }
        printf("Station%i segmented\n",i+1);
    }

    //   // Write full segmentation (sectors + pads + grid)
    //   CbmMuchGeoScheme* geoScheme = CbmMuchGeoScheme::Instance();
    //   geoScheme->Init(fStations);
    //   geoScheme->InitGrid();

    // Save parameters
    TFile* f = new TFile(fDigiFileName, "RECREATE");
    fStations->Write("stations",1);

    f->Close();


    Print();
}
// -------------------------------------------------------------------------


// -----   Private method InitLayerSide  -----------------------------------
void CbmMuchSegmentation::InitLayerSide(CbmMuchLayerSide* layerSide){
    if(!layerSide) Fatal("Init", "Incomplete layer sides array.");
    Int_t nModules = layerSide->GetNModules();
    for(Int_t iModule = 0; iModule < nModules; iModule++){
        CbmMuchModule* module = layerSide->GetModule(iModule);
        SegmentModule(module);
    }
}
// -------------------------------------------------------------------------


// -----   Private method SegmentModule  -----------------------------------
void CbmMuchSegmentation::SegmentModule(CbmMuchModule* module){
    TVector3 size = module->GetSize();
    Double_t modLx = size.X();
    Double_t modLy = size.Y();
    Double_t modLz = size.Z();
    TVector3 position = module->GetPosition();
    Double_t modX = position.X();
    Double_t modY = position.Y();
    Double_t modZ = position.Z();

    Bool_t result = modLx > modLy;
    Int_t iRatio = (result) ? (Int_t)((modLx+1e-3)/modLy)
            : (Int_t)((modLy+1e-3)/modLx);
    Int_t detectorId = module->GetDetectorId();
    //  Int_t iStation = CbmMuchGeoScheme::GetStationIndex(moduleId);
    //  Int_t iLayer = CbmMuchGeoScheme::GetLayerIndex(moduleId);
    //  Int_t iSide = CbmMuchGeoScheme::GetLayerSideIndex(moduleId);
    //  Int_t iModule = CbmMuchGeoScheme::GetModuleIndex(moduleId);
    Double_t secLx = (result) ? modLx/iRatio : modLx;
    Double_t secLy = (result) ? modLy : modLy/iRatio;
    for(Int_t i = 0; i< iRatio; i++){
        Double_t secX = (result) ? modX - modLx/2. + (i+0.5)*secLx : modX;
        Double_t secY = (result) ? modY : modY - modLy/2. + (i+0.5)*secLy;
        Int_t iSector = module->GetNSectors();//CbmMuchGeoScheme::GetDetectorId(iStation, iLayer, iSide, iModule, module->GetNSectors());

        TVector3 position(secX, secY, modZ);
        TVector3 size(secLx, secLy, modLz);
        SegmentSector(module, new CbmMuchSector(detectorId, iSector, position, size, 8, 16));
    }
}
// -------------------------------------------------------------------------

// -----  Private method SegmentSector  ------------------------------------
void CbmMuchSegmentation::SegmentSector(CbmMuchModule* module, CbmMuchSector* sector){
    TVector3 secSize = sector->GetSize();
    TVector3 secPosition = sector->GetPosition();
    Int_t detectorId = module->GetDetectorId();
    Int_t iStation = CbmMuchGeoScheme::GetStationIndex(detectorId);
    Int_t iLayer = CbmMuchGeoScheme::GetLayerIndex(detectorId);
    Int_t iSide = CbmMuchGeoScheme::GetLayerSideIndex(detectorId);
    Int_t iModule = CbmMuchGeoScheme::GetModuleIndex(detectorId);
    Double_t secLx = secSize.X();
    Double_t secLy = secSize.Y();
    Double_t secLz = secSize.Z();
    Double_t secX  = secPosition.X();
    Double_t secY  = secPosition.Y();
    Double_t secZ  = secPosition.Z();

    assert(CbmMuchGeoScheme::GetStationIndex(sector->GetDetectorId()) == iStation);
    assert(CbmMuchGeoScheme::GetLayerIndex(sector->GetDetectorId()) == iLayer);
    assert(CbmMuchGeoScheme::GetLayerSideIndex(sector->GetDetectorId()) == iSide);
    assert(CbmMuchGeoScheme::GetModuleIndex(sector->GetDetectorId()) == iModule);

    Bool_t result1 = ShouldSegmentByX(sector);
    Bool_t result2 = ShouldSegmentByY(sector);

    if(result1 || result2){
        delete sector;
    }
    else{
        CbmMuchStation* station = (CbmMuchStation*)fStations->At(iStation);
        Double_t rMax = station->GetRmax();
        if((IntersectsRad(sector, module->GetCutRadius())==2) || !IntersectsRad(sector, rMax)){
            delete sector;
            return;
        }
        module->AddSector(sector);
        return;
    }

    Int_t iSector;
    Double_t newSecX, newSecY, newSecLx, newSecLy;
    Bool_t equal = TMath::Abs(secLx - secLy) < 1e-5;
    Bool_t res = secLx > secLy;
    TVector3 position, size;
    if(result1 && result2){
        if(equal) {
            res = kTRUE;
        }
        for(Int_t i = 0; i < 2; ++i){
            iSector = module->GetNSectors();//CbmMuchGeoScheme::GetDetIdFromModule(moduleId, module->GetNSectors());
            newSecLx = res ? secLx/2. : secLx;
            newSecLy = res ? secLy    : secLy/2.;
            newSecX  = res ? secX + (i - 0.5)*newSecLx : secX;
            newSecY  = res ? secY : secY + (i - 0.5)*newSecLy;
            position.SetXYZ(newSecX, newSecY, secZ);
            size.SetXYZ(newSecLx, newSecLy, secLz);
            SegmentSector(module, new CbmMuchSector(detectorId, iSector, position, size, 8, 16));
        }
    }
    else if(result1 || result2){
        for(Int_t i = 0; i < 2; i++){
            iSector = module->GetNSectors();//CbmMuchGeoScheme::GetDetIdFromModule(moduleId, module->GetNSectors());
            newSecLx = result1 ? secLx/2. : secLx;
            newSecLy = result1 ? secLy    : secLy/2;
            newSecX  = result1 ? secX + (i - 0.5)*newSecLx : secX;
            newSecY  = result1 ? secY : secY + (i - 0.5)*newSecLy;
            position.SetXYZ(newSecX, newSecY, secZ);
            size.SetXYZ(newSecLx, newSecLy, secLz);
            SegmentSector(module, new CbmMuchSector(detectorId, iSector, position, size, 8, 16));
        }
    }
}
// -------------------------------------------------------------------------

// -----  Private method ShouldSegmentByX  ---------------------------------
Bool_t CbmMuchSegmentation::ShouldSegmentByX(CbmMuchSector* sector){
    Double_t secLx = sector->GetSize()[0];
    Double_t secLy = sector->GetSize()[1];
    Double_t secX  = sector->GetPosition()[0];
    Double_t secY  = sector->GetPosition()[1];

    Double_t ulR = TMath::Sqrt((secX - secLx/2.)*(secX - secLx/2.) + (secY + secLy/2.)*(secY + secLy/2.));
    Double_t urR = TMath::Sqrt((secX + secLx/2.)*(secX + secLx/2.) + (secY + secLy/2.)*(secY + secLy/2.));
    Double_t blR = TMath::Sqrt((secX - secLx/2.)*(secX - secLx/2.) + (secY - secLy/2.)*(secY - secLy/2.));
    Double_t brR = TMath::Sqrt((secX + secLx/2.)*(secX + secLx/2.) + (secY - secLy/2.)*(secY - secLy/2.));

    Double_t uR = (ulR < urR) ? ulR : urR;
    Double_t bR = (blR < brR) ? blR : brR;
    Double_t R  = (uR < bR) ? uR : bR;

    Int_t iStation = CbmMuchGeoScheme::GetStationIndex(sector->GetDetectorId());
    CbmMuchStationGem* station = (CbmMuchStationGem*)fStations->At(iStation);
    // Check minimum and maximum allowed resolution
    Double_t sigmaMax = station->GetSigmaXmax(); //[cm]
    Double_t sigmaMin = station->GetSigmaXmin(); //[cm]
    Double_t sigma    = sector->GetSigmaX();
    if(sigma > sigmaMin && sigma/2. < sigmaMin) return false;
    if(sigma > sigmaMax) return true;
    // Check for occupancy
    Double_t hitDensity   = exp(fExp0.at(iStation) + fExp1.at(iStation)*R);
    Double_t occupancyMax = station->GetOccupancyMax();
    Double_t sPad  = secLx*secLy/128.;
    Double_t nPads = (1.354-0.304*TMath::Log2(sPad)); // number of pads fired by one track in average
    //printf("nPads:%f\n",nPads);
    Double_t occupancy = hitDensity*sPad*nPads;
    if(occupancy > occupancyMax) return true;
    return false;
}
// -------------------------------------------------------------------------

// -----  Private method ShouldSegmentByY  ---------------------------------
Bool_t CbmMuchSegmentation::ShouldSegmentByY(CbmMuchSector* sector){
    Double_t secLx = sector->GetSize()[0];
    Double_t secLy = sector->GetSize()[1];
    Double_t secX  = sector->GetPosition()[0];
    Double_t secY  = sector->GetPosition()[1];

    Double_t ulR = TMath::Sqrt((secX - secLx/2.)*(secX - secLx/2.) + (secY + secLy/2.)*(secY + secLy/2.));
    Double_t urR = TMath::Sqrt((secX + secLx/2.)*(secX + secLx/2.) + (secY + secLy/2.)*(secY + secLy/2.));
    Double_t blR = TMath::Sqrt((secX - secLx/2.)*(secX - secLx/2.) + (secY - secLy/2.)*(secY - secLy/2.));
    Double_t brR = TMath::Sqrt((secX + secLx/2.)*(secX + secLx/2.) + (secY - secLy/2.)*(secY - secLy/2.));

    Double_t uR = (ulR < urR) ? ulR : urR;
    Double_t bR = (blR < brR) ? blR : brR;
    Double_t R  = (uR < bR) ? uR : bR;

    Int_t iStation = CbmMuchGeoScheme::GetStationIndex(sector->GetDetectorId());
    CbmMuchStationGem* station = (CbmMuchStationGem*)fStations->At(iStation);
    // Check minimum and maximum allowed resolution
    Double_t sigmaMax = station->GetSigmaYmax(); //[cm]
    Double_t sigmaMin = station->GetSigmaYmin(); //[cm]
    Double_t sigma    = sector->GetSigmaY();
    if(sigma > sigmaMin && sigma/2. < sigmaMin) return false;
    if(sigma > sigmaMax) return true;
    // Check for occupancy
    Double_t hitDensity   = exp(fExp0.at(iStation) + fExp1.at(iStation)*R);
    Double_t occupancyMax = station->GetOccupancyMax();
    Double_t sPad  = secLx*secLy/128.;
    Double_t nPads = (1.354-0.304*TMath::Log2(sPad)); // number of pads fired by one track in average
    Double_t occupancy = hitDensity*sPad*nPads;
    if(occupancy > occupancyMax) return true;
    return false;
}
// -------------------------------------------------------------------------


// -----  Private method IntersectsHole  -----------------------------------
Int_t CbmMuchSegmentation::IntersectsRad(CbmMuchSector* sector, Double_t radius){
    if(radius < 0) return 0;

    Int_t intersects = 0;

    Double_t secLx = sector->GetSize()[0];
    Double_t secLy = sector->GetSize()[1];
    Double_t secX  = sector->GetPosition()[0];
    Double_t secY  = sector->GetPosition()[1];

    Double_t ulR = TMath::Sqrt((secX - secLx/2.)*(secX - secLx/2.) + (secY + secLy/2.)*(secY + secLy/2.));
    Double_t urR = TMath::Sqrt((secX + secLx/2.)*(secX + secLx/2.) + (secY + secLy/2.)*(secY + secLy/2.));
    Double_t blR = TMath::Sqrt((secX - secLx/2.)*(secX - secLx/2.) + (secY - secLy/2.)*(secY - secLy/2.));
    Double_t brR = TMath::Sqrt((secX + secLx/2.)*(secX + secLx/2.) + (secY - secLy/2.)*(secY - secLy/2.));

    if(ulR < radius) intersects++;
    if(urR < radius) intersects++;
    if(blR < radius) intersects++;
    if(brR < radius) intersects++;

    if(intersects == 4) return 2;
    if(intersects) return 1;
    else return 0;
}
// -------------------------------------------------------------------------

void CbmMuchSegmentation::Print(){
    printf("Segmentation written to file %s\n", fDigiFileName);
    Int_t nTotSectors = 0;
    Int_t nTotChannels = 0;
    printf("=========================================================================================\n");
    printf(" Station Nr.\t| Sectors\t| Channels\t| Pad min size\t\t| Pad max length\t \n");
    printf("-----------------------------------------------------------------------------------------\n");
    for(Int_t iStation=0; iStation < fStations->GetEntries(); ++iStation){
        CbmMuchStation* station = (CbmMuchStation*) fStations->At(iStation);
        Int_t nSectors = 0;
        Int_t nChannels = 0;
        Double_t padMaxLx = std::numeric_limits<Double_t>::min();
        Double_t padMinLx = std::numeric_limits<Double_t>::max();
        Double_t padMaxLy = std::numeric_limits<Double_t>::min();
        Double_t padMinLy = std::numeric_limits<Double_t>::max();
        if(!station) continue;

        for(Int_t iLayer = 0; iLayer < station->GetNLayers(); ++iLayer){
            CbmMuchLayer* layer = station->GetLayer(iLayer);
            if(!layer) continue;
            for(Int_t iSide = 0; iSide < 2; ++iSide){
                CbmMuchLayerSide* side = layer->GetSide(iSide);
                if(!side) continue;
                for(Int_t iModule=0; iModule < side->GetNModules(); ++iModule){
                    CbmMuchModule* module = side->GetModule(iModule);
                    if(!module) continue;
                    nSectors += module->GetNSectors();
                    for(Int_t iSector=0; iSector<module->GetNSectors(); ++iSector){
                        CbmMuchSector* sector = module->GetSector(iSector);
                        if(!sector) continue;
                        Double_t padLx = sector->GetDx();
                        Double_t padLy = sector->GetDy();
                        if(padLx > padMaxLx) padMaxLx = padLx;
                        if(padLx < padMinLx) padMinLx = padLx;
                        if(padLy > padMaxLy) padMaxLy = padLy;
                        if(padLy < padMinLy) padMinLy = padLy;
                        nChannels += sector->GetNChannels();
                    }
                }
            }
        }

        printf("%i\t\t| %i\t\t| %i\t| %5.4fx%5.4f\t\t| %5.4fx%5.4f\n",iStation+1, nSectors, nChannels, padMinLx, padMinLy, padMaxLx, padMaxLy);
        nTotSectors += nSectors;
        nTotChannels += nChannels;
    }
    printf("-----------------------------------------------------------------------------------------\n");
    printf(" Total:\t\t| %i\t\t| %i\t| \t\t\t|\n", nTotSectors, nTotChannels);
    printf("=========================================================================================\n");
}

ClassImp(CbmMuchSegmentation)
