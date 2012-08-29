//* $Id: */

// -------------------------------------------------------------------------
// -----                    CbmStsDigitize header file             -----
// -----                  Created 01/07/2008  by R. Karabowicz         -----
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

#include <set>
#include "TStopwatch.h"
#include "FairTask.h"

using std::set;
using std::map;
using std::pair;

class TClonesArray;
class CbmGeostsPar;
class CbmStsDigiPar;
class CbmStsStation;

class CbmStsPoint;


class CbmStsDigitize : public FairTask
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

  void SetStep(Double_t tStep) {fStep = tStep;}

  /** Execution **/
  virtual void Exec(Option_t* opt);

  /** Virtual method Finish **/
  virtual void Finish();

  virtual void SetRealisticResponse(Bool_t real=kTRUE) {fRealistic = real;}

  void FindFiredStrips(CbmStsPoint* pnt,Int_t& nofStr,Int_t*& strips,Double_t*& signals,Int_t side);
  void ProduceHitResponse(CbmStsSensor* sensor);

  void SetFrontThreshold (Double_t  frontThr=0.)      {fFThreshold    =  frontThr;}
  void SetBackThreshold  (Double_t  backThr=0.)       {fBThreshold    =  backThr;}
  void SetFrontNoiseWidth(Double_t  frontNoW=0.)      {fFNoiseWidth   =  frontNoW;}
  void SetBackNoiseWidth (Double_t  backNoW=0.)       {fBNoiseWidth   =  backNoW;}
  
  void SetFrontNofBits   (Int_t     frontNB=0 ) {fFNofBits    =  frontNB;}
  void SetBackNofBits    (Int_t      backNB=0 ) {fBNofBits    =   backNB;}
  void SetFrontNofElPerAdc   (Double_t  frontMS=0.) {fFNofElPerAdc    =  frontMS;}
  void SetBackNofElPerAdc    (Double_t   backMS=0.) {fBNofElPerAdc    =   backMS;}
  
  void SetStripDeadTime  (Double_t  StripDeadTime=0.) {fStripDeadTime =  StripDeadTime;}


 private:

  CbmGeoStsPar*     fGeoPar;       /** Geometry parameter container **/
  CbmStsDigiPar*    fDigiPar;      /** Digitisation parameter container **/
  CbmStsDigiScheme* fDigiScheme;   /** Digitisation scheme **/
  TClonesArray*     fPoints;       /** Input array of CbmStsPoint **/
  TClonesArray*     fDigis;        /** Output array of CbmStsDigi **/
  TClonesArray*     fDigiMatches;  /** Output array of CbmStsDigiMatches**/

  // statistics
  Int_t             fNDigis;
  Int_t             fNMulti;

  Double_t          fNEvents;
  Double_t          fNPoints;     
  Double_t          fNDigisFront;
  Double_t          fNDigisBack;
  Double_t          fTime;

  // settings
  Double_t          fStep;

  TStopwatch        fTimer;

  Bool_t    fRealistic;
  Double_t  fEnergyLossToSignal;

  Double_t  fFThreshold;
  Double_t  fBThreshold;
  Double_t  fFNoiseWidth;
  Double_t  fBNoiseWidth;

  Double_t  fStripDeadTime;
  
  Int_t     fFNofBits;
  Int_t     fBNofBits;
  Double_t  fFNofElPerAdc;
  Double_t  fBNofElPerAdc;
  Int_t     fFNofSteps;
  Int_t     fBNofSteps;

  Double_t* fStripSignalF;
  Double_t* fStripSignalB;

  // map of the point indices contributing to a fired strip
  map<Int_t, set<Int_t> > fFChannelPointsMap;
  map<Int_t, set<Int_t> > fBChannelPointsMap;
 
  map<CbmStsSensor*, set<Int_t> > fPointMap;  /** sensor points **/
  Float_t occupancy [10][1000][20] ;

  /** Make sensorwise set for points **/
  void MakeSets();
  void MakeSets1();

  /** Get parameter containers **/
  virtual void SetParContainers();


  /** Intialisation **/
  virtual InitStatus Init();


  /** Reinitialisation **/
  virtual InitStatus ReInit();


  /** Reset eventwise counters **/
  void Reset();

  CbmStsDigitize(const CbmStsDigitize&);
  CbmStsDigitize operator=(const CbmStsDigitize&);

  ClassDef(CbmStsDigitize,1);

};

#endif


