#ifndef CBM_ANA_DIELECTRON_TASK_H
#define CBM_ANA_DIELECTRON_TASK_H

#include "FairTask.h"
#include "FairBaseParSet.h"

#include "TProfile.h"
#include "TTree.h"
#include "TFile.h"
#include "TString.h"

#include "CbmVertex.h"
#include "CbmStsKFTrackFitter.h"
#include "CbmStsTrack.h"
#include "CbmMCTrack.h"
#include "CbmGlobalTrack.h"
#include "CbmRichElectronIdAnn.h"

#include <map>
#include <fstream>
#include <vector>
using std::map;
using std::vector;

class TClonesArray;
class TH2D;
class TH1D;
class TH2F;

class FairRootManager;

class DiGlobalTrack{
public:
	CbmGlobalTrack *fGlobalTrack;
	CbmMCTrack *fMCTrack;
	TVector3 position, momentum;
	Double_t mass, charge, energy, rapidity;
	Double_t axisA, axisB, distance;
	Double_t chiPrimary;
	Int_t mcPdg, mcMotherID, mcMotherPdg;
	Int_t stsMCid, richMCid;
	Int_t stsInd, richInd, trdInd, tofInd;
	Int_t is_sts, is_rich, is_trd, is_tof;
	Int_t is_fake, is_wrong, is_pion, is_proton;
	Bool_t is_e;
};

class CbmAnaDielectronTask : public FairTask {

public:
    CbmAnaDielectronTask();
    CbmAnaDielectronTask(const char *name, const char *title="CBM Task");
    virtual ~CbmAnaDielectronTask();
    virtual InitStatus Init();
    virtual void Exec(Option_t *option);

    void SingleParticleAcceptance();
    void MCPairs();
    void PairAcceptance();

    virtual void Finish();
    void WriteOutput();

    ClassDef(CbmAnaDielectronTask,1);

private:
    TClonesArray *fMCTracks;

    TClonesArray *fRichRings;
    TClonesArray *fRichProj;
    TClonesArray *fRichPoints;
    TClonesArray *fRichRingMatches;
    TClonesArray *fRichHits;

    TClonesArray *fGlobalTracks;

    TClonesArray *fStsTracks;
    TClonesArray *fStsTrackMatches;
    TClonesArray *fStsHits;
    TClonesArray *fMvdHits;

    TClonesArray *fTrdTracks;
    TClonesArray *fTrdHits;
    TClonesArray *fTrdTrackMatches;

    TClonesArray *fTofHits;
    TClonesArray *fTofPoints;

    CbmVertex *fPrimVertex;

    Int_t fEvents;

    Bool_t fUseRich;
    Bool_t fUseTrd;
    Bool_t fUseTof;


    Double_t fWeight; //Multiplicity*BR

    Int_t fNofMcEp;  //number of MC e+
    Int_t fNofMcEm;  // number of MC e-

    Int_t fNofAccEp; //number of accepted e+
    Int_t fNofAccEm; //number of accepted e-
    Int_t fNofAccPairs; //number of accepted pairs of e-/e+
    
    TH2D* fh_mc_signal_pty; // pt/y distribution for signal mc
    
public:
    void SetUseRich(Bool_t use){fUseRich = use;};
    void SetUseTrd(Bool_t use){fUseTrd = use;};
    void SetUseTof(Bool_t use){fUseTof = use;};
};

#endif
