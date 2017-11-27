#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

// Cet enum contient tous les types possibles pour un noeud "operator"
typedef enum { OPER_ADD, OPER_SUB, OPER_MULT, OPER_DIV, OPER_WRITE, 
	       OPER_SKIP, OPER_IF, OPER_WHILE, OPER_SEQUENCE, OPER_APPEL_FONCTION,
	       OPER_INF, OPER_SUP, OPER_GE, OPER_LE, OPER_NE, OPER_EQ,
               OPER_NOT, OPER_AND, OPER_OR, OPER_NEG, OPER_ASSIGN,
	       OPER_DEF_FONCTION, OPER_RETURN,
	       OPER_RESERVE_SPACE, OPER_MAIN } nodeOper;

// Les deux types de noeuds possibles, "numeric" ou "operator"
typedef enum { typeNumeric, typeOperator, typeIdentifier } nodeEnum;

/* un noeud numeric (float) permet de stocker une valeur reelle */
typedef struct {
	float valeur;
} numericNodeType;

/* un noeud operator permet de stocker un operateur, le nombre d'operandes et les pointeurs vers les noeuds
   operande correspondants */
typedef struct {
	int oper;
	int nOperands;
	struct nodeTypeTag **op; 
} operatorNodeType;

/* un noeud identifier fait reference a un identificateur */
typedef struct {
	char *ident;
	int funcNum;
	int index;
} identifierNodeType;

/* Cette structure, fondamentale, permet de definir un noeud a geometrie variable. Le type
   permet de definir le type du noeud (numeric ou operator) et l'union permet, en fonction
   du type, d'acceder aux bonnes informations */

typedef struct nodeTypeTag {
	nodeEnum type;
	int digraphNode;		// Pour generer des beaux graphes avec dot
	union {
		numericNodeType t_numeric;
		operatorNodeType t_oper;
		identifierNodeType t_identifier;
	};
} nodeType;

// La, il faudrait mettre les extern de toutes les fonctions definies dans ast.c
extern nodeType *createNumericNode(float v);
extern nodeType *createIdentifierNode(char *id, int funcNum, int index);
extern nodeType *createOperatorNode(int oper, int nOperands, ...);
extern void generateAsmRec(nodeType *n, FILE *fout);
void generateAsmExpression(nodeType *n, FILE *fout);
extern void generateAsm(nodeType *n, char *filename);
extern void generateDigraphEdges(nodeType *n,FILE *fout);
extern void generateDigraph(nodeType *n);
