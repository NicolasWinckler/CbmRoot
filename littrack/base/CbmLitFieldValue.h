/** CbmLitFieldValue.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 **
 ** Data class represents magnetic field value (Bx, By, Bz components)
 ** at each point in space.
 **/

#ifndef CBMLITFIELDVALUE_H_
#define CBMLITFIELDVALUE_H_

#include <sstream>
#include <string>

#include "CbmLitFloat.h"

class CbmLitFieldValue {
public:
	/* Constructor */
	CbmLitFieldValue(){};

	/* Constructor with assignment Bx, By, Bz
	 *@param Bx Bx component of the field
	 *@param By By component of the field
	 *@param Bz Bz component of the field
	 */
	CbmLitFieldValue(myf Bx, myf By, myf Bz):
		fBx(Bx), fBy(By), fBz(Bz) {};

	/* Destructor */
	virtual ~CbmLitFieldValue(){};

	/* @return Bx component of the field */
	myf GetBx() const {return fBx;}

	/* @return By component of the field */
	myf GetBy() const {return fBy;}

	/* @return Bz component of the field */
	myf GetBz() const {return fBz;}

	/* Sets Bx component of the field
	 * @param Bx Bx component of the field
	 */
	void SetBx(myf Bx) {fBx = Bx;}

	/* Sets By component of the field
	 * @param By By component of the field
	 */
	void SetBy(myf By) {fBy = By;}

	/* Sets Bz component of the field
	 * @param Bz Bz component of the field
	 */
	void SetBz(myf Bz) {fBz = Bz;}

	/* Sets Bx, By, Bz components of the field
	 * @param Bx Bx component of the field.
	 * @param By By component of the field
	 * @param Bz Bz component of the field.
	*/
	void SetBxByBz(myf Bx, myf By, myf Bz) {fBx = Bx; fBy = By; fBz = Bz;}

	/* Returns std::string representation of this class
	 * @return String representation of the class
	 */
	virtual std::string ToString() const {
		std::stringstream ss;
		ss << "FieldValue: Bx=" << GetBx() << ", By=" << GetBy() << ", Bz=" << GetBz() << std::endl;
		return ss.str();
	}

private:
	// components of the magnetic field
	myf fBx, fBy, fBz;
};

#endif /* CBMLITFIELDVALUE_H_ */
