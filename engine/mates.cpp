#include "mates.h"

#include <string>
#include <sstream>

namespace Mates {

std::string ToStringWithPrecision(float a_value, int n)
{
	std::ostringstream out;
	out.precision(n);
	out << std::fixed << a_value;
	return out.str();
}

std::string ToHexa(int a_value)
{
	std::ostringstream out;
	out << std::hex << a_value;
	return out.str();
}

}

