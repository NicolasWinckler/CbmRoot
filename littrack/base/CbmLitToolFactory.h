#ifndef CBMLITTOOLFACTORY_H_
#define CBMLITTOOLFACTORY_H_

#include "CbmLitPtrTypes.h"

#include "TObject.h"

#include <string>
#include <iostream>

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

protected:
	CbmLitToolFactory();

private:
	static CbmLitToolFactory* fInstance;

	ClassDef(CbmLitToolFactory, 1);
};

#endif /*CBMLITTOOLFACTORY_H_*/
