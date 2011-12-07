/**
 * \file cbmrootlibs.C
 * \brief Macro loads CBMROOT libraries.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 */

void cbmrootlibs()
{
   gSystem->Load("libGeoBase");
   gSystem->Load("libParBase");
   gSystem->Load("libBase");
   gSystem->Load("libCbmBase");
   gSystem->Load("libCbmData");
   gSystem->Load("libCbmGenerators");
   gSystem->Load("libField");
   gSystem->Load("libGen");
   gSystem->Load("libPassive");
   gSystem->Load("libTrkBase");
   gSystem->Load("libGeane");
   gSystem->Load("libMvd");
   gSystem->Load("libSts");
   gSystem->Load("libRich");
   gSystem->Load("libMuch");
   gSystem->Load("libTrd");
   gSystem->Load("libTof");
   gSystem->Load("libEcal");
   gSystem->Load("libGlobal");
   gSystem->Load("libKF");
   gSystem->Load("libL1");
//   gSystem->Load("libcudalittrack");
   gSystem->Load("libLittrack");
   gSystem->Load("libLittrackparallel");
   gSystem->Load("libMinuit2"); // Needed for field fitter
}
