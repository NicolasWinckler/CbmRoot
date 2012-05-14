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
#include "finder/CbmLitNearestHitToTrackMerger.h"
#include "fitter/CbmLitKalmanFilter.h"
#include "fitter/CbmLitKalmanSmoother.h"
#include "fitter/CbmLitTrackFitterImp.h"
#include "fitter/CbmLitTrackFitterIter.h"
#include "interface/CbmLitField.h"
#include "propagation/CbmLitRK4TrackExtrapolator.h"
#include "propagation/CbmLitLineTrackExtrapolator.h"
#include "propagation/CbmLitCleverTrackExtrapolator.h"
#include "propagation/CbmLitTGeoTrackPropagator.h"
#include "selection/CbmLitTrackSelectionEmpty.h"
#include "selection/CbmLitTrackSelectionCuts.h"
#include "selection/CbmLitTrackSelectionMuch.h"
#include "selection/CbmLitTrackSelectionTrd.h"
#include "cbm/parallel/CbmLitParallelTrackFitterTestMuon.h"
#include "cbm/parallel/CbmLitParallelTrackFitterTestElectron.h"
#include "cbm/parallel/CbmLitTrackFinderNNParallel.h"

CbmLitToolFactory::CbmLitToolFactory()
{
}

CbmLitToolFactory::~CbmLitToolFactory()
{
}

CbmLitToolFactory* CbmLitToolFactory::Instance()
{
   static CbmLitToolFactory instance;
   return &instance;
}

TrackExtrapolatorPtr CbmLitToolFactory::CreateTrackExtrapolator(
   const std::string& name)
{
   if(name == "line") {
      TrackExtrapolatorPtr extrapolator(new CbmLitLineTrackExtrapolator());
      return extrapolator;
   } else if (name == "rk4") {
      CbmLitEnvironment* env = CbmLitEnvironment::Instance();
      CbmLitField* field = new CbmLitMapField(env->GetField());
      TrackExtrapolatorPtr extrapolator(new CbmLitRK4TrackExtrapolator(field));
      return extrapolator;
   } else if (name == "lit") {
      TrackExtrapolatorPtr extrapolator(new CbmLitCleverTrackExtrapolator(""));
      return extrapolator;
   }
   return TrackExtrapolatorPtr();
}

TrackPropagatorPtr CbmLitToolFactory::CreateTrackPropagator(
   const std::string& name)
{
   if(name == "lit") {
      TrackPropagatorPtr propagator(new CbmLitTGeoTrackPropagator(CreateTrackExtrapolator("lit")));
      return propagator;
   } else if(name == "rk4") {
      TrackPropagatorPtr propagator(new CbmLitTGeoTrackPropagator(CreateTrackExtrapolator("rk4")));
      return propagator;
   } else if(name == "line") {
      TrackPropagatorPtr propagator(new CbmLitTGeoTrackPropagator(CreateTrackExtrapolator("line")));
      return propagator;
   }
   return TrackPropagatorPtr();
}

TrackUpdatePtr CbmLitToolFactory::CreateTrackUpdate(
   const std::string& name)
{
   if(name == "kalman") {
      TrackUpdatePtr update(new CbmLitKalmanFilter());
      return update;
   }
   return TrackUpdatePtr();
}

TrackFitterPtr CbmLitToolFactory::CreateTrackFitter(
   const std::string& name)
{
   if(name == "lit_kalman") {
      TrackPropagatorPtr propagator = CreateTrackPropagator("lit");
      //((CbmLitTGeoTrackPropagator*) propagator)->IsCalcTransportMatrix(true);
      TrackUpdatePtr update = CreateTrackUpdate("kalman");
      TrackFitterPtr fitter(new CbmLitTrackFitterImp(propagator, update));
      return fitter;
   } else if (name == "kalman_smoother") {
      TrackFitterPtr fitter(new CbmLitKalmanSmoother());
      return fitter;
   } else if (name == "kalman_parallel_muon") {
      TrackFitterPtr fitter(new CbmLitParallelTrackFitterTestMuon());
      return fitter;
   } else if (name == "kalman_parallel_electron") {
      TrackFitterPtr fitter(new CbmLitParallelTrackFitterTestElectron());
      return fitter;
   }
   return TrackFitterPtr();
}

TrackSelectionPtr CbmLitToolFactory::CreateTrackSelection(
   const std::string& name)
{
   if(name == "empty") {
      TrackSelectionPtr selection(new CbmLitTrackSelectionEmpty());
      return selection;
   } else if(name == "momentum") {
      //TrackSelectionPtr selection(new CbmLitTrackSelectionMomentum());
      CbmLitTrackSelectionCuts* momSelection = new CbmLitTrackSelectionCuts();
      momSelection->SetMinMomentum(0.1);
      TrackSelectionPtr selection(momSelection);
      return selection;
   } else if(name == "momentum_seed") {
      CbmLitTrackSelectionCuts* momSelection = new CbmLitTrackSelectionCuts();
      momSelection->SetMinMomentum(1.0);
      TrackSelectionPtr selection(momSelection);
      return selection;
   } else if(name == "chi_square") {
      CbmLitTrackSelectionCuts* chiSqSelection = new CbmLitTrackSelectionCuts();
      chiSqSelection->SetMaxChiSq(30.);
      TrackSelectionPtr selection(chiSqSelection);
      return selection;
   } else if(name == "much_final") {
      CbmLitTrackSelectionMuch* muchSelection = new CbmLitTrackSelectionMuch();
      muchSelection->SetNofSharedHits(3);
      muchSelection->SetMinNofHits(1);
      muchSelection->SetMinLastPlaneId(0);
      TrackSelectionPtr selection(muchSelection);
      return selection;
   } else if(name == "trd_station") {
      CbmLitTrackSelectionTrd* trdSelection = new CbmLitTrackSelectionTrd();
      trdSelection->SetNofSharedHits(2);
      trdSelection->SetMinNofHits(0);
      TrackSelectionPtr selection(trdSelection);
      return selection;
   } else if(name == "trd_final") {
      CbmLitTrackSelectionTrd* trdSelection = new CbmLitTrackSelectionTrd();
      trdSelection->SetNofSharedHits(3);
      trdSelection->SetMinNofHits(0);
      TrackSelectionPtr selection(trdSelection);
      return selection;
   } else if(name == "trd_final_iter_1") {
      CbmLitTrackSelectionTrd* trdSelection = new CbmLitTrackSelectionTrd();
      trdSelection->SetNofSharedHits(3);
      trdSelection->SetMinNofHits(8);
      TrackSelectionPtr selection(trdSelection);
      return selection;
   }
   return TrackSelectionPtr();
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
		//      settings.SetFinalSelection(0, CreateTrackSelection("trd_final_iter_1"));
		settings.SetFinalSelection(0, CreateTrackSelection("trd_final"));
		settings.SetFilter(CreateTrackUpdate("kalman"));
		settings.IsUseFastSearch(true);
		settings.SetMaxNofMissingHits(4);
		settings.SetSigmaCoef(5.);
		settings.SetChiSqPixelHitCut(0, 15.);
		//      settings.SetChiSqPixelHitCut(1, 25.);
		settings.SetChiSqStripHitCut(9.);
		settings.SetPDG(0, 211);
		//      settings.SetPDG(1, 11);
		settings.IsProcessSubstationsTogether(true);
		trdFinderNN->SetSettings(settings);
		trdFinderNN->SetZPropagationForTrackSeeds(400.);
		trdFinderNN->SetUseTGeo(true);
		trdFinderNN->SetLayout(CbmLitEnvironment::Instance()->GetLayout());
		TrackFinderPtr finder(trdFinderNN);
		return finder;
	} else if(name == "e_branch") {
		CbmLitTrackFinderBranch* trdFinderBranch = new CbmLitTrackFinderBranch();
		CbmLitTrackFinderSettings settings;
		settings.SetNofIter(2);
		settings.SetPropagator(CreateTrackPropagator("lit"));
		settings.SetSeedSelection(CreateTrackSelection("momentum"));
		settings.SetStationGroupSelection(CreateTrackSelection("trd_station"));
		settings.SetFinalSelection(0, CreateTrackSelection("trd_final_iter_1"));
		settings.SetFinalSelection(1, CreateTrackSelection("trd_final"));
		settings.SetFilter(CreateTrackUpdate("kalman"));
		settings.IsUseFastSearch(true);
		settings.SetPDG(0, 211);
		settings.SetPDG(1, 11);
		settings.IsProcessSubstationsTogether(true);
		settings.SetMaxNofMissingHits(4);
		settings.IsAlwaysCreateMissingHit(false);
		settings.SetSigmaCoef(5.);
		settings.SetChiSqPixelHitCut(0, 15.);
		settings.SetChiSqPixelHitCut(1, 50.);
		settings.SetChiSqStripHitCut(9.);
		trdFinderBranch->SetSettings(settings);
		trdFinderBranch->SetZPropagationForTrackSeeds(400.);
		trdFinderBranch->SetUseTGeo(true);
		trdFinderBranch->SetLayout(CbmLitEnvironment::Instance()->GetLayout());
		TrackFinderPtr finder(trdFinderBranch);
		return finder;
   } else if(name == "mu_nn") {
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
      muchFinderNN->SetZPropagationForTrackSeeds(-1.);
	  muchFinderNN->SetUseTGeo(false);
      muchFinderNN->SetLayout(CbmLitEnvironment::Instance()->GetLayout());
      TrackFinderPtr finder(muchFinderNN);
      return finder;
   } else if(name == "e_nn_parallel") {
      CbmLitTrackFinderNNParallel* finderNN = new CbmLitTrackFinderNNParallel("nn_parallel_electron");
      TrackFinderPtr finder(finderNN);
      return finder;
   } else if(name == "mu_nn_parallel") {
      CbmLitTrackFinderNNParallel* muchFinderNN = new CbmLitTrackFinderNNParallel("nn_parallel_muon");
      TrackFinderPtr finder(muchFinderNN);
      return finder;
   } else if(name == "mu_branch") {
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
      settings.SetChiSqStripHitCut(11.);
      muchFinderBranch->SetSettings(settings);
      muchFinderBranch->SetLayout(CbmLitEnvironment::Instance()->GetLayout());
      TrackFinderPtr finder(muchFinderBranch);
      return finder;
   } else if(name == "mvd_nn") {
      CbmLitTrackFinderNN* mvdFinderNN = new CbmLitTrackFinderNN();
      CbmLitTrackFinderSettings settings;
      settings.SetNofIter(1);
      settings.SetPropagator(CreateTrackPropagator("rk4"));
      settings.SetSeedSelection(CreateTrackSelection("empty"));
      settings.SetFinalSelection(CreateTrackSelection("empty"));
      settings.SetFilter(CreateTrackUpdate("kalman"));
      settings.IsUseFastSearch(true);
      settings.SetMaxNofMissingHits(1);
      settings.SetSigmaCoef(3.5);
      settings.SetChiSqPixelHitCut(15.);
      settings.SetChiSqStripHitCut(9.);
      settings.SetPDG(211);
      settings.IsProcessSubstationsTogether(true);
      mvdFinderNN->SetSettings(settings);
      mvdFinderNN->SetLayout(CbmLitEnvironment::Instance()->GetMvdLayout());
      TrackFinderPtr finder(mvdFinderNN);
      return finder;
   }
   return TrackFinderPtr();
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
      nhMerger->SetChiSqStripHitCut(9.);
      nhMerger->SetStation(CbmLitEnvironment::Instance()->GetTofStation());
      HitToTrackMergerPtr merger(nhMerger);
      return merger;
   }
   return HitToTrackMergerPtr();
}
