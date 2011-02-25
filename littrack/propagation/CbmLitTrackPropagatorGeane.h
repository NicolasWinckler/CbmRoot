/** CbmTrackPropagatorGeane.h
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2007
 **
 ** The class implements an interface between GEANE track propagation tool and
 ** Littrack package.
 **/

#ifndef CBMLITTRACKPROPAGATORGEANE_H
#define CBMLITTRACKPROPAGATORGEANE_H

#include "interface/CbmLitTrackPropagator.h"

#include <vector>

class CbmPropagator;

class CbmLitTrackPropagatorGeane: public CbmLitTrackPropagator {
public:
	/* Constructor */
	CbmLitTrackPropagatorGeane();

	/* Destructor */
	virtual ~CbmLitTrackPropagatorGeane();

	/* Inherited from CbmLitTool */
	virtual LitStatus Initialize();

	/* Inherited from CbmLitTool */
	virtual LitStatus Finalize();

    /* Inherited from CbmLitTrackPropagator */
	virtual LitStatus Propagate(
		   const CbmLitTrackParam *parIn,
		   CbmLitTrackParam *parOut,
		   myf zOut,
		   int pdg,
		   std::vector<myf>* F);

	/* Inherited from CbmLitTrackPropagator */
	virtual LitStatus Propagate(
		   CbmLitTrackParam *pParam,
		   myf zOut,
		   int pdg,
		   std::vector<myf>* F);

private:
	CbmPropagator *fPropagator; // FairRoot interface to GEANE
};

#endif //CbmLitTrackPropagatorGeane
