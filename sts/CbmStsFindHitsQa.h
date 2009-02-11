//* $Id: */

// -------------------------------------------------------------------------
// -----                     CbmStsFindHitsQa header file              -----
// -----                  Created 01/07/2008  by R. Karabowicz         -----
// -------------------------------------------------------------------------


/** CbmStsFindHitsQa
 *@author Volker Friese <v.friese@gsi.de>
 *@since 11.09.06
 *@version 1.0
 **
 ** CBM task class for finding hits in the STS
 ** Task level RECO
 ** Produces objects of type CbmStsHits out of CbmStsDigi.
 **/


#ifndef CBMSTSFINDHITSQA_H
#define CBMSTSFINDHITSQA_H 1


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

class TCanvas;
class TPad;
class TH1;
class TH1F;
class TH2F;
class TH3F;
class TList;


class CbmStsFindHitsQa : public FairTask
{


 public :

  /** Default constructor **/
  CbmStsFindHitsQa();


  /** Standard constructor **/
  CbmStsFindHitsQa(Int_t iVerbose);


  /** Constructor with task name **/
  CbmStsFindHitsQa(const char* name, Int_t iVerbose);


  /** Destructor **/
  virtual ~CbmStsFindHitsQa();

  /** Create histograms **/
  void CreateHistos();


  /** Reset histograms and counters **/
  void Reset();

  /** Execution **/
  virtual void Exec(Option_t* opt);

  /** Virtual method Finish **/
  virtual void Finish();


 private:

  /** Pointers to data arrays **/
  CbmGeoStsPar*     fGeoPar;      /** Geometry parameters **/
  CbmStsDigiPar*    fDigiPar;     /** Digitisation parameters **/
  CbmStsDigiScheme* fDigiScheme;  /** Digitisation scheme **/

  TClonesArray* fMCTracks;           // MCtrack
  TClonesArray* fStsPoints;          // StsPoints
  TClonesArray* fStsHits;            // StsHits
  TClonesArray* fMatches;            // StsTrackMatch
  TClonesArray* fStsDigis;           // StsDigi
  TClonesArray* fStsClusters;        // StsCluster

  TStopwatch fTimer;

  TH2F* fhHitPointCorrelation [100];
  TH2F* fhHitPointCorrelationU[100];
  TH2F* fhHitPointCorrelationB[100];
  TH2F* fhHitPointPuls [100];

  TH3F* fhPrimaryVertex;

  TH1F* fhNofFiredDigis[20][300][2];     // per station, sector, side
  TH1F* fhNofDigisPChip[20][300][2][8];  // per station, sector, side, chip
  TH1F* fhNofHits      [20][300][3];     // per station, sector, sensor

  TH1F* fhClusterLength[20][300][2];     // per station, sector, side

  TH2F* fhEnergyLoss[20];

  TH2F* fhPoints    [20];
  TH2F* fhRecoPoints[20];

  TH1F* fhHitFindingEfficiency;

  /** List of histograms **/
  TList* fHistoList;
  TList* fHistoListPS[20];

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
   
  Int_t fNofFiredDigis[20][300][2];     // per station, sector, side
  Int_t fNofDigisPChip[20][300][2][8];  // per station, sector, side, chip
  Int_t fNofHits      [20][300][3];     // per station, sector, sensor

  Int_t fNofPoints    [20][300][3];
  Int_t fNofRecoPoints[20][300][3];

  /** Get parameter containers **/
  virtual void SetParContainers();


  /** Intialisation **/
  virtual InitStatus Init();


  /** Reinitialisation **/
  virtual InitStatus ReInit();


  ClassDef(CbmStsFindHitsQa,1);

};

#endif
