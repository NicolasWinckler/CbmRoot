#include "CbmEcalAnalysisSimple.h"

#include <iostream>
#include <fstream>

using namespace std;

/** Loop procedure **/
void CbmEcalAnalysisSimple::Exec(Option_t* option)
{
  fEv++;
}


CbmEcalAnalysisSimple::CbmEcalAnalysisSimple(const char* name, const Int_t iVerbose, const char* fname)
  : FairTask(name, iVerbose)
{
  fEv=0;
  fName=fname;
}

/** Initing routine **/
InitStatus CbmEcalAnalysisSimple::Init()
{
  return kSUCCESS;
}

/** Finishing routine **/
void CbmEcalAnalysisSimple::Finish()
{
  ofstream f(fName);
  f << fEv << endl;
  f.close();
}

ClassImp(CbmEcalAnalysisSimple)
