#pragma once

#include <core/macro_defs.hpp>
#include <core/math/math_defs.hpp>

#include <any>
#include <initializer_list>
#include <vector>

namespace sdl {

namespace UtilityFunctions {

/**
* @returns the amount of time passed in seconds since the program started.
*/
real_t get_passed_time();

/**
* @returns the @param variant converted to a string in the best possible way.
*/
[[nodiscard]] String to_string(const std::any &variant);

/**
* Converts @param variant into a string in the best way possible and prints it to the standard output.
*/
void print(const std::any &variant = "");

/**
* Converts one or more arguments of any type into a string in the best way possible and prints it to the standard output.
*/
void print(const std::initializer_list<std::any> &variants);


/**
* Converts @param variant into a string in the best way possible and prints it to the error output.
*/
void print_err(const std::any &variant = "");

/**
* Converts one or more arguments of any type into a string in the best way possible and prints it to the error output.
*/
void print_err(const std::initializer_list<std::any> &variants);

/**
* Acts identical to print.
*/
void prints(const std::any &variant = "");

/**
* Prints one or more arguments to the console with a space between each argument to the standard output.
*/
void prints(const std::initializer_list<std::any> &variants);

/**
* Acts identical to print_err.
*/
void prints_err(const std::any &variant = "");

/**
* Prints one or more arguments to the console with a space between each argument to the error output.
*/
void prints_err(const std::initializer_list<std::any> &variants);

/**
* Splits the @param string using a @param delimiter and returns an array of the substrings.
*/
std::vector<String> split_string(const String &string, const String &delimiter);

#define PRINT_LINE(...) ::sdl::UtilityFunctions::print({__VA_ARGS__});
#define PRINTS_LINE(...) ::sdl::UtilityFunctions::prints({__VA_ARGS__});

}

}
