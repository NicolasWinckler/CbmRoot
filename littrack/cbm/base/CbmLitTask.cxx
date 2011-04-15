/* CbmLitTask.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2010
 * @version 1.0.
 */
#include "base/CbmLitTask.h"
#include "base/CbmLitEnvironment.h"

#include <iostream>

CbmLitTask::CbmLitTask():
   fIsElectronSetup(false),
   fIsSts(false),
   fIsTrd(false),
   fIsMuch(false),
   fIsTof(false),
   fEventNo(0),
   fVerbose(0),
   fOutputDir("")
{
}

CbmLitTask::~CbmLitTask()
{
}


void CbmLitTask::DetermineSetup()
{
   CbmLitEnvironment* env = CbmLitEnvironment::Instance();
   fIsElectronSetup = env->IsElectronSetup();
   fIsSts = env->IsSts();
   fIsTrd = env->IsTrd();
   fIsMuch = env->IsMuch();
   fIsTof = env->IsTof();

   if (fIsElectronSetup) { std::cout << "-I- CBM electron setup detected" << std::endl; }
   else { std::cout << "-I- CBM muon setup detected" << std::endl; }
   std::cout << "-I- The following detectors were found in the CBM setup:" << std::endl;
   if (fIsSts) { std::cout << "STS" << std::endl; }
   if (fIsTrd) { std::cout << "TRD" << std::endl; }
   if (fIsMuch) { std::cout << "MUCH" << std::endl; }
   if (fIsTof) { std::cout << "TOF" << std::endl; }
}

ClassImp(CbmLitTask);
