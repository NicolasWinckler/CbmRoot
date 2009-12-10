// --------------------------------------------------------------------------
// -----          Header for the CbmTrdCreateDigiPar               ------
// -----              Created 06.06.08 by F.Uhlig                      ------
// --------------------------------------------------------------------------



#ifndef CBMTRDCREATEDIGIPAR_H
#define CBMTRDCREATEDIGIPAR_H

#include "CbmTrdDetectorId.h"

#include "FairTask.h"

#include <map>
#include <list>
//#include <utility>


class TClonesArray;
class TArrayI;

class CbmTrdDigiPar;
class CbmTrdDigi;
class CbmTrdModule;
class CbmTrdDigiPar;

class CbmTrdCreateDigiPar : public FairTask {

  public:

    /** Default constructor **/
    CbmTrdCreateDigiPar();

    /** Standard constructor **/
    CbmTrdCreateDigiPar(const char *name, const char *title="CBM Task");

    /** Destructor **/
    virtual ~CbmTrdCreateDigiPar();

    /** Initialisation **/
    virtual InitStatus ReInit();
    virtual InitStatus Init();
    virtual void SetParContainers();

    /** Executed task **/
    virtual void Exec(Option_t * option);

    /** Finish (called after each event) **/
    virtual void FinishEvent(){;}
 
   /** Finish task (called after all event) **/
    virtual void FinishTask();


private:

    void GetModuleInformation();
    void CalculateModuleId();

    void FillModuleMap();

    Int_t fStation;
    Int_t fLayer;
    Int_t fModuleType;
    Int_t fModuleCopy;

    Float_t fpadsizex; //pixel width in x;
    Float_t fpadsizey; //pixel width in y;
    Float_t fsizex;    //module width in x;
    Float_t fsizey;    //module width in y;
    Int_t   fCol;      //Calculated pixel column were the hit is in
    Int_t   fRow;      //Calculated pixel row were the hit is in
    Bool_t  fIsRotated;

    Int_t   fModuleID; //Unique number for detector module

    Float_t fPosX;     //Hit position in chamber coordinates
    Float_t fPosY;     //Hit position in chamber coordinates

    /** Map of Unique Trd Module Id to corresponding TrdModule **/
    std::map<Int_t, CbmTrdModule*> fModuleMap;                  //!   
    std::map<Int_t, CbmTrdModule*>::iterator fModuleMapIt;      //!   
    CbmTrdDigiPar *fDigiPar;

    CbmTrdDetectorId fTrdDetId;

    ClassDef(CbmTrdCreateDigiPar,1)

    };
#endif //CBMTRDCREATEDIGIPAR_H
