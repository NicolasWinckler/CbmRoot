#ifndef CBMLITUTILS_H_
#define CBMLITUTILS_H_

class TCanvas;

#include <string>
#include <sstream>

namespace lit {

template <class T>
std::string ToString(
		const T& value)
{
	std::stringstream ss;
	ss.precision(5);
	ss << value;
	return ss.str();
}

/* Returns -1 if x<0, +1 if x>0, 0 if x==0 */
template <class T>
int Sign(
		const T& x)
{
	static const T ZERO = 0;
	return (x > ZERO) ? 1 : ((x < ZERO) ? -1 : 0);
}

void SaveCanvasAsImage(
		TCanvas* c,
		const std::string& dir);

}

#endif /* CBMLITUTILS_H_ */
