/** CbmLitSubstation.h
 *@author A.Lebedev <andrey.lebedev@gsi.de>
 *@since 2008
 *
 * Class represents detector substation.
 **/

#ifndef CBMLITSUBSTATION_H_
#define CBMLITSUBSTATION_H_

#include "base/CbmLitFloat.h"

#include <string>
#include <sstream>

class CbmLitSubstation
{
public:
   /* Constructor */
   CbmLitSubstation(): fZ(0.) {};

   /* Destructor */
   virtual ~CbmLitSubstation() {};

   /* Sets z position of substation */
   void SetZ(litfloat z) {fZ = z;}

   /* Returns Z position of substation */
   litfloat GetZ() const {return fZ;}

   /* Returns std::string representation of the class. */
   virtual std::string ToString() const {
      std::stringstream ss;
      ss << "Substation: z=" << GetZ() << std::endl;
      return ss.str();
   }

private:
   litfloat fZ; // Z position of substation
};

#endif /*CBMLITSUBSTATION_H_*/
