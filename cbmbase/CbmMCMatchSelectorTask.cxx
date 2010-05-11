// -------------------------------------------------------------------------
// -----                CbmMCMatchSelectorTask source file             -----
// -----                  Created 18/07/08  by T.Stockmanns        -----
// -------------------------------------------------------------------------
// libc includes
#include <iostream>

// Root includes
#include "TROOT.h"
#include "TClonesArray.h"

// framework includes
#include "FairRootManager.h"
#include "CbmMCMatchSelectorTask.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairHit.h"
#include "FairLinkedData.h"

#include "CbmMCEntry.h"


// -----   Default constructor   -------------------------------------------
CbmMCMatchSelectorTask::CbmMCMatchSelectorTask()
	: FairTask("Creates CbmMCMatch"), fStart(kUnknown), fStop(kUnknown)
{
}
// -------------------------------------------------------------------------

CbmMCMatchSelectorTask::CbmMCMatchSelectorTask(DataType start, DataType stop)
	: FairTask("Creates CbmMCMatch"), fStart(start), fStop(stop)
{
}

// -----   Destructor   ----------------------------------------------------
CbmMCMatchSelectorTask::~CbmMCMatchSelectorTask()
{
}

// -----   Public method Init   --------------------------------------------
InitStatus CbmMCMatchSelectorTask::Init()
{


//  fMCMatch->InitStage(kMCTrack, "", "MCTrack");



  FairRootManager* ioman = FairRootManager::Instance();
  	if (!ioman) {
  		std::cout << "-E- CbmMCMatchSelectorTask::Init: "
  				<< "RootManager not instantiated!" << std::endl;
  		return kFATAL;
  	}

  	fMCMatch = (CbmMCMatch*)ioman->GetObject("MCMatch");

	std::cout << "-I- CbmMCMatchSelectorTask::Init: Initialization successfull" << std::endl;


  return kSUCCESS;
}


// -------------------------------------------------------------------------
void CbmMCMatchSelectorTask::SetParContainers()
{
  // Get Base Container
//  FairRun* ana = FairRun::Instance();
//  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

}


// -----   Public method Exec   --------------------------------------------
void CbmMCMatchSelectorTask::Exec(Option_t* opt)
{
	std::cout << "Output Selector: " << std::endl;
	SetWeights();
	std::cout << fMCMatch->GetMCInfo(fStart, fStop);
}

void CbmMCMatchSelectorTask::SetWeights()
{
//	std::cout << "SetWeights: CommonWeight " << fCommonWeight << " NStageWeights " << fStageWeights.size() << std::endl;
	fMCMatch->SetCommonWeightStages(fCommonWeight);
	for (int i = 0; i < fStageWeights.size();i++){
		fMCMatch->GetMCStageType(fStageWeights[i].first)->SetWeight(fStageWeights[i].second);
	}
}

void CbmMCMatchSelectorTask::Finish()
{
}


ClassImp(CbmMCMatchSelectorTask);
