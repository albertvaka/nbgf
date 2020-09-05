#include "debug.h"

#include <iostream>

namespace Debug {

	DebugStream out;

	std::ostream& _forwarded_out = std::cout;

}

DebugStreamDelegate::~DebugStreamDelegate() {
	Debug::_forwarded_out << std::endl;
}
