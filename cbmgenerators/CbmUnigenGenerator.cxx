// -------------------------------------------------------------------------
// -----                     CbmUnigenGenerator                        -----
// -----              Created 2006/Jul/04  by D. Kresan                -----
// -------------------------------------------------------------------------
#include <iostream>
using namespace std;

#include "TFile.h"
#include "TTree.h"

#include "URun.h"
#include "UEvent.h"
#include "UParticle.h"

#include "CbmPrimaryGenerator.h"
#include "CbmUnigenGenerator.h"


// ------------------------------------------------------------------------
CbmUnigenGenerator::CbmUnigenGenerator()
{
    fEvents = 0;
    fInputFile = NULL;
    fInTree = NULL;
    fEvent = NULL;
}
// ------------------------------------------------------------------------


// ------------------------------------------------------------------------
CbmUnigenGenerator::CbmUnigenGenerator(TString fileName)
{
    fEvents = 0;
    fInputFile = NULL;
    fInTree = NULL;
    fEvent = NULL;
    fFileName = fileName;
    cout << "-I- CbmUnigenGenerator: Opening input file " << fileName << endl;
    fInputFile = new TFile(fFileName);
    if (NULL == fInputFile) {
	Fatal("CbmUnigenGenerator","Cannot open input file.");
    }

    // Get run description
    URun *run = (URun*) fInputFile->Get("run");
    if(NULL==run) {
	Fatal("CbmUnigenGenerator","No run description in input file.");
    }
    fCM = kFALSE;
    if(TMath::Abs(run->GetPTarg()) > 0) fCM = kTRUE;
    fBetaCM = 0.;
    if(fCM) {
        cout << "-I- CbmUnigenGenerator: we are in CM frame" << endl;
	Double_t elab = (TMath::Power(run->GetSqrtS()/run->GetAProj(),2)-
			 2*TMath::Power(0.938271998,2))/(2*0.938271998);
	Double_t plab = TMath::Sqrt(elab*elab - TMath::Power(0.938271998,2));
        cout << "-I- CbmUnigenGenerator: Plab = " << plab << " AGeV" << endl;
        fBetaCM = plab / (elab + 0.938271998);
	cout << "-I- CbmUnigenGenerator: boost parameters:" << endl
	    << "                             betaCM = " << fBetaCM
	    << ",  gammaCM = " << fGammaCM << endl;
    } else {
        cout << "-I- CbmUnigenGenerator: we are in LAB frame" << endl;
    }
    fGammaCM = 1./TMath::Sqrt(1. - fBetaCM*fBetaCM);

    fInTree = (TTree*) fInputFile->Get("events");
    if(NULL == fInTree) {
	Fatal("CbmUnigenGenerator","No event tree in input file.");
    }
    fEvent = new UEvent();
    fInTree->SetBranchAddress("event", &fEvent);
}
// ------------------------------------------------------------------------


// ------------------------------------------------------------------------
CbmUnigenGenerator::~CbmUnigenGenerator()
{
    CloseInput();
}
// ------------------------------------------------------------------------


// ------------------------------------------------------------------------
Bool_t CbmUnigenGenerator::ReadEvent(CbmPrimaryGenerator* primGen)
{
    // Check for input file
    if(NULL==fInputFile || NULL==fInTree) {
	cout << "-E- CbmUnigenGenerator: Input file is not opened!" << endl;
	return kFALSE;
    }

    // If end of input file is reached : close it and abort run
    if(fEvents >= fInTree->GetEntries()) {
	cout << "-I- CbmUnigenGenerator: End of input file reached" << endl;
	CloseInput();
	return kFALSE;
    }

    // Read entry
    fInTree->GetEntry(fEvents);

    cout << "-I- CbmUnigenGenerator: Event " << fEvent->GetEventNr()
	<< ", multiplicity " << fEvent->GetNpa() << endl;

    UParticle *particle;
    Double_t pz;
    Double_t pz1;

    // Loop over tracks in the current event
    for (Int_t itrack = 0; itrack < fEvent->GetNpa(); itrack++) {
	// Get particle
	particle = fEvent->GetParticle(itrack);
	// Boost
	pz = particle->Pz();
	if(fCM) {
	    pz1 = fGammaCM*(pz + fBetaCM*particle->E());
	} else {
            pz1 = pz;
	}
	// Give track to PrimaryGenerator
	primGen->AddTrack(particle->GetPdg(),
			  particle->Px(),
			  particle->Py(),
			  pz1,
			  particle->X(),
			  particle->Y(),
			  particle->Z());
    }

    fEvents += 1;

    return kTRUE;
}
// ------------------------------------------------------------------------


// ------------------------------------------------------------------------
void CbmUnigenGenerator::CloseInput()
{
    if(NULL != fInputFile) {
	cout << "-I- CbmUnigenGenerator: Closing input file "
	    << fFileName << endl;
	fInputFile->Close();
	fInputFile = NULL;
    }
}
// ------------------------------------------------------------------------


ClassImp(CbmUnigenGenerator);

