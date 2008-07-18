// --------------------------------------------------------------------------
// -----             Header for the CbmTrdHitProducer                  ------
// -----          Partially Created by M. Kalisky                      ------
// -----           Last update 21.03.05 by M. Kalisky                  ------
// --------------------------------------------------------------------------


/**
 **   !!! WARNING  !!!
 **   temporary solution  ONLY for old CVS release (before implementation of the
 **   parameters by Mohammad and Dennis)
 **
 **   compatibility with the new release is not granted.
 **   This hit producer works only with geometry versions for 9 and 12
 **   TRD layers.
 **
 **   For any more detailed studies including the TRD I strongly advice
 **   to use the new release (should be provided before end of May)
 **
 **   Any futere developements will be implemented only for the new release
 **
 *@   m.kalisky@gsi.de
 **/

/* Generated by Together */

#ifndef CBMTRDHITPRODUCER_H
#define CBMTRDHITPRODUCER_H


#include "CbmTask.h"

#include "TVector3.h"

class TClonesArray;
class CbmBaseParSet;
class CbmGeoTrdPar;
class CbmTrdHit;
class CbmTrdRadiator;


    class CbmTrdHitProducer : public CbmTask {
public:

    

    /** Default constructor **/
    CbmTrdHitProducer();

    /** Standard constructor **/
    CbmTrdHitProducer(const char *name, const char *title="CBM Task");

    /** Destructor **/
    virtual ~CbmTrdHitProducer();

    /** Initialisation **/
    virtual InitStatus ReInit();
    virtual InitStatus Init();
    virtual void SetParContainers();

    /** Executed task **/
    virtual void Exec(Option_t * option);

    /** Finish task **/
    virtual void Finish();

    //TRefArray *GetRef(){return fRef;}
    void AddHit(TVector3 &posHit, TVector3 &posHitErr,
		Int_t TrackID, Int_t PlaneID, Int_t ref, Double_t ELoss,
		Double_t ELossTR, Double_t ELossdEdX);
    void SmearingXY( Double_t dx , Double_t dy ) { fDx = dx; fDy=dy ;}
    void SmearingX(Double_t dx) {fDx = dx;}
    void SmearingY(Double_t dy) {fDy = dy;}
    void Register();
    void SetNlayer(Int_t a) ;
    void SetEfficency(Float_t eff) {fEfficency=eff;}
    void SetSigmaX(Double_t sigma[]) ;
    void SetSigmaY(Double_t s1[], Double_t s2[], Double_t s3[]);

    // set the mode of TR computration
    void SetSimpleTR(Bool_t s) {fSimpleTR = s; }


    Double_t GetSigmaX (Int_t stack);
    Float_t GetEfficency () { return fEfficency;}
    Double_t GetSigmaY (Double_t teta, Int_t stack );

    void SetPar(Int_t Nfoils, Float_t FoilThick, Float_t GapThick){
	fNfoils = Nfoils;
	fFoilThick = FoilThick;
        fGapThick = GapThick;
    }


private:

    TClonesArray *fTrdPoints; //! TRD MC points
    TClonesArray *fHitCollection; //! TRD hits
    TClonesArray *fListStack;         //Tracks
    //TRefArray *fRef; //!


    Double_t fDx;               //!
    Double_t fDy;               //!
    Double_t fSigmaX[3];        //!
    Double_t fSigmaY[3] [7];    //!
    Int_t    fTrdPerStation;    //!
    Int_t    fNlayer;           //!
    TString  fVersion;          //!
    Int_t    fNHits;            //!
    Float_t  fEfficency;        //! Hit finding efficency (0-100%)

    CbmTrdRadiator*  fRadiator; //!
    CbmGeoTrdPar*    fGeoPar;   //!
    CbmBaseParSet*    fBasePar; //!

    // for the CbmTrdRadiator
    Int_t   fNfoils;       //!
    Float_t fFoilThick;    //!
    Float_t fGapThick;     //!
    Bool_t  fSimpleTR;     //! simple or "full" TR computation

    // positions of the active volume for given copyes of the chambers
    // temporary solution

    Float_t ftrdU1[8];      //!
    Float_t ftrdU2[8];      //!
    Float_t ftrdU3[8];      //!
    Float_t ftrdR1[8];      //!
    Float_t ftrdR2[8];      //!
    Float_t ftrdR3[8];      //!

    ClassDef(CbmTrdHitProducer,2) //CBMTRDHitProducer

    };
#endif //CBMTRDHITPRODUCER_H
