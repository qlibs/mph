<a href="http://www.boost.org/LICENSE_1_0.txt" target="_blank">![Boost Licence](http://img.shields.io/badge/license-boost-blue.svg)</a>
<a href="https://github.com/boost-ext/mph/releases" target="_blank">![Version](https://badge.fury.io/gh/boost-ext%2Fmph.svg)</a>
<a href="https://godbolt.org/z/rbnzEhfK3">![build](https://img.shields.io/badge/build-blue.svg)</a>
<a href="https://godbolt.org/z/49cMhWqch">![Try it online](https://img.shields.io/badge/try%20it-online-blue.svg)</a>

---------------------------------------

## [Minimal] Static perfect hash library

> https://en.wikipedia.org/wiki/Perfect_hash_function

### Use case

> A static perfect hash function maps a set of keys known in advance to a set of values with no collisions.

### Features

- Single header (https://raw.githubusercontent.com/boost-ext/mph/main/mph)
    - Easy integration (see [FAQ](#faq))
- Self verification upon include (can be disabled by `-DNTEST` - see [FAQ](#faq))
- Compiles cleanly with ([`-Wall -Wextra -Werror -pedantic -pedantic-errors -fno-exceptions -fno-rtti`](https://godbolt.org/z/WraE4q1dE))
- Minimal [API](#api)
- Optimized run-time execution (see [performance](#performance) / [benchmarks](#benchmarks))
- Fast compilation times (see [compilation](#compilation))
- Trade-offs (see [FAQ](#faq))

### Requirements

- C++20 ([gcc-12+, clang-15+](https://godbolt.org/z/WraE4q1dE)) / [optional] ([bmi2](https://en.wikipedia.org/wiki/X86_Bit_manipulation_instruction_set))

### Hello world (https://godbolt.org/z/zdo5bz3T6)

```cpp
enum class color { red, green, blue };

constexpr auto colors = std::array{
  std::pair{"red"sv, color::red},
  std::pair{"green"sv, color::green},
  std::pair{"blue"sv, color::blue},
};

static_assert(color::green == mph::lookup<colors>("green"));
static_assert(color::red   == mph::lookup<colors>("red"));
static_assert(color::blue  == mph::lookup<colors>("blue"));

std::print("{}", mph::lookup<colors>("green"sv)); // prints 1
```

> `mph::lookup` assumes only valid input and returns mapped value direclty.

```cpp
static_assert(not mph::find<colors>("unknown"));
static_assert(mph::find<colors>("green"));
static_assert(mph::find<colors>("red"));
static_assert(mph::find<colors>("blue"));

std::print("{}", *mph::find<colors>("green"sv)); // prints 1
```

> `mph::find` doesn't assume valid input and returns optional of mapped value.

---

<a name="performance"></a>
### Performance (https://godbolt.org/z/ndr5K8n8K)

```cpp
int main(int argc, const char**)
  static constexpr std::array ids{
    std::pair{54u, 91u},
    std::pair{64u, 324u},
    std::pair{91u, 234u},
  };

  return mph::lookup<ids>(argc);
}
```

```cpp
main: // g++ -DNDEBUG -std=c++20 -O3
  imull   $1275516394, %edi, %eax
  shrl    $23, %eax
  movl    $24029728, %ecx
  shrxl   %eax, %ecx, %eax
  andl    $511, %eax
  retq
```

### Performance (https://godbolt.org/z/aE3117Goh)

```cpp
int main(int argc, const char**)
  static constexpr std::array ids{
    std::pair{54u, 91u},
    std::pair{324u, 54u},
    std::pair{64u, 324u},
    std::pair{234u, 64u},
    std::pair{91u, 234u},
  };
  return mph::lookup<ids>(argc);
}
```

```cpp
main: // g++ -DNDEBUG -std=c++20 -O3
  andl    $7, %edi
  leaq    lookup(%rip), %rax
  movl    (%rax,%rdi,4), %eax
  retq

lookup:
 .long   324
 .long   0
 .long   64
 .long   234
 .long   54
 .long   0
 .long   91
```

---

### Performance (https://godbolt.org/z/ob6ejGEsh)

```cpp
int main(int, const char** argv) {
  static constexpr auto symbols = std::array{
    std::pair{"AMZN    "sv, 1},
    std::pair{"AAPL    "sv, 2},
    std::pair{"GOOGL   "sv, 3},
    std::pair{"META    "sv, 4},
    std::pair{"MSFT    "sv, 5},
    std::pair{"NVDA    "sv, 6},
    std::pair{"TSLA    "sv, 7},
  };

  return *mph::find<symbols>(
    std::span<const char, 8>(argv[1], argv[1]+8)
  );
}
```

```cpp
main: // g++ -DNDEBUG -std=c++20 -O3 -mbmi2
  movq    8(%rsi), %rax
  movl    $1031, %ecx
  leaq    lookup(%rip), %rdx
  xorl    %esi, %esi
  movq    (%rax), %rax
  pextq   %rcx, %rax, %rcx
  shll    $4, %ecx
  cmpq    (%rcx,%rdx), %rax
  movzbl  8(%rcx,%rdx), %eax
  cmovnel %esi, %eax
  retq

lookup:
  ...
```

### Performance (https://godbolt.org/z/Tds7sG16n)

```cpp
int main(int, const char** argv) {
  // values assigned from 0..N-1
  static constexpr std::array symbols{
    "BTC "sv, "ETH "sv, "BNB "sv,
    "SOL "sv, "XRP "sv, "DOGE"sv,
    "TON "sv, "ADA "sv, "SHIB"sv,
    "AVAX"sv, "LINK"sv, "BCH "sv,
  };

  return *mph::find<symbols>(
    std::span<const char, 4>(argv[1], argv[1]+4)
  );
}
```

```cpp
main: // g++ -DNDEBUG -std=c++20 -O3 -mbmi2
  movq    8(%rsi), %rax
  movl    $789, %ecx
  leaq    lookup(%rip), %rdx
  xorl    %esi, %esi
  movl    (%rax), %eax
  pextl   %ecx, %eax, %ecx
  cmpl    (%rdx,%rcx,8), %eax
  movzbl  4(%rdx,%rcx,8), %eax
  cmovnel %esi, %eax
  retq

lookup:
  ...
```

---

### Examples

- [feature] `lookup/find` customization point - https://godbolt.org/z/dnc4x8hTT
- [feature] `to` customization point - https://godbolt.org/z/Evhc57dGK
- [example] branchless dispatcher - https://godbolt.org/z/nb9M9zcqh
- [performance] `enum_to_string` (https://wg21.link/P2996) - https://godbolt.org/z/dPPnTxoac
- [performance] `string_to_enum` (https://wg21.link/P2996) - https://godbolt.org/z/o8sfYoreK

---

<a name="benchmarks"></a>
### Benchmarks (https://github.com/boost-ext/mph/tree/benchmark)

> `clang++ -std=c++20 -O3 -DNDEBUG -mbmi2 benchmark.cpp`

```
| ns/op |           op/s | err% |total | benchmark
|------:|---------------:|-----:|-----:|:----------
| 12.25 |  81,602,449.70 | 0.3% | 0.15 | `random_strings_5_len_4.std.map`
|  5.56 | 179,750,906.50 | 0.2% | 0.07 | `random_strings_5_len_4.std.unordered_map`
|  9.17 | 109,096,850.98 | 0.2% | 0.11 | `random_strings_5_len_4.boost.unordered_map`
| 13.48 |  74,210,250.54 | 0.3% | 0.16 | `random_strings_5_len_4.boost.flat_map`
|  7.70 | 129,942,965.18 | 0.3% | 0.09 | `random_strings_5_len_4.gperf`
|  1.61 | 621,532,188.81 | 0.1% | 0.02 | `random_strings_5_len_4.mph`
| 14.66 |  68,218,086.71 | 0.8% | 0.18 | `random_strings_5_len_8.std.map`
| 13.45 |  74,365,239.56 | 0.2% | 0.16 | `random_strings_5_len_8.std.unordered_map`
|  9.68 | 103,355,605.09 | 0.2% | 0.12 | `random_strings_5_len_8.boost.unordered_map`
| 16.00 |  62,517,180.19 | 0.4% | 0.19 | `random_strings_5_len_8.boost.flat_map`
|  7.70 | 129,809,356.36 | 0.3% | 0.09 | `random_strings_5_len_8.gperf`
|  1.58 | 633,084,194.24 | 0.1% | 0.02 | `random_strings_5_len_8.mph`
| 17.21 |  58,109,576.87 | 0.3% | 0.21 | `random_strings_6_len_2_5.std.map`
| 15.28 |  65,461,167.99 | 0.2% | 0.18 | `random_strings_6_len_2_5.std.unordered_map`
| 12.21 |  81,931,391.20 | 0.4% | 0.15 | `random_strings_6_len_2_5.boost.unordered_map`
| 17.15 |  58,323,741.08 | 0.5% | 0.21 | `random_strings_6_len_2_5.boost.flat_map`
|  7.94 | 125,883,197.55 | 0.5% | 0.09 | `random_strings_6_len_2_5.gperf`
|  6.05 | 165,239,616.00 | 0.5% | 0.07 | `random_strings_6_len_2_5.mph`
| 31.61 |  31,631,402.94 | 0.2% | 0.38 | `random_strings_100_len_8.std.map`
| 15.32 |  65,280,594.09 | 0.2% | 0.18 | `random_strings_100_len_8.std.unordered_map`
| 17.13 |  58,383,850.20 | 0.3% | 0.20 | `random_strings_100_len_8.boost.unordered_map`
| 31.42 |  31,822,519.67 | 0.2% | 0.38 | `random_strings_100_len_8.boost.flat_map`
|  8.04 | 124,397,773.85 | 0.2% | 0.10 | `random_strings_100_len_8.gperf`
|  1.58 | 632,813,481.73 | 0.1% | 0.02 | `random_strings_100_len_8.mph`
| 32.62 |  30,656,015.03 | 0.3% | 0.39 | `random_strings_100_len_1_8.std.map`
| 19.34 |  51,697,107.73 | 0.5% | 0.23 | `random_strings_100_len_1_8.std.unordered_map`
| 19.51 |  51,254,525.17 | 0.3% | 0.23 | `random_strings_100_len_1_8.boost.unordered_map`
| 33.58 |  29,780,574.17 | 0.6% | 0.40 | `random_strings_100_len_1_8.boost.flat_map`
| 13.06 |  76,577,037.07 | 0.7% | 0.16 | `random_strings_100_len_1_8.gperf`
|  6.02 | 166,100,665.07 | 0.2% | 0.07 | `random_strings_100_len_1_8.mph`
|  1.28 | 778,723,795.75 | 0.1% | 0.02 | `random_uints_5.mph`
```

> `g++ -std=c++20 -O3 -DNDEBUG -mbmi2 benchmark.cpp`

```cpp
| ns/op |           op/s | err% |total | benchmark
|------:|---------------:|-----:|-----:|:----------
| 12.28 |  81,460,330.38 | 0.9% | 0.15 | `random_strings_5_len_4.std.map`
|  5.29 | 188,967,241.90 | 0.3% | 0.06 | `random_strings_5_len_4.std.unordered_map`
|  9.69 | 103,163,192.67 | 0.2% | 0.12 | `random_strings_5_len_4.boost.unordered_map`
| 13.56 |  73,756,333.08 | 0.4% | 0.16 | `random_strings_5_len_4.boost.flat_map`
|  7.69 | 130,055,662.66 | 0.6% | 0.09 | `random_strings_5_len_4.gperf`
|  1.39 | 718,910,252.82 | 0.1% | 0.02 | `random_strings_5_len_4.mph`
| 14.26 |  70,103,007.82 | 2.4% | 0.17 | `random_strings_5_len_8.std.map`
| 13.36 |  74,871,047.51 | 0.4% | 0.16 | `random_strings_5_len_8.std.unordered_map`
|  9.82 | 101,802,074.00 | 0.3% | 0.12 | `random_strings_5_len_8.boost.unordered_map`
| 15.97 |  62,621,571.95 | 0.3% | 0.19 | `random_strings_5_len_8.boost.flat_map`
|  7.92 | 126,265,206.30 | 0.3% | 0.09 | `random_strings_5_len_8.gperf`
|  1.40 | 713,596,376.62 | 0.4% | 0.02 | `random_strings_5_len_8.mph`
| 15.98 |  62,576,142.34 | 0.5% | 0.19 | `random_strings_6_len_2_5.std.map`
| 17.56 |  56,957,868.12 | 0.5% | 0.21 | `random_strings_6_len_2_5.std.unordered_map`
| 11.68 |  85,637,378.45 | 0.3% | 0.14 | `random_strings_6_len_2_5.boost.unordered_map`
| 17.25 |  57,965,732.68 | 0.6% | 0.21 | `random_strings_6_len_2_5.boost.flat_map`
|  9.13 | 109,580,632.48 | 0.7% | 0.11 | `random_strings_6_len_2_5.gperf`
|  7.17 | 139,563,745.72 | 0.4% | 0.09 | `random_strings_6_len_2_5.mph`
| 30.20 |  33,117,522.76 | 0.7% | 0.36 | `random_strings_100_len_8.std.map`
| 15.01 |  66,627,962.89 | 0.4% | 0.18 | `random_strings_100_len_8.std.unordered_map`
| 16.79 |  59,559,414.60 | 0.6% | 0.20 | `random_strings_100_len_8.boost.unordered_map`
| 31.36 |  31,884,629.57 | 0.8% | 0.38 | `random_strings_100_len_8.boost.flat_map`
|  7.75 | 128,973,947.61 | 0.7% | 0.09 | `random_strings_100_len_8.gperf`
|  1.50 | 667,041,673.54 | 0.1% | 0.02 | `random_strings_100_len_8.mph`
| 30.92 |  32,340,612.08 | 0.4% | 0.37 | `random_strings_100_len_1_8.std.map`
| 25.35 |  39,450,222.09 | 0.4% | 0.30 | `random_strings_100_len_1_8.std.unordered_map`
| 19.76 |  50,609,820.90 | 0.2% | 0.24 | `random_strings_100_len_1_8.boost.unordered_map`
| 32.39 |  30,878,018.77 | 0.6% | 0.39 | `random_strings_100_len_1_8.boost.flat_map`
| 11.20 |  89,270,687.92 | 0.2% | 0.13 | `random_strings_100_len_1_8.gperf`
|  7.17 | 139,471,159.67 | 0.5% | 0.09 | `random_strings_100_len_1_8.mph`
|  1.93 | 519,047,110.39 | 0.3% | 0.02 | `random_uints_5.mph`
```

<a name="compilation"></a>
### Compilation-times

> [include] (https://godbolt.org/z/zKPP8xPfG)

```cpp
time $CXX -x c++ -O3 -std=c++20 mph -c -DNTEST        # 0.017s
time $CXX -x c++ -O3 -std=c++20 mph -c                # 0.056s
```

> [64 integral keys] (https://godbolt.org/z/j8zWof5no)

```cpp
time $CXX -std=c++20 -O3 mph_int_64.cpp -c -DNTEST    # 0.043s
time $CXX -std=c++20 -O3 mph_int_64.cpp -c            # 0.090s
```

---

### API

```cpp
namespace mph {
/**
 * Static [minimal] perfect hash lookup function
 * @tparam entries constexpr array of keys or key/value pairs
 */
template<const auto& entries>
inline constexpr auto lookup = [](const auto& key) { ... };

/**
 * Static perfect hash find function
 * @tparam entries constexpr array of keys or key/value pairs
 */
template<const auto& entries>
inline constexpr auto find = []<u8 probability = 50u>(const auto& key) { ... };
} // namespace mph
```

> Configuration

```cpp
#define MPH 4'0'1       // Current library version (SemVer)
#define MPH_PAGE_SIZE   // [default: not defined]
                        // If defined safe memcpy will be used for string-like
                        // keys if the read is close to the page boundry (4096u)
```

---

### FAQ

- Trade-offs?

    > `mph` supports different types of key/value pairs and thousands of key/value pairs, but not millions - (see [benchmarks](#benchmarks)).

  - All keys have to fit into `std::uint128_t`, that includes strings.
  - If the above criteria are not satisfied `mph` will [SFINAE](https://en.wikipedia.org/wiki/Substitution_failure_is_not_an_error) away `lookup` function.
  - In such case different backup policy should be used instead (which can be also used as customization point for user-defined `lookup` implementation), for example:

    ```cpp
    template<const auto& entries>
      requires (entries.size() > 10'000)
    inline constexpr auto mph::find = [](const auto& key) -> optional { ... }
    ```

- How `mph` is working under the hood?

    > `mph` takes advantage of knowing the key/value pairs at compile-time as well as the specific hardware instructions.
      The following is a pseudo code of the `lookup` algorithm for minimal perfect hash table.

    ```python
    def lookup[entries: array](key : any, max_attempts = 100'000):
      # 0. magic and lut for entries [compile-time]
      nbits = sizeof(u32) * CHAR_BIT - countl_zero(max(entries.second))
      mask = (1u << nbits) - 1u;
      shift = sizeof(u32) * CHAR_BIT - nbits;
      lut = {};
      while max_attempts--:
        magic = rand()
        for k, v in entries:
          lut |= v << (k * magic >> shift);

        for k, v in entries:
          if (lut >> (k * magic >> shift) & mask) != v:
            lut = {}
            break

      assert lut

      # 1. lookup [run-time]
      return (lut >> ((key * magic) >> shift)) & mask;
    ```

    > The following is a pseudo code of the `find` algorithm for perfect hash table.

    ```python
    def find[entries: array](key : any):
      # 0. find mask which uniquely identifies all keys [compile-time]
      mask = 0b111111...

      for i in range(nbits(mask)):
        masked = []
        mask.unset(i)

        for k, v in entries:
          masked.append(k & mask)

        if not unique(masked):
          mask.set(i)

      assert unique(masked)
      assert std::countl_zero(mask)

      # 1. create lookup table [compile-time]
      lookup = array(typeof(entries[0]), 2^popcount(mask))
      for k, v in entries:
        lookup[pext(k, mask)] = (k, v)

      # 2. lookup [run-time] # if key is a string convert to integral first (memcpy)
        # word: 00101011
        # mask: 11100001
        #    &: 000____1
        # pext: ____0001 # intel/intrinsics-guide/index.html#text=pext
        def pext(a : uN, mask : uN):
          dst, m, k = ([], 0, 0)

          while m < nbits(a):
            if mask[m] == 1:
              dst.append(a[m])
              k += 1
            m += 1

          return uN(dst)

      k, v = lookup[pext(key, mask)]

      if k == key:
        return v
      else:
        return 0
    ```

- How to tweak `lookup` performance for my data/use case?

    > Always measure!

  - [[bmi2](https://en.wikipedia.org/wiki/X86_Bit_manipulation_instruction_set) ([Intel Haswell](Intel)+, [AMD Zen3](https://en.wikipedia.org/wiki/Zen_3)+)] hardware instruction acceleration is faster than software emulation. (AMD Zen2 pext takes 18 cycles, is worth disabling hardware accelerated version)
  - For integral keys, use u32 or u64.
  - For strings, consider aligning the input data and passing it with compile-time size via `span`, `array`.
  - If all strings length is less than 4 that will be more optimized than if all string length will be less than 8 and 16. That will make the lookup table smaller and getting the value will have one instruction less.
  - Experiment with different `probability` values to optimize lookups. Especially benefitial if it's known that input keys are always coming from predefined `entries` (probability = 100) as it will avoid the comparison.
  - Consider passing cache size alignment (`std::hardware_destructive_interference_size` - usually `64u`) to the `lookup`. That will align the underlying lookup table.

- How to fix compilation error `constexpr evaluation hit maximum step limit`?

    > The following options can be used to increase the limits, however, compilation-times should be monitored.

    ```
    gcc:   -fconstexpr-ops-limit=N
    clang: -fconstexpr-steps=N
    ```

- Is support for [bmi2](https://en.wikipedia.org/wiki/X86_Bit_manipulation_instruction_set) instructions required?

    > `mph` works on platforms without `bmi2` instructions which can be emulated with some limitations (*).

    ```cpp
    // bmi2
    mov     ecx, 789
    pext    ecx, eax, ecx
    ```

    > [intel.com/pext](https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#text=pext) / [uops.info/pext](https://uops.info/table.html?search=PEXT%20(R64%2C%20R64%2C%20R64)&cb_lat=on&cb_HSW=on&cb_BDW=on&cb_SKL=on&cb_CFL=on&cb_CLX=on&cb_ICL=on&cb_TGL=on&cb_RKL=on&cb_ZEN2=on&cb_ZEN3=on&cb_ZEN4=on&cb_measurements=on&cb_bmi=on)

    ```cpp
    // no bmi2
    mov     ecx, eax
    and     ecx, 789
    imul    ecx, ecx, 57
    shr     ecx, 2
    and     ecx, 248
    ```

    > https://stackoverflow.com/questions/14547087/extracting-bits-with-a-single-multiplication (*)

- How to disable `cmov` generation?

    > Set `probability` value to something else than `50u` (default) - it means that the input data is predictable in some way and `jmp` will be generated instead. Additionaly the following compiler options can be used.

    ```
    clang: -mllvm -x86-cmov-converter=false
    ```

- How to disable running tests at compile-time?

    > When `-DNTEST` is defined static_asserts tests won't be executed upon inclusion.
      Note: Use with caution as disabling tests means that there are no gurantees upon inclusion that given compiler/env combination works as expected.

- How to integrate with CMake/CPM?

    ```
    CPMAddPackage(
      Name mph
      GITHUB_REPOSITORY boost-ext/mph
      GIT_TAG v3.0.1
    )
    add_library(mph INTERFACE)
    target_include_directories(mph SYSTEM INTERFACE ${mph_SOURCE_DIR})
    add_library(mph::mph ALIAS mph)
    ```

    ```
    target_link_libraries(${PROJECT_NAME} mph::mph);
    ```

- Similar projects?

    > [gperf](https://www.gnu.org/software/gperf), [frozen](https://github.com/serge-sans-paille/frozen), [nbperf](https://github.com/rurban/nbperf), [cmph](https://cmph.sourceforge.net), [perfecthash](https://github.com/tpn/perfecthash), [lemonhash](https://github.com/ByteHamster/LeMonHash), [pthash](https://github.com/jermp/pthash), [shockhash](https://github.com/ByteHamster/ShockHash), [burr](https://github.com/lorenzhs/BuRR), [hash-prospector](https://github.com/skeeto/hash-prospector)

- Acknowledgments

    > https://lemire.me/blog, http://0x80.pl, https://easyperf.net, https://www.jabperf.com, https://johnnysswlab.com, [pefect-hashing](https://github.com/tpn/pdfs/tree/master/Perfect%20Hashing), [gperf](https://www.dre.vanderbilt.edu/~schmidt/PDF/C++-USENIX-90.pdf), [cmph](https://cmph.sourceforge.net/papers), [smasher](https://github.com/rurban/smhasher), [minimal perfect hashing](http://stevehanov.ca/blog/index.php?id=119), [hash functions](https://nullprogram.com/blog/2018/07/31)
