#include <iostream>
#include <fstream>
#include <vector>
#include <map>
using namespace std;
#include "LexAnalyzer.h"
#include "SyntaxAnalyzer.h"

int main() {
    /// Lexeme Analyzer
    ifstream lexemeFile("lexemes.txt");
    if (!lexemeFile){
        cout << "can't find lexeme file" << endl;
        exit(-1);
    }
    ifstream sourceFile("source.txt");
    if (!sourceFile){
        cout << "can't find source file" << endl;
        exit(-1);
    }
    ofstream outFile("codelexemes.txt");
    LexAnalyzer lexAn(lexemeFile);

//    lexAn.printMap(outFile);
    if (!lexAn.scanFile(sourceFile, outFile)){
        cout << "error in source file" << endl;
    }
    lexAn.printVectors(outFile);

    cout << endl;
    cout << endl;
    cout << endl;

    /// Syntax Analyzer
    ifstream infile("codelexemes.txt");
    if (!infile){
        cout << "error opening lexemes.txt file" << endl;
        exit(-1);
    }

    SyntaxAnalyzer sa(infile);
    sa.parse();
    return 0;
}