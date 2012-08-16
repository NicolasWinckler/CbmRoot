/**
 * \file LitFieldValue.h
 * \brief Magnetic field value at a certain point in the space.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 */

#ifndef LITFIELDVALUE_H_
#define LITFIELDVALUE_H_

#include "LitTypes.h"
#include "LitUtils.h"
#include <string>

namespace lit {
namespace parallel {

/**
 * \class LitFieldValue
 * \brief Magnetic field value at a certain point in the space.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 */
template<class T>
class LitFieldValue
{
public:
   /**
    * \brief Returns std::string representation of the class.
    * \return Class representation as std::string.
    */
   std::string ToString() const {
      return "LitFieldValue: Bx=" + lit::parallel::ToString<T>(Bx)
            + ", By=" + lit::parallel::ToString<T>(By)
               + ", Bz=" + lit::parallel::ToString<T>(Bz) + std::endl;
   }

   /**
    * \brief Operator << for convenient output to std::ostream.
    * \return Insertion stream in order to be able to call a succession of insertion operations.
    */
   friend std::ostream& operator<<(std::ostream& strm, const LitFieldValue& v) {
      strm << v.ToString();
      return strm;
   }

public:
   // components of the magnetic field
   T Bx, By, Bz;
} _fvecalignment;

} // namespace parallel
} // namespace lit

#endif /* LITFIELDVALUE_H_ */
