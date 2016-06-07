MODULE = $(notdir $(CURDIR))
log.log: src.src ./exe.exe
	./exe.exe < $< > $@ && tail $(TAIL) $@
C = cpp.cpp $(OS).cpp ypp.tab.cpp lex.yy.c
H = hpp.hpp $(OS).hpp ypp.tab.hpp meta.hpp
CXXFLAGS += -std=gnu++11 -D MODULE=\"$(MODULE)\"
./exe.exe: $(C) $(H)
	$(CXX) $(CXXFLAGS) -o $@ $(C) $(L)
ypp.tab.cpp: ypp.ypp
	bison $<
lex.yy.c: lpp.lpp
	flex $<

.PHONY: upgrade
upgrade: README.md .gitignore
%: $(MODULE)/%
	cp $< $@
.gitignore: $(MODULE)/gitignore
	cp $< $@
	