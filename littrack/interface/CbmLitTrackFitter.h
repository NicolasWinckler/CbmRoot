// -------------------------------------------------------------------------
// -----                  CbmLitTrackFitter header file               -----
// -----                  Created 05/06/08  by A. Lebedev               -----
// -------------------------------------------------------------------------


/** CbmLitTrackFitter.h
 *@author A.Lebedev <alebedev@jinr.ru>
 **
 ** Base class for all LIT track fitter algorithms
 **/ 

#ifndef CBMLITTRACKFITTER_H_
#define CBMLITTRACKFITTER_H_

#include "CbmLitTool.h"

class CbmLitTrack;

class CbmLitTrackFitter : public CbmLitTool {

public:

   CbmLitTrackFitter();
   CbmLitTrackFitter(const std::string& name);
      
   virtual ~CbmLitTrackFitter();

   virtual LitStatus Fit(CbmLitTrack *pTrack) = 0;
   
   ClassDef(CbmLitTrackFitter,1)
};

#endif //CbmLitTrackFitter

