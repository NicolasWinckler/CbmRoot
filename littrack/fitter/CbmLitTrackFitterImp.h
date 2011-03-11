/** CbmLitTrackFitterImp.h
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2007
 **
 ** Class implements Kalman Filter track fit.
 **/

#ifndef CBMLITTRACKFITTERIMP_H_
#define CBMLITTRACKFITTERIMP_H_

#include "base/CbmLitPtrTypes.h"
#include "interface/CbmLitTrackFitter.h"

class CbmLitTrack;

class CbmLitTrackFitterImp : public CbmLitTrackFitter
{
public:
	/* Constructor
	 *@param propagator Track propagation tool
	 *@param update Track update tool */
	CbmLitTrackFitterImp(
			TrackPropagatorPtr propagator,
			TrackUpdatePtr update);

	/* Destructor */
	virtual ~CbmLitTrackFitterImp();

	/* Inherited from CbmLitTool */
	virtual LitStatus Initialize();

	/* Inherited from CbmLitTool */
	virtual LitStatus Finalize();

	/* Inherited from CbmLitTrackFitter */
	virtual LitStatus Fit(
			CbmLitTrack *track,
			bool downstream = true);
private:
	/* Track propagation tool */
	TrackPropagatorPtr fPropagator;
	/* Track update tool */
	TrackUpdatePtr fUpdate;
};

#endif /*CBMLITTRACKFITTERIMP_H_*/
