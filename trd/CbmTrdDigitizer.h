// --------------------------------------------------------------------------
// -----          Header for the CbmTrdDigitizer               ------
// -----              Created 06.06.08 by F.Uhlig                      ------
// --------------------------------------------------------------------------



#ifndef CBMTUTORIALDETDIGITIZER_H
#define CBMTUTORIALDETDIGITIZER_H

#include "CbmTrdDetectorId.h"

#include "FairTask.h"

//#include "TVector3.h"

#include <map>
#include <list>
//#include <utility>


class TClonesArray;

class CbmTrdDigiPar;
class CbmTrdDigi;
class CbmTrdModule;
class CbmTrdRadiator;

class CbmTrdDigitizer : public FairTask {

  public:

    /** Default constructor **/
    CbmTrdDigitizer();

    /** Standard constructor **/
    CbmTrdDigitizer(const char *name, const char *title="CBM Task",
                    CbmTrdRadiator *radiator=NULL, Int_t iVerbose=1);

    /** Destructor **/
    virtual ~CbmTrdDigitizer();

    /** Initialisation **/
    virtual InitStatus ReInit();
    virtual InitStatus Init();
    virtual void SetParContainers();

    /** Executed task **/
    virtual void Exec(Option_t * option);

    /** Finish (called after each event) **/
    virtual void FinishEvent();
 
   /** Finish task (called after all event) **/
    virtual void FinishTask(){;}

    /*
    void AddHit(TVector3 &posHit, TVector3 &posHitErr,
		Int_t TrackID, Int_t PlaneID, Int_t ref, Double_t ELoss,
		Double_t ELossTR, Double_t ELossdEdX);
    */

    void Register();

private:

    void GetModuleInformationFromDigiPar(Int_t, Double_t*);
    //    void GetModuleInformation();
   
    void AddDigi();

    void CalculatePixel();

    //    Int_t   fStation;
    //    Int_t   fLayer;
    //    Int_t   fModuleType;
    //    Int_t   fModuleCopy;
    Int_t   fCol; //Calculated pixel column were the hit is in
    Int_t   fRow; //Calculated pixel row were the hit is in
    Int_t   fModuleID;//Unique number for detector module
    Int_t   fMCindex;// index to MCPoint
    Float_t fpadsizex; //pixel widh in x;
    Float_t fpadsizey; //pixel width in y;
    Float_t fsizex; //module widh in x;
    Float_t fsizey; //module width in y;
    Float_t fELoss;//energy loss from MCPoint 
    Float_t fTime;//time from MCPoint
    Float_t fPosX;//Hit position in chamber coordinates
    Float_t fPosY;//Hit position in chamber coordinates
    Float_t fEfficiency; // Digi production efficiency (0-100%)
    //    Double_t fthreshold; //pixel threshold in electrons
    //    Double_t fnoise; //pixel noise in electrons

    TClonesArray *fTrdPoints; //! Trd MC points
    TClonesArray *fDigiCollection; //! TRD digis
    TClonesArray *fDigiMatchCollection; //! Corresponding MCPoints to TRD digis
    TClonesArray *fMCStack;  //! MC Track information

    CbmTrdDigiPar  *fDigiPar;
    CbmTrdModule   *fModuleInfo;
    CbmTrdRadiator *fRadiator; 

    CbmTrdDetectorId fTrdId; //!
    
    /**  map to store digis for pair of x,y position in module **/
    // map<pair<ModuleID,pair<x,y>>, CbmTrdDigi*>
    std::map<std::pair< Int_t, std::pair< Int_t, Int_t > >, CbmTrdDigi* > fDigiMap; 
    /**  iterator over map to store digis for pair of x,y position in module **/
    std::map<std::pair< Int_t, std::pair< Int_t, Int_t > >, CbmTrdDigi* >::iterator fDigiMapIt; //! iterator over array above

    ClassDef(CbmTrdDigitizer,2)

    };
#endif //CBMTUTORIALDETDIGITIZER_H
