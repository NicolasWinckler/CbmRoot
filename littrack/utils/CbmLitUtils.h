#ifndef CBMLITUTILS_H_
#define CBMLITUTILS_H_

#include "TCanvas.h"

#include <string>
#include <sstream>


template <class T>
std::string ToString(
		const T& value)
{
	std::stringstream ss;
	ss.precision(5);
	ss << value;
	return ss.str();
}

void SaveCanvasAsImage(
		TCanvas* c,
		const std::string& dir);


#endif /* CBMLITUTILS_H_ */
