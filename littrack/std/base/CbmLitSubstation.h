/**
 * \file CbmLitSubstation.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 * \brief Detector substation.
 **/

#ifndef CBMLITSUBSTATION_H_
#define CBMLITSUBSTATION_H_

#include "base/CbmLitFloat.h"
#include "utils/CbmLitUtils.h"

#include <string>

/**
 * \class CbmLitSubstation
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 * \brief Detector substation.
 **/
class CbmLitSubstation
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitSubstation(): fZ(0.) {}

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitSubstation() {}

   /**
    * \brief Set Z position of substation.
    * \param[in] z Z position.
    */
   void SetZ(litfloat z) { fZ = z; }

   /**
    * \brief Return Z position of substation.
    * \return Z position of station.
    */
   litfloat GetZ() const { return fZ; }

   /**
    * \brief Return string representation of class.
    * \return String representation of class.
    */
   virtual std::string ToString() const {
      std::string str = std::string("Substation: z=") + lit::ToString<litfloat>(GetZ()) + "\n";
      return str;
   }

private:
   litfloat fZ; // Z position of substation
};

#endif /*CBMLITSUBSTATION_H_*/
