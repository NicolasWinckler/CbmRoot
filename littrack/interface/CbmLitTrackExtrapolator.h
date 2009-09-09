/** CbmLitTrackExtrapolator.h
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2008
 **
 **  Base class for LIT track extrapolation algorithms.
 **  Extrapolation algorithm do not have to take into account material effects.
 **
 **/

#ifndef CBMLITTRACKEXTRAPOLATOR_H_
#define CBMLITTRACKEXTRAPOLATOR_H_

#include "CbmLitTool.h"
#include "CbmLitFloat.h"

#include <vector>

class CbmLitTrackParam;

class CbmLitTrackExtrapolator : public CbmLitTool {
public:
	/* Constructor */
	CbmLitTrackExtrapolator() {};

	/* Constructor with name.
	 * @param name Name of the tool.
	 */
	CbmLitTrackExtrapolator(
		   const std::string& name):CbmLitTool(name){};

	/* Destructor */
	virtual ~CbmLitTrackExtrapolator(){};

   /* Pure virtual function for track parameters extrapolation with calculation of transport matrix.
    * @param parIn Pointer to the initial track parameters.
    * @param parOut Pointer to the output track parameters.
    * @param zOut Z position to extrapolate to [cm].
    * @param F Output transport matrix. If F == NULL than transport matrix is not calculated.
    * @return Extrapolation status.
    */
   virtual LitStatus Extrapolate(
		   const CbmLitTrackParam *parIn,
           CbmLitTrackParam *parOut,
           myf zOut,
           std::vector<myf>* F = NULL) = 0;

   /* Pure virtual function for track parameters extrapolation with calculation of transport matrix.
    * @param par Pointer to the initial and output track parameters.
    * @param zOut Z position to extrapolate to [cm].
    * @param F Output transport matrix. If F == NULL than transport matrix is not calculated.
    * @return Extrapolation status.
    */
   virtual LitStatus Extrapolate(
		   CbmLitTrackParam *par,
           myf zOut,
           std::vector<myf>* F = NULL) = 0;
};

#endif
