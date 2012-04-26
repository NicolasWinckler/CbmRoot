void cbmlibs()
{

	  //gSystem->Load("/u/slebedev/Baykal/tbb21_017oss/libtbb");
	  //gSystem->Load("/u/andrey/soft/tbb/Etch32/libtbb");
	  //gSystem->Load("/u/andrey/soft/tbb/Lenny64/libtbb");

	  gSystem->Load("libGeoBase");
	  gSystem->Load("libParBase");
	  gSystem->Load("libBase");
	  gSystem->Load("libCbmBase");
	  gSystem->Load("libCbmData");
	  gSystem->Load("libField");
	  gSystem->Load("libGen");
	  gSystem->Load("libPassive");
	  gSystem->Load("libEcal");
	  gSystem->Load("libSts");
	  gSystem->Load("libKF");
	  gSystem->Load("libMvd");
     gSystem->Load("libLittrack");
	  gSystem->Load("libRich");
	  gSystem->Load("libTrd");
	  gSystem->Load("libTof");
	  gSystem->Load("libGlobal");
	  gSystem->Load("libL1");
	  //gSystem->Load("libLittrack");
     gSystem->Load("libMuch");

	  gSystem->Load("libMinuit2"); // Nedded for rich ellipse fitter
}
