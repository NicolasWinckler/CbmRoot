/** CbmLitMyField.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 **
 ** Class represents magnetic field slice in the XY plane.
 ** The field is approximated by the polynomial of the 3rd order.
 **/

#ifndef CBMLITFIELDSLICE_H_
#define CBMLITFIELDSLICE_H_

class CbmLitFieldSlice
{
public:
	/* Constructor */
	CbmLitFieldSlice() {
		for(int i = 0; i < 10; i++) fcx[i] = fcy[i] = fcz[i] = 0.;
    }

	/* Constructor with polynom coefficients.
	 *@param cx[] Polynom coefficients for Bx.
	 *@param cy[] Polynom coefficients for By.
	 *@param cz[] Polynom coefficients for Bz.
	 *@param z Z coordinate of the field slice [cm].
	 */
	CbmLitFieldSlice(myf cx[10], myf cy[10], myf cz[10], myf z) : fZ(z) {
		for(int i = 0; i < 10; i++) {
			fcx[i] = cx[i];
			fcy[i] = cy[i];
			fcz[i] = cz[i];
		}
    }

	/* Destructor */
	virtual ~CbmLitFieldSlice() {};

	/* Return the field value approximated by polynom.
	 * @param x X coordinate.
	 * @param y Y coordinate.
	 * @param B Output field value
	 */
	void GetFieldValue(myf x, myf y, CbmLitFieldValue &B) const {
		myf x2 = x*x;
		myf y2 = y*y;
		myf xy = x*y;
		myf x3 = x2*x;
		myf y3 = y2*y;
		myf xy2 = x*y2;
		myf x2y = x2*y;

		B.SetBx(fcx[0] + fcx[1]*x + fcx[2]*y + fcx[3]*x2 + fcx[4]*xy + fcx[5]*y2 + fcx[6]*x3 + fcx[7]*x2y + fcx[8]*xy2 + fcx[9]*y3);
		B.SetBy(fcy[0] + fcy[1]*x + fcy[2]*y + fcy[3]*x2 + fcy[4]*xy + fcy[5]*y2 + fcy[6]*x3 + fcy[7]*x2y + fcy[8]*xy2 + fcy[9]*y3);
		B.SetBz(fcz[0] + fcz[1]*x + fcz[2]*y + fcz[3]*x2 + fcz[4]*xy + fcz[5]*y2 + fcz[6]*x3 + fcz[7]*x2y + fcz[8]*xy2 + fcz[9]*y3);
    }

	/* Returns std::string representation of this class
	 * @return String representation of the class
	 */
	virtual std::string ToString() const {
		std::stringstream ss;
		ss << "FieldSlice:" << std::endl;
		ss << "cx=(";
		for (int i = 0; i < 10; i++) ss << fcx[i] << ", ";
		ss << std::endl;
		ss << "cy=(";
		for (int i = 0; i < 10; i++) ss << fcy[i] << ", ";
		ss << std::endl;
		ss << "cz=(";
		for (int i = 0; i < 10; i++) ss << fcz[i] << ", ";
		ss << std::endl;
		return ss.str();
	}

	/* @return Z position of the field slice. */
	myf GetZ() const {return fZ;}

	/* Sets Z coordinate of the field slice.
	 * @param z Z coordinate of the field slice [cm].
	 */
	void SetZ(myf z) {fZ = z;};

	/* @return Array of polynom coefficients for Bx approximation. */
	const myf* GetCx() const {return fcx;}

	/* @return Array of polynom coefficients for By approximation. */
	const myf* GetCy() const {return fcy;}

	/* @return Array of polynom coefficients for Bz approximation. */
	const myf* GetCz() const {return fcz;}

private:
	// polynom coefficients
	myf fcx[10], fcy[10], fcz[10];
	// Z position of the field slice [cm]
	myf fZ;
};

#endif /* CBMLITFIELDSLICE_H_ */
