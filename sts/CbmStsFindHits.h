//* $Id: */

// -------------------------------------------------------------------------
// -----                     CbmStsFindHits header file                -----
// -----                  Created 11/09/06  by V. Friese               -----
// -------------------------------------------------------------------------


/** CbmStsFindHits
 *@author Volker Friese <v.friese@gsi.de>
 *@since 11.09.06
 *@version 1.0
 **
 ** CBM task class for finding hits in the STS
 ** Task level RECO
 ** Produces objects of type CbmStsHits out of CbmStsDigi.
 **/


#ifndef CBMSTSFINDHITS_H
#define CBMSTSFINDHITS_H 1


#include "FairTask.h"

#include "TStopwatch.h"

#include <map>
#include <set>


class TClonesArray;
class CbmGeoStsPar;
class CbmStsDigiPar;
class CbmStsDigiScheme;
class CbmStsSector;
class CbmStsStation;



class CbmStsFindHits : public FairTask
{


 public :

  /** Default constructor **/
  CbmStsFindHits();


  /** Standard constructor **/
  CbmStsFindHits(Int_t iVerbose);


  /** Constructor with task name **/
  CbmStsFindHits(const char* name, Int_t iVerbose);


  /** Destructor **/
  virtual ~CbmStsFindHits();


  /** Execution **/
  virtual void Exec(Option_t* opt);



 private:

  CbmGeoStsPar*     fGeoPar;      /** Geometry parameters **/
  CbmStsDigiPar*    fDigiPar;     /** Digitisation parameters **/
  CbmStsDigiScheme* fDigiScheme;  /** Digitisation scheme **/
  TClonesArray*     fDigis;       /** Input array of CbmStsDigi **/
  TClonesArray*     fHits;        /** Output array of CbmStsHit **/
  std::map<CbmStsSector*, std::set<Int_t> > fDigiMapF;  /** sector digis (front) **/
  std::map<CbmStsSector*, std::set<Int_t> > fDigiMapB;  /** sector digis (back)  **/
  TStopwatch fTimer;

  /** Get parameter containers **/
  virtual void SetParContainers();


  /** Intialisation **/
  virtual InitStatus Init();


  /** Reinitialisation **/
  virtual InitStatus ReInit();


  /** Make sectorwise sets for sigis  **/
  void MakeSets();


  /** Sort digis sectorwise  **/
  void SortDigis();
  
  virtual void Finish();


  /** Find hits in one sector **/
  Int_t FindHits(CbmStsStation* station, CbmStsSector* sector,
		 std::set<Int_t>& fSet, std::set<Int_t>& bSet);


  ClassDef(CbmStsFindHits,1);
  
  Int_t    fNStations;
  Int_t    fNEvents;        /** Number of events with success **/
  /** Total real time used for good events **/
   Double_t  fTime1;     
   Double_t  fTime2;     
   Double_t  fTime3;
   Double_t  fTime4;
   Double_t  fTime5;
   Double_t  fTime6;
   Double_t  fTime7;
};

#endif
