/** CbmLitPDG.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 *
 * Class stores particle properties.
 **/

#ifndef CBMLITPDG_H_
#define CBMLITPDG_H_

#include "base/CbmLitFloat.h"

class CbmLitPDG
{
public:
   /* Constructor */
   CbmLitPDG();

   /* Destructor */
   virtual ~CbmLitPDG();

   /* Returns mass of the particle in [GeV/c]. */
   static myf GetMass(
      int pdg);

   /* Returns true if particle is electron. */
   static bool IsElectron(
      int pdg);

   /* Returns true if particle is muon. */
   static bool IsMuon(
      int pdg);

   /* Returns true is particle is pion. */
   static bool IsPion(
      int pdg);
};

#endif /*CBMLITPDG_H_*/
