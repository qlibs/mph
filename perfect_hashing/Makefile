#
# Copyright (c) 2024 Kris Jusiak (kris at jusiak dot net)
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
CXX=clang++-18
CXXFLAGS?=-std=c++20 -Ofast -DNDEBUG -DNTEST -march=native
LDDFLAGS?=
TMP?=/dev/shm

ifeq ($(findstring clang++,$(CXX)),clang++)
    CXXFLAGS := ${CXXFLAGS} -fconstexpr-steps=1000000000 -ftemplate-depth=1024
else ifeq ($(findstring g++,$(CXX)),g++)
    CXXFLAGS := ${CXXFLAGS} -fconstexpr-ops-limit=10000000000 -ftemplate-depth=1024
endif

gperf:
	gperf ${TMP}/data.gperf --output-file=${TMP}/gperf.hpp --multiple-iterations=1
	${CXX} \
	-I. -I.. ${CXXFLAGS} \
	-DSIZE=${SIZE} \
	-DPROBABILITY=${PROBABILITY} \
	-DSEED=${SEED} \
	-DBENCHMARK=${BENCHMARK} \
	gperf.cpp ${LDDFLAGS} -o ${TMP}/gperf

%:
	${CXX} \
	-I. -I.. -I ~/Projects/boost-1.85 ${CXXFLAGS} -DNTEST ${CONSTEXPR_LIMIT} \
	-DSIZE=${SIZE} \
	-DPROBABILITY=${PROBABILITY} \
	-DSEED=${SEED} \
	-DBENCHMARK=${BENCHMARK} \
	$*.cpp ${LDDFLAGS} -o ${TMP}/$*

all:
	jupyter notebook -ip 0.0.0.0 --no-browser benchmark.ipynb
