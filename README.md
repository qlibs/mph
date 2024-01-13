<a href="http://www.boost.org/LICENSE_1_0.txt" target="_blank">![Boost Licence](http://img.shields.io/badge/license-boost-blue.svg)</a>
<a href="https://github.com/boost-ext/mph/releases" target="_blank">![Version](https://badge.fury.io/gh/boost-ext%2Fmph.svg)</a>
<a href="https://github.com/boost-ext/mph/actions/workflows/build.yml" target="_blank">![build](https://github.com/boost-ext/mph/actions/workflows/build.yml/badge.svg)</a>
<a href="https://godbolt.org/z/YrErzz59T">![Try it online](https://img.shields.io/badge/try%20it-online-blue.svg)</a>

---------------------------------------

## Minimal perfect hash function (focused on the run-time performance)

> https://en.wikipedia.org/wiki/Perfect_hash_function#Minimal_perfect_hash_function

### Use case

> Given a list of N keys (strings known at compile-time) find a perfect hash function (maps keys into range from 0 to N-1) with the fastest run-time execution.

### Features

- Single header/module (https://raw.githubusercontent.com/boost-ext/mph/main/mph)
- Flexible (see [#API](#api))
- Fast (see [#Benchmarks](#benchmarks))

### Requirements

- C++20 ([gcc-12+](https://github.com/boost-ext/mph/actions/workflows/build.yml), [clang-15+](https://github.com/boost-ext/mph/actions/workflows/build.yml))

### Usage

> Running tests

```sh
cmake -S . -B build -DMPH_ENABLE_SANITIZERS=ON
cmake --build build
```

> Running benchmarks

```sh
cmake -S. -B build -DCMAKE_BUILD_TYPE=RELEASE -DMPH_BUILD_BENCHMARKS=ON
cmake --build build
```

---

### Hello world (https://godbolt.org/z/rqYz781ev)

```cpp
enum class color {
  unknown = 0,
  red     = 1,
  green   = 2,
  blue    = 3,
};

constexpr auto colors = std::array{
  std::pair{"red"sv,   color::red},
  std::pair{"green"sv, color::green},
  std::pair{"blue"sv,  color::blue},
};

std::print("{}", mph::hash<color::unknown, [] { return colors; }>("green"sv)); // prints 2
```

---

### Performance

```cpp
int main([[maybe_unused]] int argc, const char** argv) {
  using std::literals::operator""sv;

  constexpr std::array symbols{
    std::pair{"AAPL    "sv, 1},
    std::pair{"AMZN    "sv, 2},
    std::pair{"GOOGL   "sv, 3},
    std::pair{"MSFT    "sv, 4},
    std::pair{"NVDA    "sv, 5},
  };

  constexpr auto not_found = 0;
  constexpr auto max_bits_size = 7;

  constexpr auto hash = mph::hash<
    not_found,
    [] { return symbols; },
    []<const auto unknown, const auto symbols>(auto&&... args) {
      constexpr auto pext = mph::pext<max_bits_size, mph::branchless>{};
      return pext.template operator()<unknown, symbols>(std::forward<decltype(args)>(args)...);
    }
  >;

  constexpr auto len = 8u;
  assert(argc > 1);
  return hash(std::span<const char, len>(argv[1], argv[1] + len));
}
```

### x86-64 assembly (https://godbolt.org/z/E5KbbT76a)

```
main:
  movq 8(%rsi), %rax
  movl $436207616, %edx
  movq (%rax), %rax
  pext %rdx, %rax, %rdx
  cmpq %rax, mph::v_1_0_0::pext<7ul, mph::v_1_0_0::branchless::{lambda(bool, auto:1, auto:2)#1}{}>::operator()<0, main::{lambda()#1}{}, unsigned long, 436207616ul, std::span<char const, 8ul> const&>(std::span<char const, 8ul> const&) const::lookup(,%rdx,8)
  movl $0, %eax
  cmove mph::v_1_0_0::pext<7ul, mph::v_1_0_0::branchless::{lambda(bool, auto:1, auto:2)#1}{}>::operator()<0, main::{lambda()#1}{}, unsigned long, 436207616ul, std::span<char const, 8ul> const&>(std::span<char const, 8ul> const&) const::index(,%rdx,4), %eax
  ret
mph::v_1_0_0::pext<7ul, mph::v_1_0_0::branchless::{lambda(bool, auto:1, auto:2)#1}{}>::operator()<0, main::{lambda()#1}{}, unsigned long, 436207616ul, std::span<char const, 8ul> const&>(std::span<char const, 8ul> const&) const::index:
  .long 5
  .long 3
  .long 1
  .long 2
  .long 4
  .long 0
  .long 0
  .long 0
mph::v_1_0_0::pext<7ul, mph::v_1_0_0::branchless::{lambda(bool, auto:1, auto:2)#1}{}>::operator()<0, main::{lambda()#1}{}, unsigned long, 436207616ul, std::span<char const, 8ul> const&>(std::span<char const, 8ul> const&) const::lookup:
  .quad 2314885531374474830
  .quad 2314885720454418247
  .quad 2314885531559805249
  .quad 2314885531594018113
  .quad 2314885531693372237
  .zero 24
```

---

### Benchmarks ([v1.0.0](https://github.com/boost-ext/mph/tree/v1.0.0))

> `clang++-16 -std=c++20 -Ofast -DNDEBUG -march=skylake benchmark.cpp`

```
|  ns/op |            op/s |  err% |  total | benchmark
|-------:|----------------:|------:|-------:|:----------
|  24.98 |   40,029,546.84 |  0.7% |   0.30 | `random_5_len_4.std.map`
|  26.22 |   38,136,489.88 |  0.5% |   0.31 | `random_5_len_4.std.unordered_map`
|   8.70 |  114,877,210.89 |  0.3% |   0.10 | `random_5_len_4.boost.unordered_map`
|  27.51 |   36,355,849.54 |  0.2% |   0.33 | `random_5_len_4.boost.flat_map`
|  16.02 |   62,417,208.84 |  0.4% |   0.19 | `random_5_len_4.gperf`
|   3.29 |  304,198,779.79 |  0.4% |   0.04 | `random_5_len_4.mph`
|  30.01 |   33,325,429.88 |  0.3% |   0.36 | `random_5_len_8.std.map`
|  21.27 |   47,022,378.77 |  0.1% |   0.25 | `random_5_len_8.std.unordered_map`
|  19.45 |   51,407,017.79 |  0.2% |   0.23 | `random_5_len_8.boost.unordered_map`
|  34.04 |   29,375,189.14 |  0.3% |   0.41 | `random_5_len_8.boost.flat_map`
|  15.30 |   65,352,367.13 |  0.1% |   0.18 | `random_5_len_8.gperf`
|   3.28 |  304,843,585.79 |  0.2% |   0.04 | `random_5_len_8.mph`
|  48.78 |   20,498,534.52 |  0.9% |   0.60 | `random_6_len_2_5.std.map`
|  30.33 |   32,975,843.91 |  0.3% |   0.36 | `random_6_len_2_5.std.unordered_map`
|  28.15 |   35,527,933.59 |  0.1% |   0.34 | `random_6_len_2_5.boost.unordered_map`
|  53.44 |   18,713,131.25 |  0.3% |   0.64 | `random_6_len_2_5.boost.flat_map`
|   6.45 |  154,935,249.74 |  0.1% |   0.08 | `random_6_len_2_5.gperf`
|   3.89 |  257,239,974.93 |  0.3% |   0.05 | `random_6_len_2_5.mph`
|  65.49 |   15,269,985.09 |  0.2% |   0.78 | `random_100_len_8.std.map`
|  30.38 |   32,918,730.85 |  0.3% |   0.36 | `random_100_len_8.std.unordered_map`
|  20.97 |   47,681,569.10 |  0.2% |   0.25 | `random_100_len_8.boost.unordered_map`
|  63.38 |   15,777,214.53 |  0.1% |   0.76 | `random_100_len_8.boost.flat_map`
|  20.29 |   49,276,498.42 |  0.2% |   0.24 | `random_100_len_8.gperf`
|   4.04 |  247,291,383.66 |  0.2% |   0.05 | `random_100_len_8.mph`
| 109.81 |    9,106,852.49 |  0.0% |   1.31 | `random_100_len_1_8.std.map`
|  46.93 |   21,307,068.48 |  0.3% |   0.56 | `random_100_len_1_8.std.unordered_map`
|  37.75 |   26,486,730.39 |  0.3% |   0.45 | `random_100_len_1_8.boost.unordered_map`
| 101.29 |    9,872,843.84 |  0.2% |   1.22 | `random_100_len_1_8.boost.flat_map`
|  21.04 |   47,517,530.79 |  0.2% |   0.25 | `random_100_len_1_8.gperf`
|   5.35 |  186,969,332.95 |  0.1% |   0.06 | `random_100_len_1_8.mph`
```

> `g++-12 -std=c++20 -Ofast -DNDEBUG -march=skylake benchmark.cpp`

```cpp
|  ns/op |            op/s |  err% |  total | benchmark
|-------:|----------------:|------:|-------:|:----------
|  25.14 |   39,778,176.57 |  0.2% |   0.30 | `random_5_len_4.std.map`
|  25.16 |   39,739,318.14 |  0.6% |   0.30 | `random_5_len_4.std.unordered_map`
|   8.22 |  121,703,338.98 |  0.5% |   0.10 | `random_5_len_4.boost.unordered_map`
|  27.77 |   36,010,157.40 |  0.3% |   0.33 | `random_5_len_4.boost.flat_map`
|  16.13 |   61,985,582.63 |  0.4% |   0.19 | `random_5_len_4.gperf`
|   2.66 |  376,360,632.83 |  0.2% |   0.03 | `random_5_len_4.mph`
|  30.27 |   33,034,412.64 |  0.6% |   0.36 | `random_5_len_8.std.map`
|  19.75 |   50,645,196.71 |  0.1% |   0.24 | `random_5_len_8.std.unordered_map`
|  18.43 |   54,258,799.08 |  0.1% |   0.22 | `random_5_len_8.boost.unordered_map`
|  33.27 |   30,054,048.78 |  0.4% |   0.40 | `random_5_len_8.boost.flat_map`
|  16.53 |   60,479,393.81 |  0.2% |   0.20 | `random_5_len_8.gperf`
|   2.66 |  375,638,666.00 |  0.3% |   0.03 | `random_5_len_8.mph`
|  42.61 |   23,467,168.55 |  0.1% |   0.51 | `random_6_len_2_5.std.map`
|  33.27 |   30,060,501.50 |  1.6% |   0.40 | `random_6_len_2_5.std.unordered_map`
|  26.66 |   37,509,837.59 |  0.1% |   0.32 | `random_6_len_2_5.boost.unordered_map`
|  51.79 |   19,308,040.23 |  0.4% |   0.62 | `random_6_len_2_5.boost.flat_map`
|   6.20 |  161,281,897.93 |  0.1% |   0.07 | `random_6_len_2_5.gperf`
|   4.09 |  244,671,904.00 |  0.6% |   0.05 | `random_6_len_2_5.mph`
|  61.95 |   16,140,954.78 |  0.1% |   0.74 | `random_100_len_8.std.map`
|  28.82 |   34,692,661.89 |  0.0% |   0.35 | `random_100_len_8.std.unordered_map`
|  20.08 |   49,792,117.35 |  0.5% |   0.24 | `random_100_len_8.boost.unordered_map`
|  65.47 |   15,274,329.71 |  0.2% |   0.78 | `random_100_len_8.boost.flat_map`
|  17.80 |   56,174,321.04 |  0.1% |   0.21 | `random_100_len_8.gperf`
|   3.64 |  274,444,635.44 |  0.4% |   0.04 | `random_100_len_8.mph`
|  99.79 |   10,020,949.63 |  0.2% |   1.19 | `random_100_len_1_8.std.map`
|  48.71 |   20,528,601.73 |  0.4% |   0.58 | `random_100_len_1_8.std.unordered_map`
|  35.09 |   28,501,348.31 |  0.3% |   0.42 | `random_100_len_1_8.boost.unordered_map`
| 110.99 |    9,009,895.76 |  0.1% |   1.32 | `random_100_len_1_8.boost.flat_map`
|  16.14 |   61,945,137.67 |  0.2% |   0.19 | `random_100_len_1_8.gperf`
|   5.00 |  200,018,769.41 |  0.4% |   0.06 | `random_100_len_1_8.mph`
```

---

### API

```cpp
/**
 * Minimal perfect hashing function
 *
 * @tparam unknown returned if there is no match
 * @tparam symbols constexpr pair of id values such as std::array{{std::pair{"FOO"}, 1}, std::pair{"BAR"}, 2}}
 * @tparam policies invocable which returns the hash
 * @param data continuous input data such as std::string_view, std::span, std::array
 * @param args... optional args propagated to policies
 * @return result of executing policies
 */
template<const auto unknown, const auto symbols, const auto policies = mph::policies>
  requires (std::size(symbols()) > 0u) and std::same_as<decltype(utility::value(symbols()[0])), decltype(unknown)>
constexpr auto hash = [] [[nodiscard]] (auto&& data, auto &&...args) noexcept(true);
```

> Policies

```cpp
/**
 * Default policies
 *
 * @tparam unknown returned if there is no match
 * @tparam symbols constexpr pair of id values such as std::array{{std::pair{"FOO"}, 1}, std::pair{"BAR"}, 2}}
 * @param args... args propagated to policies
 * @return second from matched symbol or unknown if not matched
 */
constexpr auto policies = []<const auto unknown, const auto symbols>(auto&&... args) {
  if constexpr (constexpr auto min_max = utility::min_max_length<symbols>; min_max.first == min_max.second and min_max.first == sizeof(std::uint32_t) and std::size(symbols()) < 4u) {
    return mph::swar<std::uint32_t>{}.template operator()<unknown, symbols>(std::forward<decltype(args)>(args)...);
  } else if constexpr (min_max.first == min_max.second and min_max.first == sizeof(std::uint64_t) and std::size(symbols()) < 4u) {
    return mph::swar<std::uint64_t>{}.template operator()<unknown, symbols>(std::forward<decltype(args)>(args)...);
  } else if constexpr (constexpr auto pext = mph::pext<7u>{}; requires { pext.template operator()<unknown, symbols>(std::forward<decltype(args)>(args)...); }) {
    return pext.template operator()<unknown, symbols>(std::forward<decltype(args)>(args)...);
  } else if constexpr (constexpr auto pext_split = mph::pext_split<7u, utility::find_unique_char_max_dist<symbols>>{}; requires { pext_split.template operator()<unknown, symbols>(std::forward<decltype(args)>(args)...); }) {
    return pext_split.template operator()<unknown, symbols>(std::forward<decltype(args)>(args)...);
  } else {
    static_assert(false, "hash can't be created with given policies!");
  }
};
```

```cpp
/**
 * Minimal perfect hashing function based on SWAR
 *  reads sizeof(T) bytes and switches on that
 */
template<class T>
class swar {
 public:
  template <const auto unknown, const auto symbols>
    requires concepts::all_symbols_size_lt<symbols, sizeof(T)>
  [[nodiscard]] constexpr auto operator()(auto&& data, [[maybe_unused]] auto &&...args) const noexcept(true);
};
```

```cpp
/**
 * Minimal perfect hashing function based on intel's pext with support up to 2^max_bits_size elements and with max 8 characters
 *  requires platform with bmi2 support (https://en.wikipedia.org/wiki/X86_Bit_manipulation_instruction_set)
 */
template <const std::size_t max_bits_size, const auto result_policy = conditional>
class pext {
 public:
  template <const auto unknown, const auto symbols, class T = std::conditional_t<(utility::max_length<symbols> <= sizeof(std::uint32_t)), std::uint32_t, std::uint64_t>, const auto mask = utility::find_mask<T>(symbols())>
    requires concepts::bits_size_le<mask, max_bits_size> and concepts::all_symbols_size_lt<symbols, sizeof(T)>
  [[nodiscard]] [[gnu::target("bmi2")]] auto operator()(auto&& data, [[maybe_unused]] auto &&...args) const noexcept(true);
};
```

```cpp
/**
 * Minimal perfect hashing function based on intel's pext with support up to 2^max_bits_size per split on N'th character and with max 8 characters
 *  requires platform with bmi2 support (https://en.wikipedia.org/wiki/X86_Bit_manipulation_instruction_set)
 */
template <const std::size_t max_bits_size, const auto N, const auto result_policy = conditional>
class pext_split {
 public:
  template <const auto unknown, const auto symbols, class T = std::conditional_t<(utility::max_length<symbols> <= sizeof(std::uint32_t)), std::uint32_t, std::uint64_t>, const auto masks = make_masks<T, symbols>()>
    requires concepts::symbols_bits_size_lt<masks, max_bits_size> and concepts::all_symbols_size_lt<symbols, sizeof(T)>
  [[nodiscard]] [[gnu::target("bmi2")]] auto operator()(const auto data, [[maybe_unused]] auto &&...args) const noexcept(true);
};
```

> Configuration

```cpp
#define MPH 1'0'0 // Current library version (SemVer)
#define MPH_CACHE_LINE_SIZE ::std::hardware_constructive_interference_size // [default] 64u
#define MPH_ALLOW_UNSAFE_MEMCPY 1 // [enabled by default] Faster but potentially unsafe memcpy
#define MPH_PAGE_SIZE 4096u // Only used if MPH_ALLOW_UNSAFE_MEMCPY is enabled
```

---

### FAQ

- Why not `std::unordered_map`?

    > std::unordered_map is a general purpose hash map. `mph` is neither a hash map nor is general purpose.
      To use `mph` keys have to be known at compile-time so that this knowledge can be used to generate
      code with maximized performance - which is the main goal of the library.

- How `mph` works under the hood?

    > `mph` takes advantage of knowing the key/value pairs at compile-time as well as the specific hardware instructions.
      `mph` evaluates, at compile-time, which policies can be used and which will deliver the fastest performance.
      `mph, then, picks the 'best' one and apply input data to it.

- Can I do better than `mph` (performance wise)?

    > Of course! The more knowledge of the input data and the hardware the better potential performance.
      `mph` is a library with flexibility in mind as there is no silver-bullet to the performance (Always measure!).
      `mph` allows customization (See [API](#api)) for specific use-cases/platforms/etc.
      The main idea is to have different, specialized policies which will perform best in specific circumstances and
      can be chosen at compile-time based on given list of key/value paris.

- Is `mph` working on other platforms than x86-64?

    > In priniple the design is not platform specific and it depeonds on available policies.
      Some policies are platform specific such as `pext` as it requires [bmi2](https://en.wikipedia.org/wiki/X86_Bit_manipulation_instruction_set) support.

- How long it the compilation takes with `mph`?

    > Depending on the number of keys/symbols and policy used the compilation time may vary. Most use cases should compile in miliesconds/seconds on both gcc/clang.

- Is C++20 required to use `mph`?

    > Yes, C++20 support of concepts, constexpr std::vector, NTTP and other compile-time features made the implementatin of the library possible.
      However, it's doable to implement `mph` with standard below C++20, although it would require more effort.

- Do I need modules support to use `mph`?

    > No. `mph` can be either included or imported.

- How to get the max performance out of `mph`?

    > Experiment and measure in the production like environment with policies (See #api).
      For fastest performance consider aligning the input data and passing it with compilie-time size via std::span, std::array.

- Why key/value pairs are passed by lambda (`[]{ return symbols; }`)?

    > C++ only allows to pass an array of variable and/or functions via NTTP (https://godbolt.org/z/4xeKKoM9Y).

- I'm getting compilation error with longer list (>256) of symbols?

    > Likely the constexpr limit computation has been reached. To fix that, the following options can be used to increase the limits.

    ```
    gcc:   -fconstexpr-ops-limit=100000000
    clang: -fconstexpr-steps=100000000
    ```

- I'm getting sanitizers warnings/errors?

    > When passing run-time size input and when `MPH_ALLOW_UNSAFE_MEMCPY` is enabled (default) `mph` will do potentially unsafe memory reads.
      If you concern about the performance verify that the unsafe memory reads are okay (see `MPH_PAGE_SIZE`), otherwise set `MPH_ALLOW_UNSAFE_MEMCPY=0`
      to disable them which will fix the sanitzers warnings.

- Ideas for policies?

    > [radix-tree](https://en.wikipedia.org/wiki/Radix_tree), [finite-state-machine](https://en.wikipedia.org/wiki/Finite-state_machine), [gperf](https://www.dre.vanderbilt.edu/~schmidt/PDF/C++-USENIX-90.pdf), [mph](http://stevehanov.ca/blog/index.php?id=119), [hash](https://en.wikipedia.org/wiki/Hash_function), [simd](https://en.wikipedia.org/wiki/Single_instruction,_multiple_data)

- Similar projects?

    > [gperf](https://www.gnu.org/software/gperf), [frozen](https://github.com/serge-sans-paille/frozen)

- How can I contribute?

    > Please follow [CONTRIBUTING.md](.github/CONTRIBUTING.md)

- Acknowledgments / Inspirations

    > http://0x80.pl, https://lemire.me/blog, https://www.youtube.com/watch?v=DMQ_HcNSOAI&ab_channel=strager, https://www.dre.vanderbilt.edu/~schmidt/PDF/C++-USENIX-90.pdf, https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html, https://gcc.gnu.org/onlinedocs/libstdc++

---

**Disclaimer** `mph` is not an official Boost library.
