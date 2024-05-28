<a href="http://www.boost.org/LICENSE_1_0.txt" target="_blank">![Boost Licence](http://img.shields.io/badge/license-boost-blue.svg)</a>
<a href="https://github.com/boost-ext/mph/releases" target="_blank">![Version](https://badge.fury.io/gh/boost-ext%2Fmph.svg)</a>
<a href="https://godbolt.org/z/d8fbvcqd8">![build](https://img.shields.io/badge/build-blue.svg)</a>
<a href="https://godbolt.org/z/Wq6bxf656">![Try it online](https://img.shields.io/badge/try%20it-online-blue.svg)</a>

---------------------------------------

## [Minimal] Static perfect hash library

> https://en.wikipedia.org/wiki/Perfect_hash_function

### Use case

> A static perfect hash function maps a set of keys known in advance to a set of values with no collisions.

### Features

- Single header (https://raw.githubusercontent.com/boost-ext/mph/main/mph)
    - Easy integration (see [FAQ](#faq))
- Self verification upon include (can be disabled by `DISABLE_STATIC_ASSERT_TESTS` - see [FAQ](#faq))
- Compiles cleanly with ([`-Wall -Wextra -Werror -pedantic -pedantic-errors -fno-exceptions -fno-rtti`](https://godbolt.org/z/WraE4q1dE))
- Minimal [API](#api)
- Optimized run-time execution (see [performance](#performance) / [benchmarks](#benchmarks))
- Fast* compilation times (see [benchmarks](#benchmarks))
- Limitations (see [FAQ](#faq))

### Requirements

- C++20 ([gcc-12+, clang-15+](https://godbolt.org/z/WraE4q1dE)) / [optional] ([bmi2](https://en.wikipedia.org/wiki/X86_Bit_manipulation_instruction_set))

### Hello world (https://godbolt.org/z/8Mn5h1jrc)

```cpp
enum class color { red, green, blue };

constexpr auto colors = std::array{
  std::pair{"red"sv, color::red},
  std::pair{"green"sv, color::green},
  std::pair{"blue"sv, color::blue},
};

static_assert(color::green == *mph::lookup<colors>("green"));
static_assert(color::red   == *mph::lookup<colors>("red"));
static_assert(color::blue  == *mph::lookup<colors>("blue"));

std::print("{}", *mph::lookup<colors>("green"sv));
```

```
$CXX -std=c++20                -DNDEBUG -O3 && ./a.out # prints 1
$CXX -std=c++20 -mbmi2         -DNDEBUG -O3 && ./a.out # prints 1
$CXX -std=c++20 -march=skylake -DNDEBUG -O3 && ./a.out # prints 1
```

---

<a name="performance"></a>
### Performance (https://godbolt.org/z/Wq6bxf656)

```cpp
int main(int argc, const char**)
  static constexpr std::array ids{
    std::pair{54u, 91u},
    std::pair{324u, 54u},
    std::pair{64u, 324u},
    std::pair{234u, 64u},
    std::pair{91u, 234u},
  };

  static_assert(not mph::lookup<ids>(0)u);
  static_assert(mph::lookup<ids>(54u));
  static_assert(mph::lookup<ids>(32u));
  static_assert(mph::lookup<ids>(64u));
  static_assert(mph::lookup<ids>(234u));
  static_assert(mph::lookup<ids>(91u));

  return *mph::lookup<ids>(argc);
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

```cpp
Iterations:        100
Instructions:      600
Total Cycles:      154
Total uOps:        800

Dispatch Width:    6
uOps Per Cycle:    5.19
IPC:               3.90
Block RThroughput: 1.3

Instruction Info:
[1]: #uOps
[2]: Latency
[3]: RThroughput
[4]: MayLoad

[1]    [2]    [3]    [4]    Instructions:
 1      1     0.25          mov   edx, 7
 1      0     0.17          xor   eax, eax
 1      3     1.00          pext  edx, edi, edx
 1      1     0.25          mov   edx, edx
 2      6     0.50    *     cmp   dword ptr [8*rdx + lookup<ids, 1u, 0u>], edi
 2      6     0.50    *     cmove eax, dword ptr [8*rdx + lookup<ids, 1u, 0u>+4]
```

---

### Performance (https://godbolt.org/z/cMbhdPoWd)

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

  return *mph::lookup<symbols>(
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

### Performance (https://godbolt.org/z/c361MvTMa)

```cpp
int main(int, const char** argv) {
  // values assigned from 0..N-1
  static constexpr std::array symbols{
    "BTC "sv, "ETH "sv, "BNB "sv,
    "SOL "sv, "XRP "sv, "DOGE"sv,
    "TON "sv, "ADA "sv, "SHIB"sv,
    "AVAX"sv, "LINK"sv, "BCH "sv,
  };

  return *mph::lookup<symbols>(
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

### Performance (https://godbolt.org/z/cPMsMf9E8)

```cpp
int main(int, const char** argv) {
  static constexpr std::array symbols{
    "BTC "sv, "ETH "sv, "BNB "sv,
    "SOL "sv, "XRP "sv, "DOGE"sv,
    "TON "sv, "ADA "sv, "SHIB"sv,
    "AVAX"sv, "LINK"sv, "BCH "sv,
  };

  static constexpr auto lookup = mph::lookup<symbols>;
  static constexpr auto probability = 100; // input keys are always valid

  [[assume(std::find(symbols.cbegin(),
                     symbols.cend(),
                     std::string_view(argv[1])) != symbols.cend())]];

  return *lookup.operator()<probability>(
    std::span<const char, 4>(argv[1], argv[1]+4)
  );
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

- [feature] `lookup` customization point - https://godbolt.org/z/K3Yoa7c7W
- [example] branchless dispatcher - https://godbolt.org/z/Yn911sfax
- [performance] `enum_to_string` (https://wg21.link/P2996) - https://godbolt.org/z/3894Wjads

---

<a name="benchmarks"></a>
### Benchmarks (...)

---

### API

```cpp
/**
 * Static perfect hash lookup function
 *
 * @tparam entries constexpr array of keys or key/value pairs
 * @tparam bucket_size how many buckets (default: deduce based on entries size)
 * @tparam alignment lookup table alignment (default: no alignment specified)
 */
template<
  const auto& entries,
  u32 bucket_size = [](u32 size) {
    if (size <= 1024u) return 1u;
    if (size <= 2048u) return 4u;
    if (size <= 4096u) return 8u;
    return 16u;
  }(entries.size()),
  u32 alignment = u32{}
> inline constexpr auto lookup {
  template<u8 probability = 50u>
    requires (probability >= 0u and probability <= 100u)
  [[nodiscard]] constexpr auto operator()(const auto& key) const noexcept -> optional;
};
```

> Configuration

```cpp
#define MPH 3'0'0       // Current library version (SemVer)
#define MPH_PAGE_SIZE   // [default: not defined]
                        // If defined safe memcpy will be used for string-like
                        // keys if the read is close to the page boundry (4096u)
```

---

### FAQ

- Limitations?

    > `mph` supports different types of key/value pairs and thousands of key/value pairs, but not millions - (see [benchmarks](#benchmarks)).

  - All keys have to fit into `std::uint128_t`, that includes strings.
  - If the above criteria are not satisfied `mph` will [SFINAE](https://en.wikipedia.org/wiki/Substitution_failure_is_not_an_error) away `lookup` function.
  - In such case different backup policy should be used instead (which can be also used as customization point for user-defined `lookup` implementations), for example:

    ```cpp
    template<const auto& entries>
      requires (entries.size() > 10'000)
    inline constexpr auto mph::lookup = [](const auto& key) -> optional { ... }
    ```

- How `mph` is working under the hood?

    > `mph` takes advantage of knowing the key/value pairs at compile-time as well as the specific hardware instructions.
      The following is a pseudo code of the algorithm.

    ```python
    def lookup[kv: array](key : any):
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

      # 1. lookup [run-time] / if key is a string convert to integral first (memcpy)
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

- Is support for [bmi2](https://en.wikipedia.org/wiki/X86_Bit_manipulation_instruction_set) instructions required?

    > No, `mph` works on platforms without them. `bmi2` instructions can be emulated* with some limitations in software with a bit slower execution.

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

    > https://stackoverflow.com/questions/14547087/extracting-bits-with-a-single-multiplication.

- How to disable `cmov` generation?

    > Set `probability` value to something else than `50u` (default) - it means that the input data is predictable in some way and `jmp` will be generated instead. Additionaly the following compiler options can be used.

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
      GIT_TAG v2.5.0
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
