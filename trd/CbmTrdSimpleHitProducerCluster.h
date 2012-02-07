// --------------------------------------------------------------------------
// -----       Header for the CbmTrdSimplaHitProducerCluster           ------
// -----                 Created 04.03.09 by F. Uhlig                  ------
// --------------------------------------------------------------------------


/**
 **   Calculate the Hit position from the Cluster which
 **   contains only one digi
 **   Only for one digi cluster. 
 **
 *@   f.uhlig@gsi.de
 **/


#ifndef CBMTRDSIMPLEHITPRODUCERCLUSTER_H
#define CBMTRDSIMPLEHITPRODUCERCLUSTER_H

#include "CbmTrdDetectorId.h"

#include "FairTask.h"

#include "TVector3.h"

#include <vector>

class TClonesArray;
class CbmTrdDigiPar;
class CbmTrdDigi;
class CbmTrdDigiMatch;
class CbmTrdModule;

    class CbmTrdSimpleHitProducerCluster : public FairTask {
public:

    

    /** Default constructor **/
    CbmTrdSimpleHitProducerCluster();

    /** Standard constructor **/
    CbmTrdSimpleHitProducerCluster(const char *name, 
				   const char *title="CBM Task", 
				   Int_t iVerbose=1);

    /** Destructor **/
    virtual ~CbmTrdSimpleHitProducerCluster();

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

    
    TClonesArray *fTrdCluster;   //! TRD digi
    TClonesArray *fTrdDigi;      //! TRD digi
    TClonesArray *fHitCollection;//! TRD hits
    TClonesArray *fMCStack;      //! Tracks

    CbmTrdDigiPar *fDigiPar;   //!
    CbmTrdModule  *fModuleInfo; //!

    CbmTrdDetectorId fTrdId; //!

    std::vector<Int_t> fLayersBeforeStation; //! 
 
    CbmTrdSimpleHitProducerCluster(const CbmTrdSimpleHitProducerCluster&);
    CbmTrdSimpleHitProducerCluster& operator=(const CbmTrdSimpleHitProducerCluster&);

    ClassDef(CbmTrdSimpleHitProducerCluster,2) 

};
#endif //CBMTRDHITPRODUCERDIGI_H
