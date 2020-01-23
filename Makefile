CXX = g++

CLANGTIDY = -checks=*,clang-analyzer-*,-clang-analyzer-cplusplus*

#WARNINGS=-Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-missing-prototypes -pedantic -pedantic-errors
WARNINGS=-Wall -Wextra -Weffc++ -Werror -Wshadow -Wconversion -Wsign-conversion -Woverloaded-virtual -Wold-style-cast -pedantic -pedantic-errors
GCCSPECIFIC=-fopt-info
#-fcheck-pointer-bounds

INCLUDES =
LIBS = -lcurl -Lcurleasy -lcurleasy
STD = -std=c++17

COMN = $(CXX)
COMP = $(COMN) $(STD) -DNDEBUG -O3 $(WARNINGS) $(INCLUDES) -c -o $@ $<
LINK = $(COMN) -o $@ $< $(LIBS)

example : example.o curleasy/libcurleasy.a Makefile
	$(LINK)

curleasy/libcurleasy.a : curleasy/curleasy.o Makefile
	ar rvs $@ $<

%.o : %.cpp Makefile
	$(COMP)

clean:
	rm -f example example.o curleasy/curleasy.o curleasy/libcurleasy.a
