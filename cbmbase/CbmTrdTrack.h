// -------------------------------------------------------------------------
// -----                      CbmStsTrack header file                  -----
// -----                  Created 11/07/05  by M. Kalisky              -----
// -----                    according to CbmStsTrack                   -----
// -------------------------------------------------------------------------


/**  CbmTrdTrack.h
 *@author M.Kalisky <m.kalisky@gsi.de>
 **
 ** TRD local track. Holds lists of CbmTrdHits and the fitted
 ** track parameters. The fit parameters are of type FairTrackParam
 ** and can only be accesssed and modified via this class.
 **/
#ifndef CBMTRDTRACK_H
#define CBMTRDTRACK_H 1


#include "FairTrackParam.h"

#include "TArrayI.h"
#include "TObject.h"

#include <map>

class CbmTrdHit;


class CbmTrdTrack : public TObject
{
public:

    /** Default constructor **/
    CbmTrdTrack();

    /** Destructor **/
    virtual ~CbmTrdTrack();

    /** Public method AddHit **/
    void AddHit(Int_t hitID, CbmTrdHit* hit);

    /** Public method Print **/
    void Print();


    /** Public method SortHits
     ** Sorts the hits in each array in downstream direction
     ** and writes the hit indizes into the member TArrayI
     **/
    void SortHits();


    /** Accessors  **/
    Int_t GetNofTrdHits()              const { return fTrdHits.GetSize(); }
    Int_t GetTrdHitIndex(Int_t iHit)   const { return fTrdHits.At(iHit); }
    Int_t GetPidHypo()                 const { return fPidHypo; }
    Double_t GetPidWkn()               const { return fPidWkn; }
    Double_t GetPidANN()               const { return fPidANN; }
    Int_t GetFlag()                    const { return fFlag; }
    Double_t GetChi2()                 const { return fChi2; }
    Int_t    GetNDF()                  const { return fNDF; }
    Double_t GetELoss()                const { return fELoss;}
    Int_t GetStsTrackIndex()           const { return fStsTrackIndex;}
    FairTrackParam* GetParamFirst() { return &fParamFirst; }
    FairTrackParam* GetParamLast()  { return &fParamLast ; }

    Double_t GetPidLikeEL()               const { return fPidLikeEL; }
    Double_t GetPidLikePI()               const { return fPidLikePI; }
    Double_t GetPidLikeKA()               const { return fPidLikeKA; }
    Double_t GetPidLikePR()               const { return fPidLikePR; }
    Double_t GetPidLikeMU()               const { return fPidLikeMU; }



    /** Modifiers  **/
    void SetPidHypo(Int_t pid)                { fPidHypo    = pid;     }
    void SetPidWkn(Double_t pid)              { fPidWkn     = pid;     }
    void SetPidANN(Double_t pid)              { fPidANN     = pid;     }
    void SetParamFirst(FairTrackParam& par)    { fParamFirst = par;     }
    void SetParamLast(FairTrackParam& par)     { fParamLast  = par;     }
    void SetFlag(Int_t flag)                  { fFlag       = flag;    }
    void SetChi2(Double_t chi2)               { fChi2       = chi2;    }
    void SetNDF(Int_t ndf)                    { fNDF = ndf; }
    void SetELoss(Double_t eLoss)             { fELoss      = eLoss; }
    void SetStsTrackIndex(Int_t stsTrackIndex){fStsTrackIndex = stsTrackIndex;}


    void SetPidLikeEL(Double_t value)         { fPidLikeEL     = value; }
    void SetPidLikePI(Double_t value)         { fPidLikePI     = value; }
    void SetPidLikeKA(Double_t value)         { fPidLikeKA     = value; }
    void SetPidLikePR(Double_t value)         { fPidLikePR     = value; }
    void SetPidLikeMU(Double_t value)         { fPidLikeMU     = value; }

    /** Function for sorting **/
    static Bool_t CompareChi2(const CbmTrdTrack* a, const CbmTrdTrack* b)
    {
        return ( a->GetChi2()/(Double_t)a->GetNDF() < b->GetChi2()/(Double_t)b->GetNDF() );
    };

    static Bool_t CompareMomentum(CbmTrdTrack* a, CbmTrdTrack* b)
    {
	Double_t mom_a = 1./TMath::Abs(a->GetParamLast()->GetQp());
        Double_t mom_b = 1./TMath::Abs(b->GetParamLast()->GetQp());
        return ( mom_a > mom_b );
    };




private:

    TArrayI      fTrdHits;

    /** PID hypothesis **/
    Int_t fPidHypo;

    /** PID value based on Wkn method **/
    Double_t fPidWkn;

    /** PID value based on Wkn method **/
    Double_t fPidANN;

    /** PID values based on Likelihood method **/
    Double_t fPidLikeEL;
    Double_t fPidLikePI;
    Double_t fPidLikeKA;
    Double_t fPidLikePR;
    Double_t fPidLikeMU;

    /** Track parameters at first and last fitted hit **/
    FairTrackParam fParamFirst;
    FairTrackParam fParamLast;

    /** Quality flag **/
    Int_t fFlag;

    /** RMS deviation of hit coordinates to track **/
    Double32_t fChi2;

    /** Number of degrees of freedom **/
    Int_t fNDF;

    /** Total energy loss in TRD **/
    Double_t fELoss;

    /** Index of STS track if created by track following
     ** from STS, otherwise -1
     **/
    Int_t fStsTrackIndex;

    /** Maps from hit z position to hit index. STL map is used because it
     ** is automatically sorted. Temporary only; not for storgage.
     ** The Hit index arrys will be filled by the method SortHits.
     **/
    std::map<Double_t, Int_t> fTrdHitMap;        //!



    ClassDef(CbmTrdTrack, 3);
};

#endif

