#include "CbmTrdGeoHandler.h"

#include "TFile.h"
#include "TGeoManager.h"
#include "TString.h"
#include "TSystem.h"
#include "TList.h"
#include "TKey.h"
#include "TVirtualMC.h"
//#include "TVirtualMCApplication.h"

#include "gtest/gtest.h"
#include "gtest/gtest-spi.h"
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <algorithm>
#include <iostream>
//#include <boost/regex.hpp>
using std::cout;
using std::endl;

// Structure to pass filenames together with expected response into the
// parametrized test
struct InOutStructure {
  TString name;
  int result;
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
  }

  virtual void CreateFileName(TString name) {
    fFileName = gSystem->Getenv("VMCWORKDIR");
    fFileName = fFileName + "/input/qa/trd_" + name + "_geometry.root";
  }

  virtual void GetGeoManager() {
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
    gGeoManager->CloseGeometry();   // close geometry
//    gMC = new TVirtualMCApplication();
//    gMC->SetRootGeometry();         // notify VMC about Root geometry
  }
};

// This is the derived class for the non-parameterized test cases.
class  TrdGeoHandlerTest : public _TestTrdGeoHandlerBase<testing::Test> {};

// This is the derived class for the parameterized test cases.
class TrdGeoHandlerParamTest : public _TestTrdGeoHandlerBase<
  testing::TestWithParam<InOutStructure> >
{
 protected:

  
  virtual void SetUp() {
    InOutStructure const& p = GetParam();

    geomToTest = p.name;
    result = p.result;
 
    CreateFileName(geomToTest);
    fInputFile = new TFile(fFileName,"READ");
    
    GetGeoManager();
    fGeoHandler = new CbmTrdGeoHandler();
  }
};

TEST_F(TrdGeoHandlerTest, CheckDefaultSettings)
{
  cout << "Filename: " << fFileName << endl;
  fInputFile->Print();  
  Int_t retVal = fGeoHandler->CheckGeometryVersion();
  cout << "Geo Version: " << retVal <<endl;
}

//TEST_F(TrdGeoHandlerTest, CheckExtractingMCVolumeId)
//{
//  char volumeName[10];
//  int stationNr=1; 
//  std::vector<int> fStationId;     //! MC IDs of TRD stations
//  do {
//    sprintf(volumeName, "trd%d", stationNr);
//    result = fGeoHandler->GetMCId(volumeName, fStationId);
//    cout << "Result: "<< result << endl;
//    stationNr++;
//  }
//  while (result); 
//}

TEST_P(TrdGeoHandlerParamTest, checkAllDifferentGeometries)
{
  cout << "Filename: " << fFileName << endl;
  //  fInputFile->Print();  
  cout<< "Expected result: "<< result << endl;
  Int_t retVal = fGeoHandler->CheckGeometryVersion();
  cout << "Geo Version: " << retVal <<endl;
  EXPECT_EQ(result, retVal);
}

InOutStructure val1= {"monolithic", 1};
InOutStructure val2= {"quasi_monolithic", 2};
InOutStructure val3= {"rectangular_segmented", 3};
InOutStructure val4= {"squared_segmented", 4};

INSTANTIATE_TEST_CASE_P(TestAllGeometries,
                        TrdGeoHandlerParamTest,
			::testing::Values(val1, val2, val3, val4));		
