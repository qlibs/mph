#
# Copyright (c) 2024 Kris Jusiak (kris at jusiak dot net)
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
CXX?=g++
CXXFLAGS?=-std=c++20 -Ofast -DNDEBUG -DNTEST -march=native -static -s
LDDFLAGS?=
TMP?=/dev/shm

%:
	${CXX} \
	-I. -I.. -I  ~/Projects/boost-1.85 ${CXXFLAGS} \
	-DSIZE=${SIZE} \
	-DUNDERLYING_TYPE=${UNDERLYING_TYPE} \
	-DPROBABILITY=${PROBABILITY} \
	-DSEED=${SEED} \
	-DBENCHMARK=${BENCHMARK} \
	$*.cpp ${LDDFLAGS} -o ${TMP}/$*

all:
	jupyter notebook -ip 0.0.0.0 --no-browser benchmark.ipynb
