#ifndef CBMLITTOOLFACTORY_H_
#define CBMLITTOOLFACTORY_H_

#include "CbmLitPtrTypes.h"

#include <string>
#include <iostream>

//#include <boost/shared_ptr.hpp>

class CbmLitToolFactory
{
public:
	virtual ~CbmLitToolFactory();

	static CbmLitToolFactory* Instance();

	TrackExtrapolatorPtr CreateTrackExtrapolator(
			const std::string& name);
	TrackPropagatorPtr CreateTrackPropagator(
			const std::string& name);
	TrackUpdatePtr CreateTrackUpdate(
			const std::string& name);
	TrackFitterPtr CreateTrackFitter(
			const std::string& name);
	TrackSelectionPtr CreateTrackSelection(
			const std::string& name);
	TrackFinderPtr CreateTrackFinder(
			const std::string& name);
	HitToTrackMergerPtr CreateHitToTrackMerger(
			const std::string& name);

protected:
	CbmLitToolFactory();

private:
	static CbmLitToolFactory* fInstance;
};

#endif /*CBMLITTOOLFACTORY_H_*/
