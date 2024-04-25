<a href="http://www.boost.org/LICENSE_1_0.txt" target="_blank">![Boost Licence](http://img.shields.io/badge/license-boost-blue.svg)</a>
<a href="https://github.com/boost-ext/mph/releases" target="_blank">![Version](https://badge.fury.io/gh/boost-ext%2Fmph.svg)</a>
<a href="https://github.com/boost-ext/mph/actions/workflows/build.yml" target="_blank">![build](https://github.com/boost-ext/mph/actions/workflows/build.yml/badge.svg)</a>
<a href="https://godbolt.org/z/a6czaG8f7">![Try it online](https://img.shields.io/badge/try%20it-online-blue.svg)</a>

---------------------------------------

## Minimal perfect hash function (focused on the run-time performance)

> https://en.wikipedia.org/wiki/Perfect_hash_function#Minimal_perfect_hash_function

### Use case

> Given a list of N keys (known at compile-time) find a perfect hash function (maps keys into range from 0 to N-1) with the fastest run-time execution.

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

### Hello world

> Strings (https://godbolt.org/z/fWY8YYeEE)

```cpp
enum class color { red, green, blue };

constexpr auto colors = std::array{
  mph::pair{"red", color::red}, // same as std::pair{mph::fixed_string{"red", color::red}}
  mph::pair{"green", color::green},
  mph::pair{"blue", color::blue},
};

static_assert(color::green == *mph::hash<colors>("green"));
static_assert(color::red   == *mph::hash<colors>("red"));
static_assert(color::blue  == *mph::hash<colors>("blue"));

assert(mph::hash<colors>("green"));
std::print("{}", *mph::hash<colors>("green")); // prints 1
```

---

> Numbers (https://godbolt.org/z/bGE6qfYqj)

```cpp
constexpr auto primes = std::array{
  std::pair{2, 0}, std::pair{3, 1}, std::pair{5, 2},
  std::pair{7, 3}, std::pair{11, 4}, std::pair{13, 5},
  std::pair{17, 6}, std::pair{19, 7}, std::pair{23, 8},
  std::pair{29, 9}, std::pair{31, 10}, std::pair{37, 11},
  std::pair{41, 12}, std::pair{43, 13}, std::pair{47, 14},
};

constexpr auto hash = mph::hash<primes>;

static_assert(*hash(2) == 0 and *hash(3) == 1 and
              *hash(5) == 2 and *hash(7) == 3 and
              *hash(11) == 4 and *hash(13) == 5 and
              *hash(17) == 6 and *hash(19) == 7 and
              *hash(23) == 8 and *hash(29) == 9 and
              *hash(31) == 10 and *hash(37) == 11 and
              *hash(41) == 12 and *hash(43) == 13 and
              *hash(47) == 14);

assert(hash(41));
std::print("{}", *hash(41)); // prints 12
```

---

### Performance

```cpp
int main(int argc, const char** argv) {
 constexpr auto symbols = std::array{
   mph::pair{"AAPL    ", 0},
   mph::pair{"AMZN    ", 1},
   mph::pair{"GOOGL   ", 2},
   mph::pair{"MSFT    ", 3},
   mph::pair{"NVDA    ", 4}
 };

  static_assert(0 == *mph::hash<symbols>("AAPL    "));
  static_assert(1 == *mph::hash<symbols>("AMZN    "));
  static_assert(2 == *mph::hash<symbols>("GOOGL   "));
  static_assert(3 == *mph::hash<symbols>("MSFT    "));
  static_assert(4 == *mph::hash<symbols>("NVDA    "));

  return *mph::hash<symbols>(std::span<const char, 8u>(argv[1], argv[1] + 8u));
}
```

> x86-64 assembly (https://godbolt.org/z/vnb7Pxb6a)

```
main:
  movq 8(%rsi), %rax
  movl $436207616, %edx
  movq (%rax), %rax
  pext %rdx, %rax, %rdx
  cmpq %rax, mph::v_1_0_0::pext...lookup(,%rdx,8)
  movl $0, %eax
  cmove mph::v_1_0_0::pext...index(,%rdx,4), %eax
  ret
mph::v_1_0_0::pext...index:
  .byte 4
  .byte 2
  .byte 0
  .byte 1
  .byte 3
  .byte 5
  .byte 5
  .byte 5
mph::v_1_0_0::pext...lookup:
  .quad 2314885531374474830
  .quad 2314885720454418247
  .quad 2314885531559805249
  .quad 2314885531594018113
  .quad 2314885531693372237
  .zero 24
```

> `llvm-mca` (https://godbolt.org/z/EM1rrxhvW) | `uiCA` ([https://uica.uops.info](https://uica.uops.info/?code=main%3A%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20%40main%0D%0A%20%20%20%20%20%20%20%20mov%20%20%20%20%20rax%2C%20qword%20ptr%20%5Brsi%20%2B%208%5D%0D%0A%20%20%20%20%20%20%20%20mov%20%20%20%20%20ecx%2C%20436207616%0D%0A%20%20%20%20%20%20%20%20lea%20%20%20%20%20rdx%2C%20%5Brip%20%2B%20_ZZNK3mph7v_1_0_54pextILm7EXtlNS0_11conditionalMUlbT_T0_E_EEEEclITnDaXtlNS0_8optionalIiLj5EEELh5EEETnDaXtlSt5arrayINS0_4pairIPKciEELm5EEtlA5_SE_tlSE_tlNS0_18basic_fixed_stringIcLm32EEELm8EtlSA_IcLm32EEtlA32_cLc65ELc65ELc80ELc76ELc32ELc32ELc32ELc32EEEEEtlSE_tlSI_Lm8EtlSJ_tlSK_Lc65ELc77ELc90ELc78ELc32ELc32ELc32ELc32EEEELi1EEtlSE_tlSI_Lm8EtlSJ_tlSK_Lc71ELc79ELc79ELc71ELc76ELc32ELc32ELc32EEEELi2EEtlSE_tlSI_Lm8EtlSJ_tlSK_Lc77ELc83ELc70ELc84ELc32ELc32ELc32ELc32EEEELi3EEtlSE_tlSI_Lm8EtlSJ_tlSK_Lc78ELc86ELc68ELc65ELc32ELc32ELc32ELc32EEEELi4EEEEERKSt4spanISC_Lm8EEmTnDaLm436207616EJEQaasr8conceptsE12bits_size_leIXTL0_3_EXT_EEsr8conceptsE16all_keys_size_ltIXTL0_0_EXstTL0_2_EEEEDtT_EOT1_DpOT4_E6lookup%5D%0D%0A%20%20%20%20%20%20%20%20mov%20%20%20%20%20rax%2C%20qword%20ptr%20%5Brax%5D%0D%0A%20%20%20%20%20%20%20%20pext%20%20%20%20rcx%2C%20rax%2C%20rcx%0D%0A%20%20%20%20%20%20%20%20cmp%20%20%20%20%20qword%20ptr%20%5Brdx%20%2B%208*rcx%5D%2C%20rax%0D%0A%20%20%20%20%20%20%20%20lea%20%20%20%20%20rax%2C%20%5Brip%20%2B%20_ZZNK3mph7v_1_0_54pextILm7EXtlNS0_11conditionalMUlbT_T0_E_EEEEclITnDaXtlNS0_8optionalIiLj5EEELh5EEETnDaXtlSt5arrayINS0_4pairIPKciEELm5EEtlA5_SE_tlSE_tlNS0_18basic_fixed_stringIcLm32EEELm8EtlSA_IcLm32EEtlA32_cLc65ELc65ELc80ELc76ELc32ELc32ELc32ELc32EEEEEtlSE_tlSI_Lm8EtlSJ_tlSK_Lc65ELc77ELc90ELc78ELc32ELc32ELc32ELc32EEEELi1EEtlSE_tlSI_Lm8EtlSJ_tlSK_Lc71ELc79ELc79ELc71ELc76ELc32ELc32ELc32EEEELi2EEtlSE_tlSI_Lm8EtlSJ_tlSK_Lc77ELc83ELc70ELc84ELc32ELc32ELc32ELc32EEEELi3EEtlSE_tlSI_Lm8EtlSJ_tlSK_Lc78ELc86ELc68ELc65ELc32ELc32ELc32ELc32EEEELi4EEEEERKSt4spanISC_Lm8EEmTnDaLm436207616EJEQaasr8conceptsE12bits_size_leIXTL0_3_EXT_EEsr8conceptsE16all_keys_size_ltIXTL0_0_EXstTL0_2_EEEEDtT_EOT1_DpOT4_E5index%5D%0D%0A%20%20%20%20%20%20%20%20movzx%20%20%20ecx%2C%20byte%20ptr%20%5Brcx%20%2B%20rax%5D%0D%0A%20%20%20%20%20%20%20%20mov%20%20%20%20%20eax%2C%205%0D%0A%20%20%20%20%20%20%20%20cmove%20%20%20eax%2C%20ecx%0D%0A%20%20%20%20%20%20%20%20ret%0D%0A_ZZNK3mph7v_1_0_54pextILm7EXtlNS0_11conditionalMUlbT_T0_E_EEEEclITnDaXtlNS0_8optionalIiLj5EEELh5EEETnDaXtlSt5arrayINS0_4pairIPKciEELm5EEtlA5_SE_tlSE_tlNS0_18basic_fixed_stringIcLm32EEELm8EtlSA_IcLm32EEtlA32_cLc65ELc65ELc80ELc76ELc32ELc32ELc32ELc32EEEEEtlSE_tlSI_Lm8EtlSJ_tlSK_Lc65ELc77ELc90ELc78ELc32ELc32ELc32ELc32EEEELi1EEtlSE_tlSI_Lm8EtlSJ_tlSK_Lc71ELc79ELc79ELc71ELc76ELc32ELc32ELc32EEEELi2EEtlSE_tlSI_Lm8EtlSJ_tlSK_Lc77ELc83ELc70ELc84ELc32ELc32ELc32ELc32EEEELi3EEtlSE_tlSI_Lm8EtlSJ_tlSK_Lc78ELc86ELc68ELc65ELc32ELc32ELc32ELc32EEEELi4EEEEERKSt4spanISC_Lm8EEmTnDaLm436207616EJEQaasr8conceptsE12bits_size_leIXTL0_3_EXT_EEsr8conceptsE16all_keys_size_ltIXTL0_0_EXstTL0_2_EEEEDtT_EOT1_DpOT4_E6lookup%3A%0D%0A%20%20%20%20%20%20%20%20.quad%20%20%202314885531374474830%20%20%20%20%20%20%20%20%20%20%20%20%20%23%200x202020204144564e%0D%0A%20%20%20%20%20%20%20%20.quad%20%20%202314885720454418247%20%20%20%20%20%20%20%20%20%20%20%20%20%23%200x2020204c474f4f47%0D%0A%20%20%20%20%20%20%20%20.quad%20%20%202314885531559805249%20%20%20%20%20%20%20%20%20%20%20%20%20%23%200x202020204c504141%0D%0A%20%20%20%20%20%20%20%20.quad%20%20%202314885531594018113%20%20%20%20%20%20%20%20%20%20%20%20%20%23%200x202020204e5a4d41%0D%0A%20%20%20%20%20%20%20%20.quad%20%20%202314885531693372237%20%20%20%20%20%20%20%20%20%20%20%20%20%23%200x202020205446534d%0D%0A%20%20%20%20%20%20%20%20.quad%20%20%200%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%200x0%0D%0A%20%20%20%20%20%20%20%20.quad%20%20%200%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%200x0%0D%0A%20%20%20%20%20%20%20%20.quad%20%20%200%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%200x0%0D%0A%0D%0A_ZZNK3mph7v_1_0_54pextILm7EXtlNS0_11conditionalMUlbT_T0_E_EEEEclITnDaXtlNS0_8optionalIiLj5EEELh5EEETnDaXtlSt5arrayINS0_4pairIPKciEELm5EEtlA5_SE_tlSE_tlNS0_18basic_fixed_stringIcLm32EEELm8EtlSA_IcLm32EEtlA32_cLc65ELc65ELc80ELc76ELc32ELc32ELc32ELc32EEEEEtlSE_tlSI_Lm8EtlSJ_tlSK_Lc65ELc77ELc90ELc78ELc32ELc32ELc32ELc32EEEELi1EEtlSE_tlSI_Lm8EtlSJ_tlSK_Lc71ELc79ELc79ELc71ELc76ELc32ELc32ELc32EEEELi2EEtlSE_tlSI_Lm8EtlSJ_tlSK_Lc77ELc83ELc70ELc84ELc32ELc32ELc32ELc32EEEELi3EEtlSE_tlSI_Lm8EtlSJ_tlSK_Lc78ELc86ELc68ELc65ELc32ELc32ELc32ELc32EEEELi4EEEEERKSt4spanISC_Lm8EEmTnDaLm436207616EJEQaasr8conceptsE12bits_size_leIXTL0_3_EXT_EEsr8conceptsE16all_keys_size_ltIXTL0_0_EXstTL0_2_EEEEDtT_EOT1_DpOT4_E5index%3A%0D%0A%20%20%20%20%20%20%20%20.byte%20%20%204%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%200x4%0D%0A%20%20%20%20%20%20%20%20.byte%20%20%202%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%200x2%0D%0A%20%20%20%20%20%20%20%20.zero%20%20%201%0D%0A%20%20%20%20%20%20%20%20.byte%20%20%201%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%200x1%0D%0A%20%20%20%20%20%20%20%20.byte%20%20%203%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%200x3%0D%0A%20%20%20%20%20%20%20%20.byte%20%20%205%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%200x5%0D%0A%20%20%20%20%20%20%20%20.byte%20%20%205%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%200x5%0D%0A%20%20%20%20%20%20%20%20.byte%20%20%205%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%200x5&syntax=asIntel&uArchs=SKL&tools=uiCA&tools=IACA3&tools=IACA23&tools=llvm&tools=OSACA&tools=CQA&alignment=0))

```
Dispatch Width:    6
uOps Per Cycle:    5.38
IPC:               4.23
Block RThroughput: 2.5

Instruction Info:
[1]: #uOps
[2]: Latency
[3]: RThroughput
[4]: MayLoad
[5]: MayStore
[6]: HasSideEffects (U)

[1]    [2]    [3]    [4]    [5]    [6]    Instructions:
 1      5     0.50    *                   movq  8(%rsi), %rax
 1      1     0.25                        movl  $436207616, %ecx
 1      1     0.50                        leaq  _ZZNK3mph7v_1_0_34pext...lookup(%rip), %rdx
 1      5     0.50    *                   movq  (%rax), %rax
 1      3     1.00                        pextq %rcx, %rax, %rcx
 2      6     0.50    *                   cmpq  %rax, (%rdx,%rcx,8)
 1      1     0.50                        leaq  _ZZNK3mph7v_1_0_34pext...index(%rip), %rax
 1      5     0.50    *                   movzbl        (%rcx,%rax), %ecx
 1      1     0.25                        movl  $5, %eax
 1      1     0.50                        cmovel        %ecx, %eax
 3      7     1.00                  U     retq

Resources:
[0]   - SKLDivider
[1]   - SKLFPDivider
[2]   - SKLPort0
[3]   - SKLPort1
[4]   - SKLPort2
[5]   - SKLPort3
[6]   - SKLPort4
[7]   - SKLPort5
[8]   - SKLPort6
[9]   - SKLPort7

Resource pressure per iteration:
[0]    [1]    [2]    [3]    [4]    [5]    [6]    [7]    [8]    [9]
 -      -     2.18   2.32   2.50   2.50    -     2.31   2.19    -

Resource pressure by instruction:
[0]    [1]    [2]    [3]    [4]    [5]    [6]    [7]    [8]    [9]    Instructions:
 -      -      -      -     0.57   0.43    -      -      -      -     movq      8(%rsi), %rax
 -      -     0.25   0.18    -      -      -     0.33   0.24    -     movl      $436207616, %ecx
 -      -      -     0.28    -      -      -     0.72    -      -     leaq      _ZZNK3mph7v_1_0_34pext...lookup(%rip), %rdx
 -      -      -      -     0.41   0.59    -      -      -      -     movq      (%rax), %rax
 -      -      -     1.00    -      -      -      -      -      -     pextq     %rcx, %rax, %rcx
 -      -     0.42   0.33   0.37   0.63    -     0.04   0.21    -     cmpq      %rax, (%rdx,%rcx,8)
 -      -      -     0.31    -      -      -     0.69    -      -     leaq      _ZZNK3mph7v_1_0_34pext...index(%rip), %rax
 -      -      -      -     0.63   0.37    -      -      -      -     movzbl    (%rcx,%rax), %ecx
 -      -     0.38   0.11    -      -      -     0.16   0.36    -     movl      $5, %eax
 -      -     0.63    -      -      -      -      -     0.37    -     cmovel    %ecx, %eax
 -      -     0.51   0.11   0.52   0.48    -     0.38   1.00    -     retq
```

### Performance [potentially unsafe]

> If `all` possible inputs are known AND can be found in the keys, then `unconditional` policy for the config can be used which will avoid one comparison

```cpp
int main(int argc, [[maybe_unused]] const char** argv) {
  static constexpr auto symbols = std::array{
    std::pair{std::array{'A', 'A', 'P', 'L', ' ', ' ', ' ', ' '}, 0},
    std::pair{std::array{'A', 'M', 'Z', 'N', ' ', ' ', ' ', ' '}, 1},
    std::pair{std::array{'G', 'O', 'O', 'G', 'L', ' ', ' ', ' '}, 2},
    std::pair{std::array{'M', 'S', 'F', 'T', ' ', ' ', ' ', ' '}, 3},
    std::pair{std::array{'N', 'V', 'D', 'A', ' ', ' ', ' ', ' '}, 4},
  };

  constexpr auto policies = []<const auto...ts>(auto&&... args) {
    return mph::pext<config<mph::unconditional>{.max_bits_size=7u}>{}>{}.template operator()<ts...>(std::forward<decltype(args)>(args)...);
  };

  assert(argc >= 0 and argc < std::size(symbols));
  return *mph::hash<symbols, policies>(symbols[argc].first);
}
```

> x86-64 assembly (https://godbolt.org/z/s4bTrd6G4)

```
main:
  movslq %edi, %rdi
  movl $436207616, %edx
  leaq (%rdi,%rdi,2), %rax
  movq main::symbols(,%rax,4), %rax
  pext %rdx, %rax, %rax
  movzbl mph::v_1_0_3::pext...index(%rax), %eax
  ret
mph::v_1_0_3::pext...index:
  .byte 4
  .byte 2
  .byte 0
  .byte 1
  .byte 3
  .byte 5
  .byte 5
  .byte 5
```

---

### Benchmarks (https://github.com/boost-ext/mph/actions/workflows/build.yml)

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
 * Minimal perfect hash function
 *
 * @tparam keys constexpr pair of id values such as std::array{{std::pair{"FOO"}, 1}, std::pair{"BAR"}, 2}}
 * @tparam policies invocable which returns the hash
 * @param data continuous input data such as std::string_view, std::span, std::array or intergral value
 * @param args... optional args propagated to policies
 * @return optional result of executing policies (*result = { mapped key: found, max_value<keys>+1u : not found })
 */
template<const auto keys, const auto policies = mph::policies> requires (std::size(keys) > 1u)
constexpr auto hash = [] [[nodiscard]] (auto&& data, auto &&...args) noexcept(true);
```

```cpp
/**
 * Minimal perfect hash function based on SWAR
 *  reads sizeof(T) bytes and switches on that
 */
template<class T>
struct swar {
  template <const auto unknown, const auto keys>
    requires concepts::all_keys_size_lt<keys, sizeof(T)>
  [[nodiscard]] constexpr auto operator()(auto&& data, [[maybe_unused]] auto &&...args) const noexcept(true);
};
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

```cpp
inline constexpr auto branchless_table = [](const bool cond, const auto lhs, const auto rhs) {
  return std::array{rhs, lhs}[cond];
};
```

```cpp
template<const auto Fn = conditional>
struct config {
  static constexpr auto apply = Fn; /// result policy (default: conditional)
  std::size_t max_bits_size{7u};    /// max bits size for the mask (default: 7)
  std::size_t alignment{};          /// should align tables (default: 0 - don't align)
  bool single_table{true};          /// merge index and lookup tables (default: true)
  bool minimize_storage{false};     /// use minimal available type for storage (default: false)
};
```

```cpp
/**
 * Minimal perfect hash function based on intel's pext with support up to 2^max_bits_size elements and with max 8 characters
 *  requires platform with bmi2 support (https://en.wikipedia.org/wiki/X86_Bit_manipulation_instruction_set)
 */
template <const std::size_t max_bits_size, const auto config = config>
struct pext {
  template <const auto unknown, const auto keys, class T, const auto mask = utility::find_mask<T>(keys())>
    requires concepts::bits_size_le<mask, max_bits_size> and concepts::all_keys_size_lt<keys, sizeof(T)>
  [[nodiscard]] [[gnu::target("bmi2")]] auto operator()(T&& data, [[maybe_unused]] auto &&...args) const noexcept(true);
};
```

```cpp
/**
 * Minimal perfect hash function based on intel's pext with support up to 2^max_bits_size per split on N'th character and with max 8 characters
 *  requires platform with bmi2 support (https://en.wikipedia.org/wiki/X86_Bit_manipulation_instruction_set)
 */
template <const std::size_t max_bits_size, const std::size_t split_index, const auto config = config>
struct pext_split {
  template <const auto unknown, const auto keys, class T, const auto masks = make_masks<T, keys>()>
    requires concepts::keys_bits_size_lt<masks, max_bits_size> and concepts::all_keys_size_lt<keys, sizeof(T)>
  [[nodiscard]] [[gnu::target("bmi2")]] auto operator()(T&& data, [[maybe_unused]] auto &&...args) const noexcept(true);
};
```

```cpp
/**
 * Default policies
 *
 * @tparam unknown returned if there is no match
 * @tparam keys constexpr pair of id values such as std::array{{std::pair{"FOO"}, 1}, std::pair{"BAR"}, 2}}
 * @param data continuous input data such as std::string_view, std::span, std::array or integral value
 * @param args... args propagated to policies
 * @return second from matched key or unknown if not matched
 */
constexpr auto policies = []<const auto unknown, const auto keys>(auto&& data, auto&&... args) {
  if constexpr (requires { std::size(data); }) {
    if constexpr (auto min_max = utility::min_max_length<keys>; min_max.first == min_max.second and min_max.first == sizeof(std::uint32_t) and std::size(keys()) < 4u) {
      return mph::swar<std::uint32_t>{}<unknown, keys>(data, args...);
    } else if constexpr (min_max.first == min_max.second and min_max.first == sizeof(std::uint64_t) and std::size(keys()) < 4u) {
      return mph::swar<std::uint64_t>{}<unknown, keys>(data, args...);
    } else if constexpr (auto pext = mph::pext<{.max_bits_size=7u}>{}; requires { pext<unknown, keys>(data, args...); }) {
      return pext<unknown, keys>(data, args...);
    } else if constexpr (auto pext_split = mph::pext_split<{.max_bits_size=7u, .split_index=utility::find_unique_char_max_dist<keys>}>{}; requires { pext_split<unknown, keys>(data, args...); }) {
      return pext_split<unknown, keys>(data, args...);
    } else {
      static_assert(false, "string hash can't be created with given policies!");
    }
  } else if constexpr (using T = std::remove_cvref_t<decltype(data)>; std::integral<T>) {
    if constexpr (sizeof(T) <= sizeof(std::uint32_t) and std::size(keys()) < 4u) {
      return mph::swar<std::uint32_t>{}<unknown, keys>(data, args...);
    } else if constexpr (sizeof(T) <= sizeof(std::uint64_t) and std::size(keys()) < 4u) {
      return mph::swar<std::uint64_t>{}<unknown, keys>(data, args...);
    } else if constexpr (auto pext = mph::pext<{.max_bits_size=7u}>{}; requires { pext<unknown, keys, T>(data, args...); }) {
      return pext<unknown, keys, T>(data, args...);
    } else {
      static_assert(false, "integral hash can't be created with given policies!");
    }
  } else {
    static_assert(false, "hash can't be created with given policies!");
  }
};
```

> Configuration

```cpp
#define MPH 1'0'8 // Current library version (SemVer)
#define MPH_FIXED_STRING_MAX_SIZE 32u // [default]
#define MPH_ALLOW_UNSAFE_MEMCPY 1 // [enabled by default] Faster but potentially unsafe memcpy, only required for string based keys
#define MPH_PAGE_SIZE 4096u // Only used if MPH_ALLOW_UNSAFE_MEMCPY is enabled
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

- Do I need modules support to use `mph`?

    > No. `mph` can be either included or imported.

- How to get the max performance out of `mph`?

    > Always measure! Custom polices might be a good place to start. For strings, consider aligning the input data and passing it with compile-time size via std::span, std::array.

- Can I do get better performance than `mph`?

    > Of course! The more knowledge of the input data and the hardware the better potential performance.
      `mph` is a library with flexibility in mind as there is no silver-bullet to the performance (Always measure!).
      `mph` allows customization (See [API](#api)) for specific use-cases/platforms/etc.
      The main idea is to have different, specialized policies which will perform best in specific circumstances and
      can be chosen at compile-time based on given list of key/value paris.

- I'm getting compilation error with longer list (>256) of keys?

    > Likely the constexpr computation limit has been reached. To fix that, the following options can be used to increase the limits.

    ```
    gcc:   -fconstexpr-ops-limit=100000000
    clang: -fconstexpr-steps=100000000
    ```

- I'm getting sanitizers warnings/errors?

    > When passing run-time size input and when `MPH_ALLOW_UNSAFE_MEMCPY` is enabled (default) `mph` will do potentially unsafe memory reads.
      If you concern about the performance verify that the unsafe memory reads are okay (see `MPH_PAGE_SIZE`), otherwise set `MPH_ALLOW_UNSAFE_MEMCPY=0`
      to disable them which will fix the sanitzers warnings.

- Ideas for policies?

    > [radix-tree](https://en.wikipedia.org/wiki/Radix_tree), [finite-state-machine](https://en.wikipedia.org/wiki/Finite-state_machine), [gperf](https://www.dre.vanderbilt.edu/~schmidt/PDF/C++-USENIX-90.pdf), [mph](http://stevehanov.ca/blog/index.php?id=119), [hash](https://en.wikipedia.org/wiki/Hash_function), [simd](https://en.wikipedia.org/wiki/Single_instruction,_multiple_data), [cmph](https://cmph.sourceforge.net/papers)

- Similar projects?

    > [gperf](https://www.gnu.org/software/gperf), [frozen](https://github.com/serge-sans-paille/frozen), [nbperf](https://github.com/rurban/nbperf), [cmph](https://cmph.sourceforge.net), [perfecthash](https://github.com/tpn/perfecthash), [LeMonHash](https://github.com/ByteHamster/LeMonHash), [PTHash](https://github.com/jermp/pthash), [ShockHash](https://github.com/ByteHamster/ShockHash), [BuRR](https://github.com/lorenzhs/BuRR), [hash-prospector](https://github.com/skeeto/hash-prospector)

- How can I contribute?

    > Please follow [CONTRIBUTING.md](.github/CONTRIBUTING.md)

- How to report a bug?

    > Please follow [PULL_REQUEST_TEMPLATE.md](.github/PULL_REQUEST_TEMPLATE.md)

- Acknowledgments

    > http://0x80.pl, https://lemire.me/blog, https://www.youtube.com/watch?v=DMQ_HcNSOAI&ab_channel=strager, https://www.dre.vanderbilt.edu/~schmidt/PDF/C++-USENIX-90.pdf, https://cmph.sourceforge.net/papers, https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html, https://gcc.gnu.org/onlinedocs/libstdc++, https://github.com/rurban/smhasher, http://stevehanov.ca/blog/index.php?id=119, https://nullprogram.com/blog/2018/07/31, https://github.com/tpn/pdfs/tree/master/Perfect%20Hashing

---

**Disclaimer** `mph` is not an official Boost library.
