#include "CbmLitToolFactory.h"

#include "CbmLitRK4TrackExtrapolator.h"
#include "CbmLitLineTrackExtrapolator.h"
#include "CbmLitCleverTrackExtrapolator.h"
#include "CbmLitTGeoTrackPropagator.h"
#include "CbmLitMyTrackPropagator.h"
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
#include "CbmLitTrackFinderNN.h"
#include "CbmLitTrackFinderBranch.h"
#include "CbmLitTrackFinderWeight.h"
#include "CbmLitNearestHitToTrackMerger.h"
#include "CbmLitEnvironment.h"
#include "CbmLitPtrTypes.h"
//#include "CbmLitTrackFinderNNParallel.h"


CbmLitToolFactory* CbmLitToolFactory::fInstance = NULL;

CbmLitToolFactory::CbmLitToolFactory()
{
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

TrackExtrapolatorPtr CbmLitToolFactory::CreateTrackExtrapolator(
		const std::string& name)
{
	//TrackExtrapolatorPtr extrapolator = NULL;
	if(name == "line") {
		TrackExtrapolatorPtr extrapolator(new CbmLitLineTrackExtrapolator());
		extrapolator->Initialize();
		return extrapolator;
	} else
	if (name == "rk4") {
		TrackExtrapolatorPtr extrapolator(new CbmLitRK4TrackExtrapolator());
		extrapolator->Initialize();
		return extrapolator;
	} else
	if (name == "lit") {
		TrackExtrapolatorPtr extrapolator(new CbmLitCleverTrackExtrapolator());
		extrapolator->Initialize();
		return extrapolator;
	}
//	return TrackExtrapolatorPtr();
}

TrackPropagatorPtr CbmLitToolFactory::CreateTrackPropagator(
		const std::string& name)
{
//	CbmLitTrackPropagator* propagator = NULL;
	if(name == "geane") {
		TrackPropagatorPtr propagator(new CbmLitTrackPropagatorGeane());
	    propagator->Initialize();
	    return propagator;
	} else
	if(name == "lit") {
		TrackPropagatorPtr propagator(new CbmLitTGeoTrackPropagator(CreateTrackExtrapolator("lit")));
	    propagator->Initialize();
	    return propagator;
	} else
	if(name == "mylit") {
		TrackPropagatorPtr propagator(new CbmLitMyTrackPropagator(CreateTrackExtrapolator("lit")));
		propagator->Initialize();
		return propagator;
	} else
	if(name == "rk4") {
	   TrackPropagatorPtr propagator(new CbmLitTGeoTrackPropagator(CreateTrackExtrapolator("rk4")));
	   propagator->Initialize();
	   return propagator;
	} else
	if(name == "line") {
	   TrackPropagatorPtr propagator(new CbmLitTGeoTrackPropagator(CreateTrackExtrapolator("line")));
	   propagator->Initialize();
	   return propagator;
	}
//	return propagator;
}
TrackUpdatePtr CbmLitToolFactory::CreateTrackUpdate(
		const std::string& name)
{
//	CbmLitTrackUpdate* update = NULL;
	if(name == "kalman") {
		TrackUpdatePtr update(new CbmLitKalmanFilter());
		update->Initialize();
		return update;
	}
//	return update;
}

TrackFitterPtr CbmLitToolFactory::CreateTrackFitter(
		const std::string& name)
{
//	CbmLitTrackFitter* fitter = NULL;
	if(name == "lit_kalman") {
		TrackPropagatorPtr propagator = CreateTrackPropagator("lit");
		//((CbmLitTGeoTrackPropagator*) propagator)->IsCalcTransportMatrix(true);
		TrackUpdatePtr update = CreateTrackUpdate("kalman");
		TrackFitterPtr fitter(new CbmLitTrackFitterImp(propagator, update));
		return fitter;
	} else
	if(name == "geane_kalman") {
		TrackPropagatorPtr propagator = CreateTrackPropagator("geane");
		TrackUpdatePtr update = CreateTrackUpdate("kalman");
		TrackFitterPtr fitter(new CbmLitTrackFitterImp(propagator, update));
		return fitter;
	} else
	if (name == "kalman_smoother") {
		TrackFitterPtr fitter(new CbmLitKalmanSmoother());
		return fitter;
	} else
	if (name == "kalman_robust") {
		TrackFitterPtr fitter = CreateTrackFitter("lit_kalman");
		TrackFitterPtr smoother = CreateTrackFitter("kalman_smoother");
		TrackFitterPtr rfitter(new CbmLitTrackFitterRobust(fitter, smoother));
		return rfitter;
	} else
	if (name == "mylit_kalman") {
		TrackPropagatorPtr propagator = CreateTrackPropagator("mylit");
		//((CbmLitTGeoTrackPropagator*) propagator)->IsCalcTransportMatrix(true);
		TrackUpdatePtr update = CreateTrackUpdate("kalman");
		TrackFitterPtr fitter(new CbmLitTrackFitterImp(propagator, update));
		return fitter;
	}
//	return fitter;
}

TrackSelectionPtr CbmLitToolFactory::CreateTrackSelection(
		const std::string& name)
{
//	CbmLitTrackSelection* selection = NULL;
	if(name == "empty") {
		TrackSelectionPtr selection(new CbmLitTrackSelectionEmpty());
		selection->Initialize();
		return selection;
	} else
	if(name == "momentum") {
		//TrackSelectionPtr selection(new CbmLitTrackSelectionMomentum());
		CbmLitTrackSelectionMomentum* momSelection = new CbmLitTrackSelectionMomentum();
		momSelection->SetMinMomentum(0.1);
		momSelection->Initialize();
		TrackSelectionPtr selection(momSelection);
		return selection;
	} else
	if(name == "momentum_seed") {
		CbmLitTrackSelectionMomentum* momSelection = new CbmLitTrackSelectionMomentum();
		momSelection->SetMinMomentum(1.5);
		momSelection->Initialize();
		TrackSelectionPtr selection(momSelection);
		return selection;
	} else
	if(name == "chi_square") {
		CbmLitTrackSelectionChiSq* chiSqSelection = new CbmLitTrackSelectionChiSq();
		chiSqSelection->SetMaxChiSq(30);
		chiSqSelection->Initialize();
		TrackSelectionPtr selection(chiSqSelection);
		return selection;
	}
	if(name == "much_final") {
		CbmLitTrackSelectionMuch* muchSelection = new CbmLitTrackSelectionMuch();
		muchSelection->SetNofSharedHits(2);
//		Int_t nofStations = CbmLitEnvironment::Instance()->GetLayout().GetNofStationGroups();
//		muchSelection->SetMinLastPlaneId(nofStations-1);
//		Int_t nofLayers = CbmLitEnvironment::Instance()->GetLayout().GetNofPlanes();
		muchSelection->SetMinNofHits(1);
		muchSelection->Initialize();
		TrackSelectionPtr selection(muchSelection);
		return selection;
	} else
	if(name == "much_pre_final") {
		CbmLitTrackSelectionD* muchSelection = new CbmLitTrackSelectionD();
		Int_t nofStations = CbmLitEnvironment::Instance()->GetLayout().GetNofStationGroups();
		muchSelection->SetMinLastPlaneId(nofStations-1);
		Int_t nofLayers = CbmLitEnvironment::Instance()->GetLayout().GetNofPlanes();
		muchSelection->SetMinNofHits(nofLayers - 3);
		muchSelection->Initialize();
		TrackSelectionPtr selection(muchSelection);
		return selection;
	} else
	if(name == "trd_station") {
		CbmLitTrackSelectionTrd* trdSelection = new CbmLitTrackSelectionTrd();
		trdSelection->SetNofSharedHits(2);
		trdSelection->SetMinNofHits(2);
		trdSelection->SetMinLastPlaneId(0);
		trdSelection->Initialize();
		TrackSelectionPtr selection(trdSelection);
		return selection;
	} else
	if(name == "trd_final") {
		CbmLitTrackSelectionTrd* trdSelection = new CbmLitTrackSelectionTrd();
		trdSelection->SetNofSharedHits(2);
		trdSelection->SetMinNofHits(6);
		//Int_t nofStationGroups = CbmLitEnvironment::Instance()->GetTrdLayout().GetNofStationGroups();
		trdSelection->SetMinLastPlaneId(0);//nofStationGroups-1);
		trdSelection->Initialize();
		TrackSelectionPtr selection(trdSelection);
		return selection;
	}
//	return selection;
}

TrackFinderPtr CbmLitToolFactory::CreateTrackFinder(
		const std::string& name)
{
	if(name == "e_nn") {
		CbmLitTrackFinderNN* trdFinderNN = new CbmLitTrackFinderNN();
		trdFinderNN->SetPropagator(CreateTrackPropagator("lit"));
		trdFinderNN->SetSeedSelection(CreateTrackSelection("momentum"));
		trdFinderNN->SetFinalSelection(CreateTrackSelection("trd_final"));
		trdFinderNN->SetFilter(CreateTrackUpdate("kalman"));
		trdFinderNN->SetLayout(CbmLitEnvironment::Instance()->GetLayout());
		trdFinderNN->SetVerbose(1);
		trdFinderNN->SetNofIter(1);
		trdFinderNN->IsUseFastSearch(true);
		trdFinderNN->SetMaxNofMissingHits(4);
		trdFinderNN->SetSigmaCoef(7.);
		trdFinderNN->SetChiSqPixelHitCut(25.);
		trdFinderNN->SetChiSqStripHitCut(4.);
		trdFinderNN->SetPDG(11);
		trdFinderNN->Initialize();
		TrackFinderPtr finder(trdFinderNN);
		return finder;
	} else
	if(name == "e_branch") {
		CbmLitTrackFinderBranch* trdFinderBranch = new CbmLitTrackFinderBranch();
		trdFinderBranch->SetPropagator(CreateTrackPropagator("lit"));
		trdFinderBranch->SetSeedSelection(CreateTrackSelection("momentum"));
		trdFinderBranch->SetStationGroupSelection(CreateTrackSelection("trd_station"));
		trdFinderBranch->SetFinalSelection(CreateTrackSelection("trd_final"));
		trdFinderBranch->SetFilter(CreateTrackUpdate("kalman"));
		trdFinderBranch->SetLayout(CbmLitEnvironment::Instance()->GetLayout());
		trdFinderBranch->SetVerbose(1);
		trdFinderBranch->SetNofIter(1);
		trdFinderBranch->IsUseFastSearch(true);
		trdFinderBranch->SetPDG(11);
		trdFinderBranch->SetMaxNofMissingHits(4);
		trdFinderBranch->IsAlwaysCreateMissingHit(false);
		trdFinderBranch->SetSigmaCoef(10.);
		trdFinderBranch->SetChiSqPixelHitCut(25.);//15.84;
		trdFinderBranch->SetChiSqStripHitCut(4.);
		TrackFinderPtr finder(trdFinderBranch);
		return finder;
	} else
	if(name == "e_weight") {
		CbmLitTrackFinderWeight* trdFinderWeight = new CbmLitTrackFinderWeight();
		trdFinderWeight->SetPropagator(CreateTrackPropagator("lit"));
		trdFinderWeight->SetSeedSelection(CreateTrackSelection("momentum"));
		trdFinderWeight->SetFinalSelection(CreateTrackSelection("empty"));
		trdFinderWeight->SetFitter(CreateTrackFitter("kalman_robust"));
		trdFinderWeight->SetFilter(CreateTrackUpdate("kalman"));
		trdFinderWeight->SetLayout(CbmLitEnvironment::Instance()->GetLayout());
		trdFinderWeight->SetVerbose(1);
		trdFinderWeight->SetNofIter(1);
		trdFinderWeight->SetMaxNofMissingHits(0);
		trdFinderWeight->IsUseFastSearch(true);
		trdFinderWeight->SetSigmaCoef(10.);
		trdFinderWeight->SetChiSqPixelHitCut(25.);
		trdFinderWeight->SetChiSqStripHitCut(4.);
		trdFinderWeight->SetPDG(11);
		TrackFinderPtr finder(trdFinderWeight);
		return finder;
	} else
	if(name == "mu_nn") {
		CbmLitTrackFinderNN* muchFinderNN = new CbmLitTrackFinderNN();
		muchFinderNN->SetPropagator(CreateTrackPropagator("lit"));
		muchFinderNN->SetSeedSelection(CreateTrackSelection("momentum_seed"));
		muchFinderNN->SetFinalSelection(CreateTrackSelection("much_final"));
		muchFinderNN->SetFilter(CreateTrackUpdate("kalman"));
		muchFinderNN->SetLayout(CbmLitEnvironment::Instance()->GetLayout());
		muchFinderNN->SetVerbose(1);
		muchFinderNN->SetNofIter(1);
		muchFinderNN->IsUseFastSearch(true);
		muchFinderNN->SetMaxNofMissingHits(2);
		muchFinderNN->SetSigmaCoef(3.5);
		muchFinderNN->SetChiSqPixelHitCut(13.86);
		muchFinderNN->SetChiSqStripHitCut(4.);
		muchFinderNN->SetPDG(13);
		muchFinderNN->Initialize();
		TrackFinderPtr finder(muchFinderNN);
		return finder;
	} else
	if(name == "mu_branch") {
		CbmLitTrackFinderBranch* muchFinderBranch = new CbmLitTrackFinderBranch();
		muchFinderBranch->SetPropagator(CreateTrackPropagator("lit"));
		muchFinderBranch->SetSeedSelection(CreateTrackSelection("momentum_seed"));
		muchFinderBranch->SetStationGroupSelection(CreateTrackSelection("momentum"));
		muchFinderBranch->SetFinalSelection(CreateTrackSelection("much_final"));
		muchFinderBranch->SetFilter(CreateTrackUpdate("kalman"));
		muchFinderBranch->SetLayout(CbmLitEnvironment::Instance()->GetLayout());
		muchFinderBranch->SetVerbose(1);
		muchFinderBranch->SetNofIter(1);
		muchFinderBranch->IsUseFastSearch(true);
		muchFinderBranch->SetPDG(13);
		muchFinderBranch->SetMaxNofMissingHits(2);
		muchFinderBranch->IsAlwaysCreateMissingHit(false);
		muchFinderBranch->SetSigmaCoef(3.5);
		muchFinderBranch->SetChiSqPixelHitCut(13.86);
		muchFinderBranch->SetChiSqStripHitCut(4.);
		TrackFinderPtr finder(muchFinderBranch);
		return finder;
	} else
	if(name == "mu_weight") {
		CbmLitTrackFinderWeight* muchFinderWeight = new CbmLitTrackFinderWeight();
		muchFinderWeight->SetPropagator(CreateTrackPropagator("lit"));
		muchFinderWeight->SetSeedSelection(CreateTrackSelection("momentum_seed"));
		muchFinderWeight->SetFinalSelection(CreateTrackSelection("empty"));
		muchFinderWeight->SetFitter(CreateTrackFitter("kalman_robust"));
		muchFinderWeight->SetFilter(CreateTrackUpdate("kalman"));
		muchFinderWeight->SetLayout(CbmLitEnvironment::Instance()->GetLayout());
		muchFinderWeight->SetVerbose(1);
		muchFinderWeight->SetNofIter(1);
		muchFinderWeight->SetMaxNofMissingHits(2);
		muchFinderWeight->IsUseFastSearch(true);
		muchFinderWeight->SetSigmaCoef(3.5);
		muchFinderWeight->SetChiSqPixelHitCut(13.86);
		muchFinderWeight->SetChiSqStripHitCut(4.);
		muchFinderWeight->SetPDG(13);
		TrackFinderPtr finder(muchFinderWeight);
		return finder;
	}
}

HitToTrackMergerPtr CbmLitToolFactory::CreateHitToTrackMerger(
		const std::string& name)
{
	if(name == "tof_nearest_hit") {
		CbmLitNearestHitToTrackMerger* nhMerger = new CbmLitNearestHitToTrackMerger();
		nhMerger->SetPropagator(CreateTrackPropagator("lit"));
		nhMerger->SetFilter(CreateTrackUpdate("kalman"));
		nhMerger->SetPDG(11);
		nhMerger->SetStation(CbmLitEnvironment::Instance()->GetTofStation());
		nhMerger->Initialize();
		HitToTrackMergerPtr merger(nhMerger);
		return merger;
	} else
	if(name == "") {

//		return finder;
	}
}
