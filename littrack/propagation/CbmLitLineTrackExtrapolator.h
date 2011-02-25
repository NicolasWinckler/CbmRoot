/** CbmLitLineTrackExtrapolator.h
 *@author A.Lebedev <andrey.lebedev@gsi.de>
 *@since 2007
 **
 ** The class performs line track extrapolation of the track parameters.
 **/

#ifndef CBMLITLINETRACKEXTRAPOLATOR_H_
#define CBMLITLINETRACKEXTRAPOLATOR_H_

#include "interface/CbmLitTrackExtrapolator.h"

class CbmLitTrackParam;

class CbmLitLineTrackExtrapolator: public CbmLitTrackExtrapolator {
public:
	/* Constructor */
	CbmLitLineTrackExtrapolator();

	/* Destructor */
	virtual ~CbmLitLineTrackExtrapolator();

	/* Inherited from CbmLitTool */
	virtual LitStatus Initialize();

	/* Inherited from CbmLitTool */
	virtual LitStatus Finalize();

	/* Inherited from CbmLitTrackExtrapolator */
	virtual LitStatus Extrapolate(
		   const CbmLitTrackParam *parIn,
		   CbmLitTrackParam *parOut,
		   myf zOut,
		   std::vector<myf>* F);

	/* Inherited from CbmLitTrackExtrapolator */
	virtual LitStatus Extrapolate(
		   CbmLitTrackParam *par,
		   myf zOut,
		   std::vector<myf>* F);
};

#endif //CbmLitLineTrackExtrapolator

