%{
#include <stdlib.h>
#include "ast.h"
#include "y.tab.h"

%}

%%

"and"           {return T_AND;}
"not"           {return T_NOT;}
"or"            {return T_OR;}
"begin"            {return T_BEGIN;}
"end"            {return T_END;}
"var"           {return T_VAR;}
"write"         {return T_WRITE;}
"if"         {return T_IF;}
"else"         {return T_ELSE;}
"while"         {return T_WHILE;}
"do"         {return T_DO;}
"then"         {return T_THEN;}

[a-zA-Z][a-zA-Z0-9_]*   {
                		yylval.s = strdup(yytext);
                		return T_VARIABLE;
            		}

[0-9]* 			{
                		yylval.f = atof(yytext);
                		return T_NUMERIC;
            		}

[0-9]+\.[0-9]+ 			{
                		yylval.f = atof(yytext);
                		return T_NUMERIC;
            		}

">="            {return T_GE;}
"<="            {return T_LE;}
"=="            {return T_EQ;}
"!="            {return T_NE;}

[ \t\n]+
.               { return *yytext; }

%%
int yywrap(void) {
    return 1;
}
