#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <stack>
#include <string>

using namespace std;

/// You will need these forward references.
class Expr;

class Stmt;

/// Runtime Global Variables
int pc; // program counter
vector<string> lexemes;
vector<string> tokens;
vector<string>::iterator lexitr;
vector<string>::iterator tokitr;
map<string, string> symbolvalues; // map of variables and their values
map<string, string> symboltable; // map of variables to datatype (i.e. sum t_integer)
vector<Stmt *> insttable; // table of instructions
map<string, int> precMap;


/// Runtime Global Methods
void dump(); // prints vartable, instable, symboltable

// You may need a few additional global methods to manipulate the global variables


// Classes Stmt and Expr
// It is assumed some methods in statement and expression objects will change and
// you may need to add a few new ones.


class Expr { // expressions are evaluated!
public:
    virtual string toString() = 0;

    virtual ~Expr() {}
};

class StringExpr : public Expr {
public:
    virtual string eval() = 0;
};

class IntExpr : public Expr {
public:
    virtual int eval() = 0;
};

class StringConstExpr : public StringExpr {
private:
    string value;
public:
    StringConstExpr(string val) { value = val; }

    ~StringConstExpr() {}

    string eval() { return value; }

    string toString() { return value; }
};

class StringIDExpr : public StringExpr {
private:
    string id;
public:
    StringIDExpr(string val) { id = val; }

    ~StringIDExpr() {}

    string eval() { return symbolvalues[id]; }

    string toString() { return id; }
};

class PostFixExpr : public IntExpr {
private:
    vector<string> expr;
public:
    PostFixExpr() {}

    PostFixExpr(string x) { expr.push_back(x); }

    ~PostFixExpr() {}

    void add(string x) { expr.push_back(x); }

    bool isOperator(string term) {
        if (term == "+" || term == "-" || term == "/" || term == "*" || term == "%")
            return true;
        if (term == "<" || term == ">" || term == "<=" || term == ">=" || term == "==")
            return true;
        if (term == "and" || term == "or")
            return true;
        return false;
    }

    int applyOper(int a, int b, string oper) {
        if (oper == "+")
            return a + b;
        else if (oper == "-")
            return a - b;
        else if (oper == "*")
            return a * b;
        else if (oper == "/")
            return a / b;
        else if (oper == "<")
            return a < b;
        else if (oper == ">")
            return a > b;
        else if (oper == "<=")
            return a <= b;
        else if (oper == ">=")
            return a >= b;
        else if (oper == "==")
            return a == b;
        else if (oper == "!=")
            return a != b;
        else if (oper == "or")
            return a || b;
        else if (oper == "and")
            return a && b;
        else
            return a % b;
    }

    int eval() {
        stack<int> valueStk;
        for (int i = 0; i < expr.size(); i++) {
            if (isOperator(expr[i])) {
                // our grammar already determined this is a valid expression - no need to double check
                int b = valueStk.top();
                valueStk.pop();
                int a = valueStk.top();
                valueStk.pop();
                valueStk.push(applyOper(a, b, expr[i]));
            } else {
                // check if id - start with a letter
                if (isalpha(expr[i][0]))
                    valueStk.push(stoi(symbolvalues[expr[i]]));
                else
                    valueStk.push(stoi(expr[i]));
            }
        }

        return valueStk.top();
    }

    string toString() {
        string t = "";
        for (int i = 0; i < expr.size(); i++) {
            t = t + expr[i] + " ";
        }
        return t;
    }
};

class Stmt { // statements are executed!
private:
    string name;
public:
    Stmt() {}

    virtual ~Stmt() {};

    virtual string toString() = 0;

    virtual void execute() = 0;

    void setName(string n) { name = n; }

    string getName() { return name; }
};

class AssignStmt : public Stmt {
private:
    string var;
    Expr *p_expr;
public:
    AssignStmt(string n, string s, Expr *p) {
        setName(n);
        var = s;
        p_expr = p;
    }

    AssignStmt() {}

    ~AssignStmt() { delete p_expr; }

    string toString() {
        if (p_expr)
            return getName() + " " + var + " " + p_expr->toString();
        else
            return getName() + " " + var;
    }

    void execute() {
        StringExpr *t1 = dynamic_cast<StringExpr *>(p_expr);
        if (t1 != nullptr) {
            symbolvalues[var] = t1->eval();
        } else {
            IntExpr *t2 = dynamic_cast<IntExpr *>(p_expr);
            symbolvalues[var] = to_string(t2->eval());
        }
        pc++;
    }
};

class InputStmt : public Stmt {
private:
    string var;
public:
    InputStmt() {}

    InputStmt(string v) {
        setName("t_input");
        var = v;
    }

    ~InputStmt() {}

    string toString() { return getName() + " " + var; }

    void execute() {
        if (symboltable[var] == "t_integer") {
            /// Integer Var
            int x;
            cout << "enter a number: " << endl;
            cin >> x;
            symbolvalues[var] = to_string(x);
        } else {
            ///string Var
            string input;
            cout << "enter a string: " << endl;
            cin >> input;
            symbolvalues[var] = input;
        }
        pc++;
    }
};

class OutputStmt : public Stmt {
private:
    string value;
public:
    OutputStmt() {}

    OutputStmt(string s) {
        setName("t_output");
        value = s;
    }

    ~OutputStmt() {}

    string toString() { return getName() + " " + value; }

    void execute() {
//        if (*tokitr == "t_id") {
        if (symbolvalues.find(value) != symbolvalues.end()) {
            cout << symbolvalues[value] << endl;
        }
//        }
        else {
            cout << value << endl;
        }
        pc++;
    }
};

class IfStmt : public Stmt {
private:
    Expr *p_expr;
    int elsetarget;
public:
    IfStmt() {}

    IfStmt(string n, Expr *p) {
        setName(n);
        p_expr = p;
    }

    ~IfStmt() { delete p_expr; }

    void setTarget(int x) { elsetarget = x; }

    string toString() {
        if (p_expr)
            return getName() + " " + p_expr->toString() + " " + to_string(elsetarget);
        else
            return getName() + " " + to_string(elsetarget);
    }

    void execute() {
        IntExpr *t = dynamic_cast<IntExpr *>(p_expr);
        // no need to double check - syntax analyzer only accepted postfix expressions
        if (t->eval()) pc++;
        else pc = elsetarget;
    }
};

class WhileStmt : public Stmt {
    // this class needs to be completed
private:
    Expr *p_expr;
    int elsetarget;
public:
    WhileStmt(Expr *p, int target) {
        p_expr = p;
        elsetarget = target;
        setName("t_while");
    }

    ~WhileStmt() {
        delete p_expr;
    }

    string toString() {
        return getName() + " " + p_expr->toString() + " " + to_string(elsetarget);
    }

    void execute() {
        IntExpr *t = dynamic_cast<IntExpr *>(p_expr);
        if (t->eval()) {
            pc = elsetarget;
        } else {
            pc++;
        }
    }

    void setTarget(int i) {
        elsetarget = i;
    }
};

class GoToStmt : public Stmt {
private:
    int target;
public:
    GoToStmt() { setName("t_goto"); }

    ~GoToStmt() {}

    void setTarget(int a) { target = a; }

    string toString() { return getName() + " " + to_string(target); }

    void execute() {
        pc = target;
    }
};

class CodeRun {
private:
    void buildIf() {
        // move to start of expr
        tokitr++;
        lexitr++;
        tokitr++;
        lexitr++;
        Expr *expr = buildExpr(); // build conditional expression
        IfStmt *stmt = new IfStmt("t_if", expr);
        insttable.push_back(stmt);
        int ifSlot = insttable.size() - 1;
        int elseSlot;
        int gotoSlot;

        tokitr++;
        lexitr++;  // get past s_rparen
        tokitr++;
        lexitr++;  // get past t_then
        while (*tokitr != "t_else" && *tokitr != "t_end") {
            buildStmt();
        }

        if (*tokitr == "t_else") {
            *tokitr++;
            *lexitr++;
            // add goto to jump around else if needed for true block
            GoToStmt *stmtGT= new GoToStmt();  // add but come back to fix
            insttable.push_back(stmtGT);
            gotoSlot = insttable.size() - 1;

            // process body of the else block
            elseSlot = insttable.size();
            while (*tokitr != "t_end") {
                buildStmt();
            }
            // now at end of else, so go back and fix goto jump value
            GoToStmt *t = dynamic_cast<GoToStmt *>(insttable[gotoSlot]);
            t->setTarget(insttable.size());
        } else {
            elseSlot = insttable.size();  // next stmt after if ends
        }
        IfStmt *t = dynamic_cast<IfStmt *>(insttable[ifSlot]);
        if (t) t->setTarget(elseSlot);
        tokitr++;
        lexitr++;
    }

    void buildWhile() {
        tokitr++;
        lexitr++; // push past t_while
        tokitr++;
        lexitr++; // push past s_lparen

        Expr *expr = buildExpr(); // build conditional expression
        WhileStmt *stmt = new WhileStmt(expr, insttable.size());
        insttable.push_back(stmt);
        int whileSlot = insttable.size() - 1;

        tokitr++;
        lexitr++; // push past s_rparen
        tokitr++;
        lexitr++; // push past t_loop

        while (*tokitr != "t_end") {
            buildStmt();
        }
        GoToStmt *gotoStmt = new GoToStmt();
        insttable.push_back(gotoStmt);
        int gotoSlot = insttable.size() - 1;

        GoToStmt *t = dynamic_cast<GoToStmt *>(insttable[gotoSlot]);
        t->setTarget(whileSlot);

        WhileStmt *t2 = dynamic_cast<WhileStmt *>(insttable[whileSlot]);
        if (t) t2->setTarget(gotoSlot + 1);
        tokitr++;
        lexitr++;
    }


    void buildStmt() {
        if (*tokitr == "t_if")
            buildIf();
        else if (*tokitr == "t_while")
            buildWhile();
        else if (*tokitr == "t_id")
            buildAssign();
        else if (*tokitr == "t_input")
            buildInput();
        else
            buildOutput();

        tokitr++;
        lexitr++;

    }

    void buildAssign() {
        // create assignment statement object
        string varId = *lexitr;  // store variable name
        cout << *lexitr << endl;
        // skip id and s_assign
        tokitr++;
        lexitr++;
        tokitr++;
        lexitr++;
        // call buildexpr
        Expr *expr = buildExpr();
        AssignStmt *stmt = new AssignStmt("s_assign", varId, expr);
        insttable.push_back(stmt);
    }

    void buildInput() {
        tokitr++;
        lexitr++;  // get past output
        tokitr++;
        lexitr++;  // get past s_lparen
        InputStmt *stmt = new InputStmt(*lexitr);
        insttable.push_back(stmt);
        tokitr++;
        lexitr++; // move to s_paren
    }

    void buildOutput() {
        tokitr++;
        lexitr++;  // get past output
        tokitr++;
        lexitr++;  // get past s_lparen
        OutputStmt *stmt = new OutputStmt(*lexitr);
        insttable.push_back(stmt);
        tokitr++;
        lexitr++; // move to s_paren
    }

    Expr *buildExpr() {
        // we are at the start of an expression
        // determine type of expr
        // store 1st part
        Expr *expr;
        // check if string
        if (*tokitr == "t_string") {
            // string variable
            expr = new StringIDExpr(*lexitr);
            // get past ;
            tokitr++;
            lexitr++;
        } else if (*tokitr == "t_text") {
            // string constant
            expr = new StringConstExpr(*lexitr);
            // get past ;
            tokitr++;
            lexitr++;
        } else {
            // numeric expression - build postfix expression
            stack<string> operStk;
            PostFixExpr *t = new PostFixExpr();
            while (*tokitr != "s_semi" && *tokitr != "s_rparen") {
                if (isOperator(*lexitr)) {
                    // check if stack has higher operator precedence on it
                    // higher precedence terms have lower numbers
                    while (!operStk.empty() && precMap[operStk.top()] <= precMap[*lexitr]) {
                        t->add(operStk.top());
                        operStk.pop();
                    }
                    // push current term on stack
                    operStk.push(*lexitr);
                } else { // it's an operand
                    t->add(*lexitr);
                }
                tokitr++;
                lexitr++;
            }
            // pop anything left on stack and add to postFix
            while (!operStk.empty()) {
                t->add(operStk.top());
                operStk.pop();
            }
            expr = t;
        }
        return expr;
    }

    // headers for populate methods may not change
    void populateTokenLexemes(istream &infile) {
        int pos;
        string line;
        getline(infile, line);
        while (!infile.eof()) {
            pos = line.find(" ");
            tokens.push_back(line.substr(0, pos));
            lexemes.push_back(line.substr(pos + 1, line.length()));
            getline(infile, line);
        }
    }

    void populateSymbolTable(istream &infile) {
        string var, type;
        infile >> var >> type;
        while (!infile.eof()) {
            symboltable[var] = type;
            if (type == "t_integer")
                symbolvalues[var] = "0";
            else
                symbolvalues[var] = "";
            infile >> var >> type;
        }
    }

    bool isOperator(string term) {
        if (term == "+" || term == "-" || term == "/" || term == "*" || term == "%")
            return true;
        if (term == "<" || term == ">" || term == "<=" || term == ">=" || term == "==")
            return true;
        if (term == "and" || term == "or")
            return true;
        return false;
    }

public:
    // headers may not change
    CodeRun(istream &source, istream &symbols) {
        precMap["or"] = 5;
        precMap["and"] = 4;
        precMap["=="] = 3;
        precMap["!="] = 3;
        precMap[">="] = 3;
        precMap["<="] = 3;
        precMap[">"] = 3;
        precMap["<"] = 3;
        precMap["+"] = 2;
        precMap["-"] = 2;
        precMap["*"] = 1;
        precMap["/"] = 1;
        precMap["%"] = 1;
        populateTokenLexemes(source);
        populateSymbolTable(symbols);
    }

    // The codeGen method is responsible for getting the instruction
    // table built.  It will call the appropriate build methods.
    // The group is responsible for constructing the build methods
    // in a similar format.
    bool codeGen() {
        // move past tokens until you hit main
        tokitr = tokens.begin();
        lexitr = lexemes.begin();
        while (*tokitr != "t_main") {
            tokitr++;
            lexitr++;
        }
        tokitr++;
        lexitr++;
        while (*tokitr != "t_end") {
            buildStmt();
        }
        return true;
    }

    // The run method will execute the code in the instruction
    // table.
    void run() {
        pc = 0;
        int instCtr = insttable.size();
        while (pc < instCtr) {
            insttable[pc]->execute();
        }
    }

    void print() {
        cout << "\nprinting token/lexemes" << endl;
        for (int i = 0; i < tokens.size(); i++) {
            cout << tokens[i] << " " << lexemes[i] << endl;
        }
        cout << "\nprinting symboltable and symbolvalues" << endl;
        map<string, string>::iterator symboltableitr;
        for (symboltableitr = symboltable.begin();
             symboltableitr != symboltable.end();
             symboltableitr++) {
            cout << symboltableitr->first << " " << symboltableitr->second << endl;
            cout << symboltableitr->first << " " << symbolvalues[symboltableitr->first] << endl;
        }
        cout << "\nprinting instruction table" << endl;
        cout << insttable.size() << endl;
        for (int i = 0; i < insttable.size(); i++) {
            cout << insttable[i]->toString() << endl;
        }
        cout << "end of output" << endl;
    }
};


int main() {
//    /// Lexeme Analyzer
//    ifstream lexemeFile("lexemes.txt");
//    if (!lexemeFile){
//        cout << "can't find lexeme file" << endl;
//        exit(-1);
//    }
//    ifstream sourceFile("source.txt");
//    if (!sourceFile){
//        cout << "can't find source file" << endl;
//        exit(-1);
//    }
//    ofstream outFile("codelexemes.txt");
//    LexAnalyzer lexAn(lexemeFile);
//
////    lexAn.printMap(outFile);
//    if (!lexAn.scanFile(sourceFile, outFile)){
//        cout << "error in source file" << endl;
//    }
//    lexAn.printVectors(outFile);
//
//    cout << endl;
//    cout << endl;
//    cout << endl;
//
//    /// Syntax Analyzer
//    ifstream infile("codelexemes.txt");
//    if (!infile){
//        cout << "error opening lexemes.txt file" << endl;
//        exit(-1);
//    }
//
//    SyntaxAnalyzer sa(infile);
//    sa.parse();

    ifstream source("data.txt");
    ifstream symbols("vars.txt");

    CodeRun c(source, symbols);
//    c.print();
    c.codeGen();
//    c.print();
    c.run();
//    c.print();
    return 0;
}