/**
 * \class LitStation
 * \brief Detector station.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 */

#ifndef LITSTATION_H_
#define LITSTATION_H_

#include "LitMaterialGrid.h"
#include "LitFieldGrid.h"
#include <sstream>
using std::stringstream;
using std::ostream;

namespace lit {
namespace parallel {

/**
 * \class LitStation
 * \brief Detector station.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 *
 * Station stores Z position of its center material approximation
 * in silicon equivalent and field approximation.
 *
 */
template<class T>
class LitStation
{
public:
   /**
    * \brief Constructor.
    */
   LitStation():
      fMaterial(),
      fField(),
      fZ(0.) {}

   /**
    * \brief Destructor
    */
   virtual ~LitStation() {}

   /* Setters */
   void SetMaterial(const LitMaterialGrid& material) { fMaterial = material; }
   void SetField(const LitFieldGrid& field) { fField = field; }
   void SetZ(T z) { fZ = z; }

   /* Getters */
   const LitMaterialGrid& GetMaterial() const { return fMaterial; }
   const LitFieldGrid& GetField() const { return fField; }
   T GetZ() const { return fZ; }

   /**
    * \brief Returns string representation of the class.
    * \return String representation of the class.
    */
   string ToString() const {
      stringstream ss;
      ss << "LitStation: Z=" << GetZ() << "\n";
      ss << "   material: " << GetMaterial().ToString() << "\n";
      ss << "   field: " << GetField().ToString();
      return ss.str();
   }

   /**
    * \brief Operator << for convenient output to ostream.
    * \return Insertion stream in order to be able to call a succession of insertion operations.
    */
   friend ostream& operator<<(ostream& strm, const LitStation& station ) {
      strm << station.ToString();
      return strm;
   }

private:
   LitMaterialGrid fMaterial; // Material approximation
   LitFieldGrid fField; // Field approximation
   T fZ; // Z center of the station [cm]
} _fvecalignment;

/**
 * \typedef LitStation<fvec> LitStationVec
 * \brief Vector version of LitStation.
 */
typedef LitStation<fvec> LitStationVec;

/**
 * \typedef LitStation<fscal> LitStationScal
 * \brief Scalar version of LitStation.
 */
typedef LitStation<fscal> LitStationScal;

} // namespace parallel
} // namespace lit
#endif /* LITSTATION_H_ */
