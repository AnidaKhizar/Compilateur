%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "symboles.h"
#include "ast.h"

int yylex(void);
//void yyerror(char *s);

nodeType *programRoot;

int typeDeclaration;

%}

%locations

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

%token T_AND T_NUMERIC T_BEGIN T_END T_ELSE T_FUNCTION
%token T_NOT T_OR T_IF T_THEN T_DO T_WHILE T_RETURN
%token T_VAR T_VARIABLE T_WRITE T_PROGRAM T_FOR

%type <f> T_NUMERIC
%type <s> T_VARIABLE 
%type <np> program_main
%type <np> bloc_principal 
%type <np> partie_definition_fonctions
%type <np> liste_definition_fonctions
%type <np> definition_fonction
%type <np> acces_variable 
%type <np> bloc_fonction 
%type <np> instruction_composee 
%type <np> liste_instructions 
%type <np> instruction
%type <np> if_instruction 
%type <np> while_instruction 
%type <np> do_while_instruction
%type <np> for_instruction
%type <np> write_instruction 
%type <np> return_instruction 
%type <np> appel_fonction 
%type <np> affectation 
%type <np> expr 
%type <np> liste_parametres_effectifs
%type <np> prototype_fonction 

 //%type <np> liste_parametres_formels

%%

program_main : T_PROGRAM T_VARIABLE ';' bloc_principal '.' {programRoot = $4;} 
;

/* A modifier: le bloc_principal ne doit pas modifier le programRoot car programRoot a deja été affecté a un noeud dans program_main */
bloc_principal:
 	set_nbvariables_globales_to_0
	partie_declaration_variables_globales
	partie_definition_fonctions
	instruction_composee	{
	  nodeType *n0, *n1, *n2;
	  n0 = createOperatorNode(OPER_RESERVE_SPACE,0); 
	  if ($3!=NULL)
	    {
	      n2 = createOperatorNode(OPER_MAIN,1,$4);
	      n1 = createOperatorNode(OPER_SEQUENCE,2,$3,n2);
	      $$ = createOperatorNode(OPER_SEQUENCE,2,n0,n1); 
	    }
	  else
	    {
	      n2 = createOperatorNode(OPER_MAIN,1,$4);
	      $$ = createOperatorNode(OPER_SEQUENCE,2,n0,n2); 
	    }
	}
;

set_nbvariables_globales_to_0: 
		{ 
			reset_index_table_nbre_variables(0); 
			printf("nbVariablesGlobales=0\n");
		} 
	;

partie_definition_fonctions : liste_definition_fonctions
        |	{ $$ = NULL ; }
        ;


set_type_declaration_formels : 
		{
			typeDeclaration=TYPE_PARAMETRE; 
			printf("typeDeclaration=TYPE_PARAMETRE\n");
		}
	;

set_type_declaration_locales : 
		{
			typeDeclaration=TYPE_VARIABLE_LOCALE; 
			printf("typeDeclaration=TYPE_VARIABLE_LOCALE\n");
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

liste_definition_fonctions : liste_definition_fonctions definition_fonction { $$ = createOperatorNode(OPER_SEQUENCE,2,$1,$2); }
        | definition_fonction
        ;

definition_fonction : prototype_fonction '(' liste_parametres_formels ')' ';' bloc_fonction ';' 
		{ 
			$$ = createOperatorNode(OPER_DEF_FONCTION,2,$1,$6) ;
			print_current_table_symb();
		}
        ;

prototype_fonction: T_FUNCTION T_VARIABLE 
		{ 
			declarer_variable(0,$2,TYPE_FONCTION);
			/* on recupere l'indice de la fonction que l'on vient de declarer */
			int index=est_deja_declare(0,$2);
			printf("index=%d\n",index);

			/* on incremente le pointeur de fonction */
			increment_current_function();
			printf("getFunctionNum()=%d\n",getFunctionNum());
			ajouter_table_fonctions(index,getFunctionNum());
			
		  	nodeType *nodeIndex=createNumericNode((float)index);
		  	nodeType *nodeFunctionNum=createNumericNode((float)getFunctionNum());
		  	$$ = createOperatorNode(OPER_SEQUENCE,2,nodeIndex,nodeFunctionNum); 
	        }
	;

liste_parametres_formels : set_type_declaration_formels liste_variables
        |
        ;

bloc_fonction : partie_declaration_variables_locales instruction_composee { $$ = $2 ; }
        ;

partie_declaration_variables_locales : set_type_declaration_locales liste_declaration_variables
        |
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
        | if_instruction
        | while_instruction
        | appel_fonction
        | return_instruction
        | do_while_instruction
        | for_instruction
	;

for_instruction: T_FOR '(' affectation ';' expr ';' affectation ')' instruction { $$ = createOperatorNode(OPER_FOR, 4, $3, $5, $7, $9);} 
        ;

do_while_instruction : T_DO instruction T_WHILE expr { $$=createOperatorNode(OPER_DO_WHILE,2,$4,$2); }
	; 

if_instruction : T_IF expr T_THEN instruction %prec IFX { $$=createOperatorNode(OPER_IF,2,$2,$4); }
        | T_IF expr T_THEN instruction T_ELSE instruction { $$=createOperatorNode(OPER_IF,3,$2,$4,$6) ; }
        ;

return_instruction : T_RETURN expr 
		{ 
			nodeType *n0=createIdentifierNode("RET",getFunctionNum(),0) ; 
			$$ = createOperatorNode(OPER_RETURN,2,n0,$2); 
		}
	;

while_instruction : T_WHILE expr T_DO instruction { $$=createOperatorNode(OPER_WHILE,2,$2,$4); }
	;

write_instruction : T_WRITE expr { $$ = createOperatorNode(OPER_WRITE,1,$2); }
	;

affectation : acces_variable '=' expr { $$ = createOperatorNode(OPER_ASSIGN,2,$1,$3) ; printf("OPER_ASSIGN=%p\n",$$) ; }
	;

appel_fonction : T_VARIABLE '(' liste_parametres_effectifs ')' 
		{ 
			int varglob=est_deja_declare(0,$1);
			printf("appel_fonction varglob=%d\n",varglob);
			if (varglob!=-1) /* si fonction existe, on cree un noeud qui s'appelle OPER_APPEL_FONCTION */
				$$ = createOperatorNode(OPER_APPEL_FONCTION,2,createIdentifierNode($1,0,varglob),$3) ; 
			else
			{
				printf("fonction %s inconnue\n",$1);
				exit(1);
			}
		} 
        ;

liste_parametres_effectifs : liste_parametres_effectifs ',' expr 
				{ $$ = createOperatorNode(OPER_SEQUENCE,2,$1,$3) ; }
        | expr
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
        | appel_fonction
        ;
%%

/* 
void yyerror(char *s) { 
   fprintf(stdout, "%s\n", s); 
 } 
*/


int main(void) {
	init_table_symboles();
    	yyparse();
	print_table_symb(0);
	print_table_fonctions();
	//printf("generateDigraph\n");
	generateDigraph(programRoot);
	//printf("generateAsm\n");
	generateAsm(programRoot,"res.asm");
    	return 0;
}
