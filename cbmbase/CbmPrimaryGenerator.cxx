#include "CbmPrimaryGenerator.h"

#include "CbmMCEvent.h"

#include "FairRootManager.h"
#include "FairRunSim.h"
#include "FairGenerator.h"

#include "TRandom.h"
#include "TObject.h"
#include "TF1.h"

#include <iostream>
using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmPrimaryGenerator::CbmPrimaryGenerator() : FairPrimaryGenerator()
{
  fTimeProb=NULL;
}
// -------------------------------------------------------------------------



// -----   Constructor with title and name   -------------------------------
CbmPrimaryGenerator::CbmPrimaryGenerator(const char* name, 
					 const char* title) 
  : FairPrimaryGenerator(name, title)
{
  fTimeProb=NULL;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmPrimaryGenerator::~CbmPrimaryGenerator() {
}
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
void CbmPrimaryGenerator::Init() {

//  cout << "CBMPriamryGenerator::INit" << endl << endl << endl << endl;

  // Create and register MCEvent
  UInt_t runId=  FairRunSim::Instance()->GetRunId();
  fEvent = new CbmMCEvent(runId);
  FairRootManager::Instance()->Register("MCEvent", "Event", fEvent, kTRUE);

}
// -------------------------------------------------------------------------



// -----   Public method GenerateEvent   -----------------------------------
Bool_t CbmPrimaryGenerator::GenerateEvent(FairGenericStack* pStack) {

  // Check for MCEventHeader
  if ( ! fEvent) {
    cout << "-E- CbmPrimaryGenerator::GenerateEvent: "
	 << "No MCEvent branch! " << endl;
    Fatal("GenerateEvent", "No MCEventHeader branch");
  }

  // Initialise
  fStack   = pStack;
  fNTracks = 0;
  fEvent->Reset();

  // Create event vertex
  MakeVertex();
  fEvent->SetVertex(fVertex);


  // Call the ReadEvent methods from all registered generators
  fListIter->Reset();
  TObject* obj = 0;
  FairGenerator* gen = 0;
  while( (obj = fListIter->Next()) ) {
    gen = dynamic_cast<FairGenerator*> (obj);
    if ( ! gen ) return kFALSE;
    const char* genName = gen->GetName();
    Bool_t test = gen->ReadEvent(this);
    if ( ! test ) {
      cout << "-E CbmPrimaryGenerator: ReadEvent failed for generator "
	   << genName << endl;
      return kFALSE;
    }
  }

  // Set event time
  if(fTimeProb!=0){
     fEventTime = fEventTime + fTimeProb->GetRandom();
  }else{
     fEventTime = fEventTime + gRandom->Uniform( fEventTimeMin,  fEventTimeMax);
  }
  
  fEvent->SetTime(fEventTime);
	 
  // Screen output
  cout << "-I CbmPrimaryGenerator: " << fNTracks << " primary tracks "
       << "from vertex (" <<  fVertex.X() << ", " << fVertex.Y() << ", "
       << fVertex.Z() << ")" << "Event Time = " << fEventTime <<  "(ns) "<< endl;
  fEvent->SetNPrim(fNTracks);

  return kTRUE;
}
// -------------------------------------------------------------------------






ClassImp(CbmPrimaryGenerator)

