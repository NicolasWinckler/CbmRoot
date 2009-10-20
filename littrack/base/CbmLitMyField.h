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

#endif /* CBMLITMYFIELD_H_ */
