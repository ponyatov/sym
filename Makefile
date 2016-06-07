# sym : Reference SYMbolic Engine (Dynamic Language Runtime)
# (c) Dmitry Ponyatov <dponyatov@gmail.com> , GNU LesserGPL
MODULE = $(notdir $(CURDIR))
OS ?= linux
log.log : src.src ./exe.exe
	./exe.exe < $< > $> log.log && tail $(TAIL) $@
C = cpp.cpp $(OS).cpp ypp.tab.cpp lex.yy.c
H = hpp.hpp $(OS).hpp ypp.tab.hpp meta.hpp
CXXFLAGS += -std=gnu++11 -D MODULE=\"$(MODULE)\"
./exe.exe : $(C) $(H)
	$(CXX) $(CXXFLAGS) -o $@ $(C) $(L)
ypp.tab.cpp : ypp.ypp
	bison $<
lex.yy.c : lpp.lpp
	flex $<

.PHONY: upgrade
upgrade:
	cp $(MODULE)/README.md ./
	cp $(MODULE)/gitignore .gitignore
	cp $(MODULE)/bat.bat ./
	cp $(MODULE)/sh.sh ./
