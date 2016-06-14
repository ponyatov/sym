#include <sstream>
#include <cassert>

#ifndef __MINGW32__
	#include "linux.hpp"	// and all POSIX systems
#else
	#include "win32.hpp"	// win32 specific code
#endif

	map<string,Sym*> pars; void doc(Sym*);
	virtual Sym* at(Sym*);
	virtual Sym* sub(Sym*);
	virtual Sym* mul(Sym*);
	virtual Sym* plain();
	virtual int sz();
	static Sym* sz(Sym*);

struct Int: Sym { Int(string); Int(int); int val; string head(); };

Sym*str(); Sym*div(Sym*); Sym*add(Sym*); static Sym* _plain(Sym*); };
