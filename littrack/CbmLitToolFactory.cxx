#include "CbmLitToolFactory.h"

#include "CbmLitTrackPropagator.h"
#include "CbmLitTrackExtrapolator.h"
#include "CbmLitTrackUpdate.h"
#include "CbmLitTrackFitter.h"
#include "CbmLitTrackSelection.h"

#include "CbmLitRK4TrackExtrapolator.h"
#include "CbmLitTrackPropagatorImp.h"
#include "CbmTrackPropagatorGeane.h"
#include "CbmLitTrackSelectionEmpty.h"
#include "CbmLitTrackSelectionMuch.h"
#include "CbmLitTrackSelectionTrd.h"
#include "CbmLitTrackSelectionMomentum.h"
#include "CbmLitTrackSelectionMuchRobust.h"
#include "CbmLitRobustSelection.h"
#include "CbmLitKalmanFilter.h"
#include "CbmLitTrackFitterImp.h"
#include "CbmLitTrackFitterRobust.h"
#include "CbmLitEnvironment.h"

//#include <boost/tr1/memory.hpp>


CbmLitToolFactory* CbmLitToolFactory::fInstance = NULL;

CbmLitToolFactory::CbmLitToolFactory()
{
	//boost::shared_ptr<int> p(new int(4));
	
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

CbmLitTrackPropagator* CbmLitToolFactory::CreateTrackPropagator(const std::string& name)
{
	CbmLitTrackPropagator* propagator = NULL;
	if(name == "Much") {
		//TODO delete extrapolator!!!!
	   CbmLitTrackExtrapolator* extrapolator = new CbmLitRK4TrackExtrapolator();
	   extrapolator->Initialize();
	   propagator = new CbmLitTrackPropagatorImp(extrapolator);
	   propagator->Properties().SetProperty("fMass",0.105);
	   propagator->Properties().SetProperty("fApplyEnergyLoss", true);
	   propagator->Properties().SetProperty("fEnergyLoss", 0.00354);
	   propagator->Properties().SetProperty("fFms", 1.05);
	   propagator->Initialize();
	   return propagator;
	} else 
	if(name == "Trd") {
	
	} else 
	if(name == "Geane") {
		propagator = new CbmTrackPropagatorGeane();
	    propagator->Initialize();
	}
	return propagator;
}

CbmLitTrackUpdate* CbmLitToolFactory::CreateTrackUpdate(const std::string& name)
{
	CbmLitTrackUpdate* update = NULL;
	if(name == "Much" || name == "Trd") {
		update = new CbmLitKalmanFilter();
		update->Initialize();
		return update;		    
	}
	return update;
}

CbmLitTrackFitter* CbmLitToolFactory::CreateTrackFitter(const std::string& name)
{
	CbmLitTrackFitter* fitter = NULL;
	if(name == "Much") {
		//TODO delete propagator and update after use
		CbmLitTrackPropagator* propagator = CreateTrackPropagator("Much");
		CbmLitTrackUpdate* update = CreateTrackUpdate("Much");
		fitter = new CbmLitTrackFitterImp(propagator, update);
	    return fitter;
	} else 
	if(name == "MuchRobust") {
		//TODO delete propagator and update and fitter after use
		CbmLitTrackFitter* fitterImp = CreateTrackFitter("Much");
		fitter = new CbmLitTrackFitterRobust(fitterImp);
		return fitter;
	} else
	if(name == "Trd") {
	
	} else 
	if(name == "Geane") {
		CbmLitTrackPropagator* propagator = CreateTrackPropagator("Geane");
		CbmLitTrackUpdate* update = CreateTrackUpdate("Much");
		fitter = new CbmLitTrackFitterImp(propagator, update);
		return fitter;
	}
	return fitter;
}

CbmLitTrackSelection* CbmLitToolFactory::CreateTrackSelection(const std::string& name)
{
	CbmLitTrackSelection* selection = NULL;
	if(name == "Empty") {
		selection = new CbmLitTrackSelectionEmpty();
		selection->Initialize();
		return selection;
	} else
	if(name == "Momentum") {
		selection = new CbmLitTrackSelectionMomentum();
		selection->Initialize();
		return selection;
	} else
	if (name == "MuchRobustSelection") {
		CbmLitTrackFitter* robustFitter = CreateTrackFitter("MuchRobust");
		CbmLitTrackFitter* fitter = CreateTrackFitter("Much");
		selection = new CbmLitRobustSelection(robustFitter, fitter);
		selection->Initialize();
		return selection;
	} else 
	if (name == "MuchRobust") {
		selection = new CbmLitTrackSelectionMuchRobust();
		selection->Initialize();
		return selection;
	} else 
	if(name == "Much") {
		selection = new CbmLitTrackSelectionMuch();
		selection->Properties().SetProperty("fNofSharedHits", 2);
		selection->Properties().SetProperty("fMinNofHits", 1);
		Int_t nofStations = CbmLitEnvironment::Instance()->GetMuchLayout().GetNofStations();
		selection->Properties().SetProperty("fMinLastPlaneId", nofStations-1);
		selection->Initialize();   
	    return selection;
	} else 
	if(name == "Trd") {
	
	}
	return selection;
}
	
ClassImp(CbmLitToolFactory)

