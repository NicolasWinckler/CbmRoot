/** CbmMuchDigiPar.cxx
 *@author   M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@version  1.0	
 *@since    15.03.07
 **
 ** Parameter set for digitization of the MuCh detector system.
 ** It holds an array of CbmMuchStationDigiPar containing the information
 ** for each MuCh station.
 **/
#include "CbmMuchDigiPar.h"

#include "CbmMuchStationDigiPar.h"

#include "FairParIo.h"
#include "FairDetParIo.h"

#include "TString.h"
#include "TMath.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Standard constructor   ------------------------------------------
CbmMuchDigiPar::CbmMuchDigiPar(const char* name, const char* title,
			     const char* context)
  : FairParSet(name, title, context) {
  detName="MuCh";
  fStations = new TObjArray();
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmMuchDigiPar::~CbmMuchDigiPar() {
  clear();
}
// -------------------------------------------------------------------------



// -----   Public method init   --------------------------------------------
Bool_t CbmMuchDigiPar::init(FairParIo* parIo) {
  if ( ! parIo ) {
    cout << "-W- CbmMuchDigiPar::init "
	 << "No input given; could not initialise parameters." << endl;
    return kFALSE;
  }
  FairDetParIo* input = parIo->getDetParIo("CbmMuchParIo");
  if (input) return (input->init(this));
  return kFALSE;
}
// -------------------------------------------------------------------------



// -----   Public method write   -------------------------------------------
Int_t CbmMuchDigiPar::write(FairParIo* parIo) {
  if ( ! parIo ) {
    cout << "-W- CbmMuchDigiPar::write "
	 << "No output given; could not write parameters." << endl;
    return kFALSE;;
  }
  FairDetParIo* output = parIo->getDetParIo("CbmMuchParIo");
  if (output) return (output->write(this));
  return kFALSE;
}
// -------------------------------------------------------------------------



// -----   Public method clear   -------------------------------------------
void CbmMuchDigiPar::clear() {
  status = kFALSE;
  resetInputVersions();
  fStations->Delete();
  fStationMap.clear();
}
// -------------------------------------------------------------------------



// -----   Public method readline   ----------------------------------------
void CbmMuchDigiPar::readline(const char* buffer, Int_t* set, fstream* f) {

  // First line: Station ID and type
  Int_t    stationNr   = -1;
  Float_t  frotation   =  0.;
  Int_t    nSectors    =  0;
  sscanf(buffer, "%d%f%d", &stationNr, &frotation, &nSectors);
  if ( stationNr == -1 ) return;
  Double_t rotation = Double_t(frotation);

  // Create new station
  CbmMuchStationDigiPar* station = new CbmMuchStationDigiPar(stationNr,
							   rotation);

  // Read the sector lines
  Int_t    sectorNr = -1;
  Int_t    iType    = 0;
  Double_t x0       = 0.;
  Double_t y0       = 0.;
  Double_t lx       = 0.;
  Double_t ly       = 0.;
  Int_t nCols       = 0;
  Int_t nRows       = 0;
  for (Int_t iSector=0; iSector<nSectors; iSector++) {
    *f >> sectorNr >> iType >> x0 >> y0 >> lx >> ly 
       >> nCols >> nRows; 
    CbmMuchSectorDigiPar* sector = new CbmMuchSectorDigiPar(sectorNr, iType,
							  x0, y0, 
							  lx, ly, nCols, nRows);
    station->AddSector(sector);
  }   // Loop over sector lines
  fStations->Add(station);
}
// -------------------------------------------------------------------------



// -----   Public method putAsciiHeader   ----------------------------------
void CbmMuchDigiPar::putAsciiHeader(TString& header) {
  header = "Fuck - header is not yet defined";
}
// -------------------------------------------------------------------------



// -----   Public method GetStation   --------------------------------------
CbmMuchStationDigiPar* CbmMuchDigiPar::GetStation(Int_t iStation) {
  return (CbmMuchStationDigiPar*) fStations->At(iStation);
}
// -------------------------------------------------------------------------



// -------   Public method GetStationById   --------------------------------
CbmMuchStationDigiPar* CbmMuchDigiPar::GetStationByNr(Int_t stationNr) {
  Int_t index = fStationMap[stationNr];
  CbmMuchStationDigiPar* station = 
    (CbmMuchStationDigiPar*) fStations->At(index);
  if ( ! station ) {
    cout << "-W- CbmMuchDigiPar::GetStationDigiPar: "
	 << "No parameters found for station " << stationNr << endl;
    return NULL;
  }
  return station;
}
// -------------------------------------------------------------------------





ClassImp(CbmMuchDigiPar)
