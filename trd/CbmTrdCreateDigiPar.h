// --------------------------------------------------------------------------
// -----          Header for the CbmTrdCreateDigiPar               ------
// -----              Created 06.06.08 by F.Uhlig                      ------
// --------------------------------------------------------------------------



#ifndef CBMTRDCREATEDIGIPAR_H
#define CBMTRDCREATEDIGIPAR_H

#include "FairTask.h"

#include "TArrayD.h"

#include <map>

class CbmTrdDigiPar;
class CbmTrdModule;
class CbmTrdGeoHandler;

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

    void FillModuleInfoFromGeoHandler(TString FullPath);
    void CorrectOrientationOfPadPlane();
    void FillDigiPar();

    void FillModuleMapRootGeometry();
    void FillModuleMapSegmentedSquaredOneKeepingVolume();
    void FillPadInfoSegmentedSquaredOneKeepingVolume();

    void FillModuleMapSegmentedSquared();
    void FillPadInfoSegmentedSquared();

    void FillModuleMapSegmentedRectangular();
    void FillPadInfoSegmentedRectangular();

    Int_t fStation;    //station number
    Int_t fLayer;      //layer number
    Int_t fModuleType; //module type [1-8]
    Int_t fModuleCopy; //module copy number
    Int_t fSector;     //sector number

    Double_t fSizex;
    Double_t fSizey;
    Double_t fSizez;
    Double_t fX;
    Double_t fY;
    Double_t fZ;

    TArrayD fpadsizex; //pixel width in x;
    TArrayD fpadsizey; //pixel width in y;
    Float_t fsizex;    //module width in x;
    Float_t fsizey;    //module width in y;
    TArrayD fSectorSizex;    //sector width in x;
    TArrayD fSectorSizey;    //sector width in y;
    Int_t   fCol;      //Calculated pixel column were the hit is in
    Int_t   fRow;      //Calculated pixel row were the hit is in

    Int_t   fModuleID; //Unique number for detector module

    Int_t   fMaxSectors; //Max. number of sectors for all modules 

    Float_t fPosX;     //Hit position in chamber coordinates
    Float_t fPosY;     //Hit position in chamber coordinates

    /** Map of Unique Trd Module Id to corresponding TrdModule **/
    std::map<Int_t, CbmTrdModule*> fModuleMap;                  //!   
    std::map<Int_t, CbmTrdModule*>::iterator fModuleMapIt;      //!   

    CbmTrdDigiPar *fDigiPar;    //! pointer to digi parameters

    CbmTrdGeoHandler* fGeoHandler;
  
    std::map<Int_t, std::map<Int_t, std::vector<Int_t> > > fModInfoMap;
    std::map<Int_t, std::vector<Int_t> >  fModTypeMap;
    std::vector<Int_t> fModuleTypeVector;

    CbmTrdCreateDigiPar(const CbmTrdCreateDigiPar&);
    CbmTrdCreateDigiPar& operator=(const CbmTrdCreateDigiPar&);

    ClassDef(CbmTrdCreateDigiPar,4)

    };
#endif //CBMTRDCREATEDIGIPAR_H
