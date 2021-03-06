#ifndef _H_HPP
#define _H_HPP

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <map>
using namespace std;
#ifdef __MINGW32__
#include <direct.h>
#else
#include <sys/stat.h>
#endif

#include "meta.hpp"

struct Sym {
	string tag,val,doc;
	Sym(string,string); Sym(string);
	vector<Sym*> nest; void push(Sym*); Sym* pop();
	virtual string dump(int=0); virtual string head();
	static string pad(int); static string i2s(long); static string p2s(Sym*);
	virtual Sym* str();
	virtual Sym* eval(); virtual void share();
	virtual Sym* eq(Sym*);
	virtual Sym* at(Sym*);
	virtual Sym* add(Sym*);
	virtual Sym* div(Sym*);
	virtual Sym* pow(Sym*);
};
extern map<string,Sym*> glob;
extern void glob_init();

struct Error: Sym { Error(string); };

struct Var: Sym { Var(string,Sym*); Sym*str(); };

struct Str: Sym { Str(string); string head(); Sym*add(Sym*); void share();
	Sym*eq(Sym*); };

struct Vector: Sym { Vector(); Sym*div(Sym*); Sym*pow(Sym*); Sym*str();
	void share(); };

struct Op: Sym { Op(string); Sym*eval(); void share(); };

typedef Sym*(*FN)(Sym*);
struct Fn: Sym { Fn(string,FN); FN fn; Sym*at(Sym*); };

// file i/o extension
struct Dir: Sym { Dir(string); };
struct File: Sym { File(string); };

extern int yylex();
extern int yylineno;
extern char* yytext;
#define TOC(C,X) { yylval.o = new C(yytext); return X; }
extern int yyparse();
extern void yyerror(string);
#include "ypp.tab.hpp"

#endif // _H_HPP
