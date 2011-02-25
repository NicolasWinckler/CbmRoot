/** CbmLitTrackFitter.h
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2008
 **
 ** Base class for all LIT track fitter algorithms
 **/

#ifndef CBMLITTRACKFITTER_H_
#define CBMLITTRACKFITTER_H_

#include "base/CbmLitTool.h"
#include "base/CbmLitFloat.h"

class CbmLitTrack;

class CbmLitTrackFitter : public CbmLitTool
{
public:
   CbmLitTrackFitter(){};
   CbmLitTrackFitter(
		   const std::string& name):CbmLitTool(name) {};

   virtual ~CbmLitTrackFitter() {};

   virtual LitStatus Fit(
		   CbmLitTrack *track,
		   bool downstream = true) = 0;
};

#endif
