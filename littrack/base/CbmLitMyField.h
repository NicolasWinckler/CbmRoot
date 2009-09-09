/** CbmLitMyField.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 **
 **/

#ifndef CBMLITMYFIELD_H_
#define CBMLITMYFIELD_H_

#include "CbmLitField.h"
#include "CbmLitFieldSlice.h"

#include <vector>

class CbmLitMyField : public CbmLitField {
public:
	/* Constructor */
	CbmLitMyField();

	/* Destructor */
	virtual ~CbmLitMyField();

	/* Inherited from CbmLitField */
	void GetFieldValue(myf x, myf y, myf z, CbmLitFieldValue& value) const;

	/* Inherited from CbmLitField */
	virtual std::string ToString() const {
		return "CbmLitMyField::ToString";
	}

private:
	/* Combines two field values to calculate a field value at a certain Z position
	 * between them. Linear approximation is used.
	 * @param B0 First field value.
	 * @param B0z Z position of the first field value.
	 * @param B1 Second field value.
	 * @param B1z Z position of the second field value.
	 * @param z Z position where to calculate field value.
	 * @param value Output field value.
	 */
	void Combine(
			const CbmLitFieldValue& B0,
			myf B0z,
			const CbmLitFieldValue& B1,
			myf B1z,
			myf z,
			CbmLitFieldValue& value) const;

	// Vector with field slices.
	std::vector<CbmLitFieldSlice> fSlices;
};


//class L1FieldRegion{
//
// public:
//
//  fvec cx0, cx1, cx2 ; // Bx(z) = cx0 + cx1*(z-z0) + cx2*(z-z0)^2
//  fvec cy0, cy1, cy2 ; // By(z) = cy0 + cy1*(z-z0) + cy2*(z-z0)^2
//  fvec cz0, cz1, cz2 ; // Bz(z) = cz0 + cz1*(z-z0) + cz2*(z-z0)^2
//  fvec z0;
//
//  void Set( const L1FieldValue &B0, const fvec B0z,
//	    const L1FieldValue &B1, const fvec B1z,
//	    const L1FieldValue &B2, const fvec B2z )
//    {
//      z0 = B0z[0];
//      fvec dz1 = B1z-B0z, dz2 = B2z-B0z;
//      fvec det = rcp(fvec(dz1*dz2*(dz2-dz1)));
//      fvec w21 = -dz2*det;
//      fvec w22 = dz1*det;
//      fvec w11 = -dz2*w21;
//      fvec w12 = -dz1*w22;
//
//      fvec dB1 = B1.x - B0.x;
//      fvec dB2 = B2.x - B0.x;
//      cx0 = B0.x;
//      cx1 = dB1*w11 + dB2*w12 ;
//      cx2 = dB1*w21 + dB2*w22 ;
//
//      dB1 = B1.y - B0.y;
//      dB2 = B2.y - B0.y;
//      cy0 = B0.y;
//      cy1 = dB1*w11 + dB2*w12 ;
//      cy2 = dB1*w21 + dB2*w22  ;
//
//      dB1 = B1.z - B0.z;
//      dB2 = B2.z - B0.z;
//      cz0 = B0.z;
//      cz1 = dB1*w11 + dB2*w12 ;
//      cz2 = dB1*w21 + dB2*w22   ;
//    }
//
//  void Set( const L1FieldValue &B0, const fvec B0z,
//	    const L1FieldValue &B1, const fvec B1z )
//    {
//      z0 = B0z[0];
//      fvec dzi = rcp(fvec( B1z - B0z));
//      cx0 = B0.x;
//      cy0 = B0.y;
//      cz0 = B0.z;
//      cx1 = ( B1.x - B0.x )*dzi;
//      cy1 = ( B1.y - B0.y )*dzi;
//      cz1 = ( B1.z - B0.z )*dzi;
//      cx2 = cy2 = cz2 = 0;
//    }
//
//  void Shift( fvec z)
//    {
//      fvec dz = z-z0;
//      fvec cx2dz = cx2*dz;
//      fvec cy2dz = cy2*dz;
//      fvec cz2dz = cz2*dz;
//      z0 = z[0];
//      cx0+= ( cx1 + cx2dz )*dz;
//      cy0+= ( cy1 + cy2dz )*dz;
//      cz0+= ( cz1 + cz2dz )*dz;
//      cx1+= cx2dz + cx2dz;
//      cy1+= cy2dz + cy2dz;
//      cz1+= cz2dz + cz2dz;
//    }

//};

#endif /* CBMLITMYFIELD_H_ */
