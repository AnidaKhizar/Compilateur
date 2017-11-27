#include "symboles.h" 

// La table des symboles est organisee comme une matrice de taille NBRE_FONCTIONS_MAX (pour les colonnes)
// NBRE_IDENT_MAX (pour les lignes). Les colonnes representent les fonctions. La colonne 0 va donc
// contenir les variables globales, et les identifiants de fonctions.
//
// En plus de cette matrice, il y a 5 tableaux, egalement indices par un numero de colonne.
// Le tableau table_nbre_variables dit globalement combien il y a d'identificateurs declares
// dans la fonction, quel que soit leur type.

// table_fonctions est aussi indice par un numero de colonne. cette table sert a retrouver dans
// la colonne 0 (variables globales) l'index d'une fonction.

/*
        #                         #
        ###########################
        # Adresse de retour       #
        ###########################
        # variable locale nl-1    # <- BP+nl-1
        ###########################
        # ...                     #             nl variables locales
        ###########################
        # variable locale 0       # <- BP+0 ---------------- la position de reference pour cet appel de fonction
        ###########################
        # ancien BP               # <- BP-1
        ###########################
        # parametre formel nf-1   # <- BP-2
        ###########################
        # ...                     #             nf parametres formels
        ###########################
        # parametre formel     0  # <- BP-1-nf
        ###########################
        # valeur de retour        # <- BP-2-nf
        ###########################

        1) la fct appelante reserve de 1 place pour la valeur de retour

        # valeur de retour        # <- BP-2-nf
        ###########################

        2) la fct appelante empile les valeurs des parametres effectifs correspondant aux parametres formels
                le premier parametre effecif sera mis juste au dessus de la valeur de retour. nf valeurs
                seront empilees

        # parametre formel nf-1   # <- BP-2
        ###########################
        # ...                     #             nf parametres formels
        ###########################
        # parametre formel     0  # <- BP-1-nf
        ###########################
        # valeur de retour        # <- BP-2-nf
        ###########################

        3) la fct appelante sauve son propre bp dans la pile avec SAVEBP

        #                         # <- BP est mis a jour=SP+1  BP=la position de reference pour cet appel de fonction
        ###########################
        # ancien BP               # <- BP-1
        ###########################
        # parametre formel nf-1   # <- BP-2
        ###########################
        # ...                     #             nf parametres formels
        ###########################
        # parametre formel     0  # <- BP-1-nf
        ###########################
        # valeur de retour        # <- BP-2-nf
        ###########################

        4) la fct appelee, la seule qui connait son nombre de variables locales, reserve les places
                correspondantes dans la pile (nl places a reserver)

        # variable locale nl-1    # <- BP+nl-1
        ###########################
        # ...                     #             nl variables locales
        ###########################
        # variable locale 0       # <- BP+0 ---------------- la position de reference pour cet appel de fonction
        ###########################
        # ancien BP               # <- BP-1
        ###########################
        # parametre formel nf-1   # <- BP-2
        ###########################
        # ...                     #             nf parametres formels
        ###########################
        # parametre formel     0  # <- BP-1-nf
        ###########################
        # valeur de retour        # <- BP-2-nf
        ###########################

        5) la fonction appelante appelle la fonction appelee, ce qui se traduit par l'empilage de l'adresse de retour
                correspondant a la case memoire programme suivant le call.

        #                         #
        ###########################
        # Adresse de retour       #
        ###########################
        # variable locale nl-1    # <- BP+nl-1
        ###########################
        # ...                     #             nl variables locales
        ###########################
        # variable locale 0       # <- BP+0 ---------------- la position de reference pour cet appel de fonction
        ###########################
        # ancien BP               # <- BP-1
        ###########################
        # parametre formel nf-1   # <- BP-2
        ###########################
        # ...                     #             nf parametres formels
        ###########################
        # parametre formel     0  # <- BP-1-nf
        ###########################
        # valeur de retour        # <- BP-2-nf
        ###########################
*/

int currentFunction;
type_variable table_ident_fonctions[NBRE_FONCTIONS_MAX][NBRE_IDENT_MAX];
int table_nbre_variables[NBRE_FONCTIONS_MAX];
int table_fonctions[NBRE_FONCTIONS_MAX];
int table_nbre_variables_globales[NBRE_FONCTIONS_MAX];
int table_nbre_variables_locales[NBRE_FONCTIONS_MAX];
int table_nbre_formels[NBRE_FONCTIONS_MAX];

void reset_index_table_nbre_variables(int func)
{
	table_nbre_variables[func]=0;
}

void init_table_symboles()
{
	int i;

	printf("init_table_symboles\n");
	currentFunction=0;
	for (i=0;i<NBRE_FONCTIONS_MAX;i++)
	{
		table_nbre_variables[i]=0;
		table_fonctions[i]=-1;
		table_nbre_variables_globales[i]=0;
		table_nbre_variables_locales[i]=0;
		table_nbre_formels[i]=0;
	}
}

int est_deja_declare(int func, char *ident)
{
	int i;

	for (i=0;i<table_nbre_variables[func];i++)
		if (strcmp(ident,table_ident_fonctions[func][i].ident)==0)
			return i;
	return -1;
}

void print_current_table_symb()
{
	print_table_symb(currentFunction);
}

void print_table_symb(int func)
{
	int i;

	printf("------ DEBUT ------- %d\n",func);
	for (i=0;i<table_nbre_variables[func];i++)
		printf("%d: %s %d %d\n", i, table_ident_fonctions[func][i].ident,
			table_ident_fonctions[func][i].typv, table_ident_fonctions[func][i].funcNum);
	printf("nbLocales=%d\n",table_nbre_variables_locales[func]);
	printf("nbFormels=%d\n",table_nbre_formels[func]);
	printf("------ FIN ---------\n");
}

void print_table_fonctions()
{
	int i;

	for (i=0;i<=currentFunction;i++)
		printf("%d: %d\n",i,table_fonctions[i]);
}

void ajouter_variable(int func, char *ident, int typv)
{
	int index=table_nbre_variables[func];
	table_ident_fonctions[func][index].ident=strdup(ident);
	table_ident_fonctions[func][index].typv=typv;
	table_ident_fonctions[func][index].funcNum=-1;
	printf("on ajoute la variable %s,%d,%d a la fonction %d\n",
		table_ident_fonctions[func][index].ident,
		table_ident_fonctions[func][index].typv,
		table_ident_fonctions[func][index].funcNum,
		func);

	table_nbre_variables[func]++;
	if (typv==TYPE_VARIABLE_GLOBALE)
		table_nbre_variables_globales[func]++;
	else if (typv==TYPE_VARIABLE_LOCALE)
		table_nbre_variables_locales[func]++;
	else if (typv==TYPE_PARAMETRE)
		table_nbre_formels[func]++;
	
	//print_table_symb(currentFunction);
}

void ajouter_table_fonctions(int index,int funcNum)
{
	table_fonctions[currentFunction]=index;
	table_ident_fonctions[0][index].funcNum=funcNum;
}

void declarer_variable(int func, char *ident, int typv)
{
	if (est_deja_declare(func,ident)!=-1)
	{
		printf("identificateur %s deja declare\n",ident);
		exit(1);
	}
	ajouter_variable(func, ident, typv);
}

/*
void declarer_fonction(char *ident, int typv)
{
	if (est_deja_declare(0,ident)!=-1)
	{
		printf("fonction %s deja declaree\n",ident);
		exit(1);
	}
	ajouter_variable(0, ident, typv);
}
*/

void increment_current_function()
{
	currentFunction++;
}

int getFunctionNum()
{
	return currentFunction;
}
