#include "CbmLitToolFactory.h"

#include "CbmLitTrackPropagator.h"
#include "CbmLitTrackExtrapolator.h"
#include "CbmLitTrackUpdate.h"
#include "CbmLitTrackFitter.h"
#include "CbmLitTrackSelection.h"

#include "CbmLitRK4TrackExtrapolator.h"
#include "CbmLitLineTrackExtrapolator.h"
#include "CbmLitCleverTrackExtrapolator.h"
#include "CbmLitTrackPropagatorImp.h"
#include "CbmLitTrackPropagatorGeane.h"
#include "CbmLitTrackSelectionEmpty.h"
#include "CbmLitTrackSelectionChiSq.h"
#include "CbmLitTrackSelectionMuch.h"
#include "CbmLitTrackSelectionTrd.h"
#include "CbmLitTrackSelectionMomentum.h"
#include "CbmLitTrackSelectionMuchRobust.h"
#include "CbmLitTrackSelectionD.h"
#include "CbmLitKalmanFilter.h"
#include "CbmLitKalmanSmoother.h"
#include "CbmLitTrackFitterImp.h"
#include "CbmLitTrackFitterRobust.h"
#include "CbmLitTrackFitterIter.h"
#include "CbmLitEnvironment.h"

//#include <boost/tr1/memory.hpp>

//using  std::tr1;

CbmLitToolFactory* CbmLitToolFactory::fInstance = NULL;

CbmLitToolFactory::CbmLitToolFactory()
{
//	std::tr1::shared_ptr<int> p(new int(4));
//	boost::shared_ptr<int> p(new int(4));
}

CbmLitToolFactory::~CbmLitToolFactory()
{
	if (fInstance != NULL) delete fInstance;
}

CbmLitToolFactory* CbmLitToolFactory::Instance()
{
   if (fInstance == NULL) {
      fInstance = new CbmLitToolFactory();
   }
   return fInstance;
}

CbmLitTrackExtrapolator* CbmLitToolFactory::CreateTrackExtrapolator(
		const std::string& name)
{
	CbmLitTrackExtrapolator* extrapolator = NULL;
	if(name == "line") {
		extrapolator = new CbmLitLineTrackExtrapolator();
		extrapolator->Initialize();
	} else
	if (name == "rk4") {
		extrapolator = new CbmLitRK4TrackExtrapolator();
		extrapolator->Initialize();
	} else
	if (name == "lit") {
		extrapolator = new CbmLitCleverTrackExtrapolator();
		extrapolator->Initialize();
	}
	return extrapolator;
}

CbmLitTrackPropagator* CbmLitToolFactory::CreateTrackPropagator(
		const std::string& name)
{
	CbmLitTrackPropagator* propagator = NULL;
	if(name == "geane") {
		propagator = new CbmLitTrackPropagatorGeane();
	    propagator->Initialize();
	} else
	if(name == "lit") {
		propagator = new CbmLitTrackPropagatorImp(CreateTrackExtrapolator("lit"));
	    propagator->Initialize();
	} else
	if(name == "rk4") {
	   CbmLitTrackPropagatorImp* litpropagator = new CbmLitTrackPropagatorImp(CreateTrackExtrapolator("rk4"));
	   litpropagator->Initialize();
	   return litpropagator;
	} else
	if(name == "line") {
	   CbmLitTrackPropagatorImp* litpropagator = new CbmLitTrackPropagatorImp(CreateTrackExtrapolator("line"));
	   litpropagator->Initialize();
	   return litpropagator;
	}
	return propagator;
}

CbmLitTrackUpdate* CbmLitToolFactory::CreateTrackUpdate(
		const std::string& name)
{
	CbmLitTrackUpdate* update = NULL;
	if(name == "kalman") {
		update = new CbmLitKalmanFilter();
		update->Initialize();
		return update;
	}
	return update;
}

CbmLitTrackFitter* CbmLitToolFactory::CreateTrackFitter(
		const std::string& name)
{
	CbmLitTrackFitter* fitter = NULL;

	if(name == "lit_kalman") {
		CbmLitTrackPropagator* propagator = CreateTrackPropagator("lit");
		((CbmLitTrackPropagatorImp*) propagator)->IsCalcTransportMatrix(true);
		CbmLitTrackUpdate* update = CreateTrackUpdate("kalman");
		fitter = new CbmLitTrackFitterImp(propagator, update);
		return fitter;
	} else
	if(name == "geane_kalman") {
		CbmLitTrackPropagator* propagator = CreateTrackPropagator("geane");
		CbmLitTrackUpdate* update = CreateTrackUpdate("kalman");
		fitter = new CbmLitTrackFitterImp(propagator, update);
		return fitter;
	} else
	if (name == "kalman_smoother") {
		fitter = new CbmLitKalmanSmoother();
		return fitter;
	} else
	if (name == "kalman_robust") {
		CbmLitTrackFitter* fitter = CreateTrackFitter("lit_kalman");
		CbmLitTrackFitter* smoother = CreateTrackFitter("kalman_smoother");
		fitter = new CbmLitTrackFitterRobust(fitter, smoother);
		return fitter;
	}

	return fitter;
}

CbmLitTrackSelection* CbmLitToolFactory::CreateTrackSelection(
		const std::string& name)
{
	CbmLitTrackSelection* selection = NULL;
	if(name == "empty") {
		selection = new CbmLitTrackSelectionEmpty();
		selection->Initialize();
		return selection;
	} else
	if(name == "momentum") {
		CbmLitTrackSelectionMomentum* momSelection = new CbmLitTrackSelectionMomentum();
		momSelection->SetMinMomentum(0.1);
		momSelection->Initialize();
		return momSelection;
	} else
	if(name == "momentum_seed") {
		CbmLitTrackSelectionMomentum* momSelection = new CbmLitTrackSelectionMomentum();
		momSelection->SetMinMomentum(1.5);
		momSelection->Initialize();
		return momSelection;
	} else
	if(name == "chi_square") {
		CbmLitTrackSelectionChiSq* chiSqSelection = new CbmLitTrackSelectionChiSq();
		chiSqSelection->SetMaxChiSq(30);
		chiSqSelection->Initialize();
		return chiSqSelection;
	}
	if(name == "much_final") {
		CbmLitTrackSelectionMuch* muchSelection = new CbmLitTrackSelectionMuch();
		muchSelection->SetNofSharedHits(2);
		Int_t nofStations = CbmLitEnvironment::Instance()->GetMuchLayout().GetNofStationGroups();
		muchSelection->SetMinLastPlaneId(nofStations-1);
		Int_t nofLayers = CbmLitEnvironment::Instance()->GetMuchLayout().GetNofPlanes();
		muchSelection->SetMinNofHits(1);
		muchSelection->Initialize();
		return muchSelection;
	} else
	if(name == "much_pre_final") {
		CbmLitTrackSelectionD* muchSelection = new CbmLitTrackSelectionD();
		Int_t nofStations = CbmLitEnvironment::Instance()->GetMuchLayout().GetNofStationGroups();
		muchSelection->SetMinLastPlaneId(nofStations-1);
		Int_t nofLayers = CbmLitEnvironment::Instance()->GetMuchLayout().GetNofPlanes();
		muchSelection->SetMinNofHits(nofLayers - 3);
		muchSelection->Initialize();
		return muchSelection;
	} else
	if(name == "trd_station") {
		CbmLitTrackSelectionTrd* trdSelection = new CbmLitTrackSelectionTrd();
		trdSelection->SetNofSharedHits(2);
		trdSelection->SetMinNofHits(2);
		trdSelection->SetMinLastPlaneId(0);
		trdSelection->Initialize();
		return trdSelection;
	} else
	if(name == "trd_final") {
		CbmLitTrackSelectionTrd* trdSelection = new CbmLitTrackSelectionTrd();
		trdSelection->SetNofSharedHits(2);
		trdSelection->SetMinNofHits(2);
		Int_t nofStationGroups = CbmLitEnvironment::Instance()->GetTrdLayout().GetNofStationGroups();
		trdSelection->SetMinLastPlaneId(nofStationGroups-1);
		trdSelection->Initialize();
		return trdSelection;
	}
	return selection;
}

ClassImp(CbmLitToolFactory)
