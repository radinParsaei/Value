CXXFLAGS=-I..
EXAMPLES=e1
all: $(EXAMPLES)
ifeq ($(USE_GMP_LIB),1)
%: %.cpp
	$(CXX) $< -o $@ $(CXXFLAGS) -DUSE_GMP_LIB -lgmp -lgmpxx
else
%: %.cpp
	test -e ../BigNumber || (cd .. && git clone https://github.com/radinParsaei/BigNumber)
	$(MAKE) BigNumber.o -C ../BigNumber/desktop-sources
	$(MAKE) number.o -C ../BigNumber/desktop-sources
	$(CXX) $< -o $@ $(CXXFLAGS) -I../BigNumber/src/ ../BigNumber/desktop-sources/number.o ../BigNumber/desktop-sources/BigNumber.o
endif

clean:
	$(RM) $(EXAMPLES)
