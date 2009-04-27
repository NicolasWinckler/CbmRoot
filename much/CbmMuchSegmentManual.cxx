/** CbmMuchSegmentManual.cxx
 *@author Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since 02.04.09
 *@version 1.0
 *
 * class for making parameter file for MUCH digitizer
 *
 */

#include "CbmMuchSegmentManual.h"
#include "TFile.h"
#include "TMath.h"

#include "FairRuntimeDb.h"

#include "CbmGeoMuchPar.h"

#include "CbmMuchStation.h"
#include "CbmMuchStationGem.h"
#include "CbmMuchLayer.h"
#include "CbmMuchLayerSide.h"
#include "CbmMuchModule.h"
#include "CbmMuchSector.h"

// -----   Default constructor   -------------------------------------------
CbmMuchSegmentManual::CbmMuchSegmentManual(){
    fDigiFileName = (char*)"much.digi.root";
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchSegmentManual::CbmMuchSegmentManual(char* digiFileName){
    fDigiFileName = digiFileName;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmMuchSegmentManual::~CbmMuchSegmentManual() {
}
// -------------------------------------------------------------------------

// -----   Public method SetNRegions  --------------------------------------
void CbmMuchSegmentManual::SetNRegions(Int_t iStation, Int_t nRegions){
    //	if(iStation < 0 || iStation >= fStations->GetEntries())
    //		Fatal("SetNRegions", "iStation is out of range.");
    fNRegions[iStation] = nRegions;
    fRadii[iStation].resize(nRegions);
}
// -------------------------------------------------------------------------

void CbmMuchSegmentManual::SetNRegions(Int_t nRegions[]){
    for(Int_t iStation=0; iStation < fNStations; ++iStation){
        SetNRegions(iStation, nRegions[iStation]);
    }
}

void CbmMuchSegmentManual::SetNChannels(Int_t iStation, Int_t nChannels){
	fNChannels[iStation] = nChannels;
    // Deal with channels more universally
    Int_t n = (Int_t)(TMath::Log2(fNChannels[iStation]) + 1e-2);
    Int_t nChans = (Int_t)(TMath::Power(2, n) + 1e-2);
    if(nChans != fNChannels[iStation]) Fatal("Init", "Number of channels should be equal to two with integer power.");
    Int_t nPower = n/2;
    fNCols[iStation] = (Int_t)TMath::Power(2, nPower);
    fNRows[iStation] = n%2 != 0 ? (Int_t)TMath::Power(2, nPower+1) : fNCols[iStation];
}

void CbmMuchSegmentManual::SetNChannels(Int_t nChannels[]){
    for(Int_t iStation=0; iStation < fNStations; ++iStation){
        SetNChannels(iStation, nChannels[iStation]);
    }
}
// -----   Public method SetRegionRadius  ----------------------------------
void CbmMuchSegmentManual::SetRegionRadius(Int_t iStation, Int_t iRegion, Double_t radius){
    //	if(iStation < 0 || iStation >= fStations->GetEntries()) Fatal("SetRegionRadius", "iStation is out of range.");
    if(iRegion < 0 || iRegion >= fNRegions[iStation]) Fatal("SetRegionRadius", "iRegion is out of range.");
    fRadii[iStation].at(iRegion) = radius;
}
// -------------------------------------------------------------------------

void CbmMuchSegmentManual::SetRegionRadii(Int_t iStation, Double_t radii[]){
    for(Int_t iRegion=0;iRegion<fNRegions[iStation]; ++iRegion){
        SetRegionRadius(iStation, iRegion, radii[iRegion]);
    }
}

// -----   Public method SetMinSigmaX  -------------------------------------
void CbmMuchSegmentManual::SetMinSigmaX(Int_t iStation, Double_t sigmaX){
    //	if(iStation < 0 || iStation >= fStations->GetEntries()) Fatal("SetRegionRadius", "iStation is out of range.");
    fSecMinLx[iStation] = fNCols[iStation]*TMath::Sqrt(12.)*sigmaX;
}
// -------------------------------------------------------------------------

void CbmMuchSegmentManual::SetMinSigmaX(Double_t sigmaX[]){
    for(Int_t iStation=0;iStation<fNStations; ++iStation){
        SetMinSigmaX(iStation, sigmaX[iStation]);
    }
}

// -----   Public method SetMinSigmaY  -------------------------------------
void CbmMuchSegmentManual::SetMinSigmaY(Int_t iStation, Double_t sigmaY){
    //	if(iStation < 0 || iStation >= fStations->GetEntries()) Fatal("SetRegionRadius", "iStation is out of range.");
    fSecMinLy[iStation] = fNRows[iStation]*TMath::Sqrt(12.)*sigmaY;
}
// -------------------------------------------------------------------------

void CbmMuchSegmentManual::SetMinSigmaY(Double_t sigmaY[]){
    for(Int_t iStation=0;iStation<fNStations; ++iStation){
        SetMinSigmaY(iStation, sigmaY[iStation]);
    }
}

// -----   Public method SetMinPadLx   -------------------------------------
void CbmMuchSegmentManual::SetMinPadLx(Int_t iStation, Double_t padLx){
    //	if(iStation < 0 || iStation >= fStations->GetEntries()) Fatal("SetRegionRadius", "iStation is out of range.");
    fSecMinLx[iStation] = fNCols[iStation]*padLx;
}
// -------------------------------------------------------------------------

void CbmMuchSegmentManual::SetMinPadLx(Double_t padLx[]){
    for(Int_t iStation=0;iStation<fNStations; ++iStation){
        SetMinPadLx(iStation, padLx[iStation]);
    }
}

// -----   Public method SetMinPadLy   -------------------------------------
void CbmMuchSegmentManual::SetMinPadLy(Int_t iStation, Double_t padLy){
    //	if(iStation < 0 || iStation >= fStations->GetEntries()) Fatal("SetRegionRadius", "iStation is out of range.");
    fSecMinLy[iStation] = fNRows[iStation]*padLy;
}
// -------------------------------------------------------------------------

void CbmMuchSegmentManual::SetMinPadLy(Double_t padLy[]){
    for(Int_t iStation=0;iStation<fNStations; ++iStation){
        SetMinPadLy(iStation, padLy[iStation]);
    }
}

// -----   Public method SetMaxSigmaX  -------------------------------------
void CbmMuchSegmentManual::SetMaxSigmaX(Int_t iStation, Double_t sigmaX){
    //  if(iStation < 0 || iStation >= fStations->GetEntries()) Fatal("SetRegionRadius", "iStation is out of range.");
    fSecMaxLx[iStation] = fNCols[iStation]*TMath::Sqrt(12.)*sigmaX;
}
// -------------------------------------------------------------------------

void CbmMuchSegmentManual::SetMaxSigmaX(Double_t sigmaX[]){
    for(Int_t iStation=0;iStation<fNStations; ++iStation){
        SetMaxSigmaX(iStation, sigmaX[iStation]);
    }
}

// -----   Public method SetMaxSigmaY  -------------------------------------
void CbmMuchSegmentManual::SetMaxSigmaY(Int_t iStation, Double_t sigmaY){
    //  if(iStation < 0 || iStation >= fStations->GetEntries()) Fatal("SetRegionRadius", "iStation is out of range.");
    fSecMaxLy[iStation] = fNRows[iStation]*TMath::Sqrt(12.)*sigmaY;
}
// -------------------------------------------------------------------------

void CbmMuchSegmentManual::SetMaxSigmaY(Double_t sigmaY[]){
    for(Int_t iStation=0;iStation<fNStations; ++iStation){
        SetMaxSigmaY(iStation, sigmaY[iStation]);
    }
}

// -----   Public method SetMaxPadLx   -------------------------------------
void CbmMuchSegmentManual::SetMaxPadLx(Int_t iStation, Double_t padLx){
    //  if(iStation < 0 || iStation >= fStations->GetEntries()) Fatal("SetRegionRadius", "iStation is out of range.");
    fSecMaxLx[iStation] = fNCols[iStation]*padLx;
}
// -------------------------------------------------------------------------

void CbmMuchSegmentManual::SetMaxPadLx(Double_t padLx[]){
    for(Int_t iStation=0;iStation<fNStations; ++iStation){
        SetMaxPadLx(iStation, padLx[iStation]);
    }
}

// -----   Public method SetMaxPadLy   -------------------------------------
void CbmMuchSegmentManual::SetMaxPadLy(Int_t iStation, Double_t padLy){
    //  if(iStation < 0 || iStation >= fStations->GetEntries()) Fatal("SetRegionRadius", "iStation is out of range.");
    fSecMaxLy[iStation] = fNRows[iStation]*padLy;
}
// -------------------------------------------------------------------------

void CbmMuchSegmentManual::SetMaxPadLy(Double_t padLy[]){
    for(Int_t iStation=0;iStation<fNStations; ++iStation){
        SetMaxPadLy(iStation, padLy[iStation]);
    }
}

// -----   Private method SetParContainers  --------------------------------
void CbmMuchSegmentManual::SetParContainers() {
    // Get runtime database
    FairRuntimeDb* db = FairRuntimeDb::instance();
    if ( ! db ) Fatal("Init", "No runtime database");
    fGeoPar = (CbmGeoMuchPar*) db->getContainer("CbmGeoMuchPar");
}
// -------------------------------------------------------------------------

// -----   Private method Init ---------------------------------------------
InitStatus CbmMuchSegmentManual::Init(){
    // Get MUCH geometry parameter container
    fStations = fGeoPar->GetStations();
    if(!fStations) Fatal("Init", "No input array of MUCH stations.");
    if(fStations->GetEntries() != fNStations) Fatal("Init", "Incorrect number of stations.");

    printf("Number of stations: %i\n", fStations->GetEntries());

    // Define sector sizes for each region
    for(Int_t iStation=0; iStation<fStations->GetEntries(); ++iStation){
        CbmMuchStation* st = (CbmMuchStation*)fStations->At(iStation);
        if (st->GetDetectorType()==2) continue;
        CbmMuchStationGem* station = (CbmMuchStationGem*)st;

        // Default initialization if required
        if(fNChannels.find(iStation)==fNChannels.end()) SetNChannels(iStation, 128);
        if(fNRegions.find(iStation)==fNRegions.end()) SetNRegions(iStation, 2);
        if(fRadii[iStation].at(0)==0) {
            Double_t radii[] = {station->GetRmax() - 30., station->GetRmax()};
            SetRegionRadii(iStation, radii);
        }
        if(fSecMinLx.find(iStation)==fSecMinLx.end()) {
            if(iStation == 0) SetMinPadLx(iStation, 0.1386);
            else if(iStation == 1) SetMinPadLx(iStation, 0.4);
            else SetMinPadLx(iStation, 0.8);
        }
        if(fSecMinLy.find(iStation)==fSecMinLy.end()){
            if(iStation == 0) SetMinPadLy(iStation, 0.1386);
            else if(iStation == 1) SetMinPadLy(iStation, 0.4);
            else SetMinPadLy(iStation, 0.8);
        }
        if(fSecMaxLx.find(iStation)==fSecMaxLx.end()) SetMaxSigmaX(iStation, 0.32);
        if(fSecMaxLy.find(iStation)==fSecMaxLy.end()) SetMaxSigmaY(iStation, 0.32);

        fSecLx[iStation].push_back(fSecMinLx[iStation]);
        fSecLy[iStation].push_back(fSecMinLy[iStation]);
        for(Int_t iRegion=1; iRegion<fNRegions[iStation]; ++iRegion){
            Bool_t result = TMath::Abs(fSecLx[iStation].at(iRegion-1) - fSecLy[iStation].at(iRegion-1)) < 1e-5;
            Double_t secLx = result ? fSecLx[iStation].at(iRegion-1)   : 2*fSecLx[iStation].at(iRegion-1);
            Double_t secLy = result ? 2*fSecLy[iStation].at(iRegion-1) : fSecLy[iStation].at(iRegion-1);
            fSecLx[iStation].push_back(secLx);
            fSecLy[iStation].push_back(secLy);
        }
    }

    // Segment MuCh
    SegmentMuch();
    return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method SegmentMuch  --------------------------------------
void CbmMuchSegmentManual::SegmentMuch(){
    for(Int_t iStation=0; iStation<fStations->GetEntries(); ++iStation){
        CbmMuchStation* station = (CbmMuchStation*)fStations->At(iStation);
        if (station->GetDetectorType()==2) continue;

        Int_t nLayers = station->GetNLayers();
        for(Int_t iLayer = 0; iLayer < nLayers; ++iLayer){
            CbmMuchLayer* layer = station->GetLayer(iLayer);
            if(!layer) Fatal("SegmentMuch", "Incomplete layers array.");
            // Segment layer sides
            SegmentLayerSide(layer->GetSideF());
            SegmentLayerSide(layer->GetSideB());
        }
        printf("Station %i segmented\n",iStation+1);
    }

    // Save parameters
    TFile* f = new TFile(fDigiFileName, "RECREATE");
    fStations->Write("stations",1);
    f->Close();

    Print();
}
// -------------------------------------------------------------------------

// -----   Private method SegmentLayerSide  --------------------------------
void CbmMuchSegmentManual::SegmentLayerSide(CbmMuchLayerSide* layerSide){
    if(!layerSide) Fatal("SegmentLayerSide", "Incomplete layer sides array.");
    Int_t nModules = layerSide->GetNModules();
    for(Int_t iModule = 0; iModule < nModules; iModule++){
        CbmMuchModule* module = layerSide->GetModule(iModule);
        if(nModules > 1) SegmentModule(module, true);
        else SegmentModule(module, false);
    }
}
// -------------------------------------------------------------------------

// -----   Private method SegmentSector  -----------------------------------
void CbmMuchSegmentManual::SegmentModule(CbmMuchModule* module, Bool_t useModuleDesign){
    Double_t secMaxLx = GetSectorMaxSize(module, "Width");
    Double_t secMaxLy = GetSectorMaxSize(module, "Length");
    Double_t padMaxLx = GetPadMaxSize(module, "Width");
    Double_t padMaxLy = GetPadMaxSize(module, "Length");
    TVector3 size = module->GetSize();
    Double_t modLx = size.X();
    Double_t modLy = size.Y();
    Double_t modLz = size.Z();
    TVector3 position = module->GetPosition();
    Double_t modX = position.X();
    Double_t modY = position.Y();
    Double_t modZ = position.Z();
    Int_t detectorId = module->GetDetectorId();
    Int_t iStation = CbmMuchGeoScheme::GetStationIndex(detectorId);
    CbmMuchStationGem* station = (CbmMuchStationGem*)fStations->At(iStation);

    Int_t nCols = Int_t(modLx/secMaxLx);
    Int_t nRows = Int_t(modLy/secMaxLy);
    Int_t nX = modX < 0 ? -1 : 1;
    Int_t nY = modY < 0 ? -1 : 1;

    Double_t secX, secY;
    TVector3 secSize, secPosition;
    Int_t iSector;
    for(Int_t iCol=0; iCol<nCols; ++iCol){
        secX = useModuleDesign ? modX + nX*((iCol + 0.5)*secMaxLx - modLx/2.)
                : secMaxLx*nCols/2. - (0.5 + iCol)*secMaxLx;
        for(Int_t iRow=0; iRow<nRows; ++iRow){
            secY = useModuleDesign ? modY + nY*((iRow + 0.5)*secMaxLy - modLy/2.)
                    : secMaxLy*nRows/2. - (0.5 + iRow)*secMaxLy;
            iSector = module->GetNSectors();
            secSize.SetXYZ(secMaxLx, secMaxLy, modLz);
            secPosition.SetXYZ(secX, secY, modZ);
            SegmentSector(module, new CbmMuchSector(detectorId, iSector, secPosition, secSize, fNCols[iStation], fNRows[iStation]));
        }
    }

    // Process incomplete sectors
    Int_t nPadRows, nPadCols;
    Int_t n = useModuleDesign ? 1 : 2;
    Double_t ly = useModuleDesign ? modLy - secMaxLy*nRows : (modLy - secMaxLy*nRows)/2.;
    if(ly > padMaxLy){
        nPadRows = Int_t(ly/padMaxLy);
        for(Int_t i=0;i<n; ++i){
            secY = useModuleDesign ? modY + nY*(modLy/2. - ly/2.)
                    : TMath::Power(-1, i)*(modLy/2. - ly/2.);
            for(Int_t iCol=0; iCol<nCols; ++iCol){
                secX = useModuleDesign ? modX + nX*((iCol + 0.5)*secMaxLx - modLx/2.)
                        : secMaxLx*nCols/2. - (0.5 + iCol)*secMaxLx;
                iSector = module->GetNSectors();
                secSize.SetXYZ(secMaxLx, ly, modLz);
                secPosition.SetXYZ(secX, secY, modZ);
                CbmMuchSector* sector = new CbmMuchSector(detectorId, iSector, secPosition, secSize, fNCols[iStation], nPadRows);
                if(!IntersectsRad(sector, station->GetRmax())) {
                    delete sector;
                    continue;
                }
                module->AddSector(sector);
            }
        }
    }

    Double_t lx = useModuleDesign ? modLx - secMaxLx*nCols : (modLx - secMaxLx*nCols)/2.;
    if(lx > padMaxLx){
        nPadCols = (Int_t)(lx/padMaxLx);
        for(Int_t i=0; i<n; ++i){
            secX = useModuleDesign ? modX + nX*(modLx/2. - lx/2.)
                    : TMath::Power(-1, i)*(modLx/2. - lx/2.);
            for(Int_t iRow=0; iRow<nRows; ++iRow){
                secY = useModuleDesign ? modY + nY*((iRow + 0.5)*secMaxLy - modLy/2.)
                        : secMaxLy*nRows/2. - (0.5 + iRow)*secMaxLy;
                iSector = module->GetNSectors();
                secSize.SetXYZ(lx, secMaxLy, modLz);
                secPosition.SetXYZ(secX, secY, modZ);
                CbmMuchSector* sector = new CbmMuchSector(detectorId, iSector, secPosition, secSize, nPadCols, fNRows[iStation]);
                if(!IntersectsRad(sector, station->GetRmax())) {
                    delete sector;
                    continue;;
                }
                module->AddSector(sector);
            }
        }
    }

    if(lx > padMaxLx && ly > padMaxLy){
        nPadCols = (Int_t)(lx/padMaxLx);
        nPadRows = (Int_t)(ly/padMaxLy);
        secX = modX + nX*(modLx/2. - lx/2.);
        secY = modY + nY*(modLy/2. - ly/2.);
        iSector = module->GetNSectors();
        secSize.SetXYZ(lx, ly, modLz);
        secPosition.SetXYZ(secX, secY, modZ);
        CbmMuchSector* sector = new CbmMuchSector(detectorId, iSector, secPosition, secSize, nPadCols, nPadRows);
        if(!IntersectsRad(sector, station->GetRmax())) delete sector;
        else module->AddSector(sector);
    }
}
// -------------------------------------------------------------------------

// -----   Private method SegmentSector  -----------------------------------
void CbmMuchSegmentManual::SegmentSector(CbmMuchModule* module, CbmMuchSector* sector){
    TVector3 secSize = sector->GetSize();
    TVector3 secPosition = sector->GetPosition();
    Int_t detectorId = module->GetDetectorId();
    Int_t iStation = CbmMuchGeoScheme::GetStationIndex(detectorId);
    Double_t secLx = secSize.X();
    Double_t secLy = secSize.Y();
    Double_t secLz = secSize.Z();
    Double_t secX  = secPosition.X();
    Double_t secY  = secPosition.Y();
    Double_t secZ  = secPosition.Z();

    Bool_t resultX = ShouldSegment(sector, "X");
    Bool_t resultY = ShouldSegment(sector, "Y");

    if(!resultX && !resultY){
        CbmMuchStation* station = (CbmMuchStation*)fStations->At(iStation);
        Double_t rMax = station->GetRmax();
        if((IntersectsRad(sector, module->GetCutRadius())==2) || !IntersectsRad(sector, rMax)){
            delete sector;
            return;
        }
        module->AddSector(sector);
        return;
    }

    delete sector;
    Int_t iSector;
    TVector3 position, size;
    for(Int_t i=0; i<2; ++i){
        Double_t newSecLx = resultX ? secLx/2. : secLx;
        Double_t newSecLy = resultY ? secLy/2. : secLy;
        Double_t newSecX  = resultX ? secX + (i - 0.5)*newSecLx : secX;
        Double_t newSecY  = resultY ? secY + (i - 0.5)*newSecLy : secY;
        position.SetXYZ(newSecX, newSecY, secZ);
        size.SetXYZ(newSecLx, newSecLy, secLz);
        iSector = module->GetNSectors();
        SegmentSector(module, new CbmMuchSector(detectorId, iSector, position, size, fNCols[iStation], fNRows[iStation]));
    }
}
// -------------------------------------------------------------------------

// -----   Private method IntersectsRad  -----------------------------------
Int_t CbmMuchSegmentManual::IntersectsRad(CbmMuchModule* module, Double_t radius){
    if(radius < 0) return 0;

    Int_t intersects = 0;

    Double_t modLx = module->GetSize()[0];
    Double_t modLy = module->GetSize()[1];
    Double_t modX  = module->GetPosition()[0];
    Double_t modY  = module->GetPosition()[1];

    Double_t ulR = TMath::Sqrt((modX - modLx/2.)*(modX - modLx/2.) + (modY + modLy/2.)*(modY + modLy/2.));
    Double_t urR = TMath::Sqrt((modX + modLx/2.)*(modX + modLx/2.) + (modY + modLy/2.)*(modY + modLy/2.));
    Double_t blR = TMath::Sqrt((modX - modLx/2.)*(modX - modLx/2.) + (modY - modLy/2.)*(modY - modLy/2.));
    Double_t brR = TMath::Sqrt((modX + modLx/2.)*(modX + modLx/2.) + (modY - modLy/2.)*(modY - modLy/2.));

    if(ulR < radius) intersects++;
    if(urR < radius) intersects++;
    if(blR < radius) intersects++;
    if(brR < radius) intersects++;

    if(intersects == 4) return 2;
    if(intersects) return 1;
    else return 0;
}
// -------------------------------------------------------------------------

// -----   Private method IntersectsRad  -----------------------------------
Int_t CbmMuchSegmentManual::IntersectsRad(CbmMuchSector* sector, Double_t radius){
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

// -----   Private method ShouldSegment  -----------------------------------
Bool_t CbmMuchSegmentManual::ShouldSegment(CbmMuchSector* sector, const TString direction){
    Double_t secLx = sector->GetSize()[0];
    Double_t secLy = sector->GetSize()[1];
    Double_t secX  = sector->GetPosition()[0];
    Double_t secY  = sector->GetPosition()[1];

    if(TMath::Abs(secLx - secLy) < 1e-5 && direction=="Y") return false;
    if(secLy > secLx && direction=="X") return false;

    Double_t ulR = TMath::Sqrt((secX - secLx/2.)*(secX - secLx/2.) + (secY + secLy/2.)*(secY + secLy/2.));
    Double_t urR = TMath::Sqrt((secX + secLx/2.)*(secX + secLx/2.) + (secY + secLy/2.)*(secY + secLy/2.));
    Double_t blR = TMath::Sqrt((secX - secLx/2.)*(secX - secLx/2.) + (secY - secLy/2.)*(secY - secLy/2.));
    Double_t brR = TMath::Sqrt((secX + secLx/2.)*(secX + secLx/2.) + (secY - secLy/2.)*(secY - secLy/2.));

    Double_t uR = TMath::Min(ulR, urR);
    Double_t bR = TMath::Min(blR, brR);
    Double_t R  = TMath::Min(uR, bR);

    Int_t iStation = CbmMuchGeoScheme::GetStationIndex(sector->GetDetectorId());
    Double_t secMaxL = direction=="X" ? fSecMaxLx[iStation] : fSecMaxLy[iStation];
    Double_t secMinL = direction=="X" ? fSecMinLx[iStation] : fSecMinLy[iStation];
    Double_t secL    = direction=="X" ? secLx : secLy;
    if(secL > secMinL && secL/2. < secMinL) return false;
    if(secL > secMaxL) return true;

    // Check sector size in the corresponding region
    for(Int_t iRegion=0; iRegion<fNRegions[iStation]; ++iRegion){
        Double_t regionRad = fRadii[iStation].at(iRegion);
        if(R > regionRad) continue;
        if(direction=="X" && fSecLx[iStation].at(iRegion) < secLx) return true;
        if(direction=="Y" && fSecLy[iStation].at(iRegion) < secLy) return true;
        break;
    }
    return false;
}
// -------------------------------------------------------------------------

// -----   Private method GetSectorMaxSize  --------------------------------
Double_t CbmMuchSegmentManual::GetSectorMaxSize(CbmMuchModule* module, const TString side){
    Int_t iStation = CbmMuchGeoScheme::GetStationIndex(module->GetDetectorId());
    Int_t nRegions = fNRegions[iStation];
    for(Int_t iRegion=0; iRegion<nRegions; ++iRegion){
        Double_t rad = fRadii[iStation].at(iRegion);
        Int_t result = IntersectsRad(module, rad);
        if(result == 2) return side == "Width" ? fSecLx[iStation].at(iRegion) :fSecLy[iStation].at(iRegion);
    }
    return side == "Width" ? fSecLx[iStation].at(nRegions-1) :fSecLy[iStation].at(nRegions-1);
}
// -------------------------------------------------------------------------

// -----   Private method GetPadMaxSize  -----------------------------------
Double_t CbmMuchSegmentManual::GetPadMaxSize(CbmMuchModule* module, const TString side){
    Int_t iStation = CbmMuchGeoScheme::GetStationIndex(module->GetDetectorId());
    Double_t sectorSize = GetSectorMaxSize(module, side);
    return side=="Width" ? sectorSize/fNCols[iStation] : sectorSize/fNRows[iStation];
}
// -------------------------------------------------------------------------

// -----   Private method Print  -------------------------------------------
void CbmMuchSegmentManual::Print(){
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
// -------------------------------------------------------------------------

ClassImp(CbmMuchSegmentManual)

