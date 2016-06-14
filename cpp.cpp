#include "hpp.hpp"
#define YYERR "\n\n"<<yylineno<<":"<<msg<<"["<<yytext<<"]\n\n"
void yyerror(string msg) { cout<<YYERR; cerr<<YYERR; exit(-1); }
int main() { glob_init(); return yyparse(); }

Sym::Sym(string T,string V) { tag=T; val=V; }
Sym::Sym(string V):Sym("sym",V) {}
void Sym::push(Sym*o) { nest.push_back(o); }
Sym* Sym::pop() { auto R = nest.end(); nest.pop_back(); return *R; }

string Sym::pad(int n) { string S; for (int i=0;i<n;i++) S+='\t'; return S; }
string Sym::i2s(int n) { ostringstream os; os<<n; return os.str(); }

string Sym::head() { return "<"+tag+":"+val+">"; }
string Sym::dump(int depth) { string S = "\n"+pad(depth)+head();
for (auto it=nest.begin(),e=nest.end();it!=e;it++)
	S += (*it)->dump(depth+1);
return S; }

Sym* Sym::eval() {
	Sym*G = glob[val]; if (G) return (G);
	for (auto it=nest.begin(),e=nest.end();it!=e;it++)
		(*it) = (*it)->eval();
	return this; }

Sym* Sym::eq(Sym*o) { glob[val]=new Var(val,o); return glob[val]; }

Sym* Sym::add(Sym*o) { return new Error(head()+" + "+o->head()); }
Sym* Sym::div(Sym*o) { return new Error(head()+" / "+o->head()); }
Sym* Sym::pow(Sym*o) { return new Error(head()+" ^ "+o->head()); }

Sym* Sym::str() { return new Str(val); }

Error::Error(string V):Sym("error",V) { yyerror(V); }

Var::Var(string V,Sym*o):Sym("var",V){ push(o); }
Sym* Var::str() { return nest[0]->str(); }

Str::Str(string V):Sym("str",V){}
string Str::head() { return "'"+val+"'"; }
Sym* Str::add(Sym*o) { return new Str(val+o->str()->val); }

Vector::Vector():Sym("vector","[]"){}
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
Sym* Op::eval() {
	if (val=="~") return nest[0]; else Sym::eval();
	if (val=="=") return nest[0]->eq(nest[1]);
	if (val=="+") return nest[0]->add(nest[1]);
	if (val=="/") return nest[0]->div(nest[1]);
	if (val=="^") return nest[0]->pow(nest[1]);
	return this; }

Fn::Fn(string V, FN F):Sym("fn",V) { fn=F; }
Sym* Fn::at(Sym*o) { return fn(o); }

Term::Term(Sym*A,Sym*B):Sym("term",A->str()->val) {
	for (auto it=B->nest.begin(),e=B->nest.end();it!=e;it++,arity++)
		push(*it); }
string Term::head() { return val+"/"+i2s(arity); }

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
