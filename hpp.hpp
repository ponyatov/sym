#ifndef _H_SYM
#define _H_SYM

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cassert>
#include <vector>
#include <map>
using namespace std;
#include "meta.hpp"

#ifndef __MINGW32__
	#include "linux.hpp"	// and all POSIX systems
#else
	#include "win32.hpp"	// win32 specific code
#endif

struct Sym {
	string tag,val;
	Sym(string,string); Sym(string);
	vector<Sym*> nest; void push(Sym*);
	map<string,Sym*> pars; void doc(Sym*);
	virtual string dump(int=0); virtual string head(); string pad(int);
	virtual Sym* eval();
	virtual Sym* str();
	virtual Sym* eq(Sym*);
	virtual Sym* at(Sym*);
	virtual Sym* add(Sym*);
	virtual Sym* sub(Sym*);
	virtual Sym* mul(Sym*);
	virtual Sym* div(Sym*);
	virtual Sym* pow(Sym*);
	virtual Sym* plain();
	virtual int sz();
	static Sym* sz(Sym*);
};
extern map<string,Sym*> glob;
extern void glob_init();

struct Error: Sym { Error(string); };

struct Int: Sym { Int(string); Int(int); int val; string head(); };

struct Str: Sym { Str(string); string head(); Sym*add(Sym*); Sym*eval(); };

struct Vector: Sym { Vector(); Sym*str(); Sym*div(Sym*); Sym*add(Sym*);
	static Sym* _plain(Sym*); };

struct Op: Sym { Op(string); Sym*eval(); };
typedef Sym* (*FN)(Sym*);
struct Fn: Sym { Fn(string,FN); FN fn; Sym*at(Sym*); };

struct Dir: Sym { Dir(string); static Sym* dir(Sym*); Sym*div(Sym*o); };
struct File: Sym { File(string); ~File(); static Sym* file(Sym*); FILE*fh; Sym*eq(Sym*o); };

struct Var: Sym { Var(string); };

extern int yylex();
extern int yylineno;
extern char* yytext;
#define TOC(C,X) { yylval.o = new C(yytext); return X; }
extern int yyparse();
extern void yyerror(string);
#include "ypp.tab.hpp"

#endif // _H_SYM
