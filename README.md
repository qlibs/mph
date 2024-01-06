<a href="http://www.boost.org/LICENSE_1_0.txt" target="_blank">![Boost Licence](http://img.shields.io/badge/license-boost-blue.svg)</a>
<a href="https://github.com/boost-ext/mph/releases" target="_blank">![Version](https://badge.fury.io/gh/boost-ext%2Fmph.svg)</a>
<a href="https://godbolt.org/z/dTYdec4jz">![Try it online](https://img.shields.io/badge/try%20it-online-blue.svg)</a>

---------------------------------------

## Minimal perfect hash function

> https://en.wikipedia.org/wiki/Perfect_hash_function#Minimal_perfect_hash_function

### Features

- Single header/module (https://raw.githubusercontent.com/boost-ext/mph/main/mph)
- Flexible (see [#Design](#design))
- Fast (see [#Benchmarks](#benchmarks))

### Requirements

- C++20 (gcc-12+, clang-16+)

### Design

```
hash<symbols, policies>(data) {
    for policy in policies:
        if r = policy<symbols>(data)
            return r;

    assert "hash can't be constructed with given policies!"
}
```

### Usage

```cpp
constexpr std::array symbols{
  "FBC"sv,
  "SPY"sv,
  "CDC"sv,
};

constexpr auto hash = mph::hash{[] { return symbols; }};

assert(0 == hash(""sv));    // not found
assert(0 == hash("FOO"sv)); // not found

assert(1 == hash("FBC"sv));
assert(2 == hash("SPY"sv));
assert(3 == hash("CDC"sv));
```

### x86-64 assembly (https://godbolt.org/z/dTYdec4jz)

```
main:
  movq 8(%rsi), %rax
  movl $1049600, %edx
  movq (%rax), %rax
  pext %rdx, %rax, %rax
  movzbl mph::v_1_0_0::hash<main::{lambda()#1}{}, mph::v_1_0_0::cfg{64ul, 5ul}, char const*>(char const*)::index(%rax), %eax
  ret
mph::v_1_0_0::hash<main::{lambda()#1}{}, mph::v_1_0_0::cfg{64ul, 5ul}, char const*>(char const*)::index:
  .byte 1
  .byte 3
  .byte 2
  .zero 1
```

### API

```
/**
 * Minimal perfect hash function (https://en.wikipedia.org/wiki/Perfect_hash_function#Minimal_perfect_hash_function)
 *
 * @tparam TSymbols compile time array of symbols
 * @tparam TPolicies compile time tuple of policies to be executed in given order
 */
template<class TSymbols,
         class TPolicies = std::tuple<pext_direct<5>, pext_split_first_char<5>>>
  requires (std::size(TSymbols{}()) > 0 and std::tuple_size_v<TPolicies> > 0)
class hash final {
  public:
    constexpr explicit(true) hash(TSymbols&& symbols, TPolicies&& policies = {})
      : symbols{std::move(symbols)}, policies{std::move(policies)}
    { }

    /**
     * @param data continuous input data
     * @param args... additonal parameters propagated to policies
     * @return result from executed policy
     */
    [[nodiscard]] constexpr auto operator()(const auto data, auto&&... args) const noexcept(true) -> std::size_t;
};
```

```
/*
 * Policy based for less than 2^5 elements
 *
 * @return 0 if string doesn't match, 1..N for matches
 *  - requires symbols to have the same size <= 8 bytes
 *  - requires platform with bmi2 support (https://en.wikipedia.org/wiki/X86_Bit_manipulation_instruction_set)
 */
template <auto max_bits_size = 5> class pext_direct;

/*
 * Policy based for more than 2^5 elements
 * Splits the input based on the first letter
 *
 * @return 0 if string doesn't match, 1..N for matches
 *  - requires symbols to have the same size <= 8 bytes
 *  - requires platform with bmi2 support (https://en.wikipedia.org/wiki/X86_Bit_manipulation_instruction_set)
 */
template <auto max_bits_size = 5> class pext_split_first_char;
```

### Benchmarks

```
g++-12  -pedantic-errors -DNDEBUG -std=c++20  -march=native benchmark.cpp -Ofast

|               ns/op |                op/s |    err% |     total | benchmark
|--------------------:|--------------------:|--------:|----------:|:----------
|               27.80 |       35,969,763.11 |    0.4% |      0.33 | `all.unordered_map`
|              141.66 |        7,059,116.90 |    0.2% |      1.69 | `all.bsearch`
|               35.71 |       28,001,334.02 |    0.1% |      0.43 | `all.frozen`
|                5.11 |      195,868,074.50 |    0.2% |      0.06 | `all.gperf`
|                3.19 |      313,661,527.85 |    0.5% |      0.04 | `all.mph`
|               28.62 |       34,939,806.64 |    1.0% |      0.34 | `random.unordered_map`
|              150.75 |        6,633,589.45 |    0.2% |      1.80 | `random.bsearch`
|               36.03 |       27,753,488.08 |    0.1% |      0.43 | `all.frozen`
|                5.11 |      195,722,305.78 |    0.2% |      0.06 | `random.gperf`
|                3.20 |      312,040,589.13 |    0.3% |      0.04 | `random.mph`

clang++-16 -DNDEBUG -stdlib=libc++ -std=c++20 -march=native benchmark.cpp

|               ns/op |                op/s |    err% |     total | benchmark
|--------------------:|--------------------:|--------:|----------:|:----------
|               24.04 |       41,595,460.47 |    0.3% |      0.29 | `all.unordered_map`
|              134.81 |        7,418,035.21 |    0.1% |      1.61 | `all.bsearch`
|               34.97 |       28,599,672.56 |    0.4% |      0.42 | `all.frozen`
|                5.61 |      178,177,450.43 |    0.1% |      0.07 | `all.gperf`
|                3.52 |      284,370,419.98 |    0.2% |      0.04 | `all.mph`
|               23.56 |       42,441,220.15 |    0.5% |      0.28 | `random.unordered_map`
|              137.83 |        7,255,541.20 |    0.1% |      1.65 | `random.bsearch`
|               35.23 |       28,388,007.09 |    0.4% |      0.42 | `all.frozen`
|                5.61 |      178,120,749.60 |    0.2% |      0.07 | `random.gperf`
|                3.51 |      284,605,400.76 |    0.4% |      0.04 | `random.mph`
```

### FAQ

### Can I use it with run-time data?

> Yes, as long as there is a policy to handle run-time data.

#### Doenst' compile

```
gcc: -fconstexpr-ops-limit=100000000
clang: -fconstexpr-steps=100000000
```

#### Ideas for policies?

- radix-tree
- finite-state-machine
- gperf
- mph
- ....
