CXX = g++

CLANGTIDY = -checks=*,clang-analyzer-*,-clang-analyzer-cplusplus*

#CLANGWARNINGS=-Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-missing-prototypes -pedantic -pedantic-errors
WARNINGS=-Wall -Wextra -Wshadow -Wconversion -Wsign-conversion -Woverloaded-virtual -Wold-style-cast -Weffc++ -pedantic -pedantic-errors
GCCSPECIFIC=-fopt-info
#-fcheck-pointer-bounds

INCLUDES =
LIBS = -lcurl -Lcurleasy -lcurleasy
STD = c++17

example : example.o curleasy/libcurleasy.a Makefile
	$(CXX) -DNDEBUG $(INCLUDES) -std=$(STD) -O3 $(WARNINGS) -o $@ $< $(LIBS)

curleasy/curleasy.o : curleasy/curleasy.cpp curleasy/curleasy.hpp Makefile
	$(CXX) -DNDEBUG $(INCLUDES) -std=$(STD) -O3 $(WARNINGS) -c -o $@ $<

curleasy/libcurleasy.a : curleasy/curleasy.o Makefile
	ar rvs $@ $<

%.o : %.cpp
	$(CXX) -DNDEBUG $(INCLUDES) -std=$(STD) -O3 $(WARNINGS) -c -o $@ $<

clean:
	rm -f curleasy/curleasy.o curleasy/libcurleasy.a
