// -------------------------------------------------------------------------
// -----                  CbmLitTrackUpdate header file               -----
// -----                  Created 4/06/08  by A. Lebedev               -----
// -------------------------------------------------------------------------


/** CbmLitTrackUpdate.h
 *@author A.Lebedev <alebedev@jinr.ru>
 **
 ** Base class for all track update algorithms
 **/ 


#ifndef CBMLITTRACKUPDATE_H
#define CBMLITTRACKUPDATE_H

#include "CbmTool.h"

class CbmLitHit;
class CbmLitTrackParam;

class CbmLitTrackUpdate : public CbmTool {

public:

   CbmLitTrackUpdate();
   CbmLitTrackUpdate(const std::string& name);
   
   virtual ~CbmLitTrackUpdate();
   
   virtual LitStatus Update( 
		   const CbmLitTrackParam *pParamIn,
           CbmLitTrackParam *pParamOut,
           const CbmLitHit *pHit) = 0;
                              
   virtual LitStatus Update( 
		   CbmLitTrackParam *pParam, 
		   const CbmLitHit *pHit) = 0;
   
   ClassDef(CbmLitTrackUpdate,1)
}; 

#endif //CbmLitTrackUpdate

