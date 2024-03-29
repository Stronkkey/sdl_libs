#pragma once

#include <core/string/string_def.hpp>

#define GET_LINE_PRETTY() "file: \"" + __FILE__ + "\" on line " + std::to_string(__LINE__)
#define CONDITION_THING(condition) String("Assert \"") + #condition + "\", in " + GET_LINE_PRETTY() + " failed."
#define TEST_CASE(condition) if (!(condition)) { _test_fail((CONDITION_THING(condition))); return false; }

namespace sdl {

namespace Tests {

class Test {
private:
	virtual bool _test() = 0;

protected:
	void _test_fail(const String &message = "");

public:
	bool run_test();
};

#define __OVERRIDE_TEST__(test_name) class test_name : public Test { private: bool _test() override; };

__OVERRIDE_TEST__(SuccessTest);
__OVERRIDE_TEST__(FailTest);

}
}
