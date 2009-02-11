//CbmTofHit
//Author E. Cordier
//Modified by D. Gonzalez-Diaz 06/09/06
//From CbmTofPoint to smeared hits

#ifndef CBMTOFHIT_H
#define CBMTOFHIT_H

#include "FairHit.h"

class CbmTofHit : public FairHit
{

 public:

  /** Default constructor **/
  CbmTofHit();

  /** Constructor with hit parameters (1)**/
  CbmTofHit(Int_t detectorID, TVector3 pos, TVector3 dpos, Int_t refIndex, Double_t tof, Int_t flag);

  /** Constructor with hit parameters (2) [not the flag]**/
  CbmTofHit(Int_t detectorID, TVector3 pos, TVector3 dpos, Int_t refIndex, Double_t tof);

  /** Destructor **/
  virtual ~CbmTofHit();

  void Print(const Option_t* opt = 0) const;

  /** Accessors **/
  Double_t GetTime()       const { return fTime; };
  Int_t    GetFlag()       const { return fFlag; }; 

  Int_t GetCell()   const   {return ((fDetectorID>>4) & 1023);};
  Int_t GetModule() const   {return ((fDetectorID>>14) & 1023);};
  Int_t GetRegion() const   {return fDetectorID>>24;};
 

  /** Modifiers **/
  void SetTime(Double_t time)        { fTime = time; };
  void SetFlag(Int_t flag)           { fFlag = flag; };

  

 protected:

  Double32_t fTime;              // Time since event start [ns]
  Int_t      fFlag;              // Flag for general purposes [TDC, event tagging...]

  ClassDef(CbmTofHit,1)

};


#endif
