<a href="http://www.boost.org/LICENSE_1_0.txt" target="_blank">![Boost Licence](http://img.shields.io/badge/license-boost-blue.svg)</a>
<a href="https://github.com/boost-ext/mph/releases" target="_blank">![Version](https://badge.fury.io/gh/boost-ext%2Fmph.svg)</a>
<a href="https://godbolt.org/z/M747ocGfx">![build](https://img.shields.io/badge/build-blue.svg)</a>
<a href="https://godbolt.org/z/a6czaG8f7">![Try it online](https://img.shields.io/badge/try%20it-online-blue.svg)</a>

---------------------------------------

## Perfect hash function

> https://en.wikipedia.org/wiki/Perfect_hash_function#Perfect_hash_function

### Use case

> Given a list of N keys (known at compile-time) find a perfect hash function (maps keys into range from 0 to N-1) with the fastest run-time execution.

### Features

- Single header/module (https://raw.githubusercontent.com/boost-ext/mph/main/mph)
- Self verfication upon include (can be disabled by `DISABLE_STATIC_ASSERT_TESTS`)
- Compiles cleanly with ([`-Wall -Wextra -Werror -pedantic -pedantic-errors`]())
- Flexible (see [#API](#api))
- Fast (see [#Benchmarks](#benchmarks))

### Requirements

- C++20 ([gcc-12+](), [clang-15+]())

### Hello world

> Strings (https://godbolt.org/z/nfTWM1544)

```cpp
enum class color { red = 1, green = 2, blue = 3 };

constexpr auto colors = std::array{
  std::pair{mph::fixed_string{"red"}, color::red},
  std::pair{mph::fixed_string{"green"}, color::green},
  std::pair{mph::fixed_string{"blue"}, color::blue},
};

static_assert(color::green == mph::hash<colors, color(0)>("green"));
static_assert(color::red   == mph::hash<colors, color(0)>("red"));
static_assert(color::blue  == mph::hash<colors, color(0)>("blue"));

std::print("{}", mph::hash<colors, color(0)>("green")); // prints 1
```

---

> Numbers (https://godbolt.org/z/3TW9M9YGq)

```cpp
constexpr auto primes = std::array{
  std::pair{2, 0}, std::pair{3, 1}, std::pair{5, 2},
  std::pair{7, 3}, std::pair{11, 4}, std::pair{13, 5},
  std::pair{17, 6}, std::pair{19, 7}, std::pair{23, 8},
  std::pair{29, 9}, std::pair{31, 10}, std::pair{37, 11},
  std::pair{41, 12}, std::pair{43, 13}, std::pair{47, 14},
};

std::print("{}", hash<primes, 0>(41)); // prints 12
```

---

### Performance

```cpp
int main(int argc, const char** argv) {
  constexpr auto symbols = std::array{
    std::pair{mph::fixed_string{"AAPL    "}, 1},
    std::pair{mph::fixed_string{"AMZN    "}, 2},
    std::pair{mph::fixed_string{"GOOGL   "}, 3},
    std::pair{mph::fixed_string{"MSFT    "}, 4},
    std::pair{mph::fixed_string{"NVDA    "}, 5},
  };

  return mph::hash<symbols, 0>(std::span<const char, 8u>(argv[1], argv[1] + 8u));
}
```

> x86-64 assembly (https://godbolt.org/z/3894jYa5T)

```
```

> `llvm-mca` () | `uiCA` ([https://uica.uops.info]())

```
```

### Performance [potentially unsafe]

> If `all` possible inputs are known AND can be found in the keys, then `unconditional` policy for the config can be used which will avoid one comparison

```cpp
int main(int argc, [[maybe_unused]] const char** argv) {
  static constexpr auto symbols = std::array{
    std::pair{std::array{'A', 'A', 'P', 'L', ' ', ' ', ' ', ' '}, 1},
    std::pair{std::array{'A', 'M', 'Z', 'N', ' ', ' ', ' ', ' '}, 2},
    std::pair{std::array{'G', 'O', 'O', 'G', 'L', ' ', ' ', ' '}, 3},
    std::pair{std::array{'M', 'S', 'F', 'T', ' ', ' ', ' ', ' '}, 4},
    std::pair{std::array{'N', 'V', 'D', 'A', ' ', ' ', ' ', ' '}, 5},
  };

  return mph::hash<symbols, 0, mph::unconditionl>(symbols[argc].first);
}
```

> x86-64 assembly (https://godbolt.org/z/s4bTrd6G4)

```
```

---

### Benchmarks (https://github.com/boost-ext/mph/tree/benchmark)

> `clang++-16 -std=c++20 -Ofast -DNDEBUG -march=skylake benchmark.cpp`

```
|  ns/op |            op/s |  err% |  total | benchmark
|-------:|----------------:|------:|-------:|:----------
|  24.98 |   40,029,546.84 |  0.7% |   0.30 | `random_strings_5_len_4.std.map`
|  26.22 |   38,136,489.88 |  0.5% |   0.31 | `random_strings_5_len_4.std.unordered_map`
|   8.70 |  114,877,210.89 |  0.3% |   0.10 | `random_strings_5_len_4.boost.unordered_map`
|  27.51 |   36,355,849.54 |  0.2% |   0.33 | `random_strings_5_len_4.boost.flat_map`
|  16.02 |   62,417,208.84 |  0.4% |   0.19 | `random_strings_5_len_4.gperf`
|   3.29 |  304,198,779.79 |  0.4% |   0.04 | `random_strings_5_len_4.mph`
|  30.01 |   33,325,429.88 |  0.3% |   0.36 | `random_strings_5_len_8.std.map`
|  21.27 |   47,022,378.77 |  0.1% |   0.25 | `random_strings_5_len_8.std.unordered_map`
|  19.45 |   51,407,017.79 |  0.2% |   0.23 | `random_strings_5_len_8.boost.unordered_map`
|  34.04 |   29,375,189.14 |  0.3% |   0.41 | `random_strings_5_len_8.boost.flat_map`
|  15.30 |   65,352,367.13 |  0.1% |   0.18 | `random_strings_5_len_8.gperf`
|   3.28 |  304,843,585.79 |  0.2% |   0.04 | `random_strings_5_len_8.mph`
|  48.78 |   20,498,534.52 |  0.9% |   0.60 | `random_strings_6_len_2_5.std.map`
|  30.33 |   32,975,843.91 |  0.3% |   0.36 | `random_strings_6_len_2_5.std.unordered_map`
|  28.15 |   35,527,933.59 |  0.1% |   0.34 | `random_strings_6_len_2_5.boost.unordered_map`
|  53.44 |   18,713,131.25 |  0.3% |   0.64 | `random_strings_6_len_2_5.boost.flat_map`
|   6.45 |  154,935,249.74 |  0.1% |   0.08 | `random_strings_6_len_2_5.gperf`
|   3.89 |  257,239,974.93 |  0.3% |   0.05 | `random_strings_6_len_2_5.mph`
|  65.49 |   15,269,985.09 |  0.2% |   0.78 | `random_strings_100_len_8.std.map`
|  30.38 |   32,918,730.85 |  0.3% |   0.36 | `random_strings_100_len_8.std.unordered_map`
|  20.97 |   47,681,569.10 |  0.2% |   0.25 | `random_strings_100_len_8.boost.unordered_map`
|  63.38 |   15,777,214.53 |  0.1% |   0.76 | `random_strings_100_len_8.boost.flat_map`
|  20.29 |   49,276,498.42 |  0.2% |   0.24 | `random_strings_100_len_8.gperf`
|   4.04 |  247,291,383.66 |  0.2% |   0.05 | `random_strings_100_len_8.mph`
| 109.81 |    9,106,852.49 |  0.0% |   1.31 | `random_strings_100_len_1_8.std.map`
|  46.93 |   21,307,068.48 |  0.3% |   0.56 | `random_strings_100_len_1_8.std.unordered_map`
|  37.75 |   26,486,730.39 |  0.3% |   0.45 | `random_strings_100_len_1_8.boost.unordered_map`
| 101.29 |    9,872,843.84 |  0.2% |   1.22 | `random_strings_100_len_1_8.boost.flat_map`
|  21.04 |   47,517,530.79 |  0.2% |   0.25 | `random_strings_100_len_1_8.gperf`
|   5.35 |  186,969,332.95 |  0.1% |   0.06 | `random_strings_100_len_1_8.mph`
```

> `g++-12 -std=c++20 -Ofast -DNDEBUG -march=skylake benchmark.cpp`

```cpp
|  ns/op |            op/s |  err% |  total | benchmark
|-------:|----------------:|------:|-------:|:----------
|  25.14 |   39,778,176.57 |  0.2% |   0.30 | `random_strings_5_len_4.std.map`
|  25.16 |   39,739,318.14 |  0.6% |   0.30 | `random_strings_5_len_4.std.unordered_map`
|   8.22 |  121,703,338.98 |  0.5% |   0.10 | `random_strings_5_len_4.boost.unordered_map`
|  27.77 |   36,010,157.40 |  0.3% |   0.33 | `random_strings_5_len_4.boost.flat_map`
|  16.13 |   61,985,582.63 |  0.4% |   0.19 | `random_strings_5_len_4.gperf`
|   2.66 |  376,360,632.83 |  0.2% |   0.03 | `random_strings_5_len_4.mph`
|  30.27 |   33,034,412.64 |  0.6% |   0.36 | `random_strings_5_len_8.std.map`
|  19.75 |   50,645,196.71 |  0.1% |   0.24 | `random_strings_5_len_8.std.unordered_map`
|  18.43 |   54,258,799.08 |  0.1% |   0.22 | `random_strings_5_len_8.boost.unordered_map`
|  33.27 |   30,054,048.78 |  0.4% |   0.40 | `random_strings_5_len_8.boost.flat_map`
|  16.53 |   60,479,393.81 |  0.2% |   0.20 | `random_strings_5_len_8.gperf`
|   2.66 |  375,638,666.00 |  0.3% |   0.03 | `random_strings_5_len_8.mph`
|  42.61 |   23,467,168.55 |  0.1% |   0.51 | `random_strings_6_len_2_5.std.map`
|  33.27 |   30,060,501.50 |  1.6% |   0.40 | `random_strings_6_len_2_5.std.unordered_map`
|  26.66 |   37,509,837.59 |  0.1% |   0.32 | `random_strings_6_len_2_5.boost.unordered_map`
|  51.79 |   19,308,040.23 |  0.4% |   0.62 | `random_strings_6_len_2_5.boost.flat_map`
|   6.20 |  161,281,897.93 |  0.1% |   0.07 | `random_strings_6_len_2_5.gperf`
|   4.09 |  244,671,904.00 |  0.6% |   0.05 | `random_strings_6_len_2_5.mph`
|  61.95 |   16,140,954.78 |  0.1% |   0.74 | `random_strings_100_len_8.std.map`
|  28.82 |   34,692,661.89 |  0.0% |   0.35 | `random_strings_100_len_8.std.unordered_map`
|  20.08 |   49,792,117.35 |  0.5% |   0.24 | `random_strings_100_len_8.boost.unordered_map`
|  65.47 |   15,274,329.71 |  0.2% |   0.78 | `random_strings_100_len_8.boost.flat_map`
|  17.80 |   56,174,321.04 |  0.1% |   0.21 | `random_strings_100_len_8.gperf`
|   3.64 |  274,444,635.44 |  0.4% |   0.04 | `random_strings_100_len_8.mph`
|  99.79 |   10,020,949.63 |  0.2% |   1.19 | `random_strings_100_len_1_8.std.map`
|  48.71 |   20,528,601.73 |  0.4% |   0.58 | `random_strings_100_len_1_8.std.unordered_map`
|  35.09 |   28,501,348.31 |  0.3% |   0.42 | `random_strings_100_len_1_8.boost.unordered_map`
| 110.99 |    9,009,895.76 |  0.1% |   1.32 | `random_strings_100_len_1_8.boost.flat_map`
|  16.14 |   61,945,137.67 |  0.2% |   0.19 | `random_strings_100_len_1_8.gperf`
|   5.00 |  200,018,769.41 |  0.4% |   0.06 | `random_strings_100_len_1_8.mph`
```

---

### API

```cpp
/**
 * Perfect hash function
 *
 * @tparam kv constexpr pair of id values such as std::array{{std::pair{"FOO"}, 1}, std::pair{"BAR"}, 2}}
 * @tparam unknown default value
 * @param key continuous input data such as std::string_view, std::span, std::array or intergral value
 */
template<auto kv, typename decltype(kv)::value_type::second_type unknown, auto policy = conditional>
[[nodiscard]] [[gnu::target("bmi2")]] constexpr auto hash(auto&& key) noexcept -> decltype(unknown);
```

> Policies

```cpp
inline constexpr auto conditional = [](const bool cond, const auto lhs, const auto rhs) { // default
  return cond ? lhs : rhs; // generates jmp (x86-64)
};
```

```cpp
inline constexpr auto unconditional = []([[maybe_unused]] const bool cond, const auto lhs, [[maybe_unused]] const auto rhs) {
  return lhs; // [unsafe] returns unconditionally
};
```

```cpp
inline constexpr auto likely = [](const bool cond, const auto lhs, const auto rhs) {
  if (cond) [[likely]] {
    return lhs;
  } else {
    return rhs;
  }
};
```

```cpp
inline constexpr auto unlikely = [](const bool cond, const auto lhs, const auto rhs) {
  if (cond) [[unlikely]] {
    return lhs;
  } else {
    return rhs;
  }
};
```

```cpp
template<auto Probablity>
inline constexpr auto conditional_probability = [](const bool cond, const auto lhs, const auto rhs) {
  if (__builtin_expect_with_probability(cond, 1, Probablity)) {
    return lhs;
  } else {
    return rhs;
  }
};
```

```cpp
inline constexpr auto branchless = [](const bool cond, const auto lhs, [[maybe_unused]] const auto rhs) {
  return cond * lhs; // generates cmov (x86-64)
};
```

> Configuration

```cpp
#define MPH 2'0'0 // Current library version (SemVer)
#define MPH_FIXED_STRING_MAX_SIZE 8u // [default]
#define MPH_PAGE_SIZE 4096u // only used for string-like keys
```

---

### FAQ

- How `mph` works under the hood?

    > `mph` takes advantage of knowing the key/value pairs at compile-time as well as the specific hardware instructions.
      `mph` evaluates, at compile-time, which policies can be used and which will deliver the fastest performance.
      `mph`, then, picks the 'best' one and apply input data to it.

- Is `mph` working on other platforms than x86-64?

    > In priniple the design is not platform specific and it depends on available policies.
      Some policies are platform specific such as `pext` as it requires [bmi2](https://en.wikipedia.org/wiki/X86_Bit_manipulation_instruction_set) support.

- How long it the compilation takes with `mph`?

    > Depending on the number of keys and chosen policies the compilation times may vary. Most of the use cases should compile in miliesconds/seconds on both gcc/clang.

- Is C++20 required to use `mph`?

    > Yes, C++20 support of concepts, constexpr std::vector, NTTP and other compile-time features made the implementatin of the library possible.
      However, it's doable to implement `mph` with standard below C++20, although it would require more effort.

- How to get the max performance out of `mph`?

    > Always measure! Custom polices might be a good place to start. For strings, consider aligning the input data and passing it with compile-time size via std::span, std::array.

- Ideas for policies?

    > [radix-tree](https://en.wikipedia.org/wiki/Radix_tree), [finite-state-machine](https://en.wikipedia.org/wiki/Finite-state_machine), [gperf](https://www.dre.vanderbilt.edu/~schmidt/PDF/C++-USENIX-90.pdf), [mph](http://stevehanov.ca/blog/index.php?id=119), [hash](https://en.wikipedia.org/wiki/Hash_function), [simd](https://en.wikipedia.org/wiki/Single_instruction,_multiple_data), [cmph](https://cmph.sourceforge.net/papers)

- Similar projects?

    > [gperf](https://www.gnu.org/software/gperf), [frozen](https://github.com/serge-sans-paille/frozen), [nbperf](https://github.com/rurban/nbperf), [cmph](https://cmph.sourceforge.net), [perfecthash](https://github.com/tpn/perfecthash), [LeMonHash](https://github.com/ByteHamster/LeMonHash), [PTHash](https://github.com/jermp/pthash), [ShockHash](https://github.com/ByteHamster/ShockHash), [BuRR](https://github.com/lorenzhs/BuRR), [hash-prospector](https://github.com/skeeto/hash-prospector)

- Acknowledgments

    > http://0x80.pl, https://lemire.me/blog, https://www.youtube.com/watch?v=DMQ_HcNSOAI&ab_channel=strager, https://www.dre.vanderbilt.edu/~schmidt/PDF/C++-USENIX-90.pdf, https://cmph.sourceforge.net/papers, https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html, https://gcc.gnu.org/onlinedocs/libstdc++, https://github.com/rurban/smhasher, http://stevehanov.ca/blog/index.php?id=119, https://nullprogram.com/blog/2018/07/31, https://github.com/tpn/pdfs/tree/master/Perfect%20Hashing

---

**Disclaimer** `mph` is not an official Boost library.
