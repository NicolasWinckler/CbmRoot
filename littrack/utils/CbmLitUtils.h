#ifndef CBMLITUTILS_H_
#define CBMLITUTILS_H_

#include <string>
#include <sstream>

template <class T>
std::string ToString(
		const T& value)
{
	std::stringstream ss;
//	ss.precision(5);
	ss << value;
	return ss.str();
}

#endif /* CBMLITUTILS_H_ */
