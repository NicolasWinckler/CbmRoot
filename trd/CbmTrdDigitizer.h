// --------------------------------------------------------------------------
// -----          Header for the CbmTrdDigitizer               ------
// -----              Created 06.06.08 by F.Uhlig                      ------
// --------------------------------------------------------------------------



#ifndef CBMTUTORIALDETDIGITIZER_H
#define CBMTUTORIALDETDIGITIZER_H


#include "FairTask.h"

#include "TVector3.h"

class TClonesArray;

class CbmTrdDigiPar;

    class CbmTrdDigitizer : public FairTask {
public:

    

    /** Default constructor **/
    CbmTrdDigitizer();

    /** Standard constructor **/
    CbmTrdDigitizer(const char *name, const char *title="CBM Task");

    /** Destructor **/
    virtual ~CbmTrdDigitizer();

    /** Initialisation **/
    virtual InitStatus ReInit();
    virtual InitStatus Init();
    virtual void SetParContainers();

    /** Executed task **/
    virtual void Exec(Option_t * option);

    /** Finish task **/
    virtual void Finish();

    void AddHit(TVector3 &posHit, TVector3 &posHitErr,
		Int_t TrackID, Int_t PlaneID, Int_t ref, Double_t ELoss,
		Double_t ELossTR, Double_t ELossdEdX);
    void Register();

private:

    void GetModuleInformation();

    Int_t fStation;
    Int_t fLayer;
    Int_t fModuleType;
    Int_t fModuleCopy;

    TClonesArray *fTrdPoints; //! Tutorial Det MC points
    //    TClonesArray *fDigiCollection; //! TRD hits
    //TClonesArray *fListStack;         //Tracks

    CbmTrdDigiPar *fDigiPar;

    //    Double_t fDx;               //!
    //Double_t fDy;               //!
    //Int_t fNHits;

    ClassDef(CbmTrdDigitizer,1)

    };
#endif //CBMTUTORIALDETDIGITIZER_H
