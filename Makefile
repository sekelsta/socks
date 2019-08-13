CXX = g++
# For debugging use -Og -g
# For going fast use -O3
CXXFLAGS = -std=c++14 -Wall -D_GLIBCXX_DEBUG -D_LIBCXX_DEBUG_PEDANTIC -Og -g

# Keep track of dependencies
utils.hh.d = utils.hh
filereader.hh.d = filereader.hh $(fileheader.hh.d) $(jsoninfo.hh.d)
fileheader.hh.d = fileheader.hh
jsoninfo.hh.d = jsoninfo.hh json.hpp
doc_table.hh.d = doc_table.hh jsoninfo.hh $(fileheader.hh.d)
file_utils.hh.d = file_utils.hh

all: main

main: main.cc utils.hh $(filereader.hh.d) filereader.cc $(jsoninfo.hh.d) $(fileheader.hh.d) fileheader.cc $(doc_table.hh.d) doc_table.cc
	$(CXX) $(CXXFLAGS) main.cc filereader.cc fileheader.cc doc_table.cc -o $@

main.o: main.cc $(utils.hh.d) $(filereader.hh.d) json.hpp
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

fileheader.o: fileheader.cc $(fileheader.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

filereader.o: filereader.cc $(filereader.hh.d) $(file_utils.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

test_header: test_header.cc $(fileheader.hh.d) $(doc_table.hh.d) fileheader.cc doc_table.cc
	$(CXX) $(CXXFLAGS) test_header.cc fileheader.cc doc_table.cc -o $@

# To remove generated files
clean: 
	rm -f *.o *.gch main

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

.PHONY: all clean

include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(SOURCEFILES))))
