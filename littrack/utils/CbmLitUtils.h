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
	ss.precision(3);
	ss << value;
	return ss.str();
}

void SaveCanvasAsImage(
		TCanvas* c)
{
	c->SaveAs(std::string(std::string(c->GetTitle()) + ".eps").c_str());
	c->SaveAs(std::string(std::string(c->GetTitle()) + ".gif").c_str());
}

#endif /* CBMLITUTILS_H_ */
