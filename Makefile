all:
	g++ --language c++ -Wall -Wextra -pedantic -Werror -pedantic-errors -std=c++20 -fconstexpr-ops-limit=100000000 -march=native mph -Ofast -DMPH_TEST && ./a.out
	clang++-16 --language c++ -Wall -Wextra -pedantic -Werror -pedantic-errors -fconstexpr-steps=100000000 -stdlib=libc++ -std=c++20 -march=native mph -Ofast -DMPH_TEST && ./a.out
