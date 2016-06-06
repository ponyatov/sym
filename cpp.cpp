#include "hpp.hpp"
#define YYERR "\n\n"<<yylineno<<":"<<msg<<"["<<yytext<<"]\n\n"
void yyerror(string msg) { cout<<YYERR; cerr<<YYERR; exit(-1); }
int main() { glob_init(); return yyparse(); }

Sym::Sym(string T,string V) { tag=T; val=V; }
Sym::Sym(string V):Sym("sym",V) {}
void Sym::push(Sym*o) { nest.push_back(o); }
void Sym::doc(Sym*o) { pars["doc"]=o; }

string Sym::head() { return "<"+tag+":"+val+">"; }
string Sym::pad(int n) { string S; for (int i=0;i<n;i++) S+='\t'; return S; }
string Sym::dump(int depth) { string S = "\n"+pad(depth)+head();
	for (auto pr=pars.begin(),e=pars.end();pr!=e;pr++)
		S += ","+pr->first+"="+pr->second->head();
	for (auto it=nest.begin(),e=nest.end();it!=e;it++)
		S += (*it)->dump(depth+1);
	return S; }

Sym* Sym::eval() {
	Sym*G = glob[val]; if (G) return (G);
	for (auto it=nest.begin(),e=nest.end();it!=e;it++)
		(*it) = (*it)->eval();
	return this; }

Sym* Sym::eq(Sym*o) { Sym*V = new Var(str()->val); V->push(o);
	glob[val]=V; return V; }
Sym* Sym::at(Sym*o) { push(o); }

Sym* Sym::add(Sym*o) { return new Error(head()+"+"+o->head()); }

Sym* Sym::str() { return new Str(val); }

Error::Error(string V):Sym("error",V) { yyerror(V); }

Str::Str(string V):Sym("str",V){}
Sym* Str::add(Sym*o) { return new Str(val+o->str()->val); }
string Str::head() { string S = "'";
	for (int i=0;i<val.length();i++)
		switch (val[i]) {
			case '\n': S+="\\n"; break;
			case '\t': S+="\\t"; break;
			default: S+=val[i];
		}
	return S+"'"; }

Vector::Vector():Sym("vector","[]"){}

Op::Op(string V):Sym("op",V){}
Sym* Op::eval() {
	if (val=="~") return nest[0]; else Sym::eval();
	if (val=="=") return nest[0]->eq(nest[1]);
	if (val=="@") return nest[0]->at(nest[1]);
	if (val=="+") return nest[0]->add(nest[1]);
	return this; }

Var::Var(string V):Sym("var",V){}

map<string,Sym*> glob;
void glob_init() {
	glob["MODULE"]	= new Str(MODULE);
	glob["ABOUT"]	= new Str(ABOUT);
	glob["AUTHOR"]	= new Str(AUTHOR);
	glob["LICENSE"]	= new Str(LICENSE);
	glob["GITHUB"]	= new Str(GITHUB);
	glob["LOGO"]	= new Str(LOGO);
}
