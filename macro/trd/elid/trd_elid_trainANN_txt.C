/*  Description: This macro train the ANN for electron
 identification in the TRD.

 Author : Semen Lebedev
 E-mail : S.Lebedev@gsi.de
 */
#include <vector>

void trd_elid_trainANN_txt()
{

   // ----  Load libraries   -------------------------------------------------
   gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
   basiclibs();
   gROOT->LoadMacro("$VMCWORKDIR/macro/rich/cbmlibs.C");
   cbmlibs();
   gSystem->Load("libTMVA");

   int nofTrdLayers = 12;

  // nofTrdLayers = TString(gSystem->Getenv("NOF_TRD_LAYERS")).Atoi();

   std::stringstream ss;
   ss << "results/" <<nofTrdLayers<<"/";
	CbmTrdElectronsTrainAnn* trainer = new CbmTrdElectronsTrainAnn(nofTrdLayers);
	trainer->SetOutputDir(ss.str());
   //kANN = 0, kBDT = 1, kMEDIANA = 2, kLIKELIHOOD = 3, kMeanCut = 4
   trainer->SetIdMethod(4);
   trainer->SetTransformType(2);
   trainer->SetIsDoTrain(true);
   trainer->SetNofTrainSamples(2500);
   trainer->SetSigmaError(0.0);
	trainer->RunReal();
}
