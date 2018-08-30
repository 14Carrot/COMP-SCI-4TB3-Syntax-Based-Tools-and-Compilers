#include <iostream>
#include <fstream>
#include <string>
#include <ctype.h>

enum class Symbols {
	TimesSym, DivSym, ModSym, AndSym, OrSym, PlusSym, MinusSym,
	EqlSym, NeqSym, LssSym, GeqSym, LeqSym, GtrSym, PeriodSym,
	CommaSym, ColonSym, RparenSym, RbrakSym, OfSym, ThenSym, DoSym,
	LparenSym, LbrakSym, NotSym, BecomesSym, NumberSym, IdentSym,
	SemicolonSym, BeginSym, EndSym, ElseSym, IfSym, WhileSym, ArraySym, 
	RecordSym, ConstSym, TypeSym, VarSym, ProcedureSym, ProgramSym,
	EofSym
};

// global storing character to be looked at
char ch;

// String that defines the symbol
std::string Identifier;

// current symbol
Symbols sym;

//	current line number
int line = 0;

// current position within the line 
int pos = 0;

// current value associated with symbol
int val;

void getSymbol(std::ifstream& is);

// get next character from the stream
void getChar(std::ifstream& is) {
    if (is.eof()) {
		ch = '\0';
	}
    else {
        is >> ch;
		//std::cout << line << ", " << pos << std::endl;
		pos++;
		if (pos == 1 && line == 0) {
			//std::cout << "Character read: " << ch << std::endl;
			getSymbol(is);
		}
		else if (is.peek() == '\n') {
			//std::cout << "Character read: " << ch << std::endl;
			line++; 
			pos = 0;
			
		}	
        else if (is.fail()) {
			ch = '\0';
		}
		
        // debugging purposes only
        //else
            //std::cout << "Character read: " << ch << std::endl;
    }
}

void Mark () {
	std::cout << "Error occurred at line: " << line << " and position: " << pos << std::endl;
}

// get symbol type of the current symbol ch....
// then retrieve next character from the input stream
void getSymbol(std::ifstream& is) {
    switch (ch) {
        case 0:
			if (is.eof()) {
				sym = Symbols::EofSym; std::cout << "EofSym" << "\n"; break;
			}
        case '+':
            sym = Symbols::PlusSym; std::cout << "PlusSym" << "\n"; getChar(is); getSymbol(is); break;
        case '-':
            sym = Symbols::MinusSym; std::cout << "MinusSym" << "\n"; getChar(is); getSymbol(is); break;
        case '*':
            sym = Symbols::TimesSym; std::cout << "TimesSym" << "\n"; getChar(is); getSymbol(is); break;
        case 'd':
			Identifier += ch;
            
            while (true) {
				if (Identifier =="div") {
					sym = Symbols::DivSym;
					std::cout << "DivSym" << "\n";
					Identifier = "";
					getChar(is);
					getSymbol(is);
					break;
				}
				else if (Identifier =="do") {
					sym = Symbols::DoSym;
					std::cout << "DoSym" << "\n"; 
					Identifier = "";
					getChar(is);
					getSymbol(is);
					break;
				}
				else if(isspace(is.peek()) || ispunct(is.peek())) {
					sym = Symbols::IdentSym; 
					std::cout << "IdentSym:" << Identifier << "\n"; 
					Identifier = "";
					getChar(is);
					getSymbol(is);
					break;
				}
                getChar(is);
				Identifier += ch;
            }
			break;
			            
		case 'c':
			Identifier += ch;
            
            while (true) {
				if (Identifier =="const") {
					sym = Symbols::ConstSym; 
					std::cout << "ConstSym" << "\n";
					Identifier = "";
					getChar(is);
					getSymbol(is);
					break;
				}
				else if(isspace(is.peek()) || ispunct(is.peek())) {
					sym = Symbols::IdentSym; 
					std::cout << "IdentSym:" << Identifier << "\n"; 
					Identifier = "";
					getChar(is);
					getSymbol(is);
					break;
				}
                getChar(is);
				Identifier += ch;
            }
			break;
            
		case 'm':
			Identifier += ch;
            
            while (true) {
				if (Identifier =="mod") {
					sym = Symbols::ModSym; 
					std::cout << "ModSym" << "\n"; 
					Identifier = "";
					getChar(is);
					getSymbol(is);
					break;
				}
				else if(isspace(is.peek()) || ispunct(is.peek())) {
					sym = Symbols::IdentSym; 
					std::cout << "IdentSym:" << Identifier << "\n"; 
					Identifier = "";
					getChar(is);
					getSymbol(is);
					break;
				}
                getChar(is);
				Identifier += ch;
            }
			break;
            
		case 'a':
			Identifier += ch;
            while (true) {
				if (Identifier =="and") {
					sym = Symbols::AndSym; 
					std::cout << "AndSym" << "\n"; 
					Identifier = "";
					getChar(is);
					getSymbol(is);
					break;
				}
				else if(Identifier =="array") {
					sym = Symbols::ArraySym; 
					std::cout << "ArraySym" << "\n"; 
					Identifier = "";
					getChar(is);
					getSymbol(is);
					break;
				}
				else if(isspace(is.peek()) || ispunct(is.peek())) {
					sym = Symbols::IdentSym; 
					std::cout << "IdentSym:" << Identifier << "\n"; 
					Identifier = "";
					getChar(is);
					getSymbol(is);
					break;
				}
                getChar(is);
				Identifier += ch;
            }
			
			break;
            
		case 'o':
			Identifier += ch;
            
            while (true) {
				if (Identifier =="or") {
					sym = Symbols::OrSym; 
					std::cout << "OrSym" << "\n";  
					Identifier = ""; 
					getChar(is);
					getSymbol(is);
					break;
				}
				else if (Identifier =="of") {
					sym = Symbols::OfSym;
					std::cout << "OfSym" << "\n";   
					Identifier = ""; 
					getChar(is);
					getSymbol(is);
					break;
				}
				else if(isspace(is.peek()) || ispunct(is.peek())) {
					sym = Symbols::IdentSym; 
					std::cout << "IdentSym:" << Identifier << "\n"; 
					Identifier = "";
					getChar(is);
					getSymbol(is);
					break;
				}
                getChar(is);
				Identifier += ch;
            }		
			break;
			
		case '=':
            sym = Symbols::EqlSym; std::cout << "EqlSym" << "\n"; getChar(is); getSymbol(is); break;
		case '<':
			Identifier += ch;
			if (is.peek() == '=') {
				getChar(is);
				Identifier += ch;
				sym = Symbols::LeqSym; 
				std::cout << "LeqSym" << "\n"; 
			} 
			else if (is.peek() == '>') {
				getChar(is);
				Identifier += ch;
				sym = Symbols::NeqSym; 
				std::cout << "NeqSym" << "\n";
			} 
			else {
				sym = Symbols::LssSym; 
				std::cout << "LssSym" << "\n"; 
			}
			Identifier = "";
			getChar(is);
			getSymbol(is);
			break;
            
		case '>':
			Identifier += ch;
			if (is.peek() == '=') {
				getChar(is);
				Identifier += ch;
				sym = Symbols::GeqSym; 
				std::cout << "GeqSym" << "\n"; 
			} 
			else {
				sym = Symbols::GtrSym; 
				std::cout << "GtrSym" << "\n";  
			}
			Identifier = "";
			getChar(is);
			getSymbol(is);
			break;
		
		case '.':
            sym = Symbols::PeriodSym; std::cout << "PeriodSym" << "\n"; getChar(is); getSymbol(is); break;
		case ',':
            sym = Symbols::CommaSym; std::cout << "CommaSym" << "\n"; getChar(is); getSymbol(is); break;
		case ':':
			Identifier += ch;
			if (is.peek() == '=') {
				getChar(is);
				Identifier += ch;
				sym = Symbols::BecomesSym; 
				std::cout << "BecomesSym" << "\n";
			} 
			else {
				sym = Symbols::ColonSym;
				std::cout << "ColonSym" << "\n";
			}
			Identifier = "";
			getChar(is);
			getSymbol(is);
			break;
            
		case 't':
			Identifier += ch;
            while (true) {
				if (Identifier =="then") {
					sym = Symbols::ThenSym; 
					std::cout << "ThenSym" << "\n"; 
					Identifier = ""; 
					getChar(is);
					getSymbol(is);
					break;
				}
				else if (Identifier =="type") {
					sym = Symbols::TypeSym;
					std::cout << "TypeSym" << "\n";  
					Identifier = ""; 
					getChar(is);
					getSymbol(is);
					break;
				}
				else if(isspace(is.peek()) || ispunct(is.peek())) {
					sym = Symbols::IdentSym; 
					std::cout << "IdentSym:" << Identifier << "\n"; 
					Identifier = "";
					getChar(is);
					getSymbol(is);
					break;
				}
                getChar(is);
				Identifier += ch;
            }		
			break;
			
		case 'n':
			Identifier += ch;
            while (true) {
				if (Identifier == "not") {
					sym = Symbols::NotSym; 
					std::cout << "NotSym" << "\n"; 
					Identifier = ""; 
					getChar(is);
					getSymbol(is);
					break;
				}
				else if(isspace(is.peek()) || ispunct(is.peek())) {
					sym = Symbols::IdentSym; 
					std::cout << "IdentSym:" << Identifier << "\n"; 
					Identifier = "";
					getChar(is);
					getSymbol(is);
					break;
				}
                getChar(is);
				Identifier += ch;
            }		
			break;
			
		case 'r':
			Identifier += ch;
            while (true) {
				if (Identifier == "record") {
					sym = Symbols::RecordSym; 
					std::cout << "RecordSym" << "\n"; 
					Identifier = ""; 
					getChar(is);
					getSymbol(is);
					break;
				}
				else if(isspace(is.peek()) || ispunct(is.peek())) {
					sym = Symbols::IdentSym; 
					std::cout << "IdentSym:" << Identifier << "\n"; 
					Identifier = "";
					getChar(is);
					getSymbol(is);
					break;
				}
                getChar(is);
				Identifier += ch;
            }		
			break;
	
		case 'e':
			Identifier += ch;
            
            while (true) {
				if (Identifier =="else") {
					sym = Symbols::ElseSym; 
					std::cout << "ElseSym" << "\n";
					Identifier = ""; 
					getChar(is);
					getSymbol(is);
					break;
				}
				else if (Identifier =="end") {
					sym = Symbols::EndSym; 
					std::cout << "EndSym" << "\n"; 
					Identifier = ""; 
					getChar(is);
					getSymbol(is);
					break;
				}
				else if(isspace(is.peek()) || ispunct(is.peek())) {
					sym = Symbols::IdentSym; 
					std::cout << "IdentSym:" << Identifier << "\n"; 
					Identifier = "";
					getChar(is);
					getSymbol(is);
					break;
				}
                getChar(is);
				Identifier += ch;
            }
			break;
	
		case 'b':
			Identifier += ch;
            while (true) {
				if (Identifier =="begin") {
					sym = Symbols::BeginSym; 
					std::cout << "BeginSym" << "\n";  
					Identifier = ""; 
					getChar(is);
					getSymbol(is);
					break;
				}
				else if(isspace(is.peek()) || ispunct(is.peek())) {
					sym = Symbols::IdentSym; 
					std::cout << "IdentSym:" << Identifier << "\n"; 
					Identifier = "";
					getChar(is);
					getSymbol(is);
					break;
				}
                getChar(is);
				Identifier += ch;
            }
			break;
            
		case ';':
            sym = Symbols::SemicolonSym; std::cout << "SemicolonSym" << "\n"; getChar(is); getSymbol(is); break;

		case 'i':
			Identifier += ch;
            while (true) {
				if (Identifier =="if") {
					sym = Symbols::IfSym; 
					std::cout << "IfSym" << "\n";  
					Identifier = "";
					getChar(is);
					getSymbol(is);
					break;
				}
				else if(isspace(is.peek()) || ispunct(is.peek())) {
					sym = Symbols::IdentSym; 
					std::cout << "IdentSym:" << Identifier << "\n"; 
					Identifier = "";
					getChar(is);
					getSymbol(is);
					break;
				}
                getChar(is);
				Identifier += ch;
            }
			
			break;
            
		case 'w':
			Identifier += ch;
            while (true) {
				if (Identifier =="begin") {
					sym = Symbols::WhileSym; 
					std::cout << "WhileSym" << "\n"; 
					Identifier = "";
					getChar(is);
					getSymbol(is);
					break;
				}
				else if(isspace(is.peek()) || ispunct(is.peek())) {
					sym = Symbols::IdentSym; 
					std::cout << "IdentSym:" << Identifier << "\n"; 
					Identifier = "";
					getChar(is);
					getSymbol(is);
					break;
				}
                getChar(is);
				Identifier += ch;
            }
			break;
            
		case 'p':
			Identifier += ch;
            while (true) {
				if (Identifier =="procedure") {
					sym = Symbols::ProcedureSym; 
					std::cout << "ProcedureSym" << "\n";
					Identifier = "";
					getChar(is);
					getSymbol(is);
					break;
				}
				else if (Identifier =="program") {
					sym = Symbols::ProgramSym; 
					std::cout << "ProgramSym" << "\n"; 
					Identifier = "";
					getChar(is);
					getSymbol(is);
					break;
				}
				else if(isspace(is.peek()) || ispunct(is.peek())) {
					sym = Symbols::IdentSym; 
					std::cout << "IdentSym:" << Identifier << "\n"; 
					Identifier = "";
					getChar(is);
					getSymbol(is);
					break;
				}
                getChar(is);
				Identifier += ch;
            }
			break;
            
		case 'v':
			Identifier += ch;
            
            while (true) {
				if (Identifier =="var") {
					sym = Symbols::VarSym; 
					std::cout << "VarSym" << "\n";
					Identifier = "";
					getChar(is);
					getSymbol(is);
					break;
				}
				else if(isspace(is.peek()) || ispunct(is.peek())) {
					sym = Symbols::IdentSym; 
					std::cout << "IdentSym:" << Identifier << "\n"; 
					Identifier = "";
					getChar(is);
					getSymbol(is);
					break;
				}
                getChar(is);
				Identifier += ch;
            }
			break;
			
        case '(':
            sym = Symbols::LparenSym; std::cout << "LparenSym" << "\n"; getChar(is); getSymbol(is); break;
        case ')':
            sym = Symbols::RparenSym; std::cout << "RparenSym" << "\n"; getChar(is); getSymbol(is); break;
		case '[':
            sym = Symbols::LbrakSym; std::cout << "LbrakSym" << "\n"; getChar(is); getSymbol(is); break;
        case ']':
            sym = Symbols::RbrakSym; std::cout << "RbrakSym" << "\n"; getChar(is); getSymbol(is); break;
        case '0': case '1': case '2': case '3':
        case '4': case '5': case '6': case '7':
        case '8': case '9':
            
            sym = Symbols::NumberSym;
            val = atoi(&ch);
            getChar(is);
            
            while (isdigit(ch)) {
                val *= 10; val += atoi(&ch);
                getChar(is);
            }
            
			std::cout << "NumberSym:" << val << "\n";
			getSymbol(is);
            break;
			
		case '\n': case ' ':
			getChar(is); break;
		case '{':
			getChar(is);
			while (ch != '}') {
                getChar(is);
				if (is.eof()) {
					Mark();
					break;
				}
            }
			if (!is.eof()) {
				getChar(is);
				getSymbol(is);
			}
			break;
		
		case '$':
			Mark();
			break;
			
		case '%':
			Mark();
			break;
		
		default:
			Identifier += ch;
            
            while (true) {
				if(isspace(is.peek()) || ispunct(is.peek())) {
					sym = Symbols::IdentSym; 
					std::cout << "IdentSym:" << Identifier << "\n"; 
					Identifier = "";
					getChar(is);
					getSymbol(is);
					break;
				}
                getChar(is);
				Identifier += ch;
            }
			break;
		
    }
}

//entry point
int main() {

    // open file stream
    std::ifstream f("input.txt");
    
    if (f.is_open()) {
		
        // get first character, get first symbol
        getChar(f);
    }
    else { std::cout << "input not opened!" << "\n"; }
    
    
    if (sym == Symbols::EofSym)
        std::cout << "Successful scan" << "\n";
    else
        std::cout << "Scan failed";
}