/** LitField.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 **/

#ifndef LITFIELD_H_
#define LITFIELD_H_

template<class T>
class LitFieldValue
{
public:
	// components of the magnetic field
	T Bx, By, Bz;
} _fvecalignment;



template<class T>
class LitFieldSlice
{
public:
	LitFieldSlice() {
		for(unsigned int i=0; i<10; i++) cx[i] = cy[i] = cz[i] = 0.;
    }

	void GetFieldValue(
			const T &x,
			const T &y,
			LitFieldValue<T> &B) const {
		T x2 = x*x;
		T y2 = y*y;
		T yx2 = y*x2;
		T xy2 = x*y2;
		T x3 = x2*x;
		T y3 = y2*y;
		B.Bx = cx[0] + cx[1]*x + cx[2]*y + cx[3]*x*y + cx[4]*x2 + cx[5]*y2 + cx[6]*xy2 + cx[7]*yx2 + cx[8]*x3 + cx[9]*y3;
		B.By = cy[0] + cy[1]*x + cy[2]*y + cy[3]*x*y + cy[4]*x2 + cy[5]*y2 + cy[6]*xy2 + cy[7]*yx2 + cy[8]*x3 + cy[9]*y3;
		B.Bz = cz[0] + cz[1]*x + cz[2]*y + cz[3]*x*y + cz[4]*x2 + cz[5]*y2 + cz[6]*xy2 + cz[7]*yx2 + cz[8]*x3 + cz[9]*y3;
    }

	T cx[10], cy[10], cz[10]; // polinom coefficients
	T Z; // Z position of the slice

	friend std::ostream & operator<<(std::ostream &strm, const LitFieldSlice &slice){
		strm << "LitFieldSlice: Z=" << slice.Z << ", cx=";
		for(unsigned int i = 0; i < 10; i++) strm << slice.cx[i] << " ";
		strm << std::endl;
		strm << "LitFieldSlice: cy=";
		for(unsigned int i = 0; i < 10; i++) strm << slice.cy[i] << " ";
		strm << std::endl;
		strm << "LitFieldSlice: cz=";
		for(unsigned int i = 0; i < 10; i++) strm << slice.cz[i] << " ";
		strm << std::endl;
		return strm;
	}

} _fvecalignment;



template<class T>
class LitFieldRegion
{
public:
  void Set(
		  const LitFieldValue<T> &B0,
		  const T B0z,
		  const LitFieldValue<T> &B1,
		  const T B1z,
		  const LitFieldValue<T> &B2,
		  const T B2z ) {
      z0 = B0z[0];
      T dz1 = B1z - B0z, dz2 = B2z - B0z;
      T det = rcp(T(dz1 * dz2 * (dz2 - dz1)));
      T w21 = -dz2 * det;
      T w22 = dz1 * det;
      T w11 = -dz2 * w21;
      T w12 = -dz1 * w22;

      T dB1 = B1.Bx - B0.Bx;
      T dB2 = B2.Bx - B0.Bx;
      cx0 = B0.Bx;
      cx1 = dB1 * w11 + dB2 * w12;
      cx2 = dB1 * w21 + dB2 * w22;

      dB1 = B1.By - B0.By;
      dB2 = B2.By - B0.By;
      cy0 = B0.By;
      cy1 = dB1 * w11 + dB2 * w12;
      cy2 = dB1 * w21 + dB2 * w22;

      dB1 = B1.Bz - B0.Bz;
      dB2 = B2.Bz - B0.Bz;
      cz0 = B0.Bz;
      cz1 = dB1 * w11 + dB2 * w12;
      cz2 = dB1 * w21 + dB2 * w22;
    }

	void Set(
			const LitFieldValue<T>& B0,
			const T B0z,
			const LitFieldValue<T>& B1,
			const T B1z) {
		z0 = B0z[0];
		T dzi = rcp(T(B1z - B0z));
		cx0 = B0.Bx;
		cy0 = B0.By;
		cz0 = B0.Bz;
		cx1 = (B1.Bx - B0.Bx) * dzi;
		cy1 = (B1.By - B0.By) * dzi;
		cz1 = (B1.Bz - B0.Bz) * dzi;
		cx2 = cy2 = cz2 = 0;
	}

	void Shift(
			T z) {
		T dz = z-z0;
		T cx2dz = cx2*dz;
		T cy2dz = cy2*dz;
		T cz2dz = cz2*dz;
		z0 = z[0];
		cx0+= ( cx1 + cx2dz )*dz;
		cy0+= ( cy1 + cy2dz )*dz;
		cz0+= ( cz1 + cz2dz )*dz;
		cx1+= cx2dz + cx2dz;
		cy1+= cy2dz + cy2dz;
		cz1+= cz2dz + cz2dz;
	}

	void GetFieldValue(
			const T &z,
			LitFieldValue<T> &B) const {
		T dz = z - z0;
		T dzdz = dz * dz;
		B.Bx = cx0 + cx1 * dz + cx2 * dzdz;
		B.By = cy0 + cy1 * dz + cy2 * dzdz;
		B.Bz = cz0 + cz1 * dz + cz2 * dzdz;
	}

	T cx0, cx1, cx2 ; // Bx(z) = cx0 + cx1*(z-z0) + cx2*(z-z0)^2
	T cy0, cy1, cy2 ; // By(z) = cy0 + cy1*(z-z0) + cy2*(z-z0)^2
	T cz0, cz1, cz2 ; // Bz(z) = cz0 + cz1*(z-z0) + cz2*(z-z0)^2
	T z0;
} _fvecalignment;


#endif /* LITFIELD_H_ */
