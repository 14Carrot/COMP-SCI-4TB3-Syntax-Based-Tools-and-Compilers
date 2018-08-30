//
//  main.cpp
//  AttributeGrammar
//
//  Created by Eden Burton on 2018-01-24.
//  Copyright © 2018 Eden Burton. All rights reserved.
//
//	exp(v0) → term(v1)				v0 := v1
//	| exp(v1) "+" term_1(v2) 		v0 := v1 + v2
//	| exp(v1) "–" term_1(v2) 		v0 := v1 – v2
//	| exp(v1) "&" term_2(v2)		v0 := v1 & v2
//	| exp(v1) "|" term_2(v2)		v0 := v1 | v2 
//
//	exp_1(v0) → term_1(v1)			v0 := v1
//	| exp_1(v1) "+" term_1(v2) 		v0 := v1 + v2
//	| exp_1(v1) "–" term_1(v2) 		v0 := v1 – v2
//
//	exp_2(v0) → term_2(v1)			v0 := v1
//	| exp_2(v1) "&" term_2(v2)		v0 := v1 & v2
//	| exp_2(v1) "|" term_2(v2)		v0 := v1 | v2 
//
//	term(v1) → factor(v1)			v0 := v1
//	| term(v1) "*" factor_1(v2) 	v0 := v1 * v2
//	| term(v1) "/" factor_1(v2) 	v0 := v1 / v2
//	| term(v0) "=" factor_2(v2)		v0 := v1 = v2
//	| term(v0) "#" factor_2(v2)		v0 := v1 # v2
//
//	term_1(v0) → factor_1(v1) 		v0 := v1
//	| term_1(v1) "*" factor_1(v2) 	v0 := v1 * v2
//	| term_1(v1) "/" factor_1(v2) 	v0 := v1 / v2
//
//	term_2(v0) → factor_2(v2)		v0 := v1
//	| term_2(v0) "=" factor_2(v2)	v0 := v1 = v2
//	| term_2(v0) "#" factor_2(v2)	v0 := v1 # v2
//
//	factor(v0) → number(v1) 		v0 := v1
//	| boolean(v1)		 			v0 := v1
//	| "~" factor_2(v0)		 		v0 := v1
//	| "(" exp(v1) ")" 			v0 := v1
//
//	factor_1(v0) → number(v1) 		v0 := v1
//	| "(" exp_1(v1) ")" 			v0 := v1
//
//	factor_2(v0) → boolean(v1) 		v0 := v1
//	| "~" factor_2(v0)		 		v0 := v1
//	| "(" exp_2(v1) ")" 			v0 := v1
//
// Precedence (high to low) = neg, eq/ineq, and/or

#include <iostream>
#include <fstream>

// enumeration describes types of symbols in the grammar
enum class Symbols {
    plus, minus, times, div, lParen,
    rParen, integer, boolean, conj, 
	disj, neg, eq, ineq, eof, other
};

// structure which holds synthesized attributes
// that are augumented to symbols in the attribute grammar
struct AttrSet {
    int num;
	int typ;
};


// global storing character to be looked at
char ch;

// current symbol
Symbols sym;


// current value associated with symbol
AttrSet symVal;

// forward declarations for non-terminal
// production rules
void factor(AttrSet& as, std::ifstream& is);
void factor_1(AttrSet& as, std::ifstream& is);
void factor_2(AttrSet& as, std::ifstream& is);
void term(AttrSet& as, std::ifstream& is);
void term_1(AttrSet& as, std::ifstream& is);
void term_2(AttrSet& as, std::ifstream& is);

// get next character from the stream
void getChar(std::ifstream& is) {
    
    if (is.eof()) ch = '\0';
    else {
        is >> ch;
        if (is.fail())
            ch = '\0';
        // debugging purposes only
        else
            std::cout << "Character read: " << ch << std::endl;
    }
}

// get symbol type of the current symbol ch....
// then retrieve next character from the input stream
void getSymbol(std::ifstream& is) {
    switch (ch) {
        case  0:
            sym = Symbols::eof; break;
        case '+':
            sym = Symbols::plus; getChar(is); break;
        case '-':
            sym = Symbols::minus; getChar(is); break;
        case '*':
            sym = Symbols::times; getChar(is); break;
        case '/':
            sym = Symbols::div; getChar(is); break;
        case '(':
            sym = Symbols::lParen; getChar(is); break;
        case ')':
            sym = Symbols::rParen; getChar(is); break;
		case '&':
			sym = Symbols::conj; getChar(is); break;
		case '|':
			sym = Symbols::disj; getChar(is); break;
		case '=':
			sym = Symbols::eq; getChar(is); break;
		case '~':
			sym = Symbols::neg; getChar(is); break;
		case '#':
			sym = Symbols::ineq; getChar(is); break;
		case 't': case 'f':
			sym = Symbols::boolean; 
			if (ch == 't') {
				symVal.num = 1;
				symVal.typ = 2;
				getChar(is);
			}
			else {
				symVal.num = 0;
				symVal.typ = 2;
				getChar(is);
			}
			break;
        case '0': case '1': case '2': case '3':
        case '4': case '5': case '6': case '7':
        case '8': case '9':
            
            sym = Symbols::integer;
            symVal.num = atoi(&ch);
            getChar(is);
            
            // grab the subsequent digits,
            // iteratively calculate the attribute value of
            // symbol
            while (isdigit(ch)) {
                symVal.num *= 10; symVal.num += atoi(&ch);
                getChar(is);
            }
            symVal.typ = 1;
            break;
        default:
            sym = Symbols::other;
    }
}

/*
 # Parser
 # expression :: =  term{ ("+" | "-") term) }
 # term :: = factor{ ("*" | "/") factor }
 # factor :: = integer | "(" expression ")"
 */
//	exp(v0) → term(v1)				v0 := v1
//	| exp(v1) "+" term_1(v2) 		v0 := v1 + v2
//	| exp(v1) "–" term_1(v2) 		v0 := v1 – v2
//	| exp(v1) "&" term_2(v2)		v0 := v1 & v2
//	| exp(v1) "|" term_2(v2)		v0 := v1 | v2 

// expression(v0) :: = term(v1) <<v0 = v1>>
// expression(v0) :: = expression(v1) "+" term (v2) << v0 = v1+v2 >>
// expression(v0) :: = expression(v1) "-" term (v2) << v0 = v1-v2 >>


// equivalent form used in code
// expression :: = term { ("+" | "-") term) }

void exp(AttrSet& as, std::ifstream& is) {
    
    // initialize term attribute values
    AttrSet as1{ 0,0 };
    AttrSet as2{ 0,0 };
    
    term(as1, is);
    
    if ((sym == Symbols::plus) || (sym == Symbols::minus)) {
		while ((sym == Symbols::plus) || (sym == Symbols::minus)) {
			//store symbol before going to next one
			Symbols op = sym;
			getSymbol(is);
        
			term_1(as2, is);
        
			//calculate synthesized attributes
			if (as2.typ == 1 && as2.typ == 1) {
			//calculate synthesized attributes
				if (op == Symbols::plus) as1.num += as2.num;
				else as1.num -= as2.num;
			}
			else {
				printf("wrond type\n");
				exit(0);
			}
		}
	}
	else {
		while ((sym == Symbols::disj) || (sym == Symbols::conj)) {
			//store symbol before going to next one
			Symbols op = sym;
			getSymbol(is);
        
			term_2(as2, is);
			if (as2.typ == 2 && as2.typ == 2) {
			//calculate synthesized attributes
				if (op == Symbols::conj) as1.num = as1.num && as2.num;
				else as1.num = as1.num || as2.num;
			}
			else {
				printf("wrond type\n");
				exit(0);
			}
			
		}
	}
	
    //  assign result to parent
	as.typ = as1.typ;
	as.num = as1.num;
}

//	exp_1(v0) → term_1(v1)			v0 := v1
//	| exp_1(v1) "+" term_1(v2) 		v0 := v1 + v2
//	| exp_1(v1) "–" term_1(v2) 		v0 := v1 – v2

void exp_1(AttrSet& as, std::ifstream& is) {
    
    // initialize term attribute values
    AttrSet as1{ 0,0 };
    AttrSet as2{ 0,0 };
    
    term_1(as1, is);
    
    while ((sym == Symbols::plus) || (sym == Symbols::minus)) {
        //store symbol before going to next one
        Symbols op = sym;
        getSymbol(is);
        
        term_1(as2, is);
        
        //calculate synthesized attributes

		if (as2.typ == 1 && as2.typ == 1) {
			//calculate synthesized attributes
			if (op == Symbols::plus) as1.num += as2.num;
			else as1.num -= as2.num;
		}
		else {
			printf("wrond type\n");
			exit(0);
		}
    }
	
    //  assign result to parent

    as.num = as1.num;
}

//	exp_2(v0) → term_2(v1)			v0 := v1
//	| exp_2(v1) "&" term_2(v2)		v0 := v1 & v2
//	| exp_2(v1) "|" term_2(v2)		v0 := v1 | v2 

void exp_2(AttrSet& as, std::ifstream& is) {
    
    // initialize term attribute values
    AttrSet as1{ 0,0 };
    AttrSet as2{ 0,0 };
    
    term_2(as1, is);
    
	while ((sym == Symbols::disj) || (sym == Symbols::conj)) {
		//store symbol before going to next one
		Symbols op = sym;
		getSymbol(is);
        
		term_2(as2, is);
        
		//calculate synthesized attributes
		if (as2.typ == 2 && as2.typ == 2) {
			//calculate synthesized attributes
			if (op == Symbols::conj) as1.num = as1.num && as2.num;
			else as1.num = as1.num || as2.num;
		}
		else {
			printf("wrond type\n");
			exit(0);
		}
	}
    
    //  assign result to parent

    as.num = as1.num;
}

// term (v0) :: = factor(v1) <<v0 = v1>>
// term (v0) :: = factor(v1) "*" factor (v2) << v0 = v1*v2 >>
// term (v0) :: = factor(v1) "/" factor (v2) << v0 = v1/v2 >>

//	term(v1) → factor(v1)			v0 := v1
//	| term(v1) "*" factor_1(v2) 	v0 := v1 * v2
//	| term(v1) "/" factor_1(v2) 	v0 := v1 / v2
//	| term(v0) "=" factor_2(v2)		v0 := v1 = v2
//	| term(v0) "#" factor_2(v2)		v0 := v1 # v2


// equivalent form used in code
// term :: = factor{ ("*" | "/") factor }
void term(AttrSet& as, std::ifstream& is) {
    
    AttrSet as1{ 0,0 };
    AttrSet as2{ 0,0 };
    factor(as1, is);
	
	if ((sym == Symbols::div) || (sym == Symbols::times)) {
		while ((sym == Symbols::div) || (sym == Symbols::times)) {
			//store symbol before going to next one
			Symbols op;
			op = sym;
			getSymbol(is);
        
			factor_1(as2, is);
			//calculate synthesized attributes
			if (as2.typ == 1 && as2.typ == 1) {
				if (op == Symbols::div) {
				
					as1.num /= as2.num;
				}
				else {
					as1.num *= as2.num;
				}
			}
			else {
				printf("wrond type\n");
				exit(0);
			}
		}
	}    
    
	else {
		while ((sym == Symbols::eq) || (sym == Symbols::ineq)) {
			//store symbol before going to next one
			Symbols op;
			op = sym;
			getSymbol(is);
			
			factor_2(as2, is);
			//calculate synthesized attributes
			if (as2.typ == 2 && as2.typ == 2) {
				if (op == Symbols::eq)  as1.num = (as1.num == as2.num);
				else as1.num = (as1.num != as2.num);
			}
			else {
				printf("wrond type\n");
				exit(0);
			}
		}
	}
    
    //  assign result to parent
    as.num = as1.num;
	as.num = as1.num;
}

//	term_1(v0) → factor_1(v1) 		v0 := v1
//	| term_1(v1) "*" factor_1(v2) 	v0 := v1 * v2
//	| term_1(v1) "/" factor_1(v2) 	v0 := v1 / v2

void term_1(AttrSet& as, std::ifstream& is) {
    
    AttrSet as1{ 0 };
    AttrSet as2{ 0 };
    factor_1(as1, is);
	
	
	while ((sym == Symbols::div) || (sym == Symbols::times)) {
		//store symbol before going to next one
		Symbols op;
		op = sym;
		getSymbol(is);
       
		factor_1(as2, is);
		//calculate synthesized attributes
		if (as2.typ == 1 && as2.typ == 1) {
			if (op == Symbols::div) {
				
				as1.num /= as2.num;
			}
			else {
				as1.num *= as2.num;
			}
		}
		else {
			printf("wrond type\n");
			exit(0);
		}
	} 
    //  assign result to parent
    as.num = as1.num;
}

//	term_2(v0) → factor_2(v2)		v0 := v1
//	| term_2(v0) "=" factor_2(v2)	v0 := v1 = v2
//	| term_2(v0) "#" factor_2(v2)	v0 := v1 # v2

void term_2(AttrSet& as, std::ifstream& is) {
    
    AttrSet as1{ 0,0 };
    AttrSet as2{ 0,0 };
    factor_2(as1, is);
	
	while ((sym == Symbols::eq) || (sym == Symbols::ineq)) {
		//store symbol before going to next one
		Symbols op;
		op = sym;
		getSymbol(is);
			
		factor_2(as2, is);
		//calculate synthesized attributes
		if (as2.typ == 2 && as2.typ == 2) {
			if (op == Symbols::eq)  as1.num = (as1.num == as2.num);
			else as1.num = (as1.num != as2.num);
		}
		else {
			printf("wrond type\n");
			exit(0);
		}
	}
    //  assign result to parent
    as.num = as1.num;
}

// factor(v0) :: = integer(v1)  << v0 = v1 >>
// factor(v0) :: = "(" expression(v1) ")" << v0 = v1 >>

//	factor(v0) → number(v1) 		v0 := v1
//	| boolean(v1)		 			v0 := v1
//	| "~" exp_2(v0)		 		v0 := v1
//	| "(" exp(v1) ")" 			v0 := v1


void factor(AttrSet& as, std::ifstream& is) {
	if (sym == Symbols::integer) {
		as.typ = symVal.typ;
        as.num = symVal.num;
    }
    if (sym == Symbols::lParen) {
        getSymbol(is);
        exp(as, is);
        getSymbol(is);
        if (sym != Symbols::rParen)
            std::cout << "problem";
    }
	if (sym == Symbols::boolean) {
		as.typ = symVal.typ;
		as.num = symVal.num;
		
	}
	if (sym == Symbols::neg) {
		getSymbol(is);
		exp_2(as, is);
		//calculate synthesized attributes
		if (as.typ == 2) {
			as.num = !as.num;
		}
		else {
			printf("wrond type\n");
			exit(0);
		}
		
	}
    
    getSymbol(is);
    
}

//
//	factor_1(v0) → number(v1) 		v0 := v1
//	| "(" exp_1(v1) ")" 			v0 := v1

void factor_1(AttrSet& as, std::ifstream& is) {
    
    if (sym == Symbols::integer) {
		as.typ = symVal.typ;
        as.num = symVal.num;
    }
    else if (sym == Symbols::lParen) {
        getSymbol(is);
        exp_1(as, is);
        getSymbol(is);
        if (sym != Symbols::rParen)
            std::cout << "problem";
    }
    
    getSymbol(is);
    
}
//	factor_2(v0) → boolean(v1) 		v0 := v1
//	| "~" factor_2(v0)		 		v0 := v1
//	| "(" exp_2(v1) ")" 			v0 := v1

void factor_2(AttrSet& as, std::ifstream& is) {
    
    if (sym == Symbols::lParen) {
        getSymbol(is);
        exp_2(as, is);
        getSymbol(is);
        if (sym != Symbols::rParen)
            std::cout << "problem";
    }
	if (sym == Symbols::boolean) {
		as.typ = symVal.typ;
		as.num = symVal.num;

	}
	//calculate synthesized attributes
	if (as.typ == 2) {
		as.num = !as.num;
	}
	else {
		printf("wrond type\n");
		exit(0);
	}
    
    getSymbol(is);
    
}

//entry point
int main() {
    //initialize synthesized attribute
    AttrSet as{ 0,0 };
    
    // open file stream
    std::ifstream f("input.txt");
    
    if (f.is_open()) {
        // get first character, get first symbol
        getChar(f);
        getSymbol(f);
        //retrieve synthesized attribute from children
        exp(as, f);
    }
    else { std::cout << "input not opened!"; }
    
    
    if (sym == Symbols::eof)
		std::cout << "success parse: val:" << as.num << "\n";
    else
		
        std::cout << "fail" << "\n";
}
