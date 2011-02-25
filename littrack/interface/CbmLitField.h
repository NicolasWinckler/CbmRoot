/** CbmLitField.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 **
 ** Interface for accessing the magnetic field.
 **/
#ifndef CBMLITFIELD_H_
#define CBMLITFIELD_H_

#include "base/CbmLitTypes.h"

class CbmLitField {
public:
	/* Constructor */
	CbmLitField(){};

	/* Destructor */
	virtual ~CbmLitField(){};

	/* Returns field value at (x,y,z) position
	 * @param x X coordinate [cm].
	 * @param y Y coordinate [cm].
	 * @param z Z coordinate [cm].
	 * @param Bx Output Bx field value [kGauss].
	 * @param By Output By field value [kGauss]
	 * @param Bz Output Bz field value [kGauss]
	 */
	virtual void GetFieldValue(
			myf x, myf y, myf z,
			myf &Bx, myf &By, myf &Bz) const = 0;

	/* Returns std::string representation of this class */
	virtual std::string ToString() const {
		return "CbmLitField::ToString";
	}
};

#endif /* CBMLITFIELD_H_ */
