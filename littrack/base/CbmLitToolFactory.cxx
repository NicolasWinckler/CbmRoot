/** CbmLitToolFactory.cxx
 *@author A.Lebedev <andrey.lebedev@gsi.de>
 *@since 2008
 **/

#include "base/CbmLitToolFactory.h"

#include "base/CbmLitMapField.h"
#include "base/CbmLitEnvironment.h"
#include "base/CbmLitPtrTypes.h"
#include "base/CbmLitTrackFinderSettings.h"
#include "finder/CbmLitTrackFinderNN.h"
#include "finder/CbmLitTrackFinderBranch.h"
#include "finder/CbmLitTrackFinderWeight.h"
#include "finder/CbmLitNearestHitToTrackMerger.h"
#include "finder/CbmLitTrackFinderNNParallel.h"
#include "fitter/CbmLitKalmanFilter.h"
#include "fitter/CbmLitKalmanSmoother.h"
#include "fitter/CbmLitTrackFitterImp.h"
#include "fitter/CbmLitTrackFitterWeight.h"
#include "fitter/CbmLitTrackFitterIter.h"
#include "interface/CbmLitField.h"
#include "propagation/CbmLitRK4TrackExtrapolator.h"
#include "propagation/CbmLitLineTrackExtrapolator.h"
#include "propagation/CbmLitCleverTrackExtrapolator.h"
#include "propagation/CbmLitTGeoTrackPropagator.h"
#include "propagation/CbmLitMyTrackPropagator.h"
#include "propagation/CbmLitTrackPropagatorGeane.h"
#include "selection/CbmLitTrackSelectionEmpty.h"
#include "selection/CbmLitTrackSelectionCuts.h"
#include "selection/CbmLitTrackSelectionMuch.h"
#include "selection/CbmLitTrackSelectionTrd.h"
#include "selection/CbmLitTrackSelectionMuchRobust.h"
#include "qa/CbmLitParallelTrackFitterTest.h"
#include "qa/CbmLitParallelTrackFitterTestElectron.h"

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
		CbmLitEnvironment* env = CbmLitEnvironment::Instance();
		CbmLitField* field = new CbmLitMapField(env->GetField());
		TrackExtrapolatorPtr extrapolator(new CbmLitRK4TrackExtrapolator(field));
		extrapolator->Initialize();
		return extrapolator;
	} else
	if (name == "lit") {
		TrackExtrapolatorPtr extrapolator(new CbmLitCleverTrackExtrapolator(""));
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
		TrackFitterPtr rfitter(new CbmLitTrackFitterWeight(fitter, smoother));
		return rfitter;
	} else
	if (name == "mylit_kalman") {
		TrackPropagatorPtr propagator = CreateTrackPropagator("mylit");
		//((CbmLitTGeoTrackPropagator*) propagator)->IsCalcTransportMatrix(true);
		TrackUpdatePtr update = CreateTrackUpdate("kalman");
		TrackFitterPtr fitter(new CbmLitTrackFitterImp(propagator, update));
		return fitter;
	} else
	if (name == "kalman_parallel_muon") {
		TrackFitterPtr fitter(new CbmLitParallelTrackFitterTest());
		return fitter;
	} else
		if (name == "kalman_parallel_electron") {
		TrackFitterPtr fitter(new CbmLitParallelTrackFitterTestElectron());
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
		CbmLitTrackSelectionCuts* momSelection = new CbmLitTrackSelectionCuts();
		momSelection->SetMinMomentum(0.1);
		momSelection->Initialize();
		TrackSelectionPtr selection(momSelection);
		return selection;
	} else
	if(name == "momentum_seed") {
		CbmLitTrackSelectionCuts* momSelection = new CbmLitTrackSelectionCuts();
		momSelection->SetMinMomentum(1.0);
		momSelection->Initialize();
		TrackSelectionPtr selection(momSelection);
		return selection;
	} else
	if(name == "chi_square") {
		CbmLitTrackSelectionCuts* chiSqSelection = new CbmLitTrackSelectionCuts();
		chiSqSelection->SetMaxChiSq(30.);
		chiSqSelection->Initialize();
		TrackSelectionPtr selection(chiSqSelection);
		return selection;
	} else
	if(name == "much_final") {
		CbmLitTrackSelectionMuch* muchSelection = new CbmLitTrackSelectionMuch();
		muchSelection->SetNofSharedHits(3);
		muchSelection->SetMinNofHits(1);
		muchSelection->SetMinLastPlaneId(0);
		muchSelection->Initialize();
		TrackSelectionPtr selection(muchSelection);
		return selection;
	} else
	if(name == "trd_station") {
		CbmLitTrackSelectionTrd* trdSelection = new CbmLitTrackSelectionTrd();
		trdSelection->SetNofSharedHits(2);
		trdSelection->Initialize();
		TrackSelectionPtr selection(trdSelection);
		return selection;
	} else
	if(name == "trd_final") {
		CbmLitTrackSelectionTrd* trdSelection = new CbmLitTrackSelectionTrd();
		trdSelection->SetNofSharedHits(3);
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
		CbmLitTrackFinderSettings settings;
		settings.SetNofIter(1);
		settings.SetPropagator(CreateTrackPropagator("lit"));
		settings.SetSeedSelection(CreateTrackSelection("momentum"));
		settings.SetFinalSelection(CreateTrackSelection("trd_final"));
		settings.SetFilter(CreateTrackUpdate("kalman"));
		settings.IsUseFastSearch(true);
		settings.SetMaxNofMissingHits(4);
		settings.SetSigmaCoef(5.);
		settings.SetChiSqPixelHitCut(25.);
		settings.SetChiSqStripHitCut(9.);
		settings.SetPDG(11);
		settings.IsProcessSubstationsTogether(true);
		trdFinderNN->SetSettings(settings);
		trdFinderNN->SetLayout(CbmLitEnvironment::Instance()->GetLayout());
		trdFinderNN->SetVerbose(1);
		trdFinderNN->Initialize();
		TrackFinderPtr finder(trdFinderNN);
		return finder;
	} else
	if(name == "e_branch") {
		CbmLitTrackFinderBranch* trdFinderBranch = new CbmLitTrackFinderBranch();
		CbmLitTrackFinderSettings settings;
		settings.SetNofIter(1);
		settings.SetPropagator(CreateTrackPropagator("lit"));
		settings.SetSeedSelection(CreateTrackSelection("momentum"));
		settings.SetStationGroupSelection(CreateTrackSelection("trd_station"));
		settings.SetFinalSelection(CreateTrackSelection("trd_final"));
		settings.SetFilter(CreateTrackUpdate("kalman"));
		settings.IsUseFastSearch(true);
		settings.SetPDG(11);
		settings.IsProcessSubstationsTogether(true);
		settings.SetMaxNofMissingHits(4);
		settings.IsAlwaysCreateMissingHit(false);
		settings.SetSigmaCoef(5.);
		settings.SetChiSqPixelHitCut(25.);
		settings.SetChiSqStripHitCut(9.);
		trdFinderBranch->SetSettings(settings);
		trdFinderBranch->SetLayout(CbmLitEnvironment::Instance()->GetLayout());
		trdFinderBranch->SetVerbose(1);
		trdFinderBranch->Initialize();
		TrackFinderPtr finder(trdFinderBranch);
		return finder;
	} else
	if(name == "e_weight") {
		CbmLitTrackFinderWeight* trdFinderWeight = new CbmLitTrackFinderWeight();
		CbmLitTrackFinderSettings settings;
		settings.SetNofIter(1);
		settings.SetPropagator(CreateTrackPropagator("lit"));
		settings.SetSeedSelection(CreateTrackSelection("momentum"));
		settings.SetFinalSelection(CreateTrackSelection("empty"));
		settings.SetFitter(CreateTrackFitter("kalman_robust"));
		settings.SetFilter(CreateTrackUpdate("kalman"));
		settings.SetMaxNofMissingHits(3);
		settings.IsUseFastSearch(true);
		settings.SetSigmaCoef(5.);
		settings.SetChiSqPixelHitCut(25.);
		settings.SetChiSqStripHitCut(9.);
		settings.SetPDG(11);
		trdFinderWeight->SetSettings(settings);
		trdFinderWeight->SetLayout(CbmLitEnvironment::Instance()->GetLayout());
		trdFinderWeight->SetVerbose(1);
		trdFinderWeight->Initialize();
		TrackFinderPtr finder(trdFinderWeight);
		return finder;
	} else
	if(name == "mu_nn") {
		CbmLitTrackFinderNN* muchFinderNN = new CbmLitTrackFinderNN();
		CbmLitTrackFinderSettings settings;
		settings.SetNofIter(1);
		settings.SetPropagator(CreateTrackPropagator("lit"));
		settings.SetSeedSelection(CreateTrackSelection("momentum_seed"));
		settings.SetFinalSelection(CreateTrackSelection("much_final"));
		settings.SetFilter(CreateTrackUpdate("kalman"));
		settings.IsUseFastSearch(true);
		settings.SetMaxNofMissingHits(3);
		settings.SetSigmaCoef(3.5);
		settings.SetChiSqPixelHitCut(15.);//13.86);
		settings.SetChiSqStripHitCut(9.);
		settings.SetPDG(13);
		settings.IsProcessSubstationsTogether(true);
		muchFinderNN->SetSettings(settings);
		muchFinderNN->SetLayout(CbmLitEnvironment::Instance()->GetLayout());
		muchFinderNN->SetVerbose(1);
		muchFinderNN->Initialize();
		TrackFinderPtr finder(muchFinderNN);
		return finder;
	} else
	if(name == "mu_nn_parallel") {
		CbmLitTrackFinderNNParallel* muchFinderNN = new CbmLitTrackFinderNNParallel("nn_parallel_muon");
		muchFinderNN->Initialize();
		TrackFinderPtr finder(muchFinderNN);
		return finder;
	} else
	if(name == "e_nn_scalar") {
		CbmLitTrackFinderNNParallel* finderNN = new CbmLitTrackFinderNNParallel("nn_scalar_electron");
		finderNN->Initialize();
		TrackFinderPtr finder(finderNN);
		return finder;
	} else
	if(name == "e_nn_vec") {
		CbmLitTrackFinderNNParallel* finderNN = new CbmLitTrackFinderNNParallel("nn_vec_electron");
		finderNN->Initialize();
		TrackFinderPtr finder(finderNN);
		return finder;
	} else
	if(name == "mu_branch") {
		CbmLitTrackFinderBranch* muchFinderBranch = new CbmLitTrackFinderBranch();
		CbmLitTrackFinderSettings settings;
		settings.SetNofIter(1);
		settings.SetPropagator(CreateTrackPropagator("lit"));
		settings.SetSeedSelection(CreateTrackSelection("momentum_seed"));
		settings.SetStationGroupSelection(CreateTrackSelection("momentum"));
		settings.SetFinalSelection(CreateTrackSelection("much_final"));
		settings.SetFilter(CreateTrackUpdate("kalman"));
		settings.IsUseFastSearch(true);
		settings.SetPDG(13);
		settings.IsProcessSubstationsTogether(true);
		settings.SetMaxNofMissingHits(3);
		settings.IsAlwaysCreateMissingHit(false);
		settings.SetSigmaCoef(3.5);
		settings.SetChiSqPixelHitCut(15.);//13.86);
		settings.SetChiSqStripHitCut(9.);
		muchFinderBranch->SetSettings(settings);
		muchFinderBranch->SetLayout(CbmLitEnvironment::Instance()->GetLayout());
		muchFinderBranch->SetVerbose(1);
		muchFinderBranch->Initialize();
		TrackFinderPtr finder(muchFinderBranch);
		return finder;
	} else
	if(name == "mu_weight") {
		CbmLitTrackFinderWeight* muchFinderWeight = new CbmLitTrackFinderWeight();
		CbmLitTrackFinderSettings settings;
		settings.SetNofIter(1);
		settings.SetPropagator(CreateTrackPropagator("lit"));
		settings.SetSeedSelection(CreateTrackSelection("momentum_seed"));
		settings.SetFinalSelection(CreateTrackSelection("empty"));
		settings.SetFitter(CreateTrackFitter("kalman_robust"));
		settings.SetFilter(CreateTrackUpdate("kalman"));
		settings.SetMaxNofMissingHits(3);
		settings.IsUseFastSearch(true);
		settings.SetSigmaCoef(3.5);
		settings.SetChiSqPixelHitCut(15.);//13.86);
		settings.SetChiSqStripHitCut(9.);
		settings.SetPDG(13);
		muchFinderWeight->SetSettings(settings);
		muchFinderWeight->SetLayout(CbmLitEnvironment::Instance()->GetLayout());
		muchFinderWeight->SetVerbose(1);
		muchFinderWeight->Initialize();
		TrackFinderPtr finder(muchFinderWeight);
		return finder;
	} else
	if(name == "mvd_nn") {
		CbmLitTrackFinderNN* mvdFinderNN = new CbmLitTrackFinderNN();
		CbmLitTrackFinderSettings settings;
		settings.SetNofIter(1);
		settings.SetPropagator(CreateTrackPropagator("lit"));
		settings.SetSeedSelection(CreateTrackSelection("empty"));
		settings.SetFinalSelection(CreateTrackSelection("empty"));
		settings.SetFilter(CreateTrackUpdate("kalman"));
		settings.IsUseFastSearch(true);
		settings.SetMaxNofMissingHits(1);
		settings.SetSigmaCoef(3.5);
		settings.SetChiSqPixelHitCut(15.);//13.86);
		settings.SetChiSqStripHitCut(9.);
		settings.SetPDG(13);
		settings.IsProcessSubstationsTogether(true);
		mvdFinderNN->SetSettings(settings);
		mvdFinderNN->SetLayout(CbmLitEnvironment::Instance()->GetMvdLayout());
		mvdFinderNN->SetVerbose(1);
		mvdFinderNN->Initialize();
		TrackFinderPtr finder(mvdFinderNN);
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
        nhMerger->IsUseFastSearch(true);
		nhMerger->SetSigmaCoef(5.);
		nhMerger->SetChiSqPixelHitCut(50.);//13.86);
		nhMerger->SetChiSqStripHitCut(4.);
		nhMerger->SetStation(CbmLitEnvironment::Instance()->GetTofStation());
		nhMerger->Initialize();
		HitToTrackMergerPtr merger(nhMerger);
		return merger;
	} else
	if(name == "") {

//		return finder;
	}
}
