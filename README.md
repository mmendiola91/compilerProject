# complierProject
This repository contains code for compiler following a custom grammar written in C++;

--------------------------------------------------------------------------------------------
The following is the structure and design of the grammar

There are 5 grammar symbols:    (implies)   | (or)    Ө (empty set)     {} (0 or more)    [] (0 or 1)

PROG -> VDEC main STMTLIST end
VDEC  -> var  VARS  {VARS} | Ө
VARS -> TYPE  id  {, id} ;
TYPE  ->  integer  |  string
STMTLIST  ->  STMT {STMT} | Ө 
STMT  ->  IFSTMT  |  WHILESTMT  |  ASSIGNSTMT   |  INPUTSTMT  |  OUTPUTSTMT  
IFSTMT ->  if  (LOGEXPR)  then  STMTLIST  ELSEPART  end if
ELSEPART  ->  else  STMTLIST  |  Ө
WHILESTMT  ->  while  (LOGEXPR)  loop  STMTLIST  end loop
ASSIGNSTMT  ->  id = ARITHEXPR;  |  id = STRTERM;
INPUTSTMT  ->  input (id )
OUTPUTSTMT  ->  output (NUMTERM)  | output (STRTERM) 

LOGEXPR -> RELEXPR {LOGICOP RELEXPR} 
RELEXPR ->  ARITHEXP RELOP ARITHEXPR
ARITHEXPR ->  NUMTERM  {ARITHOP NUMTERM} 

NUMTERM ->  number | integer-id  
STRTERM ->  text |  string-id  

LOGICOP -> and | or
RELOP  ->  ==  |  <  |  <=  |  >  |  >=  |  != 
ARITHOP  ->  +  |  -  |  *  |  /  |  %  
