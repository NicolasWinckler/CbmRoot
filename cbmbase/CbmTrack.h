// -------------------------------------------------------------------------
// -----                        CbmTrack header file                   -----
// -----                  Created 29/11/07  by V. Friese               -----
// -------------------------------------------------------------------------


/**  CbmTrack.h
 *@author V.Friese <v.friese@gsi.de>
 **
 ** Base class for local tracks in the CBM detectors. 
 ** Holds a list of CbmTrkHits and the fitted track parameters. 
 ** The fit parameters are of type FairTrackParam
 ** and can only be accesssed and modified via this class.
 **/

#ifndef CBMTRACK_H
#define CBMTRACK_H 1

#include "CbmDetectorList.h"
#include "FairTrackParam.h"

#include "TArrayI.h"
#include "TObject.h"

#include <map>

class CbmTrkHit;



class CbmTrack : public TObject
{

 public:

  /** Default constructor **/
  CbmTrack();


  /** Destructor **/
  virtual ~CbmTrack();


  /** Add a StsHit to the list **/
  void AddHit(Int_t hitIndex, CbmTrkHit* hit);


  /** Public method Print
   ** Output to screen 
   **/
  void Print();


  /** Public method SortHits
   ** Sorts the hits in downstream direction
   ** and writes the hit indizes into the member fHits
   **/
  void SortHits();

  void SortMvdHits() { };


  /** Accessors  **/
  Int_t GetNHits()                 const { return fHits.GetSize(); }
  Int_t GetHitIndex(Int_t iHit)    const { return fHits.At(iHit); }  
  Int_t GetPidHypo()               const { return fPidHypo; }
  Int_t GetFlag()                  const { return fFlag; }
  Double_t GetChi2()               const { return fChi2; }
  Int_t GetNDF()                   const { return fNDF; }
  Int_t GetPreviousDetId()         const { return fPreviousDetId; }
  Int_t GetPreviousTrackId()       const { return fPreviousTrackId; }
  FairTrackParam* GetParamFirst() { return &fParamFirst; }
  FairTrackParam* GetParamLast()  { return &fParamLast ; }


  /** Modifiers  **/
  void SetPidHypo(Int_t pid)                { fPidHypo    = pid;  }
  void SetParamFirst(FairTrackParam& par)    { fParamFirst = par;  }
  void SetParamLast(FairTrackParam& par)     { fParamLast  = par;  }
  void SetFlag(Int_t flag)                  { fFlag       = flag; }
  void SetChi2(Double_t chi2)               { fChi2       = chi2; }
  void SetNDF(Int_t ndf)                    { fNDF        = ndf;  }
  


 private:

  /** Array containg the indizes of the STS hits attached to the track **/
  TArrayI fHits;


  /** PID hypothesis used by the track fitter **/
  Int_t fPidHypo;


  /** Track parameters at first and last fitted hit **/
  FairTrackParam fParamFirst;
  FairTrackParam fParamLast;


  /** Quality flag **/
  Int_t fFlag;


  /** Chi square and NDF of track fit **/
  Double32_t fChi2;
  Int_t fNDF;
  
  
  /** DetectorID of the previous track segment **/
  Int_t fPreviousDetId;
  
  
  /** Index of previous track segment **/
  Int_t fPreviousTrackId;


  /** Maps from hit z position to hit index. STL map is used because it
   ** is automatically sorted. Temporary only; not for storgage.
   ** The Hit index arrays will be filled by the method SortHits.
   **/
  std::map<Double_t, Int_t> fHitMap;        //!
     

  ClassDef(CbmTrack,1);

};



#endif
