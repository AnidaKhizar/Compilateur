#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NBRE_IDENT_MAX 30
#define NBRE_FONCTIONS_MAX 10

#define TYPE_VARIABLE_GLOBALE 0
#define TYPE_VARIABLE_LOCALE 1
#define TYPE_PARAMETRE 2
#define TYPE_FONCTION 3

typedef struct
{
	char *ident;
	int typv;
	int funcNum;
} type_variable;

extern type_variable table_ident_fonctions[NBRE_FONCTIONS_MAX][NBRE_IDENT_MAX];
extern int table_nbre_variables[NBRE_FONCTIONS_MAX];
extern int table_nbre_variables_globales[NBRE_FONCTIONS_MAX];
extern int table_nbre_variables_locales[NBRE_FONCTIONS_MAX];
extern int table_nbre_formels[NBRE_FONCTIONS_MAX];
extern char* table_noms_fonctions[NBRE_FONCTIONS_MAX];
extern int currentFunction;

void reset_index_table_nbre_variables(int func);
extern void init_table_symboles();
extern int est_deja_declare(int func, char *ident);
extern void print_table_symb(int func);
extern void ajouter_variable(int func, char *ident, int typv);
extern void declarer_variable(int func,char *ident, int typv);
extern void increment_current_function();
//extern void declarer_fonction(char *ident, int typv);
void print_current_table_symb();
extern int getFunctionNum();
void ajouter_table_fonctions(int index,int numFunc);
void print_table_fonctions();
