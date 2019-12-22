#include "terminal_utils.hpp"

#include <sstream>
#include <string>

namespace {

// enable or disable color printing in linux terminal.
const bool kTerminalColorEnabled = true;

}  // namespace

namespace terminal {

// operator<< overloaded
std::ostream& operator<<( std::ostream& stream, SGR code) {
	if (kTerminalColorEnabled) {
		return stream << "\x1B[" << static_cast<int>(code) << "m";
	} else {
		return stream;
	}
}

// Moves cursor to beginning of the line n (default 1) lines up
std::string CursorPreviousLine(int n) {
	if (kTerminalColorEnabled) {
		std::ostringstream stringStream;
		stringStream << "\x1B[" << n << "F";
		return stringStream.str();
	} else {
		return "";
	}
}

// Clears part of the Line. If n is 0 (or missing), clear from cursor to the
// end of the line. If n is 1, clear from cursor to beginning of the line.
// If n is 2, clear entire line. Cursor position does not change.
std::string EraseLine(int n) {
	if (kTerminalColorEnabled) {
		std::ostringstream stringStream;
		stringStream << "\x1B[" << n << "K";
		return stringStream.str();
	} else {
		return "";
	}
}

}  // namespace terminal
