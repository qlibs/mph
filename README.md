<a href="http://www.boost.org/LICENSE_1_0.txt" target="_blank">![Boost Licence](http://img.shields.io/badge/license-boost-blue.svg)</a>
<a href="https://github.com/boost-ext/mph/releases" target="_blank">![Version](https://badge.fury.io/gh/boost-ext%2Fmph.svg)</a>
<a href="https://github.com/boost-ext/mph/actions/workflows/build.yml" target="_blank">![build](https://github.com/boost-ext/mph/actions/workflows/build.yml/badge.svg)</a>
<a href="https://godbolt.org/z/9qTKoj7no">![Try it online](https://img.shields.io/badge/try%20it-online-blue.svg)</a>

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
```

### Usage

```cpp
```

### x86-64 assembly ()

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

- github workflow
- memcpy speed up
- README
- endianess
- swar in the policies for < 4
