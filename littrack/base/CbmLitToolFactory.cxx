#include "CbmLitToolFactory.h"

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
#include "CbmLitTrackFinderNN.h"
#include "CbmLitTrackFinderBranch.h"
#include "CbmLitTrackFinderWeight.h"
#include "CbmLitNearestHitToTrackMerger.h"
#include "CbmLitEnvironment.h"
#include "CbmLitPtrTypes.h"

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
		TrackPropagatorPtr propagator(new CbmLitTrackPropagatorImp(CreateTrackExtrapolator("lit")));
	    propagator->Initialize();
	    return propagator;
	} else
	if(name == "rk4") {
	   TrackPropagatorPtr propagator(new CbmLitTrackPropagatorImp(CreateTrackExtrapolator("rk4")));
	   propagator->Initialize();
	   return propagator;
	} else
	if(name == "line") {
	   TrackPropagatorPtr propagator(new CbmLitTrackPropagatorImp(CreateTrackExtrapolator("line")));
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
		//((CbmLitTrackPropagatorImp*) propagator)->IsCalcTransportMatrix(true);
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
		Int_t nofStations = CbmLitEnvironment::Instance()->GetMuchLayout().GetNofStationGroups();
		muchSelection->SetMinLastPlaneId(nofStations-1);
		Int_t nofLayers = CbmLitEnvironment::Instance()->GetMuchLayout().GetNofPlanes();
		muchSelection->SetMinNofHits(1);
		muchSelection->Initialize();
		TrackSelectionPtr selection(muchSelection);
		return selection;
	} else
	if(name == "much_pre_final") {
		CbmLitTrackSelectionD* muchSelection = new CbmLitTrackSelectionD();
		Int_t nofStations = CbmLitEnvironment::Instance()->GetMuchLayout().GetNofStationGroups();
		muchSelection->SetMinLastPlaneId(nofStations-1);
		Int_t nofLayers = CbmLitEnvironment::Instance()->GetMuchLayout().GetNofPlanes();
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
		trdFinderNN->SetLayout(CbmLitEnvironment::Instance()->GetTrdLayout());
		trdFinderNN->SetVerbose(1);
		trdFinderNN->SetNofIter(1);
		trdFinderNN->IsUseFastSearch(true);
		trdFinderNN->SetMaxNofMissingHits(4);
		trdFinderNN->SetSigmaCoef(7.);
		trdFinderNN->SetChiSqPixelHitCut(20.);
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
		trdFinderBranch->SetFinalPreSelection(CreateTrackSelection("empty"));
		trdFinderBranch->SetFilter(CreateTrackUpdate("kalman"));
		trdFinderBranch->SetFitter(CreateTrackFitter("lit_kalman"));
		trdFinderBranch->SetLayout(CbmLitEnvironment::Instance()->GetTrdLayout());
		trdFinderBranch->SetVerbose(1);
		trdFinderBranch->SetNofIter(1);
		trdFinderBranch->IsUseFastSearch(true);
		trdFinderBranch->SetBeginStationGroup(0);
		trdFinderBranch->SetEndStationGroup(CbmLitEnvironment::Instance()->GetTrdLayout().GetNofStationGroups() - 1);
		trdFinderBranch->SetPDG(11);
		trdFinderBranch->SetMaxNofMissingHits(4);
		trdFinderBranch->IsAlwaysCreateMissingHit(false);
		trdFinderBranch->SetSigmaCoef(10.);
		trdFinderBranch->SetChiSqPixelHitCut(20.);//15.84;
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
		trdFinderWeight->SetLayout(CbmLitEnvironment::Instance()->GetTrdLayout());
		trdFinderWeight->SetVerbose(1);
		trdFinderWeight->SetNofIter(1);
		trdFinderWeight->SetMaxNofMissingHits(0);
		trdFinderWeight->IsUseFastSearch(true);
		trdFinderWeight->SetSigmaCoef(10.);
		trdFinderWeight->SetChiSqPixelHitCut(20.);
		trdFinderWeight->SetChiSqStripHitCut(4.);
		trdFinderWeight->SetPDG(11);
		TrackFinderPtr finder(trdFinderWeight);
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

ClassImp(CbmLitToolFactory)
