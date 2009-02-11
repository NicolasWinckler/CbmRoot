// -------------------------------------------------------------------------
// -----                     CbmUnigenGenerator                        -----
// -----              Created 2006/Jul/04  by D. Kresan                -----
// -------------------------------------------------------------------------
#ifndef CBM_UNIGEN_GENERATOR
#define CBM_UNIGEN_GENERATOR

#include "TString.h"
#include "FairGenerator.h"

class TFile;
class TTree;
class UEvent;
class FairPrimaryGenerator;


class CbmUnigenGenerator : public FairGenerator {

private:
    Int_t         fEvents;           //! Current event number
    TFile        *fInputFile;        //! Input file
    TString       fFileName;         //! Input file Name
    TTree        *fInTree;           //! Input tree
    UEvent       *fEvent;            //! Input event
    Bool_t        fCM;               //! CM flag
    Double_t      fBetaCM;           //! CM velocity
    Double_t      fGammaCM;          //! CM gamma factor

    void CloseInput();

public:
    CbmUnigenGenerator();
    CbmUnigenGenerator(TString fileName);
    virtual ~CbmUnigenGenerator();

    virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen);

    ClassDef(CbmUnigenGenerator,1);
};


#endif
