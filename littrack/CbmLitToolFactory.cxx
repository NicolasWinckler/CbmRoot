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
#include "CbmLitKalmanSmoother.h"
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
	   CbmLitRK4TrackExtrapolator* extrapolator = new CbmLitRK4TrackExtrapolator();
	   extrapolator->Initialize();
	   CbmLitTrackPropagatorImp* litpropagator = new CbmLitTrackPropagatorImp(extrapolator);
	   litpropagator->Initialize();
	   return litpropagator;
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
		((CbmLitTrackPropagatorImp*) propagator)->IsCalcTransportMatrix(true);
		CbmLitTrackUpdate* update = CreateTrackUpdate("Much");
		fitter = new CbmLitTrackFitterImp(propagator, update);
	    return fitter;
	} else 
	if(name == "MuchRobust") {
		//TODO delete propagator and update and fitter after use
		CbmLitTrackFitter* fitterImp = CreateTrackFitter("Much");
		CbmLitTrackFitter* smoother = new CbmLitKalmanSmoother();
		
		fitter = new CbmLitTrackFitterRobust(fitterImp, smoother);
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
		CbmLitTrackSelectionMomentum* momSelection = new CbmLitTrackSelectionMomentum();
		momSelection->SetMinMomentum(0.1);
		momSelection->Initialize();
		return momSelection;
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
		CbmLitTrackSelectionMuch* muchSelection = new CbmLitTrackSelectionMuch();
		muchSelection->SetNofSharedHits(2);
		muchSelection->SetMinNofHits(1);
		Int_t nofStations = CbmLitEnvironment::Instance()->GetMuchLayout().GetNofStations();
		muchSelection->SetMinLastPlaneId(nofStations-1);
		muchSelection->Initialize(); 
		return muchSelection;
	} else 
	if(name == "TrdStation") {
		CbmLitTrackSelectionTrd* trdSelection = new CbmLitTrackSelectionTrd();
		trdSelection->SetNofSharedHits(2);
		trdSelection->SetMinNofHits(1);
		trdSelection->SetMinLastPlaneId(0);
		trdSelection->Initialize(); 
		return trdSelection;
	} else
	if(name == "TrdFinal") {
		CbmLitTrackSelectionTrd* trdSelection = new CbmLitTrackSelectionTrd();
		trdSelection->SetNofSharedHits(2);
		trdSelection->SetMinNofHits(1);
		Int_t nofStations = CbmLitEnvironment::Instance()->GetTrdLayout().GetNofStations();
		trdSelection->SetMinLastPlaneId(nofStations-1);
		trdSelection->Initialize(); 
		return trdSelection;
	}
	return selection;
}
	
ClassImp(CbmLitToolFactory)
