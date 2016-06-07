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

Sym* Sym::eq(Sym*o) { glob[val]=o; return o; }
Sym* Sym::at(Sym*o) { push(o); return this; }

Sym* Sym::add(Sym*o) { return new Error(head()+" + "+o->head()); }
Sym* Sym::sub(Sym*o) { return new Error(head()+" - "+o->head()); }
Sym* Sym::mul(Sym*o) { return new Error(head()+" * "+o->head()); }
Sym* Sym::div(Sym*o) { return new Error(head()+" / "+o->head()); }
Sym* Sym::pow(Sym*o) { return new Error(head()+" ^ "+o->head()); }

Sym* Sym::str() { return new Str(val); }

Error::Error(string V):Sym("error",V) { yyerror(V); }

Int::Int(string V):Sym("int",V) { val=atoi(V.c_str()); }
Int::Int(int I):Sym("int","") { val=I; }
string Int::head() { ostringstream os;
	os << "<" << tag << ":" << val << ">"; return os.str(); }

Str::Str(string V):Sym("str",V){}
Sym* Str::eval() { return this; }
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
Sym* Vector::add(Sym*o) { push(o); return this; }
Sym* Vector::div(Sym*o) { Vector *V = new Vector();
	for (auto it=nest.begin(),e=nest.end();it!=e;it++) {
		V->push(*it); V->push(o); }
	V->nest.pop_back();
	return V; }
Sym* Vector::str() { string S;
	for (auto it=nest.begin(),e=nest.end();it!=e;it++)
		S += (*it)->str()->val;
	return new Str(S); }

Sym* Vector::_plain(Sym*o) { return o->plain(); }
int Sym::sz() { return nest.size(); }
Sym* Sym::sz(Sym*o) { return new Int(o->sz()); }
Sym* Sym::plain() { Sym*L = new Vector(); Sym*T=NULL;
	for (auto it=nest.begin(),e=nest.end();it!=e;it++)
		if ((*it)->sz()==0)
			L -> push(*it);
		else {
			T = (*it)->plain();
			for (auto in=T->nest.begin(),ee=T->nest.end();in!=ee;in++)
				L->push(*in);
		}
	return L; }

Op::Op(string V):Sym("op",V){}
Sym* Op::eval() {
	if (val=="~") return nest[0]; else Sym::eval();
	if (val=="=") return nest[0]->eq(nest[1]);
	if (val=="@") return nest[0]->at(nest[1]);
	if (val=="+") return nest[0]->add(nest[1]);
	if (val=="/") return nest[0]->div(nest[1]);
	return this; }

Fn::Fn(string V, FN F):Sym("fn",V) { fn=F; }
Sym* Fn::at(Sym*o) { return fn(o); }

Sym* Dir::dir(Sym*o) { return new Dir(o->val); }
Sym* Dir::div(Sym*o) { return new File(val+"/"+o->val); }

File::File(string V):Sym("file",V) {
	fh=fopen(val.c_str(),"w"); assert(fh!=NULL); }
File::~File() { if (fh) fclose(fh); }
Sym* File::eq(Sym*o) { fprintf(fh,"%s",o->str()->val.c_str());
	return o; }
Sym* File::file(Sym*o) { return new File(o->val.c_str()); }

Var::Var(string V):Sym("var",V){}

map<string,Sym*> glob;
void glob_init() {
	//---------------------------------------- metainfo
	glob["MODULE"]	= new Str(MODULE);
	glob["ABOUT"]	= new Str(ABOUT);
	glob["AUTHOR"]	= new Str(AUTHOR);
	glob["LICENSE"]	= new Str(LICENSE);
	glob["GITHUB"]	= new Str(GITHUB);
	glob["LOGO"]	= new Str(LOGO);
	//---------------------------------------- constants
	glob["sp"]		= new Str(" ");
	glob["nl"]		= new Str("\n");
	glob["tab"]		= new Str("\t");
	//---------------------------------------- vector manipulations
	glob["plain"]	= new Fn("plain",Vector::_plain);
	glob["sz"]		= new Fn("sz",Sym::sz);
	//---------------------------------------- file i/o
	glob["dir"]		= new Fn("dir",Dir::dir);
	glob["file"]	= new Fn("file",File::file);
}
