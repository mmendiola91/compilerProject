
#ifndef COMPILER_SYNTAXANALYZER_H
#define COMPILER_SYNTAXANALYZER_H

class SyntaxAnalyzer{
private:
    vector<string> lexemes;
    vector<string> tokens;
    vector<string>::iterator lexitr;
    vector<string>::iterator tokitr;
    map<string, string> symboltable;
    map<string, string> symbolvalues;

    // other private methods
    bool vdec();
    int vars();
    bool stmtlist();
    int stmt();
    bool ifstmt();
    bool elsepart();
    bool whilestmt();
    bool assignstmt();
    bool inputstmt();
    bool outputstmt();
    bool logexpr();
    bool relexpr();
    bool arithexpr();
    bool strterm();
    bool numterm();
    bool logicop();
    bool arithop();
    bool relop();
    istream& getline_safe(istream& input, string& output);
public:
    SyntaxAnalyzer(istream& infile);
    // pre: 1st parameter consists of an open file containing a source code's
    //	valid scanner output.This data must be in the form: token lexeme
    // post: the vectors have been populated

    bool parse();
    // pre: none
    // post: The lexemes/tokens have been parsed.
    // If an error occurs, a message prints indicating the token/lexeme pair
    // that caused the error.
    // If no error, vectors contain syntactically correct source code
};

#endif //COMPILER_SYNTAXANALYZER_H