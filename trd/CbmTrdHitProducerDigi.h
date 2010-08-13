// --------------------------------------------------------------------------
// -----             Header for the CbmTrdHitProducerDigi              ------
// -----                 Created 04.03.09 by F. Uhlig                  ------
// --------------------------------------------------------------------------


/**
 **   Calculate from digi osition the Hit position
 **   Only for one digi cluster. 
 **
 *@   f.uhlig@gsi.de
 **/

/* Generated by Together */

#ifndef CBMTRDHITPRODUCERDIGI_H
#define CBMTRDHITPRODUCERDIGI_H

#include "CbmTrdDetectorId.h"

#include "FairTask.h"

#include "TVector3.h"

#include <vector>

class TClonesArray;
class CbmTrdDigiPar;
class CbmTrdDigi;
class CbmTrdDigiMatch;
class CbmTrdModule;

    class CbmTrdHitProducerDigi : public FairTask {
public:

    

    /** Default constructor **/
    CbmTrdHitProducerDigi();

    /** Standard constructor **/
    CbmTrdHitProducerDigi(const char *name, const char *title="CBM Task", Int_t iVerbose=1);

    /** Destructor **/
    virtual ~CbmTrdHitProducerDigi();

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
    
    TClonesArray *fTrdDigi;      //! TRD digi
    TClonesArray *fTrdDigiMatch; //! TRD digi match
    TClonesArray *fHitCollection;//! TRD hits
    TClonesArray *fMCStack;      //! Tracks

    CbmTrdDigiPar *fDigiPar;   //!
    CbmTrdModule  *fModuleInfo; //!

    CbmTrdDetectorId fTrdId; //!

    std::vector<Int_t> fLayersBeforeStation; //! 
 
    ClassDef(CbmTrdHitProducerDigi,2) 

};
#endif //CBMTRDHITPRODUCERDIGI_H
