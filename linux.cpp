#include "hpp.hpp"

Dir::Dir(string V):Sym("dir",V){ mkdir(V.c_str(),
		S_IRUSR|S_IWUSR|S_IXUSR); }
