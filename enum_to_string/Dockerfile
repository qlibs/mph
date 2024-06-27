#
# Copyright (c) 2024 Kris Jusiak (kris at jusiak dot net)
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
FROM ubuntu:jammy
ARG CLANG=18
ARG GCC=13
ARG BOOST=1_85_0

# dependencies
RUN apt-get update && apt-get install -y software-properties-common lsb-release gnupg wget bzip2 git cmake linux-tools-generic hwloc valgrind python3-pip
RUN pip3 install jupyter pyperf numpy matplotlib joblib

# clang
RUN wget https://apt.llvm.org/llvm.sh && chmod +x llvm.sh && ./llvm.sh $CLANG && rm llvm.sh

## gcc
RUN add-apt-repository ppa:ubuntu-toolchain-r/test && apt update && apt install g++-$GCC -y

# boost
RUN wget -c https://archives.boost.io/release/`echo $BOOST | sed "s/_/\./g"`/source/boost_$BOOST.tar.bz2 && tar xvjf boost_$BOOST.tar.bz2 && cp -r boost_$BOOST/boost /usr/include

# absl
RUN git clone https://github.com/abseil/abseil-cpp.git && cd abseil-cpp && cmake -Bbuild -H. -DCMAKE_BUILD_TYPE=Release && cmake --build build --target install --config Release
