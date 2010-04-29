// -------------------------------------------------------------------------
// -----                CbmMCMatchCreatorTask source file             -----
// -----                  Created 18/07/08  by T.Stockmanns        -----
// -------------------------------------------------------------------------
// libc includes
#include <iostream>

// Root includes
#include "TROOT.h"
#include "TClonesArray.h"

// framework includes
#include "FairRootManager.h"
#include "CbmMCMatchCreatorTask.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairHit.h"
#include "FairLinkedData.h"

#include "CbmDetectorList.h"
#include "CbmMCEntry.h"


// -----   Default constructor   -------------------------------------------
CbmMCMatchCreatorTask::CbmMCMatchCreatorTask() : FairTask("Creates CbmMCMatch"), fEventNr(0)
{
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmMCMatchCreatorTask::~CbmMCMatchCreatorTask()
{
}

// -----   Public method Init   --------------------------------------------
InitStatus CbmMCMatchCreatorTask::Init()
{


//  fMCMatch->InitStage(kMCTrack, "", "MCTrack");

	fMCMatch = new CbmMCMatch("CbmMCMatch", "CbmMCMatch");

  fMCMatch->InitStage(kStsPoint, "", "StsPoint");
  fMCMatch->InitStage(kStsDigi, "", "StsDigi");
  fMCMatch->InitStage(kStsCluster, "", "StsCluster");
  fMCMatch->InitStage(kStsHit, "", "StsHit");

  InitStatus status = InitBranches();


  FairRootManager* ioman = FairRootManager::Instance();
  	if (!ioman) {
  		std::cout << "-E- CbmMCMatchCreatorTask::Init: "
  				<< "RootManager not instantiated!" << std::endl;
  		return kFATAL;
  	}

	fMCLink = new TClonesArray("CbmMCEntry");
	ioman->Register("MCLink", "MCInfo", fMCLink, kTRUE);

	ioman->Register("MCMatch", "MCMatch", fMCMatch, kFALSE);

	std::cout << "-I- CbmMCMatchCreatorTask::Init: Initialization successfull" << std::endl;


  return status;
}

InitStatus CbmMCMatchCreatorTask::InitBranches()
{

	 // Get RootManager
	FairRootManager* ioman = FairRootManager::Instance();
	if (!ioman) {
		std::cout << "-E- CbmMCMatchCreatorTask::Init: "
				<< "RootManager not instantiated!" << std::endl;
		return kFATAL;
	}

	int NStages = fMCMatch->GetNMCStages();
	for (int i = NStages-1; i > -1; i--){
		TClonesArray* myBranch = (TClonesArray*)ioman->GetObject(fMCMatch->GetMCStage(i)->GetBranchName().c_str());
		if (!myBranch)	{
			//std::cout << "NMCStages: " << fMCMatch->GetNMCStages() << std::endl;
			std::cout << "-W- CbmMCMatchCreatorTask::Init: "<< "No "<<fMCMatch->GetMCStage(i)->GetBranchName() << " array!" << std::endl;
			fMCMatch->GetMCStage(i)->SetFill(kFALSE); //RemoveStage(fMCMatch->GetMCStage(i)->GetStageId());

			continue;
		}
		else fMCMatch->GetMCStage(i)->SetFill(kTRUE);
		fBranches[fMCMatch->GetMCStage(i)->GetBranchName()] = myBranch;
	}
	return kSUCCESS;
}
// -------------------------------------------------------------------------
void CbmMCMatchCreatorTask::SetParContainers()
{
  // Get Base Container
//  FairRun* ana = FairRun::Instance();
//  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

}


// -----   Public method Exec   --------------------------------------------
void CbmMCMatchCreatorTask::Exec(Option_t* opt)
{

	if (!fMCLink) Fatal("Exec", "No fMCLinkDet");
	fMCLink->Delete();
	fMCMatch->ClearMCList();

	fMCMatch->LoadInMCLists(fMCLink);

	std::cout << "NMCStages: " << fMCMatch->GetNMCStages() << std::endl;
	for (int i = 0; i < fMCMatch->GetNMCStages(); i++){
		if (fMCMatch->GetMCStage(i)->GetFill() == kTRUE && fMCMatch->GetMCStage(i)->GetLoaded() == kFALSE){
			std::cout << i << ": ";
			std::cout << "BranchName: " << fMCMatch->GetMCStage(i)->GetBranchName() << std::endl;
			TClonesArray* clArray = fBranches[fMCMatch->GetMCStage(i)->GetBranchName()];
			for (int j = 0; j < clArray->GetEntries(); j++){
				FairLinkedData* myData = (FairLinkedData*)clArray->At(j);
				fMCMatch->SetElements(fMCMatch->GetMCStage(i)->GetStageId(), j, myData);
			}
			if (fMCMatch->GetMCStage(i)->GetNEntries() > 0)
				fMCMatch->GetMCStage(i)->SetLoaded(kTRUE);
		}
	}

	int i = 0;
	for (int index = 0; index < fMCMatch->GetNMCStages(); index++){
		CbmMCStage myStage(*(fMCMatch->GetMCStage(index)));

		for (int indStage = 0; indStage < myStage.GetNEntries(); indStage++){

			CbmMCEntry myLink(myStage.GetMCLink(indStage));
			//std::cout << "myLink: " << myStage.GetMCLink(indStage).GetSource() << "/" << myStage.GetMCLink(indStage).GetPos() << std::endl;
			new((*fMCLink)[i]) CbmMCEntry(myLink.GetLinks(), myLink.GetSource(), myLink.GetPos());
			i++;
		}
	}

	if (fVerbose > 0){
		fMCMatch->Print();
		std::cout << std::endl;
	}
}

void CbmMCMatchCreatorTask::Finish()
{
}


ClassImp(CbmMCMatchCreatorTask);
