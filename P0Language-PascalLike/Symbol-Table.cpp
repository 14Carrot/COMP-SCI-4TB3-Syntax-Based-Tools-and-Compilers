#include <iostream>
#include <fstream>
#include <string>
#include <ctype.h>

enum class Class {
    HeadClass, VarClass, ParClass, ConstClass, 
	FldClass, TypClass, ProcClass, SProcClass
};

struct ObjDesc {
	Class cls;
	std::string name;
	int val;
	int lev;
	ObjDesc *dsc;
	ObjDesc *next;
};

void OpenScope();

ObjDesc *universe = new ObjDesc;
ObjDesc *topScope = universe;
ObjDesc *guard;

void init() {
	OpenScope();
	universe -> cls = Class::HeadClass;
	universe -> next = new ObjDesc;
	guard = universe -> next;
}
	

void Mark(std::string msg) {
	std::cout << msg << std::endl;;
}

void NewObj(ObjDesc *obj, Class cls) {
	ObjDesc *x;
	x = topScope;
	ObjDesc *n = new ObjDesc;
	std::string id = obj -> name;
		
	while (x -> next && ((x -> next -> name) != id)) {
		x = x -> next;
	}
	if (!x -> next) {
		n -> name = obj -> name;
		n -> cls = cls;
		n -> next = guard;
		n -> val = obj -> val;
		n -> lev = obj -> lev;
		x -> next = n;	
		obj = n;
	}
	else {
		obj = x -> next;
		Mark("mult def");
	}
}

void OpenScope(){
	ObjDesc *s = new ObjDesc;
	s -> cls = Class::HeadClass;
	s -> dsc = topScope;
	s -> next = guard;
	topScope = s;
}

void CloseScope() {
	topScope = topScope -> dsc;
}

void find (ObjDesc *obj) {
	ObjDesc *s;
	ObjDesc *x;
	s = topScope; 
	std::string id = obj -> name;
	while(true) {
		x = s -> next;
		if (x -> name != id) {
			while (x -> next && ((x -> next -> name) != id)) {
				x = x -> next;
			}
		}
		if((x && ((x -> name) == id)) || (x -> next && ((x -> next -> name) != id))) {
			obj = x -> next;
			break;
		}
		if(s == universe) {
			obj = x;
			Mark("undef");
			break;
		}
		s = s -> dsc;
	}
	
}

int main() {
	
	init();
	ObjDesc *obj = new ObjDesc;
	obj -> cls = Class::VarClass;
	obj -> name = "x";
	
	ObjDesc *obj_1 = new ObjDesc;
	obj_1 -> cls = Class::VarClass;
	obj_1 -> name = "y";

	NewObj(obj, obj -> cls);
	NewObj(obj, obj -> cls);
	find(obj);
	find(obj_1);
	
	CloseScope();
	init(); 
	ObjDesc *obj_2 = new ObjDesc;
	obj_2 -> cls = Class::VarClass;
	obj_2 -> name = "x";
	NewObj(obj_2, obj_2 -> cls);
	find(obj);
}
