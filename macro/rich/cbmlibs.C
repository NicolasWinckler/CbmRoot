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
	  gSystem->Load("libMvd");
	  gSystem->Load("libSts");
	  gSystem->Load("libRich");
	  gSystem->Load("libTrd");
	  gSystem->Load("libTof");
	  gSystem->Load("libEcal");
	  gSystem->Load("libGlobal");
	  gSystem->Load("libKF");
	  gSystem->Load("libL1");
	  gSystem->Load("libLittrack");

	  gSystem->Load("libMinuit2"); // Nedded for rich ellipse fitter
}
