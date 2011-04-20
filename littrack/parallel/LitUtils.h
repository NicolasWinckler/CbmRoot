#ifndef LITUTILS_H_
#define LITUTILS_H_

#include <string>
#include <sstream>

namespace lit {
namespace parallel {

template <class T>
std::string ToString(
   const T& value)
{
   std::stringstream ss;
   ss.precision(5);
   ss << value;
   return ss.str();
}

} // namespace parallel
} // namespace lit
#endif /* LITUTILS_H_ */
