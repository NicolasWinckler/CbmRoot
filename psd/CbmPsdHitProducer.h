// -------------------------------------------------------------------------
// -----                 CbmPsdHitProducerIdel header file             -----
// -----                  Created 15/05/12  by Alla                   -----
// -------------------------------------------------------------------------


/** CbmPsdIdealDigitize.h
 *@author Alla Maevskaya <alla@inr.ru>
 **
 ** The ideal hit producer produces hits of type CbmPsdHit as 
 ** full energy deposited in 1 module
 **/


#ifndef CBMPSDHitProducer_H
#define CBMPSDHitProducer_H 1


#include "FairTask.h"

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


 private:
  Int_t             fNHits;
   
  /** Input array of CbmPsdHit **/
  TClonesArray* fHitArray;

  /** Output array of CbmPsdDigits **/
  TClonesArray* fDigiArray; 

  CbmPsdHitProducer(const CbmPsdHitProducer&);
  CbmPsdHitProducer operator=(const CbmPsdHitProducer&);

  void Reset();

  ClassDef(CbmPsdHitProducer,1);

};

#endif
