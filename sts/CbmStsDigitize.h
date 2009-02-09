//* $Id: */

// -------------------------------------------------------------------------
// -----                    CbmStsDigitize header file                 -----
// -----                  Created 30/08/06  by V. Friese               -----
// -------------------------------------------------------------------------


/** CbmStsDigitise
 *@author Volker Friese <v.friese@gsi.de>
 *@since 30.08.06
 *@version 1.0
 **
 ** CBM task class for digitising STS
 ** Task level SIM
 ** Produces objects of type CbmStsDigi out of CbmStsPoint.
 **/


#ifndef CBMSTSDIGITIZE_H
#define CBMSTSDIGITIZE_H 1

#include "CbmTask.h"

#include "TStopwatch.h"

#include <list>
#include <map>

class TClonesArray;
class CbmGeoStsPar;
class CbmStsDigiPar;
class CbmStsDigiScheme;
class CbmStsStation;



class CbmStsDigitize : public CbmTask
{

 public:

  /** Default constructor **/
  CbmStsDigitize();


  /** Standard constructor **/
  CbmStsDigitize(Int_t iVerbose);


  /** Constructor with name **/
  CbmStsDigitize(const char* name, Int_t iVerbose);


  /** Destructor **/
  virtual ~CbmStsDigitize();


  /** Execution **/
  virtual void Exec(Option_t* opt);
  
  /** Virtual method Finish **/
  virtual void Finish();



 private:

  CbmGeoStsPar*     fGeoPar;       /** Geometry parameter container **/
  CbmStsDigiPar*    fDigiPar;      /** Digitisation parameter container **/
  CbmStsDigiScheme* fDigiScheme;   /** Digitisation scheme **/
  TClonesArray*     fPoints;       /** Input array of CbmStsPoint **/
  TClonesArray*     fDigis;        /** Output array of CbmStsDigi **/
  TClonesArray*     fDigiMatches;  /** Output array of CbmStsDigiMatches**/
  Int_t             fNPoints;     
  Int_t             fNFailed;
  Int_t             fNOutside;
  Int_t             fNMulti;
  Int_t             fNDigis;
  TStopwatch        fTimer;

  /** Map of active channels (pair detectorId, channel number) 
   ** to index of StsDigi **/
  std::map<std::pair<Int_t, Int_t>, Int_t> fChannelMap; //!
 

  /** Get parameter containers **/
  virtual void SetParContainers();


  /** Intialisation **/
  virtual InitStatus Init();


  /** Reinitialisation **/
  virtual InitStatus ReInit();


  /** Reset eventwise counters **/
  void Reset();

  Int_t    fNStations;
  Int_t    fNEvents;        /** Number of events with success **/
      /** Total real time used for good events **/
  Double_t fTime1     ;
  Double_t fTime2     ;
  Double_t fTime3     ;
  Double_t fTime4     ;
  Double_t fTime5     ;
  ClassDef(CbmStsDigitize,1);

};

#endif


