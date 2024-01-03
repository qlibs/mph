all:
	g++ -Wall -Wextra -pedantic -Werror -pedantic-errors -DNDEBUG -std=c++20 -fconstexpr-ops-limit=100000000 -march=native benchmark.cpp -Ofast -I. -I.. && ./a.out
	clang++ -Wall -Wextra -pedantic -Werror -pedantic-errors -DNDEBUG -fconstexpr-steps=100000000 -stdlib=libc++ -std=c++20 -march=native benchmark.cpp -Ofast -I. -I.. && ./a.out
