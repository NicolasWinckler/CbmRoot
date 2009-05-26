//CbmTofHit
//Author E. Cordier
//Modified by D. Gonzalez-Diaz 06/09/06
//Modified by A.Lebedev 26/05/09
//From CbmTofPoint to smeared hits

#ifndef CBMTOFHIT_H
#define CBMTOFHIT_H

#include "CbmPixelHit.h"

class CbmTofHit : public CbmPixelHit
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

  void Print() const;

  /** Accessors **/
  Double_t GetTime()       const { return fTime; };
  Int_t    GetFlag()       const { return fFlag; };

  Int_t GetCell()   const   {return ((GetDetectorId()>>4) & 1023);};
  Int_t GetModule() const   {return ((GetDetectorId()>>14) & 1023);};
  Int_t GetRegion() const   {return GetDetectorId()>>24;};

  Int_t GetPlaneId() const { return 0; }

  /** Modifiers **/
  void SetTime(Double_t time)        { fTime = time; };
  void SetFlag(Int_t flag)           { fFlag = flag; };

protected:

  Double32_t fTime;              // Time since event start [ns]
  Int_t      fFlag;              // Flag for general purposes [TDC, event tagging...]

  ClassDef(CbmTofHit,1)
};

#endif
