%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "symboles.h"
#include "ast.h"

int yylex(void);
void yyerror(char *s);

nodeType *programRoot;

int typeDeclaration;

%}

%union {
	float f;
	char *s;
	nodeType *np;
};

%nonassoc IFX
%nonassoc T_ELSE

%left T_GE T_LE T_EQ T_NE '>' '<'
%left '+' '-' T_OR
%left '*' '/' T_AND
%nonassoc T_UMINUS
%nonassoc T_NNOT

%token T_AND T_NUMERIC T_BEGIN T_END
%token T_NOT T_OR
%token T_VAR T_VARIABLE T_WRITE

%type <f> T_NUMERIC
%type <s> T_VARIABLE 
%type <np> program_main
%type <np> instruction_composee
%type <np> acces_variable 
%type <np> liste_instructions 
%type <np> instruction
%type <np> write_instruction 
%type <np> affectation 
%type <np> expr 

//%type <np> liste_parametres_formels

%%

program_main : 
	set_nbvariables_globales_to_0
	partie_declaration_variables_globales
	instruction_composee	{
 
	  nodeType *n0, *n1;
	  n0 = createOperatorNode(OPER_RESERVE_SPACE,0); 
	  n1 = createOperatorNode(OPER_SEQUENCE,2,n0,$3);
	  programRoot = n1;
	}
;

set_nbvariables_globales_to_0: 
		{ 
			reset_index_table_nbre_variables(0); 
			printf("nbVariablesGlobales=0\n");
		} 
	;

partie_declaration_variables_globales : set_type_declaration_globales liste_declaration_variables
	|
	;

set_type_declaration_globales : 
		{ 
			typeDeclaration=TYPE_VARIABLE_GLOBALE; 
			printf("typeDeclaration=TYPE_VARIABLE_GLOBALE\n");
		}
	;

liste_declaration_variables : liste_declaration_variables declaration_variable
	| declaration_variable
	;

declaration_variable : T_VAR liste_variables ';' 
	;

liste_variables : liste_variables ',' T_VARIABLE	{ declarer_variable(getFunctionNum(),$3,typeDeclaration); }
	| T_VARIABLE					{ declarer_variable(getFunctionNum(),$1,typeDeclaration); }
	;

acces_variable : T_VARIABLE 
		{ 
			int varloc=est_deja_declare(getFunctionNum(),$1);
			int varglob=est_deja_declare(0,$1);
			if (varloc!=-1)
				$$=createIdentifierNode($1,getFunctionNum(),varloc) ; 
			else if (varglob!=-1)
				$$=createIdentifierNode($1,0,varglob) ; 
			else
			{
				printf("variable %s non declaree\n",$1);
				exit(1);
			}
		}
	;

instruction_composee : T_BEGIN liste_instructions T_END { $$ = $2 ; }
	;

liste_instructions : liste_instructions instruction ';' { $$ = createOperatorNode(OPER_SEQUENCE,2,$1,$2); }
	| instruction ';'
	;

instruction : write_instruction
	| affectation
        | instruction_composee
	;

write_instruction : T_WRITE expr { $$ = createOperatorNode(OPER_WRITE,1,$2); }
	;

affectation : acces_variable '=' expr { $$ = createOperatorNode(OPER_ASSIGN,2,$1,$3) ; printf("OPER_ASSIGN=%p\n",$$) ; }
	;

expr: T_NUMERIC { $$ = createNumericNode($1); }
    	| expr '+' expr { $$ = createOperatorNode(OPER_ADD,2,$1,$3); }
        | expr '-' expr { $$ = createOperatorNode(OPER_SUB,2,$1,$3); }
        | expr '*' expr { $$ = createOperatorNode(OPER_MULT,2,$1,$3); }
        | expr '/' expr { $$ = createOperatorNode(OPER_DIV,2,$1,$3); }
        | '(' expr ')' { $$ = $2 ; }
        | expr '<' expr { $$ = createOperatorNode(OPER_INF,2,$1,$3); }
        | expr '>' expr { $$ = createOperatorNode(OPER_SUP,2,$1,$3); }
        | expr T_GE expr { $$ = createOperatorNode(OPER_GE,2,$1,$3); }
        | expr T_LE expr { $$ = createOperatorNode(OPER_LE,2,$1,$3); }
        | expr T_NE expr { $$ = createOperatorNode(OPER_NE,2,$1,$3); }
        | expr T_EQ expr { $$ = createOperatorNode(OPER_EQ,2,$1,$3); }
        | T_NOT expr %prec T_NNOT { $$ = createOperatorNode(OPER_NOT,1,$2); }
        | expr T_AND expr { $$ = createOperatorNode(OPER_AND,2,$1,$3); }
        | expr T_OR expr { $$ = createOperatorNode(OPER_OR,2,$1,$3); }
        | '-' expr %prec T_UMINUS { $$ = createOperatorNode(OPER_NEG,1,$2); }
	| acces_variable 
        ;
%%

void yyerror(char *s) {
    fprintf(stdout, "%s\n", s);
}

int main(void) {
	init_table_symboles();
    	yyparse();
	print_table_symb(0);
	print_table_fonctions();
	//printf("generateDigraph\n");
	//generateDigraph(programRoot);
	//printf("generateAsm\n");
	generateAsm(programRoot,"res.asm");
    	return 0;
}
