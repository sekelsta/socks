CXX = g++
# For debugging use -Og -g
# For going fast use -O3
CXXFLAGS = -std=c++14 -Wall -D_GLIBCXX_DEBUG -D_LIBCXX_DEBUG_PEDANTIC -Og -g

# Keep track of dependencies
utils.hh.d = utils.hh
filereader.hh.d = filereader.hh $(fileheader.hh.d) $(jsoninfo.hh.d)
fileheader.hh.d = fileheader.hh
jsoninfo.hh.d = jsoninfo.hh json.hpp

all: main

main: main.cc utils.hh filereader.hh filereader.cc jsoninfo.hh fileheader.hh fileheader.cc
	$(CXX) $(CXXFLAGS) main.cc filereader.cc fileheader.cc -o $@

main.o: main.cc $(utils.hh.d) $(filereader.hh.d) json.hpp
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

fileheader.o: fileheader.cc $(fileheader.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

filereader.o: filereader.cc $(filereader.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

# To remove generated files
clean: 
	rm -f *.o *.gch main

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

.PHONY: all clean

include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(SOURCEFILES))))
