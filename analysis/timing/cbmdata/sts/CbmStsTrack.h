// -------------------------------------------------------------------------
// -----                      CbmStsTrack header file                  -----
// -----                  Created 26/01/05  by V. Friese               -----
// -------------------------------------------------------------------------


/**  CbmStsTrack.h
 *@author V.Friese <v.friese@gsi.de>
 **
 ** STS local track. Holds lists of CbmStsHits and the fitted
 ** track parameters. The fit parameters are of type FairTrackParam
 ** and can only be accesssed and modified via this class.
 **/

#ifndef CBMSTSTRACK_H
#define CBMSTSTRACK_H 1


#include "FairTrackParam.h"

#include "TArrayI.h"
#include "TObject.h"

#include <map>

class FairHit;



class CbmStsTrack : public TObject
{

 public:

  /** Default constructor **/
  CbmStsTrack();


  /** Destructor **/
  virtual ~CbmStsTrack();


  /** Add a StsHit to the list **/
  void AddStsHit(Int_t hitIndex, FairHit* hit);


  /** Associate a MvdHit to the track **/
  void AddMvdHit(Int_t hitIndex, FairHit* mvdHit);



  /** Public method Print
   ** Output to screen 
   **/
  void Print();


  /** Public method SortHits
   ** Sorts the hits in downstream direction
   ** and writes the hit indizes into the member fHits
   **/
  void SortHits();

  void SortMvdHits();


  /** Accessors  **/
  Int_t GetNStsHits()              const { return fStsHits.GetSize(); }
  Int_t GetNMvdHits()              const { return fMvdHits.GetSize(); }
  Int_t GetStsHitIndex(Int_t iHit) const { return fStsHits.At(iHit); }  
  Int_t GetMvdHitIndex(Int_t iHit) const { return fMvdHits.At(iHit); }  
  Int_t GetPidHypo()               const { return fPidHypo; }
  Int_t GetFlag()                  const { return fFlag; }
  Double_t GetChi2()               const { return fChi2; }
  Int_t GetNDF()                   const { return fNDF; }
  Double_t GetB()                  const { return fB; }
  FairTrackParam* GetParamFirst() { return &fParamFirst; }
  FairTrackParam* GetParamLast()  { return &fParamLast ; }
  Double_t GetTime()               const { return fTime; }

  /** Modifiers  **/
  void SetPidHypo(Int_t pid)                { fPidHypo    = pid;  }
  void SetParamFirst(FairTrackParam& par)    { fParamFirst = par;  }
  void SetParamLast(FairTrackParam& par)     { fParamLast  = par;  }
  void SetFlag(Int_t flag)                  { fFlag       = flag; }
  void SetChi2(Double_t chi2)               { fChi2       = chi2; }
  void SetNDF(Int_t ndf)                    { fNDF        = ndf;  }
  void SetB(Double_t b)                     { fB          = b;    }
  void SetTime(Double_t time)               { fTime       = time; }


 private:

  /** Array containg the indizes of the STS hits attached to the track **/
  TArrayI fStsHits;

  /** Array containg the indizes of the MVD hits attached to the track **/
  TArrayI fMvdHits;


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


  /** Impact parameter of track at target z, in units of its error **/
  Double32_t fB;


  /** Maps from hit z position to hit index. STL map is used because it
   ** is automatically sorted. Temporary only; not for storgage.
   ** The Hit index arrays will be filled by the method SortHits.
   **/
  std::map<Double_t, Int_t> fStsHitMap;        //!
  std::map<Double_t, Int_t> fMvdHitMap;        //!
  
  Double32_t fTime;
  
  ClassDef(CbmStsTrack,2);

};



#endif
