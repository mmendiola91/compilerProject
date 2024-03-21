#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>

using namespace std;

#include "SyntaxAnalyzer.h"

SyntaxAnalyzer::SyntaxAnalyzer(istream &infile) {
    string line, tok, lex;
    int pos;
    getline_safe(infile, line);
    bool valid = true;
    while (!infile.eof() && (valid)) {
        pos = line.find(" ");
        tok = line.substr(0, pos);
        lex = line.substr(pos + 1, line.length());
        tokens.push_back(tok);
        lexemes.push_back(lex);
        getline_safe(infile, line);
    }
    tokitr = tokens.begin();
    lexitr = lexemes.begin();
}

bool SyntaxAnalyzer::parse() {
    if (vdec()) {
        if (tokitr != tokens.end() && *tokitr == "t_main") {
            tokitr++;
            lexitr++;
            if (tokitr != tokens.end() && stmtlist()) {
                if (tokitr != tokens.end())
                    if (*tokitr == "t_end") {
                        tokitr++;
                        lexitr++;
                        if (tokitr == tokens.end()) {  // end was last thing in file
                            cout << "Valid source code file" << endl;
                            return true;
                        } else {
                            cout << "end came too early" << endl;
                        }
                    } else {
                        cout << "invalid statement ending code" << endl;
                    }
                else {
                    cout << "no end" << endl;
                }
            } else {
                cout << "bad/no stmtlist" << endl;
            }
        } else {
            cout << "no main" << endl;
        }
    } else {
        cout << "bad var list" << endl;
    }
    return false;

}

bool SyntaxAnalyzer::vdec() {
    if (tokitr == tokens.end()) return true;
    if (*tokitr != "t_var")
        return true;
    else {
        tokitr++;
        lexitr++;
        int result = 0;   // 0 - valid, 1 - done, 2 - error
        result = vars();
        if (result == 2)
            return false;
        while (result == 0) {
            if (tokitr != tokens.end())
                result = vars(); // parse vars
        }
        if (result == 1)
            return true;
        else
            return false;
    }
}

int SyntaxAnalyzer::vars() {
    int result = 0;  // 0 - valid, 1 - done, 2 - error
    string temp;
    if (tokitr != tokens.end() && *tokitr == "t_integer") {
        temp = "t_integer";
        tokitr++;
        lexitr++;
    } else if (tokitr != tokens.end() && *tokitr == "t_string") {
        temp = "t_string";
        tokitr++;
        lexitr++;
    } else
        return 1;
    bool semiFound = false;
    while (tokitr != tokens.end() && result == 0 && !semiFound) {
        if (tokitr != tokens.end() && *tokitr == "t_id") {
            symboltable[*lexitr] = temp;
            if (temp == "t_integer")
                symbolvalues[*lexitr] = "0";
            else
                symbolvalues[*lexitr] = "";
            tokitr++;
            lexitr++;
            if (tokitr != tokens.end() && *tokitr == "s_comma") {
                tokitr++;
                lexitr++;
            } else if (tokitr != tokens.end() && *tokitr == "s_semi") {
                semiFound = true;
                tokitr++;
                lexitr++;
            } else
                result = 2;
        } else {
            result = 2;
        }
    }
    return result;
}

bool SyntaxAnalyzer::stmtlist() {
    int result = stmt();
    while (result == 1) {
        result = stmt();
    }
    if (result == 0)
        return false;
    else
        return true;
}

int SyntaxAnalyzer::stmt() {  // returns 1 or 2 if valid, 0 if invalid
    if (tokitr != tokens.end() && *tokitr == "t_if") {
        tokitr++;
        lexitr++;
        if (ifstmt()) return 1;
        else return 0;
    } else if (tokitr != tokens.end() && *tokitr == "t_while") {
        tokitr++;
        lexitr++;
        if (whilestmt()) return 1;
        else return 0;
    } else if (tokitr != tokens.end() && *tokitr == "t_id") {  // assignment starts with identifier
        tokitr++;
        lexitr++;
        if (assignstmt()) return 1;
        else return 0;
    } else if (tokitr != tokens.end() && *tokitr == "t_input") {
        tokitr++;
        lexitr++;
        if (inputstmt()) return 1;
        else return 0;
    } else if (tokitr != tokens.end() && *tokitr == "t_output") {
        tokitr++;
        lexitr++;
        if (outputstmt()) return 1;
        else return 0;
    }
    return 2;  //stmtlist can be null
}

bool SyntaxAnalyzer::ifstmt() {
    if (tokitr == tokens.end()) return false;
    if (*tokitr == "s_lparen") {
        tokitr++;
        lexitr++;
        if (logexpr()) {
            if (tokitr != tokens.end() && *tokitr == "s_rparen") {
                tokitr++;
                lexitr++;
                if (tokitr != tokens.end() && *tokitr == "t_then") {
                    tokitr++;
                    lexitr++;
                    if (stmtlist() && elsepart()) {
                        if (tokitr != tokens.end() && *tokitr == "t_end") {
                            tokitr++;
                            lexitr++;
                            if (tokitr != tokens.end() && *tokitr == "t_if") {
                                tokitr++;
                                lexitr++;
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool SyntaxAnalyzer::elsepart() {
    if (tokitr != tokens.end() && *tokitr == "t_else") {
        tokitr++;
        lexitr++;
        if (stmtlist())
            return true;
        else
            return false;
    }
    return true;   // elsepart can be null
}

bool SyntaxAnalyzer::whilestmt() {
    if (tokitr == tokens.end()) return false;
    if (*tokitr == "s_lparen") {
        tokitr++;
        lexitr++;
        if (logexpr()) {
            if (tokitr != tokens.end() && *tokitr == "s_rparen") {
                tokitr++;
                lexitr++;
                if (tokitr != tokens.end() && *tokitr == "t_loop") {
                    tokitr++;
                    lexitr++;
                    if (stmtlist()) {
                        if (tokitr != tokens.end() && *tokitr == "t_end") {
                            tokitr++;
                            lexitr++;
                            if (tokitr != tokens.end() && *tokitr == "t_loop") {
                                tokitr++;
                                lexitr++;
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool SyntaxAnalyzer::assignstmt() {
    lexitr--;  // check if t_id was an integer or string
    if (symboltable[*lexitr] == "t_integer") {
        lexitr++;
        if (tokitr != tokens.end() && *tokitr == "s_assign") {
            tokitr++;
            lexitr++;
            if (!arithexpr()) return false;
        }
    } else { // id must be a string
        lexitr++;
        if (tokitr != tokens.end() && *tokitr == "s_assign") {
            tokitr++;
            lexitr++;
            if (!strterm()) return false;
        }
    }
    if (tokitr != tokens.end() && *tokitr == "s_semi") {
        tokitr++;
        lexitr++;
        return true;
    } else
        return false;

}

bool SyntaxAnalyzer::inputstmt() {
    if (tokitr != tokens.end() && *tokitr == "s_lparen") {
        tokitr++;
        lexitr++;
        if (tokitr != tokens.end() && *tokitr == "t_id") {
            tokitr++;
            lexitr++;
            if (tokitr != tokens.end() && *tokitr == "s_rparen") {
                tokitr++;
                lexitr++;
                return true;
            }
        }
    }
    return false;
}

bool SyntaxAnalyzer::outputstmt() {
    if (tokitr != tokens.end() && *tokitr == "s_lparen") {
        tokitr++;
        lexitr++;
        if (!numterm())
            if (!strterm())
                return false;
        if (tokitr != tokens.end() && *tokitr == "s_rparen") {
            tokitr++;
            lexitr++;
            return true;
        }
    }
    return false;
}

bool SyntaxAnalyzer::logexpr() {
    if (relexpr()) {
        while (logicop()) {
            if (!relexpr()) return false;
        }
        return true;
    }
    return false;
}

bool SyntaxAnalyzer::relexpr() {
    if (arithexpr() && relop() && arithexpr())
        return true;
    return false;
}

bool SyntaxAnalyzer::arithexpr() {
    if (numterm()) {
        while (arithop()) {
            if (!numterm()) return false;
        }
        return true;
    }
    return false;
}

bool SyntaxAnalyzer::numterm() {
    if (tokitr != tokens.end()) {
        if (*tokitr == "t_number") {
            tokitr++;
            lexitr++;
            return true;
        }
        if (*tokitr == "t_id") {
            // make sure integer id
            if (symboltable[*lexitr] != "t_integer")
                return false;
            tokitr++;
            lexitr++;
            return true;
        }
    }
    return false;
}

bool SyntaxAnalyzer::strterm() {
    if (tokitr != tokens.end()) {
        if (*tokitr == "t_text") {
            tokitr++;
            lexitr++;
            return true;
        }
        if (*tokitr == "t_id") {
            // make sure string id
            if (symboltable[*lexitr] != "t_string")
                return false;
            tokitr++;
            lexitr++;
            return true;
        }
    }
    return false;
}

bool SyntaxAnalyzer::logicop() {
    if (tokitr != tokens.end() && ((*tokitr == "s_and") || (*tokitr == "s_or"))) {
        tokitr++;
        lexitr++;
        return true;
    } else
        return false;
}

bool SyntaxAnalyzer::arithop() {
    if (tokitr != tokens.end() && ((*tokitr == "s_mult") || (*tokitr == "s_plus") ||
                                   (*tokitr == "s_minus") || (*tokitr == "s_div") ||
                                   (*tokitr == "s_mod"))) {
        tokitr++;
        lexitr++;
        return true;
    } else
        return false;
}

bool SyntaxAnalyzer::relop() {
    if (tokitr != tokens.end() && ((*tokitr == "s_lt") || (*tokitr == "s_gt") ||
                                   (*tokitr == "s_ge") || (*tokitr == "s_eq") ||
                                   (*tokitr == "s_ne") || (*tokitr == "s_le"))) {
        tokitr++;
        lexitr++;
        return true;
    } else {
        return false;
    }
}

istream &SyntaxAnalyzer::getline_safe(istream &input, string &output) {
    char c;
    output.clear();
    input.get(c);
    while (input && c != '\n') {
        if (c != '\r' || input.peek() != '\n') {
            output += c;
        }
        input.get(c);
    }
    return input;
}
