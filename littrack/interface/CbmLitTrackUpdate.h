/** CbmLitTrackUpdate.h
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2008
 **
 ** Base class for all track update algorithms
 **/

#ifndef CBMLITTRACKUPDATE_H_
#define CBMLITTRACKUPDATE_H_

#include "base/CbmLitTool.h"

class CbmLitHit;
class CbmLitTrackParam;

class CbmLitTrackUpdate : public CbmLitTool
{
public:
   CbmLitTrackUpdate(){};
   CbmLitTrackUpdate(
		   const std::string& name):CbmLitTool(name){};

   virtual ~CbmLitTrackUpdate(){};

   virtual LitStatus Update(
		   const CbmLitTrackParam *pParamIn,
           CbmLitTrackParam *pParamOut,
           const CbmLitHit *pHit) = 0;

   virtual LitStatus Update(
		   CbmLitTrackParam *pParam,
		   const CbmLitHit *pHit) = 0;
};

#endif /* CBMLITTRACKUPDATE_H_ */

