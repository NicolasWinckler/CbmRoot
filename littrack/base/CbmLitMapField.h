/** CbmLitMapField.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 **
 **Interface to CBM field map.
 **/

#ifndef CBMLITMAPFIELD_H_
#define CBMLITMAPFIELD_H_

#include "CbmLitField.h"
#include "FairField.h"

class CbmLitMapField :public CbmLitField {
public:
	/* Constructor */
	CbmLitMapField(FairField* field):fField(field) {}

	/* Destructor */
	virtual ~CbmLitMapField(){};

	/* Inherited from CbmLitField */
	void GetFieldValue(myf x, myf y, myf z, CbmLitFieldValue& value) const {
		double pos[3] = {x, y, z};
		double B[3];
		fField->GetFieldValue(pos, B);
		value.SetBx(B[0]);
		value.SetBy(B[1]);
		value.SetBz(B[2]);
	}

	/* Inherited from CbmLitField */
	virtual std::string ToString() const {
		return "CbmLitMapField::ToString";
	}

private:
	FairField* fField;
};

#endif /* CBMLITMAPFIELD_H_ */
