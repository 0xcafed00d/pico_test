#ifndef PICO_TEST_H_0xCAFED00D
#define PICO_TEST_H_0xCAFED00D

#include <cstddef>
#include <exception>
#include <functional>
#include <iostream>
#include <ostream>
#include <span>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace pico_test {

	class Failure : public std::runtime_error {
	   public:
		using std::runtime_error::runtime_error;
	};

	inline void fail(std::string_view message) {
		throw Failure(std::string(message));
	}

	inline void expect(bool condition, std::string_view message) {
		if (!condition) {
			fail(message);
		}
	}

	template <typename T, typename U>
	void expect_eq(const T& actual, const U& expected, std::string_view message) {
		if (!(actual == expected)) {
			std::ostringstream out;
			out << message << " (expected " << expected << ", got " << actual << ")";
			throw Failure(out.str());
		}
	}

	template <typename Function>
	void should_throw(Function&& function, std::string_view message) {
		try {
			function();
		} catch (...) {
			return;
		}

		std::ostringstream out;
		out << message << " (expected exception)";
		throw Failure(out.str());
	}

	template <typename Exception, typename Function>
	void should_throw(Function&& function, std::string_view message) {
		try {
			function();
		} catch (const Exception&) {
			return;
		} catch (...) {
			std::ostringstream out;
			out << message << " (unexpected exception type)";
			throw Failure(out.str());
		}

		std::ostringstream out;
		out << message << " (expected exception)";
		throw Failure(out.str());
	}

	template <typename Function>
	void should_not_throw(Function&& function, std::string_view message) {
		try {
			function();
		} catch (const std::exception& ex) {
			std::ostringstream out;
			out << message << " (unexpected exception: " << ex.what() << ")";
			throw Failure(out.str());
		} catch (...) {
			std::ostringstream out;
			out << message << " (unexpected non-standard exception)";
			throw Failure(out.str());
		}
	}

	struct TestCase {
		const char* name;
		std::function<void()> function;
	};

	inline int run_tests(std::string_view suite_name,
	                     std::span<const TestCase> tests,
	                     std::ostream& out = std::cout,
	                     std::ostream& err = std::cerr) {
		std::size_t failures = 0;

		out << "Testing " << suite_name << '\n';
		for (const auto& test : tests) {
			try {
				test.function();
				out << "[PASS] " << test.name << '\n';
			} catch (const std::exception& ex) {
				++failures;
				err << "[FAIL] " << test.name << ": " << ex.what() << '\n';
			} catch (...) {
				++failures;
				err << "[FAIL] " << test.name << ": unknown exception\n";
			}
		}

		out << (tests.size() - failures) << '/' << tests.size() << " tests passed\n";

		return failures == 0 ? 0 : 1;
	}

}  // namespace pico_test

#endif /* PICO_TEST_H_0xCAFED00D */
