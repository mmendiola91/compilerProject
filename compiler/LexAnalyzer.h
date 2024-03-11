
#ifndef COMPILER_LEXANALYZER_H
#define COMPILER_LEXANALYZER_H


class LexAnalyzer {
private:
    vector<string> lexemes; // source code lexemes
    vector<string> tokens;  // source code tokens
    map<string, string> tokenmap;  // lexeme/token map for language
    bool isKeyword(string s);
    void keywordIdCheck(string line, int& i);
    bool integerCheck(string line, int& i);
    bool textCheck(string line, int& i);
    bool symbolsCheck(string line, int& i);
    map<string, string>::iterator inMap(char x);
public:
    // pre: parameter refers to open data file of token and lexeme pairs
    //      for the language (i.e.  t_and and t_begin begin s_gt >)
    // post: tokenmap has been populated
    explicit LexAnalyzer(istream& infile);

    // pre: 1st parameter refers to an open text file
    //      of source code written in the language
    //      2nd parameter refers to an open empty output file
    // post: valid token and lexeme pairs of the source code
    //       are in the output file.  If there is an error,
    //       the output file is considered incomplete.
    //       A success or fail message has printed to the console.
    bool scanFile(istream& infile, ostream& outfile);

    void printVectors(ostream& out);

    void printMap(ostream& out);
};


#endif //COMPILER_LEXANALYZER_H