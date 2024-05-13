<a href="http://www.boost.org/LICENSE_1_0.txt" target="_blank">![Boost Licence](http://img.shields.io/badge/license-boost-blue.svg)</a>
<a href="https://github.com/boost-ext/mph/releases" target="_blank">![Version](https://badge.fury.io/gh/boost-ext%2Fmph.svg)</a>
<a href="https://godbolt.org/z/7KfKEE1br">![build](https://img.shields.io/badge/build-blue.svg)</a>
<a href="https://godbolt.org/z/ej81YP3G9">![Try it online](https://img.shields.io/badge/try%20it-online-blue.svg)</a>

---------------------------------------

## [Minimal] Perfect hash library

> https://en.wikipedia.org/wiki/Perfect_hash_function

### Use case

> Given a list of N keys (known at compile-time) find a perfect hash (map keys to values).

### Features

- Single header (https://raw.githubusercontent.com/boost-ext/mph/main/mph)
    - Easy integration (see [FAQ](#faq))
- Self verification upon include (can be disabled by `DISABLE_STATIC_ASSERT_TESTS` - see [FAQ](#faq))
- Compiles cleanly with ([`-Wall -Wextra -Werror -pedantic -pedantic-errors -fno-exceptions -fno-rtti`](https://godbolt.org/z/3zh43YTMd))
- Minimal [API](#api)
- Optimized run-time execution (see [performance](#performance) / [benchmarks](#benchmarks))
- Fast* compilation times (see [compilation-times](#compilation))
- Limitations (see [FAQ](#faq))

### Requirements

- C++20 ([gcc-12+, clang-16+](https://godbolt.org/z/WraE4q1dE))

### [Optional] Hardware acceleration

- [bmi2](https://en.wikipedia.org/wiki/X86_Bit_manipulation_instruction_set)
- [simd](https://en.wikipedia.org/wiki/Advanced_Vector_Extensions)

### Hello world (https://godbolt.org/z/1eKbEYrTr)

```cpp
enum class color { red = 1, green = 2, blue = 3 };

constexpr auto colors = std::array{
  pair("red",   color::red),
  pair("green", color::green),
  pair("blue",  color::blue),
};

static_assert(color::green == mph::hash<colors>("green"));
static_assert(color::red   == mph::hash<colors>("red"));
static_assert(color::blue  == mph::hash<colors>("blue"));

std::print("{}", mph::hash<colors>("green"sv));
```

```
$CXX -std=c++20               -DNDEBUG -O3 && ./a.out # prints 2
$CXX -std=c++20 -mbmi2        -DNDEBUG -O3 && ./a.out # prints 2
$CXX -std=c++20 -mbmi2 -mavx2 -DNDEBUG -O3 && ./a.out # prints 2
```

---

<a name="performance"></a>
### Performance (https://godbolt.org/z/MhPa6exqc)

```cpp
int main(int argc, char**)
  constexpr std::array ids{
    std::pair{ 54u,  91u},
    std::pair{324u,  54u},
    std::pair{ 64u, 324u},
    std::pair{234u,  64u},
    std::pair{ 91u, 234u},
  };

  return mph::hash<ids>(argc);
}
```

```cpp
main(int): // g++ -DNDEBUG -std=c++20 -O3 -mbmi2
  movl $7, %edx
  xorl %eax, %eax
  pext %edx, %edi, %edx
  movl %edx, %edx
  cmpl %edi, lookup(,%rdx,8)
  cmove lookup+4(,%rdx,8), %eax
  ret

lookup: # size = 2^popcount(mask) of {key, value}
  .long   64
  .long   324
  .zero   8
  .long   234
  .long   64
  .long   91
  .long   234
  .long   324
  .long   54
  .zero   8
  .long   54
  .long   91
  .zero   8
```

---

### Performance (https://godbolt.org/z/65YncG4h9)

```cpp
int main(int, const char** argv) {
  constexpr auto symbols = std::array{
    pair("AMZN",  1),
    pair("AAPL",  2),
    pair("GOOGL", 3),
    pair("META",  4),
    pair("MSFT",  5),
    pair("NVDA",  6),
    pair("TESLA", 7),
  };

  return mph::hash<symbols>(
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

### Performance (https://godbolt.org/z/9hd49MTcd)

```cpp
int main(int, const char** argv) {
  constexpr std::array symbols{
    pair("BTC",  1),
    pair("ETH",  2),
    pair("BNB",  3),
    pair("SOL",  4),
    pair("XRP",  5),
    pair("DOGE", 6),
    pair("TON",  7),
    pair("ADA",  8),
    pair("SHIB", 9),
    pair("AVAX", 10),
    pair("LINK", 11),
    pair("BCH",  12),
  };

  return mph::hash<symbols>(std::span<const char, 4>(argv[1], argv[1]+4));
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

### Performance (https://godbolt.org/z/MjMzG9zvE)

```cpp
int main(int, const char** argv) {
  constexpr std::array symbols{
    pair("BTC",  1),
    pair("ETH",  2),
    pair("BNB",  3),
    pair("SOL",  4),
    pair("XRP",  5),
    pair("DOGE", 6),
    pair("TON",  7),
    pair("ADA",  8),
    pair("SHIB", 9),
    pair("AVAX", 10),
    pair("LINK", 11),
    pair("BCH",  12),
  };

  // input keys are always valid aka coming from the predefined set
  return mph::hash<symbols, mph::config<symbols>{.probability = 100u}>(
    std::span<const char, 4>(argv[1], argv[1]+4));
}
```

```cpp
main: // g++ -DNDEBUG -std=c++20 -O3 -mbmi2
  movq 8(%rsi), %rax
  movl $789, %edx
  movl (%rax), %eax
  pext %edx, %eax, %eax
  movl %eax, %eax
  movzbl lookup(%rax), %eax
  ret

lookup:
  ...
```

---

### Examples

- [feature] custom config -
- [feature] custom hash -
- [example] unordered_map -
- [example] branchless dispatcher - https://godbolt.org/z/1s7fsrjTs
- [performance] enum name -

---

<a name="compilation"></a>
### Compilation-times

> [include] (https://godbolt.org/z/zKPP8xPfG)

```cpp
time $CXX -x c++ -O3 -std=c++20 mph -c -DDISABLE_STATIC_ASSERT_TESTS        # 0.017s
time $CXX -x c++ -O3 -std=c++20 mph -c                                      # 0.056s
```

> [64 integral key/value pairs] (https://godbolt.org/z/96871efPo)

```cpp
time $CXX -std=c++20 -O3 mph_int_64.cpp -c -DDISABLE_STATIC_ASSERT_TESTS    # 0.043s
time $CXX -std=c++20 -O3 mph_int_64.cpp -c                                  # 0.090s
```

> [1024 integral key/value pairs] (https://godbolt.org/z/fjEYacE6G)

```cpp
time $CXX -std=c++20 -O3 mph_int_1024.cpp -c -DDISABLE_STATIC_ASSERT_TESTS  # 0.132s
time $CXX -std=c++20 -O3 mph_int_1024.cpp -c                                # 0.141s
```

> [6548 string key/value pairs] (https://godbolt.org/z/6q44dhq6c)

```cpp
time $CXX -std=c++20 -O3 mph_str_6548.cpp -c -DDISABLE_STATIC_ASSERT_TESTS  # 2.821s
time $CXX -std=c++20 -O3 mph_str_6548.cpp -c                                # 2.912s
```

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

---

### API

```cpp
/**
 * Converts data to T
 *
 * @tparam T type
 * @param data input
 * @return data as T
 */
template<class T>
[[nodiscard]] constexpr auto to(const auto& data) noexcept -> T;
```

```cpp
namespace concepts {
template<auto cfg> concept config = requires {
  cfg.probability;
  cfg.lookup;
  cfg.alignment;
} and (
  cfg.probability >= 0u and
  cfg.probability <= 100u and
  cfg.lookup >= 0u and
  not (cfg.alignment % 2)
);

template<auto kv> concept range = requires(u32 n) {
  kv.size();
  kv.begin();
  kv.end();
  kv[n];
};
} // namespace concepts
```

```cpp
template<auto kv>
struct config {
  /// 0         - none of the input data can be found in the kv
  /// (0, 50)   - input data is unlikely to be found in the kv
  /// 50        - unpredictable (default)
  /// (50, 100) - input data is likely to be found in the kv
  /// 100       - all input data can be found in the kv
  u8 probability{50};

  /// 0 - simd not used
  /// N - simd width
  u32 simd_width{[] {
    #if defined(__AVX2__)
    return 256u / __CHAR_BIT__;
    #else
    return 0u;
    #endif
  }()};

  /// 1 - one element per lookup (faster but larger memory footprint)
  /// N - n elements per lookup  (slower but smaller memory footprint)
  u32 lookup{
    [this]() -> u32 {
      using key_type = typename decltype(kv)::value_type::first_type;
      if (sizeof(key_type) == sizeof(u32) and
          kv.size() <= sizeof(key_type) * (1u << 10u)) {
        return 1u;
      } else if (sizeof(key_type) == sizeof(u64) and
                 kv.size() <= sizeof(key_type) * (1u << 7u)) {
        return 1u;
      } else if (probability == 100u) {
        return 2u * sizeof(key_type) - 4u;
      } else if (sizeof(key_type) < simd_width) {
        return simd_width / sizeof(key_type);
      } else {
        return 2u * sizeof(key_type) - 4u;
      }
    }()
  };

  /// 0 - no alignment for the lookup table
  /// n - alignas(lookup) - needs to be power of 2, required for simd
  u32 alignment{simd_width / 4u};
};
```

```cpp
/**
 * Perfect hash function
 *
 * @tparam kv constexpr array of key/value pairs
 * @tparam config configuration
 * @param key input data
 */
template<auto kv, config cfg = config<kv>{}>
  requires concepts::range<kv> and concepts::config<cfg>
[[nodiscard]] constexpr auto hash(const auto& key) noexcept;
```

> Configuration

```cpp
#define MPH 2'3'0           // Current library version (SemVer)
#define MPH_PAGE_SIZE 4096u // If set __builtin_memcpy(data, sizeof(T)) & bzhi(size)
                            // If not __builtin_memcpy(data, size)
```

---

### FAQ

- Limitations?

    > `mph` supports different types of key/value pairs. `mph` supports thousands of key/value pairs, but not millions - (see [compilation-times](#compilation)).

  - All keys have to fit into `std::uint64_t`, that includes strings which are converted to integral types with `mph::to<u32/u64>` call.
  - If the above criteria are not satisfied `mph` will [SFINAE](https://en.wikipedia.org/wiki/Substitution_failure_is_not_an_error) away `hash` function.
  - In such case different backup policy should be used instead (which can be also used as customization point for user-defined hash implementations), for example:

    ```cpp
    template<auto kv, auto cfg = config<kv>{}>
      requires concepts::range<kv> and concepts::config<cfg> and (
        kv.size() > 1'000'000
      )
    [[nodiscard]] constexpr auto mph::hash(const auto& key) noexcept;
    ```

- How `mph` is working under the hood?

    > `mph` takes advantage of knowing the key/value pairs at compile-time as well as the specific hardware instructions.
      The following is a pseudo code of the algorithm.

    ```python
    def hash[kv: array, unknown: typeof(kv[0][0])](key : any):
      # 0. find mask which uniquely identifies all keys [compile-time]
      mask = ~typeof(kv[0][0]) # 0b111111...

      for i in range(nbits(mask)):
        masked = []
        mask.unset(i)

        for k, v in kv:
          masked.append(k & mask)

        if not unique(masked):
          mask.set(i)

      assert unique(masked)
      assert mask != ~typeof(kv[0][0])

      lookup = array(typeof(kv[0]), 2^popcount(mask)) # static constexpr + alignment
      for k, v in kv:
        lookup[pext(k, mask)] = (k, v)

      # 1. lookup [run-time] / if key is a string convert to u32 or u64 first (memcpy)
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

      if k == key: # policies (conditional, branchless, ...)
        return v
      else:
        return unknown
    ```

    Additional resources can be found in the `Acknowledgments` section.

- How to tweak `hash` performance for my data/use case?

    > Always measure!

  - [[bmi2](https://en.wikipedia.org/wiki/X86_Bit_manipulation_instruction_set) ([Intel Haswell](Intel)+, [AMD Zen3](https://en.wikipedia.org/wiki/Zen_3)+)] hardware instruction acceleration is faster than software emulation. (AMD Zen2 pext takes 18 cycles, is worth disabling hardware accelerated version)
  - In case config.N is greater than (N collisions supported) [avx2](https://en.wikipedia.org/wiki/Advanced_Vector_Extensions) will be faster than multiple comparision (it's enabled if compiled with `-mbmi2 -mavx2` or `-march=skylake` etc.).
  - For integral keys, use u32 or u64, esepcially if cfg.N > 1u as that enables avx2 optimizations. For values, measure, as size optimization can lead to performance degradation.
  - For strings, consider aligning the input data and passing it with compile-time size via `span`, `array`.
  - Passing `string_view` will be slower and requires to set `MPH_PAGE_SIZE` properly when passing dynamically sized input. By default `MPH_PAGE_SIZE` is set to `4096u`. That's required as, by default, `mph` will try to optimize `memcpy` of input bytes.
  - If all strings length is less than 4 that will be more optimized than if all string length will be less than 8 (max available). That will make the lookup table smaller and it will avoid `shl` for getting the value.
  - Consider using minimial required size for values. That will make the lookup table smaller.
  - Experiment with different `config.probability` to optimize lookups. Especially benefitial if it's known that input keys are always valid (probability = 100) as it will avoid final `cmp` instruction.
  - Consider passing cache size alignment (`std::hardware_destructive_interference_size` - usually `64u`) to the hash config. That will align the underlying lookup table. That's done automatically if simd acceleration is used.

- Is support for [bmi2](https://en.wikipedia.org/wiki/X86_Bit_manipulation_instruction_set)/[simd](https://en.wikipedia.org/wiki/Advanced_Vector_Extensions) instructions required?

    > No, `mph` works on platforms without them. `bmi2` instructions can be emulated in software with a bit slower execution.

    ```cpp
    // bmi2
    mov     ecx, 789
    pext    ecx, eax, ecx

    // no bmi2
    mov     ecx, eax
    and     ecx, 789
    imul    ecx, ecx, 57
    shr     ecx, 2
    and     ecx, 248
    ```

    `simd` instructions are only used if available and aren't required for `mph` to function properly.

- How to disable `cmov` generation?

    > Set `config.probability` to value different than `50` - meaning that input data is predictable in some way. Additionaly the following compiler options can be used.

    ```
    clang: -mllvm -x86-cmov-converter=false
    ```

- Getting a compilation error `constexpr evaluation hit maximum step limit`?

    > The following options can be used to increase the limits, however, compilation-times should be monitored.

    ```
    gcc:   -fconstexpr-ops-limit=N
    clang: -fconstexpr-steps=N
    ```

- How to disable running tests at compile-time?

    > When `DISABLE_STATIC_ASSERT_TESTS` is defined static_asserts tests won't be executed upon inclusion.
      Note: Use with caution as disabling tests means that there are no gurantees upon inclusion that given compiler/env combination works as expected.

- How to integrate with CMake/CPM?

    ```
    CPMAddPackage(
      Name mph
      GITHUB_REPOSITORY boost-ext/mph
      GIT_TAG v2.3.0
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
