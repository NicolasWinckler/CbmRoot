/** CbmLitTrackUpdate.h
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2008
 *
 * Interface for track update algorithms
 **/

#ifndef CBMLITTRACKUPDATE_H_
#define CBMLITTRACKUPDATE_H_

#include "base/CbmLitTool.h"

class CbmLitHit;
class CbmLitTrackParam;

class CbmLitTrackUpdate : public CbmLitTool
{
public:
   /* Constructor */
   CbmLitTrackUpdate() {};

   /* Constructor with name
    * @param name Name of the tool */
   CbmLitTrackUpdate(
      const std::string& name):CbmLitTool(name) {};

   /* Destructor */
   virtual ~CbmLitTrackUpdate() {};

   /* Main function to be implemented for the concrete track update algorithm
    * @param pParamIn Pointer to input track parameter
    * @param pParamOut Pointer to output track parameter
    * @parma pHit Pointer to hit
    * @param chiSq Output value of the contribution to the chi-square
    * @return Status code */
   virtual LitStatus Update(
      const CbmLitTrackParam* pParamIn,
      CbmLitTrackParam* pParamOut,
      const CbmLitHit* pHit,
      myf& chiSq) = 0;

   /* Main function to be implemented for the concrete track update algorithm
    * @param pParam Pointer to input/output track parameter
    * @parma pHit Pointer to hit
    * @param chiSq Output value of the contribution to the chi-square
    * @return Status code */
   virtual LitStatus Update(
      CbmLitTrackParam* pParam,
      const CbmLitHit* pHit,
      myf& chiSq) = 0;
};

#endif /* CBMLITTRACKUPDATE_H_ */

