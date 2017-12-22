%{
#include <stdlib.h>
#include "ast.h"
#include "y.tab.h"

%}
%option yylineno

%%

"and"           {return T_AND;}
"not"           {return T_NOT;}
"or"            {return T_OR;}
"begin"            {return T_BEGIN;}
"end"            {return T_END;}
"var"           {return T_VAR;}
"program"       {return T_PROGRAM;}
"return"        {return T_RETURN;}
"write"         {return T_WRITE;}
"if"         {return T_IF;}
"else"         {return T_ELSE;}
"while"         {return T_WHILE;}
"do"         {return T_DO;}
"then"         {return T_THEN;}
"function"      {return T_FUNCTION;}
"for"      {return T_FOR;}

[a-zA-Z][a-zA-Z0-9_]*   {
                		yylval.s = strdup(yytext);
                		return T_VARIABLE;
            		}

[0-9]* 			{
                		yylval.f = atof(yytext);
                		return T_NUMERIC;
            		}

[0-9]+\.[0-9]+ 			{
                		yylval.f = atof(yytext);                		return T_NUMERIC;
            		}

">="            {return T_GE;}
"<="            {return T_LE;}
"=="            {return T_EQ;}
"!="            {return T_NE;}
"//"((.)*)\n	{printf("LineComment\n");}
"/*"(([^*]|(("*"+)[^*/]))*)("*"+)"/"\n {printf("MultilineComment\n");}
[ \t\n]+
.               { return *yytext; }

%%

void yyerror(char *s) {
  fprintf(stderr,"Error | Line: %d\n%s\n",yylineno,s);
}


int yywrap(void) {
    return 1;
}
