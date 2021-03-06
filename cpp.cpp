#include "hpp.hpp"
#define YYERR "\n\n"<<yylineno<<":"<<msg<<"["<<yytext<<"]\n\n"
void yyerror(string msg) { cout<<YYERR; cerr<<YYERR; exit(-1); }
int main() { glob_init(); return yyparse(); }

Sym::Sym(string T,string V) { tag=T; val=V; }
Sym::Sym(string V):Sym("sym",V) {}
void Sym::push(Sym*o) { nest.push_back(o); }
Sym* Sym::pop() { auto R = nest.end(); nest.pop_back(); return *R; }

string Sym::pad(int n) { string S; for (int i=0;i<n;i++) S+='\t'; return S; }
string Sym::i2s(long n) { ostringstream os; os<<n; return os.str(); }
string Sym::p2s(Sym*o) { ostringstream os; os<<o; return os.str(); }

string Sym::head() { return "<"+tag+":"+val+"> @"+p2s(this)+" "+doc; }
string Sym::dump(int depth) { string S = "\n"+pad(depth)+head();
for (auto it=nest.begin(),e=nest.end();it!=e;it++)
	S += (*it)->dump(depth+1);
return S; }

Sym* Sym::eval() {
	Sym*G = glob[val]; if (G) return (G); else share();
	for (auto it=nest.begin(),e=nest.end();it!=e;it++)
		(*it) = (*it)->eval();
	return this; }
void Sym::share() { glob[val]=this; }

Sym* Sym::eq(Sym*o) { nest.assign(1,o); return this; }//glob[val]=new Var(val,o); return glob[val]; }
Sym* Sym::at(Sym*o) { push(o); return this; }

Sym* Sym::add(Sym*o) { return new Error(head()+" + "+o->head()); }
Sym* Sym::div(Sym*o) { return new Error(head()+" / "+o->head()); }
Sym* Sym::pow(Sym*o) { return new Error(head()+" ^ "+o->head()); }

Sym* Sym::str() { return new Str(val); }

Error::Error(string V):Sym("error",V) { yyerror(V); }

Var::Var(string V,Sym*o):Sym("var",V){ push(o); }
Sym* Var::str() { return nest[0]->str(); }

Str::Str(string V):Sym("str",V){}
void Str::share(){}
string Str::head() { return "'"+val+"' @"+p2s(this); }
Sym* Str::add(Sym*o) { return new Str(val+o->str()->val); }
Sym* Str::eq(Sym*o) { val=o->str()->val; return this; }

Vector::Vector():Sym("vector","[]"){}
void Vector::share(){}
Sym* Vector::div(Sym*o) { Sym*V =new Vector();
	for (auto it=nest.begin(),e=nest.end();it!=e;it++) {
		V->push(*it); V->push(o); }
	V->pop(); return V; }
Sym* Vector::str() { string S;
	for (auto it=nest.begin(),e=nest.end();it!=e;it++)
		S += (*it)->str()->val;
	return new Str(S); }
Sym* Vector::pow(Sym*o) { return div(o)->str(); }

Op::Op(string V):Sym("op",V){}
void Op::share(){}
Sym* Op::eval() {
	if (val=="~") return nest[0]; else Sym::eval();
	if (val=="$") return nest[0]->eval()->nest[0]->eval();
	if (val=="doc") { nest[0]->doc=nest[1]->val; return nest[0]; }
	if (val=="=") return nest[0]->eq(nest[1]);
	if (val=="@") return nest[0]->at(nest[1]);
	if (val=="+") return nest[0]->add(nest[1]);
	if (val=="/") return nest[0]->div(nest[1]);
	if (val=="^") return nest[0]->pow(nest[1]);
	return this; }

Fn::Fn(string V, FN F):Sym("fn",V) { fn=F; }
Sym* Fn::at(Sym*o) { return fn(o); }

map<string,Sym*> glob;
void glob_init() {
	//---------------------------------------- metainfo
	glob["MODULE"]	= new Str(MODULE);
	glob["BRANCH"]	= new Str(BRANCH);
	glob["ABOUT"]	= new Str(ABOUT);
	glob["AUTHOR"]	= new Str(AUTHOR);
	glob["LICENSE"]	= new Str(LICENSE);
	glob["GITHUB"]	= new Str(GITHUB);
	//---------------------------------------- constants
	glob["sp"]		= new Str(" ");
	glob["nl"]		= new Str("\n");
	glob["tab"]		= new Str("\t");
}
