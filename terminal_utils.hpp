//  @file namespace_terminal.hpp
//  @brief namespace terminal designed for manipulating with linux terminal
//
//  @example std::cout << terminal::TEXTCOLOR_BLUE after this all typed in the
//       terminal will be blue
//  @example std::cout << terminal::TEXT_BOLD after this all typed in the
//       terminal will be bold
//  @example std::cout << terminal::RESET_ALL resets all to the default mode

#ifndef TERMINAL_UTILS_HPP
#define TERMINAL_UTILS_HPP

#include <ostream>

namespace terminal {

// Sets SGR(Select Graphic Rendition) parameters, including text colors.
enum SGR {
	RESET_ALL          = 0,
	TEXT_BOLD          = 1,
	TEXTCOLOR_BLACK    = 30,
	TEXTCOLOR_RED      = 31,
	TEXTCOLOR_GREEN    = 32,
	TEXTCOLOR_YELLOW   = 33,
	TEXTCOLOR_BLUE     = 34,
	TEXTCOLOR_MAGENDA  = 35,
	TEXTCOLOR_CYAN     = 36,
	TEXTCOLOR_WHITE    = 37,
	TEXTCOLOR_DEFAULT  = 39,
};

// operator<< overloaded
std::ostream& operator<<( std::ostream& stream, SGR code);

// Moves cursor to beginning of the line n (default 1) lines up
std::string CursorPreviousLine(int n = 1);

// Clears part of the Line. If n is 0 (or missing), clear from cursor to the
// end of the line. If n is 1, clear from cursor to beginning of the line.
// If n is 2, clear entire line. Cursor position does not change.
std::string EraseLine(int n = 2);

}  // namespace terminal

#endif  // TERMINAL_UTILS_HPP
