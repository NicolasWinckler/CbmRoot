//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Event                                                                //
//                                                                      //
// Description of the event and track parameters                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef CBML1MYEVENT
#define CBML1MYEVENT

#include "TObject.h"

class CbmZdcEvent : public TObject {

 private:
      
  Float_t          fImpact;
  Int_t          fNumberOfEvent;
  Float_t fChannel[7200]; 

 public:
  CbmZdcEvent() ;
  virtual ~CbmZdcEvent();
  Float_t GetChannel(Int_t mod);
  void SetChannel(Int_t mod, Float_t de);

  Float_t Impact() const {return fImpact;}
  Int_t Nevent() const {return fNumberOfEvent;}
  void  SetImpact(Float_t impact) {fImpact = impact;}
  void  SetNevent(Int_t numberOfEvent) {fNumberOfEvent = numberOfEvent;}
  //  void Print(Int_t layer);
  void Reset();
  
  ClassDef(CbmZdcEvent,1)  //Event structure
    };

#endif

