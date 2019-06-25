
#CXX = clang++
CXX = g++
STATIC_ANALYSIS = scan-build -v --force-analyze-debug-code
#SANITIZE = -fsanitize=undefined -fsanitize=address
SANITIZE = -fsanitize=undefined

# UBSAN  ... libubsan.x86_64 etc. hur använda?

CLANGTIDY = -checks=*,clang-analyzer-*,-clang-analyzer-cplusplus*

#WARNINGS=-Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-missing-prototypes -pedantic -pedantic-errors
WARNINGS=-Wall -Wextra -Wshadow -Wconversion -Wsign-conversion -Woverloaded-virtual -Wold-style-cast -Weffc++ -pedantic -pedantic-errors
GCCSPECIFIC=-fopt-info
#-fcheck-pointer-bounds

INCGTK = -I/usr/include/gtkmm-3.0 -I/usr/include/glibmm-2.4 -I/usr/lib64/glibmm-2.4/include -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include -I/usr/include/sigc++-2.0 -I/usr/lib64/sigc++-2.0/include
INCPELLE = -I ~/proj/random/evensen_mixer
INCLUDES = -I ~/git/yaml-cpp/include -I /usr/include/python3.7m

BIGNUMLIBS = -lgmpxx -lgmp -lmpfr
LIBS = -lstdc++fs -lpthread -lm -lcrypto -lpam -ltbb -lcurl $(BIGNUMLIBS)

STD = c++17

% : %.cpp

#clang-tidy $< $(CLANGTIDY);

% : %.cpp
	clang-format -i $< ;
	$(STATIC_ANALYSIS) $(CXX) -DNDEBUG $(INCLUDES) $(SANITIZE) -std=$(STD) -g3 -O3 $(WARNINGS) -o $@ $< $(LIBS)


%.o : %.cpp
	$(STATIC_ANALYSIS) $(CXX) -DNDEBUG $(INCLUDES) $(SANITIZE) -std=$(STD) -g3 -O3 $(WARNINGS) -c -o $@ $<

#%.clang.cpp : %.cpp
#	clang-format $< > $@

interface_use : interface_use.o interface.o interface.hpp Makefile
	$(STATIC_ANALYSIS) $(CXX) -DNDEBUG $(INCLUDES) $(SANITIZE) -std=$(STD) -g3 -O3 $(WARNINGS) -o $@ interface_use.o interface.o $(LIBS)

boostpython.o : boostpython.cpp
	$(CXX) -DNDEBUG $(INCLUDES) -std=$(STD) -O3 $(WARNINGS) -c -fPIC -o $@ $< $(LIBS) -lboost_python3

my_first.so : boostpython.o
	g++ -shared -o $@ boostpython.o -lboost_python3

