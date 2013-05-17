// -------------------------------------------------------------------------
// -----                 CbmPsdHitProducerIdel header file             -----
// -----                  Created 15/05/12  by Alla                   -----
// -----------------------------------------------------------
//--------------


/** CbmPsdIdealDigitize.h
 *@author Alla Maevskaya <alla@inr.ru>
 **
 ** The ideal hit producer produces hits of type CbmPsdHit as 
 ** full energy deposited in 1 module
 **/

#ifndef CBMPSDHitProducer_H
#define CBMPSDHitProducer_H 


#include "FairTask.h"
#include "TH1F.h"

class TClonesArray;



class CbmPsdHitProducer : public FairTask
{

 public:

  /** Default constructor **/  
  CbmPsdHitProducer();


  /** Destructor **/
  ~CbmPsdHitProducer();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);
  virtual void Finish();


 private:
  Int_t             fNHits;
   
  /** Input array of CbmPsdHit **/
  TClonesArray* fHitArray;

  /** Output array of CbmPsdDigits **/
  TClonesArray* fDigiArray; 

  Float_t       fXi[44];  //X coordinate of center of module
  Float_t       fYi[44];  //X coordinate of center of module
  TH1F *fhModXNewEn; //edep in each module for Marina
 
  void Reset();

  CbmPsdHitProducer(const CbmPsdHitProducer&);
  CbmPsdHitProducer operator=(const CbmPsdHitProducer&);

  ClassDef(CbmPsdHitProducer,2);

};

#endif
