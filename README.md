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

assert(0 == hash(""));    // not found
assert(0 == hash("FOO")); // not found

assert(1 == hash("FBC"));
assert(2 == hash("SPY"));
assert(3 == hash("CDC"));
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
g++ -DNDEBUG -std=c++20 -fconstexpr-ops-limit=100000000 -march=native benchmark.cpp -Ofast

|               ns/op |                op/s |    err% |          ins/op |          cyc/op |    IPC |         bra/op |   miss% |     total | benchmark
|--------------------:|--------------------:|--------:|----------------:|----------------:|-------:|---------------:|--------:|----------:|:----------
|               28.21 |       35,449,266.81 |    0.5% |          134.07 |           75.79 |  1.769 |          21.17 |    2.0% |      0.34 | `all.unordered_map`
|              139.61 |        7,162,626.58 |    0.1% |          747.20 |          375.07 |  1.992 |         174.27 |    1.7% |      1.67 | `all.bsearch`
|               36.55 |       27,361,525.10 |    0.2% |          177.47 |           98.18 |  1.808 |          35.60 |    1.3% |      0.44 | `all.frozen`
|                5.15 |      194,230,567.13 |    0.2% |           42.00 |           13.83 |  3.037 |           1.00 |    0.0% |      0.06 | `all.gperf`
|                3.65 |      273,921,850.07 |    0.2% |           34.00 |            9.80 |  3.468 |           0.00 |    0.0% |      0.04 | `all.mph`
|               28.90 |       34,601,348.26 |    0.3% |          134.72 |           77.64 |  1.735 |          21.32 |    2.3% |      0.34 | `random.unordered_map`
|              149.05 |        6,709,003.72 |    0.3% |          749.26 |          400.42 |  1.871 |         170.28 |    2.0% |      1.78 | `random.bsearch`
|               37.21 |       26,876,608.82 |    0.7% |          175.18 |           99.95 |  1.753 |          35.20 |    1.4% |      0.45 | `all.frozen`
|                5.15 |      194,319,068.34 |    0.4% |           42.00 |           13.82 |  3.040 |           1.00 |    0.0% |      0.06 | `random.gperf`
|                3.63 |      275,448,002.28 |    0.5% |           34.00 |            9.75 |  3.487 |           0.00 |    0.0% |      0.04 | `random.mph`
```

```
clang++ -DNDEBUG -fconstexpr-steps=100000000 -stdlib=libc++ -std=c++20 -march=native benchmark.cpp -Ofast

|               ns/op |                op/s |    err% |          ins/op |          cyc/op |    IPC |         bra/op |   miss% |     total | benchmark
|--------------------:|--------------------:|--------:|----------------:|----------------:|-------:|---------------:|--------:|----------:|:----------
|               24.18 |       41,364,054.39 |    0.2% |          170.99 |           64.92 |  2.634 |          33.07 |    0.7% |      0.29 | `all.unordered_map`
|              135.85 |        7,360,967.69 |    0.2% |          687.15 |          364.91 |  1.883 |         173.96 |    1.7% |      1.63 | `all.bsearch`
|               35.05 |       28,533,555.25 |    0.3% |          156.28 |           94.14 |  1.660 |          26.24 |    1.8% |      0.42 | `all.frozen`
|                5.61 |      178,178,755.95 |    0.1% |           49.00 |           15.07 |  3.251 |           1.00 |    0.0% |      0.07 | `all.gperf`
|                3.87 |      258,091,376.21 |    0.1% |           33.00 |           10.41 |  3.171 |           0.00 |    0.0% |      0.05 | `all.mph`
|               25.10 |       39,837,311.69 |    0.2% |          171.13 |           67.40 |  2.539 |          33.25 |    0.9% |      0.30 | `random.unordered_map`
|              139.41 |        7,173,292.81 |    0.4% |          687.64 |          374.30 |  1.837 |         174.43 |    1.7% |      1.67 | `random.bsearch`
|               35.09 |       28,499,194.01 |    0.2% |          154.77 |           94.25 |  1.642 |          26.08 |    1.9% |      0.42 | `all.frozen`
|                5.66 |      176,813,439.22 |    0.5% |           49.00 |           15.16 |  3.232 |           1.00 |    0.0% |      0.07 | `random.gperf`
|                3.88 |      257,650,783.17 |    0.3% |           33.00 |           10.42 |  3.167 |           0.00 |    0.0% |      0.05 | `random.mph`
```

### FAQ

### Can I use it with run-time data?

> Yes, as long as there is a policy to handle run-time data.

#### Ideas for policies?

- radix-tree
- finite-state-machine
- gperf
- mph
- ....
