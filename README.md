# Pico Test User Manual

`pico_test` is a small header-only C++ test harness. It is intended for projects
that want simple executable-based tests without bringing in a larger testing
framework.

The public API lives in:

```cpp
#include "pico_test.h"
```

Add this repository's `include` directory to your compiler include path.

## Requirements

- C++20 or newer
- A standard library with `<functional>`, `<iostream>`, `<span>`, `<stdexcept>`,
  `<string_view>`, and `<sstream>`

## Quick Start

```cpp
#include <span>

#include "pico_test.h"

void addition_works() {
	pico_test::expect_eq(2 + 2, 4, "basic addition should work");
}

void truth_check() {
	pico_test::expect(true, "condition should be true");
}

int main() {
	const pico_test::TestCase tests[] = {
	    {"addition works", addition_works},
	    {"truth check", truth_check},
	};

	return pico_test::run_tests("example", std::span<const pico_test::TestCase>{tests});
}
```

Example output:

```text
Testing example
[PASS] addition works
[PASS] truth check
2/2 tests passed
```

`run_tests` takes a `std::span<const pico_test::TestCase>` and returns `0` when
all tests pass or `1` when any test fails, so it can be used directly as
`main`'s return value.

## Defining Tests

Each test is a callable with no arguments and no return value:

```cpp
void my_test() {
	pico_test::expect(true, "this should pass");
}
```

Register tests with `pico_test::TestCase` and pass them to `run_tests` as a
span:

```cpp
const pico_test::TestCase tests[] = {
    {"my test", my_test},
};

return pico_test::run_tests("suite name", std::span<const pico_test::TestCase>{tests});
```

`TestCase::function` is a `std::function<void()>`, so captured lambdas are also
supported:

```cpp
int expected = 42;

const pico_test::TestCase tests[] = {
    {"captured lambda",
     [expected] {
	     pico_test::expect_eq(expected, 42, "captured value should match");
     }},
};
```

## Running Tests

`run_tests` accepts any contiguous sequence of test cases exposed as
`std::span<const pico_test::TestCase>`.

For a fixed array, construct a span:

```cpp
return pico_test::run_tests("suite name", std::span<const pico_test::TestCase>{tests});
```

For a `std::vector`:

```cpp
std::vector<pico_test::TestCase> tests;
return pico_test::run_tests("suite name", std::span<const pico_test::TestCase>{tests});
```

The runner also accepts optional output streams:

```cpp
std::ostringstream out;
std::ostringstream err;

int result = pico_test::run_tests(
    "quiet suite",
    std::span<const pico_test::TestCase>{tests},
    out,
    err);
```

The runner catches exceptions thrown by test functions:

- `std::exception` failures are printed with `what()`.
- Non-standard exceptions are reported as unknown exceptions.
- The runner continues after a failed test.

## Assertions

### `expect`

```cpp
pico_test::expect(condition, "failure message");
```

Passes when `condition` is `true`. Fails by throwing `pico_test::Failure` when
`condition` is `false`.

### `expect_eq`

```cpp
pico_test::expect_eq(actual, expected, "failure message");
```

Passes when `actual == expected`. On failure, the thrown message includes both
values:

```text
failure message (expected 4, got 5)
```

Both values must be printable to `std::ostream`.

### `fail`

```cpp
pico_test::fail("failure message");
```

Fails immediately by throwing `pico_test::Failure`.

## Exception Assertions

### `should_throw`

```cpp
pico_test::should_throw(
    [] {
	    throw std::runtime_error("expected");
    },
    "operation should throw");
```

Passes when the callable throws any exception. Fails when the callable returns
normally.

### Typed `should_throw`

```cpp
pico_test::should_throw<std::runtime_error>(
    [] {
	    throw std::runtime_error("expected");
    },
    "operation should throw runtime_error");
```

Passes only when the callable throws the requested exception type. Fails when no
exception is thrown or when a different exception type is thrown.

### `should_not_throw`

```cpp
pico_test::should_not_throw(
    [] {
	    // no exception
    },
    "operation should not throw");
```

Passes when the callable returns normally. Fails when it throws.

## Failure Type

All pico-test assertion failures throw:

```cpp
pico_test::Failure
```

`Failure` derives from `std::runtime_error`, so it can be caught as either
`pico_test::Failure`, `std::runtime_error`, or `std::exception`.

## Complete Example

```cpp
#include <stdexcept>
#include <span>

#include "pico_test.h"

int divide(int lhs, int rhs) {
	if (rhs == 0) {
		throw std::invalid_argument("division by zero");
	}
	return lhs / rhs;
}

void divides_numbers() {
	pico_test::expect_eq(divide(8, 2), 4, "8 / 2 should be 4");
}

void rejects_zero_divisor() {
	pico_test::should_throw<std::invalid_argument>(
	    [] {
		    divide(8, 0);
	    },
	    "division by zero should throw");
}

int main() {
	const pico_test::TestCase tests[] = {
	    {"divides numbers", divides_numbers},
	    {"rejects zero divisor", rejects_zero_divisor},
	};

	return pico_test::run_tests("math", std::span<const pico_test::TestCase>{tests});
}
```

## When To Use Pico Test

`pico_test` is a good fit for small libraries, smoke tests, examples that double
as tests, and projects that want tests to be plain executables.

It intentionally does not provide discovery, fixtures, parameterized tests,
matchers, command-line filtering, or rich reporting. If you need those features,
use a larger testing framework.
