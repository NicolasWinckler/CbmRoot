/** CbmMuchDigiPar.h
 *@author   M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@version  1.0	
 *@since    15.03.07
 **
 ** Parameter set for digitization of the MuCh detector system.
 ** It holds an array of CbmMuchStationDigiPar containing the information
 ** for each MuCh station.
 **/



#ifndef CBMMUCHDIGIPAR_H
#define CBMMUCHDIGIPAR_H

#include "CbmParSet.h"

#include "TObjArray.h"

#include <fstream>
#include <map>


class TString;
class CbmParIo;
class CbmMuchStationDigiPar;

class CbmMuchDigiPar : public CbmParSet 
{

 public:

  /** Standard constructor **/
  CbmMuchDigiPar(const char* name    = "MuchDigiPar",
		const char* title   = "MuCh digitisation parameters",
		const char* context = "Default");


  /** Destructor **/
  virtual ~CbmMuchDigiPar();


  /** Initialisation from input device**/
  virtual Bool_t init(CbmParIo* input);


  /** Output to file **/
  virtual Int_t write(CbmParIo* output);


  /** Reset all parameters **/
  virtual void clear();


  /** Read one line (station) from ASCII input **/
  void readline(const char* buffer, Int_t* set, fstream* f);


  /** Put an Ascii header for Ascii file output  **/
  void putAsciiHeader(TString&);


  /** Accessor to number of stations **/
  Int_t GetNStations() { return fStations->GetEntries(); }


  /** Accessor to station array **/
  TObjArray* GetStations() { return fStations; }


  /** Accessor to station parameters (by index) **/
  CbmMuchStationDigiPar* GetStation(Int_t iStation);

  
  /** Accessor to station parameters (by station ID) **/
  CbmMuchStationDigiPar* GetStationByNr(Int_t stationId);
  


 private:

  TObjArray* fStations;           // Array of CbmMuchStationDigiPar

  std::map<Int_t, Int_t> fStationMap;  //! Map from station ID to index


  ClassDef(CbmMuchDigiPar,1);


};


#endif





