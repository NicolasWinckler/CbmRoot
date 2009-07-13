/** CbmMuchHitProducer class
 *
 * @author  A.Kiseleva
 * @version 0.0
 * @since   13.04.06
 *
 *  Hit producer for MUon CHambers detector
 *
 */

#ifndef CBMMUCHHITPRODUCERIDEAL_H
#define CBMMUCHHITPRODUCERIDEAL_H

#include "FairTask.h"

class TClonesArray;
class TString;

class CbmMuchHitProducerIdeal : public FairTask {

  public:

    CbmMuchHitProducerIdeal(const char *name="MuchHitProducer", Int_t verbose = 1,
        Double_t SigmaXY = 0.0100, Double_t SigmaZ = 0. );

    virtual ~CbmMuchHitProducerIdeal();

    virtual InitStatus Init();// *MENU*
    virtual void Exec(Option_t * option);
    virtual void FinishTask();// *MENU*

    void Register();

    void AddHit(Int_t detID, TVector3 &posHit, TVector3 &posHitErr, Int_t ref );

    void SetSigmaXY(Double_t sigma);
    void SetSigmaZ(Double_t sigma);

    Double_t GetSigmaXY();
    Double_t GetSigmaZ();

  private:

    Int_t fVerbose;

    TClonesArray *fMuchPoints; //! Much MC points
    TClonesArray *fHitCollection; //! Much hits

    Double_t fSigmaXY;//!
    Double_t fSigmaZ; //!

    TString fVersion; //!
    Int_t  fNHits;    //!!

    ClassDef(CbmMuchHitProducerIdeal,1)
};

#endif

