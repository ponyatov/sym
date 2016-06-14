void Sym::doc(Sym*o) { pars["doc"]=o; }

	for (auto pr=pars.begin(),e=pars.end();pr!=e;pr++)
		S += ","+pr->first+"="+pr->second->head();

Sym* Sym::at(Sym*o) { push(o); return this; }

Sym* Sym::sub(Sym*o) { return new Error(head()+" - "+o->head()); }
Sym* Sym::mul(Sym*o) { return new Error(head()+" * "+o->head()); }

Int::Int(string V):Sym("int",V) { val=atoi(V.c_str()); }
Int::Int(int I):Sym("int","") { val=I; }
string Int::head() { ostringstream os;
	os << "<" << tag << ":" << val << ">"; return os.str(); }

Sym* Str::eval() { return this; }
string Str::head() { string S = "'";
	for (int i=0;i<val.length();i++)
		switch (val[i]) {
			case '\n': S+="\\n"; break;
			case '\t': S+="\\t"; break;
			default: S+=val[i];
		}
	return S+"'"; }

Sym* Vector::add(Sym*o) { push(o); return this; }

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

	if (val=="@") return nest[0]->at(nest[1]);

Sym* Dir::dir(Sym*o) { return new Dir(o->val); }
Sym* Dir::div(Sym*o) { return new File(val+"/"+o->val); }

File::File(string V):Sym("file",V) {
	fh=fopen(val.c_str(),"w"); assert(fh!=NULL); }
File::~File() { if (fh) fclose(fh); }
Sym* File::eq(Sym*o) { fprintf(fh,"%s",o->str()->val.c_str());
	return o; }
Sym* File::file(Sym*o) { return new File(o->val.c_str()); }


	//---------------------------------------- vector manipulations
	glob["plain"]	= new Fn("plain",Vector::_plain);
	glob["sz"]		= new Fn("sz",Sym::sz);
	//---------------------------------------- file i/o
	glob["dir"]		= new Fn("dir",Dir::dir);
	glob["file"]	= new Fn("file",File::file);
}
