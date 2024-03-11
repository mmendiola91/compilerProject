#include <iostream>
#include <fstream>
#include <vector>
#include <map>
using namespace std;
#include "LexAnalyzer.h"
LexAnalyzer::LexAnalyzer(istream& infile) {
    string tok, lex;
    infile >> tok >> lex;
    while (!infile.eof()) {
        tokenmap[lex] = tok;
        infile >> tok >> lex;
    }
}
bool LexAnalyzer::scanFile(istream& infile, ostream& outfile) {
    string line;
    getline(infile, line);
    bool error = false;
    while (!infile.eof() && !error) {
        int i = 0;
        while (i < line.size() && !error) {
            //eliminating opening whitespace
            while (isspace(line[i])) i++;

            if (isalpha(line[i])) {
                if (i > 0 && !isspace(line[i-1]) && inMap(line[i-1]) == tokenmap.end())
                    error = true;
                else
                    keywordIdCheck(line, i);
            }
            else if (isdigit(line[i])){
                if (i > 0 && !isspace(line[i-1]) && inMap(line[i-1]) == tokenmap.end())
                    error = true;
                else
                    error = integerCheck(line, i);
            }
            else if (line[i] == '"'){
                if (i > 0 && !isspace(line[i-1]) && inMap(line[i-1]) == tokenmap.end())
                    error = true;
                else
                    error = textCheck(line, i);
            }
            else{ //gray area stuff
                error = symbolsCheck(line, i);
            }
            //i++;
        }
        getline(infile, line);
    }
    // return true if scanfile works and false otherwise
    return !error;
}
void LexAnalyzer::keywordIdCheck(string line, int& i){
    string tempLex = "";
    // pull off characters until isspace or non-alpha or non-underscore
    do {
        tempLex += line[i];
        i++;

    } while (isalpha(line[i]) || isdigit(line[i]) || line[i] == '_');

    while (isspace(line[i])) i++;

    if (isKeyword(tempLex)) {
        lexemes.push_back(tempLex);
        tokens.push_back("t_" + tempLex);
    } else {  // t_id
        lexemes.push_back(tempLex);
        tokens.push_back("t_id");
    }
    tempLex = "";
}
bool LexAnalyzer::integerCheck(string line, int& i){
    string tempLex = "";
    bool error = false;
    // pull off characters until non digit
    do {
        tempLex += line[i];
        i++;
    } while (isdigit(line[i]));
    //if (!isalpha(line[i])){
    lexemes.push_back(tempLex);
    tokens.push_back("t_number");
//    }
//    else{
//        error = true;
//    }
    while (isspace(line[i])) i++;
    return error;
}
bool LexAnalyzer::textCheck(string line, int& i){
    string tempLex = "";
    bool error = false;
    // pull off characters until " or end of line found
    i++;
    while (i<line.size() && line[i] != '"'){
        tempLex += line[i];
        i++;
    }
    if (line[i] == '"'){
        lexemes.push_back(tempLex);
        tokens.push_back("t_text");
        i++;
    }
    else{
        error = true;
    }
    while (isspace(line[i])) i++;
    return error;
}
bool LexAnalyzer::symbolsCheck(string line, int& i){
    // this function was causing errors
    // i must be left at next character to analyze
    bool error = false;
    switch(line[i]) {
        case '<' :
            if (i < line.size() && line[i+1] == '=') {
                i++;
                lexemes.push_back("<=");
                tokens.push_back("s_le");
            } else {
                lexemes.push_back("<");
                tokens.push_back("s_lt");
            }
            break;
        case '>' :
            if (i < line.size() && line[i+1] == '=') {
                i++;
                lexemes.push_back(">=");
                tokens.push_back("s_ge");
            } else {
                lexemes.push_back(">");
                tokens.push_back("s_gt");
            }
            break;
        case '=' :
            if (i < line.size() && line[i+1] == '=') {
                i++;
                lexemes.push_back("==");
                tokens.push_back("s_eq");
            } else {
                lexemes.push_back("=");
                tokens.push_back("s_assign");
            }
            break;
        case '!' :
            if (i < line.size() && line[i+1] == '=') {
                i++;
                lexemes.push_back("!=");
                tokens.push_back("s_ne");
            } else {
                error = true;
            }
            break;
        default :
//            string t;
//            t = line[i];  //string t = line[i] won't work
            auto itr = inMap(line[i]);
            if (itr != tokenmap.end()) {
                lexemes.push_back(itr->first);
                tokens.push_back(itr->second);
            } else {
                error = true;
            }
            break;
    }
    i++;
    return error;
}
map<string, string>::iterator LexAnalyzer::inMap(char x){
    string t;
    t = x;
    return tokenmap.find(t);
}
bool LexAnalyzer::isKeyword(string s){
    if (s=="and" || s == "input" || s == "output" || s == "end" ||
        s == "integer" || s=="string" || s == "var" || s == "else" ||
        s=="while" || s == "if" || s == "loop" ||
        s == "or" || s=="then" || s == "main")
        return true;
    else
        return false;

}
void LexAnalyzer::printVectors(ostream& out){
    cout << "Token and Lexemes for Source Code" << endl;
    for (int i=0; i<lexemes.size(); i++){
        cout << tokens[i] << " " << lexemes[i] << endl;
        out << tokens[i] << " " << lexemes[i] << endl;
    }
}
void LexAnalyzer::printMap(ostream& out){
    cout << "Token and Lexemes for Language" << endl;
    // datatype for itr is map<string, string>::iterator
    for(auto itr=tokenmap.begin(); itr!=tokenmap.end(); itr++) {
        cout << itr->first << " " << itr->second << endl;
    }
}
