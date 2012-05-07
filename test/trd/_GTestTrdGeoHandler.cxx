#include "CbmTrdGeoHandler.h"

#include "TFile.h"
#include "TGeoManager.h"
#include "TString.h"
#include "TSystem.h"
#include "TList.h"
#include "TKey.h"

#include "FairMockVirtualMC.h"

#include "gtest/gtest.h"
#include "gtest/gtest-spi.h"
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <vector>

using std::cout;
using std::endl;

// Structure to pass filenames together with expected response into the
// parametrized test
struct InOutStructure {
  TString name;
  int result;
} ;

struct ParamStructure2 {
  TString name;
  int result;
  Int_t stationIdLength;
  Int_t* stationId;
  Int_t moduleIdLength;
  Int_t* moduleId;
} ;

// Base class to use the same basic setup for parameterized and
// non-parameterized tests
// Here one defines everything which is common for all the different
// test cases
template <class T> class _TestTrdGeoHandlerBase : public T
{
  protected:

  TFile* fInputFile;
  TString geomToTest;
  TString fFileName;
  CbmTrdGeoHandler* fGeoHandler;
  int result;
  FairMockVirtualMC* fMockVMC;

  virtual void SetUp() {

    geomToTest="squared_segmented";
    CreateFileName(geomToTest);
    fInputFile = new TFile(fFileName,"READ");

    GetGeoManager();
    fGeoHandler = new CbmTrdGeoHandler();
  }

  virtual void TearDown() {
    gGeoManager=NULL;
    delete fGeoHandler;
    fInputFile->Close();
    delete fInputFile;
    //    delete fMockVMC;
    
  }

  virtual void CreateFileName(TString name) {
    fFileName = gSystem->Getenv("VMCWORKDIR");
    fFileName = fFileName + "/input/qa/trd_" + name + "_geometry.root";
  }

  virtual void GetGeoManager() {
    gGeoManager->SetVerboseLevel(0);
    TList* l= fInputFile->GetListOfKeys();  
    TKey* key;
    TIter next( l);
    while ((key = (TKey*)next())) {
      /**loop inside the delivered root file and try to fine a TGeoManager object
       * the first TGeoManager found will be read
       */  
      if (strcmp(key->GetClassName(),"TGeoManager") != 0) { continue; }
      gGeoManager = (TGeoManager*)key->ReadObj();
    }
    fMockVMC = new FairMockVirtualMC("Mock implementation of VMC interface");
  }
};

// This is the derived class for the non-parameterized test cases.
class  TrdGeoHandlerTest : public _TestTrdGeoHandlerBase<testing::Test> {};

TEST_F(TrdGeoHandlerTest, CheckUniqueIdCreation)
{
  //  fGeoHandler->CheckGeometryVersion();
  //  fGeoHandler->FillInternalStructures();
  fGeoHandler->Init();

  TString TopNode = gGeoManager->GetTopNode()->GetName();
  cout <<"TopNode: "<<TopNode<<endl;
  const char* path = "/cave_1/trd1_0/trd1mod1_1001/trd1mod1gas_0";
  gGeoManager->cd(path);
  const char* path1 = gGeoManager->GetPath();

  cout << "Path: "<<path1<<endl;

  TGeoNode *node = gGeoManager->GetCurrentNode();
  Int_t copy = node->GetNumber();
  Int_t id = node->GetVolume()->GetNumber();
  cout << "Id: "<<id<<" , "<<copy<<endl;
  
  Int_t uniqueId = fGeoHandler->GetUniqueDetectorId();
  
  cout <<"UniqueId: "<<uniqueId<<endl;
  EXPECT_EQ(135717, uniqueId);

}

TEST_F(TrdGeoHandlerTest, CheckDefaultSettings)
{
  // Fill vector with expected volume ids for this geometry.
  std::vector<Int_t>  expStationId;
  expStationId.push_back(218);
  expStationId.push_back(243);
  expStationId.push_back(268);

  std::vector<Int_t>  expModuleId;
  expModuleId.push_back(219);
  expModuleId.push_back(227);
  expModuleId.push_back(235);
  expModuleId.push_back(244);
  expModuleId.push_back(252);
  expModuleId.push_back(260);
  expModuleId.push_back(0);
  expModuleId.push_back(0);
  expModuleId.push_back(269);

  Int_t retVal = fGeoHandler->CheckGeometryVersion();
  EXPECT_EQ(4, retVal);
  EXPECT_EQ(4, fGeoHandler->GetGeoVersion());

  fGeoHandler->FillInternalStructures();

  std::vector<Int_t> stationId = fGeoHandler->GetStationId();
  std::vector<Int_t>::iterator vecIt;
  Int_t counter=0;
  for(vecIt = stationId.begin(); vecIt < stationId.end(); vecIt++){
    EXPECT_EQ(expStationId[counter], *vecIt);
    counter++;
  }

  std::vector< std::vector <Int_t> > moduleId = fGeoHandler->GetModuleId();
  std::vector< std::vector <Int_t> >::iterator vecIt1;
  counter=0;
  for(vecIt1 = moduleId.begin(); vecIt1 < moduleId.end(); vecIt1++){
    std::vector<Int_t> modInfo = *vecIt1;
    for(vecIt = modInfo.begin(); vecIt < modInfo.end(); vecIt++){
      EXPECT_EQ(expModuleId[counter], *vecIt);
      counter++;
    } 
  }
}

TEST_F(TrdGeoHandlerTest, CheckExtractingMCVolumeId)
{
  char volumeName[10];
  int stationNr=1; 
  std::vector<int> fStationId;     //! MC IDs of TRD stations
  do {
    sprintf(volumeName, "trd%d", stationNr);
    result = fGeoHandler->GetMCId(volumeName, fStationId);
    cout << "Result: "<< result << endl;
    cout << "StationId: " << fStationId[stationNr-1] << endl;
    stationNr++;
  }
  while (result); 
  
}

// This is the derived class for the parameterized test cases.
class TrdGeoHandlerParamTest2 : public _TestTrdGeoHandlerBase<
  testing::TestWithParam<ParamStructure2> >
{
 protected:

  std::vector<Int_t> expStationId; 
  std::vector<Int_t> expModuleId; 
  
  virtual void SetUp() {
    ParamStructure2 const& p = GetParam();

    geomToTest = p.name;
    result = p.result;
    for (Int_t i=0; i< p.stationIdLength; i++) {
      expStationId.push_back(p.stationId[i]);
    }
    for (Int_t i=0; i< p.moduleIdLength; i++) {
      expModuleId.push_back(p.moduleId[i]);
    }

    CreateFileName(geomToTest);
    fInputFile = new TFile(fFileName,"READ");
    
    GetGeoManager();
    fGeoHandler = new CbmTrdGeoHandler();
  }
};


TEST_P(TrdGeoHandlerParamTest2, checkAllDifferentGeometries)
{
  Int_t retVal = fGeoHandler->Init();
  EXPECT_EQ(result, retVal);
  EXPECT_EQ(result, fGeoHandler->GetGeoVersion());

  std::vector<Int_t> stationId = fGeoHandler->GetStationId();
  std::vector<Int_t>::iterator vecIt;
  Int_t counter=0;
  for(vecIt = stationId.begin(); vecIt < stationId.end(); vecIt++){
    EXPECT_EQ(expStationId[counter], *vecIt);
    counter++;
  }

  std::vector< std::vector <Int_t> > moduleId = fGeoHandler->GetModuleId();
  std::vector< std::vector <Int_t> >::iterator vecIt1;
  counter=0;
  for(vecIt1 = moduleId.begin(); vecIt1 < moduleId.end(); vecIt1++){
    std::vector<Int_t> modInfo = *vecIt1;
    for(vecIt = modInfo.begin(); vecIt < modInfo.end(); vecIt++){
      EXPECT_EQ(expModuleId[counter], *vecIt);
      counter++;
    } 
  }
}

// Fill vector with expected volume ids for this geometry.
Int_t stationIdLength=3;
Int_t stationId[3]={218, 243, 268};
Int_t moduleIdLength=9;
Int_t moduleId[9]={219, 227, 235, 244, 252, 260, 0, 0, 269};

ParamStructure2 val1= {"squared_segmented", 4, stationIdLength, 
		       stationId, moduleIdLength, moduleId};



INSTANTIATE_TEST_CASE_P(TestAllGeometries,
                        TrdGeoHandlerParamTest2,
			::testing::Values(val1));		

