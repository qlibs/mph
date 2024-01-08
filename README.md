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
```

### API

```
```

```
```

### Benchmarks

```
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

#### TODO

- memcpy pruning
- pext_32 if <= 4 bytes
- enable github workflow
- find hash policy
- custom return type array {
-     "av" : 1,
-     "ab" : 3,
- }
- remove array_view if span gives the same asm
- cmove?
