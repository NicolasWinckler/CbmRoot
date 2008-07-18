// -------------------------------------------------------------------------
// -----                    CbmGlobalTrack header file                 -----
// -----                  Created 01/12/05  by V. Friese               -----
// -------------------------------------------------------------------------


/**  CbmGlobalTrack.h
 *@author V.Friese <v.friese@gsi.de>
 **
 ** Data class for Global CBM track. Data level RECO.
 ** It consists of local tracks in STS and TRD and a RICH ring. 
 ** 
 **/


#ifndef CBMGLOBALTRACK_H
#define CBMGLOBALTRACK_H 1


#include "CbmTrackParam.h"

#include "TObject.h"

class CbmStsTrack;
class CbmTrdTrack;
class CbmRichRing;



class CbmGlobalTrack : public TObject
{

 public:

  /** Default constructor **/
  CbmGlobalTrack();


  /** Destructor **/
  virtual ~CbmGlobalTrack();


  /** Accessors **/
  Int_t GetStsTrackIndex()  const { return fStsTrack; }
  Int_t GetTrdTrackIndex()  const { return fTrdTrack; }
  Int_t GetRichRingIndex()  const { return fRichRing; }
  Int_t GetTofHitIndex()    const { return fTofHit;   }
  CbmTrackParam* GetParamFirst()       { return &fParamFirst;   }
  CbmTrackParam* GetParamLast()       { return &fParamLast;   }
  Int_t GetPidHypo()        const { return fPidHypo;  }
  Double_t GetChi2()        const { return fChi2;     }
  Int_t GetNDF()            const { return fNDF;      }
  Int_t GetFlag()           const { return fFlag;     }
  Double_t GetLength()      const { return fLength;   }


  /** Modifiers **/
  void SetStsTrackIndex(Int_t iSts)  { fStsTrack = iSts;  }
  void SetTrdTrackIndex(Int_t iTrd)  { fTrdTrack = iTrd;  }
  void SetRichRingIndex(Int_t iRing) { fRichRing = iRing; }
  void SetTofHitIndex(Int_t iTofHit) { fTofHit = iTofHit; }
  void SetParamFirst(CbmTrackParam& parFirst)  { fParamFirst    = parFirst;   }
  void SetParamLast(CbmTrackParam& parLast)  { fParamLast    = parLast;   }
  void SetPidHypo(Int_t iPid)        { fPidHypo  = iPid;  }
  void SetChi2(Double_t chi2)        { fChi2     = chi2;  }
  void SetNDF(Int_t ndf)             { fNDF      = ndf;   }
  void SetFlag(Int_t iFlag)          { fFlag     = fFlag; }
  void SetLength(Double_t length)    { fLength   = length;}


  /** Output to screen **/
  void Print();


 private:

  /** Indices of local StsTrack, TrdTrack, RichRing and TofHit **/
  Int_t fStsTrack;
  Int_t fTrdTrack;
  Int_t fRichRing;
  Int_t fTofHit;

  /** Global track parameters at first and last plane **/
  CbmTrackParam fParamFirst;
  CbmTrackParam fParamLast;

  /** PID hypothesis used for global track fit **/
  Int_t fPidHypo;

  /** Chi2 of global track fit **/
  Double32_t fChi2;

  /** NDF of global track fit **/
  Int_t fNDF;

  /** Quality flag **/
  Int_t fFlag;

  /** Track length **/
  Double32_t fLength;


  ClassDef(CbmGlobalTrack,1);

};


#endif
