#include "ast.h"
#include "symboles.h"

// Cette variable globale vaut au depart 0, et est incrementee a chaque fois
int countDigraph;

// Cette variable permet de declarer des etiquettes a la volee
int currentLabel;

nodeType *createNumericNode(float v)
{
	nodeType *p;

	if ((p=(nodeType*)malloc(sizeof(nodeType))) == NULL)
	{
		printf("out of memory error\n");
		exit(1);
	}

	p->type=typeNumeric;
	p->t_numeric.valeur=v;
	
	return p;
}

nodeType *createOperatorNode(int oper, int nops, ...) 
{
    	va_list ap;
    	nodeType *p;
    	int i;

    	/* allocate node */
    	if ((p = (nodeType*)malloc(sizeof(nodeType))) == NULL)
	{
		printf("out of memory error\n");
		exit(1);
	}
    	if ((p->t_oper.op = (nodeType**)malloc(nops * sizeof(nodeType))) == NULL)
	{
		printf("out of memory error\n");
		exit(1);
	}

    	/* copy information */
    	p->type = typeOperator;
    	p->t_oper.oper = oper;
    	p->t_oper.nOperands = nops;
    	va_start(ap, nops);
    	for (i = 0; i < nops; i++)
        	p->t_oper.op[i] = va_arg(ap, nodeType*);
    	va_end(ap);
    	return p;
}

nodeType *createIdentifierNode(char *id, int funcNum, int index)
{
        nodeType *p;

        if ((p=malloc(sizeof(nodeType))) == NULL)
        {
                printf("out of memory error\n");
                exit(1);
        }

        p->type=typeIdentifier;
        p->t_identifier.ident=strdup(id);
        p->t_identifier.funcNum=funcNum;
        p->t_identifier.index=index;

        return p;
}

void generateAsmRec(nodeType *n, FILE *fout)
{
	int label1, label2;

	if (n==NULL)
		return;

}

void generateAsmExpression(nodeType *n, FILE *fout)
{
        if (n==NULL)
                return;

}

void generateAsm(nodeType *n, char *filename)
{
	FILE *fout;

	currentLabel=0;
	fout=fopen(filename,"w");
	generateAsmRec(n,fout);
	fprintf(fout,"\thalt\n");
	fprintf(fout,"\tend\n");
	fclose(fout);	
}

void generateDigraphNameNode(nodeType *n,FILE *fout)
{
/*
        if (n==NULL)
                return;

        switch (n->type)
        {
                case typeNumeric:
                        {
				n->digraphNode=countDigraph++;
                                fprintf(fout,"\tA%3.3d [label=\"%f\"]\n",n->digraphNode,n->t_numeric.valeur);
                        }
                        break;
                case typeIdentifier:
                        {
				n->digraphNode=countDigraph++;
                                fprintf(fout,"\tA%3.3d [label=\"%s\"]\n",n->digraphNode,n->t_identifier.ident);
                        }
                        break;
                case typeOperator:
                        {
                                switch (n->t_oper.oper)
                                {
                                        case OPER_APPEL_FONCTION:
                                                generateDigraphNameNode(n->t_oper.op[0],fout);
                                                generateDigraphNameNode(n->t_oper.op[1],fout);
                                                n->digraphNode=countDigraph++;
                                                fprintf(fout,"\tA%3.3d [label=\"appel fonc\"]\n",n->digraphNode);
                                                break;
                                        case OPER_ADD:
                                                generateDigraphNameNode(n->t_oper.op[0],fout);
                                                generateDigraphNameNode(n->t_oper.op[1],fout);
                                                n->digraphNode=countDigraph++;
                                                fprintf(fout,"\tA%3.3d [label=\"+\"]\n",n->digraphNode);
                                                break;
                                        case OPER_SUB:
                                                generateDigraphNameNode(n->t_oper.op[0],fout);
                                                generateDigraphNameNode(n->t_oper.op[1],fout);
                                                n->digraphNode=countDigraph++;
                                                fprintf(fout,"\tA%3.3d [label=\"-\"]\n",n->digraphNode);
                                                break;
                                        case OPER_MULT:
                                                generateDigraphNameNode(n->t_oper.op[0],fout);
                                                generateDigraphNameNode(n->t_oper.op[1],fout);
                                                n->digraphNode=countDigraph++;
                                                fprintf(fout,"\tA%3.3d [label=\"*\"]\n",n->digraphNode);
                                                break;
                                        case OPER_DIV:
                                                generateDigraphNameNode(n->t_oper.op[0],fout);
                                                generateDigraphNameNode(n->t_oper.op[1],fout);
                                                n->digraphNode=countDigraph++;
                                                fprintf(fout,"\tA%3.3d [label=\"/\"]\n",n->digraphNode);
                                                break;
                                        case OPER_INF:
                                                generateDigraphNameNode(n->t_oper.op[0],fout);
                                                generateDigraphNameNode(n->t_oper.op[1],fout);
                                                n->digraphNode=countDigraph++;
                                                fprintf(fout,"\tA%3.3d [label=\"<\"]\n",n->digraphNode);
                                                break;
                                        case OPER_SUP:
                                                generateDigraphNameNode(n->t_oper.op[0],fout);
                                                generateDigraphNameNode(n->t_oper.op[1],fout);
                                                n->digraphNode=countDigraph++;
                                                fprintf(fout,"\tA%3.3d [label=\">\"]\n",n->digraphNode);
                                                break;
                                        case OPER_EQ:
                                                generateDigraphNameNode(n->t_oper.op[0],fout);
                                                generateDigraphNameNode(n->t_oper.op[1],fout);
                                                n->digraphNode=countDigraph++;
                                                fprintf(fout,"\tA%3.3d [label=\"==\"]\n",n->digraphNode);
                                                break;
                                        case OPER_NE:
                                                generateDigraphNameNode(n->t_oper.op[0],fout);
                                                generateDigraphNameNode(n->t_oper.op[1],fout);
                                                n->digraphNode=countDigraph++;
                                                fprintf(fout,"\tA%3.3d [label=\"!=\"]\n",n->digraphNode);
                                                break;
                                        case OPER_NOT:
                                                generateDigraphNameNode(n->t_oper.op[0],fout);
                                                n->digraphNode=countDigraph++;
                                                fprintf(fout,"\tA%3.3d [label=\"not\"]\n",n->digraphNode);
                                                break;
                                        case OPER_SKIP:
						n->digraphNode=countDigraph++;
                                		fprintf(fout,"\tA%3.3d [label=\"skip\"]\n",n->digraphNode);
                                                break;
                                        case OPER_RETURN:
                                                generateDigraphNameNode(n->t_oper.op[0],fout);
                                                generateDigraphNameNode(n->t_oper.op[1],fout);
						n->digraphNode=countDigraph++;
                                		fprintf(fout,"\tA%3.3d [label=\"return\"]\n",n->digraphNode);
                                                break;
                                        case OPER_WRITE:
                                                generateDigraphNameNode(n->t_oper.op[0],fout);
						n->digraphNode=countDigraph++;
                                		fprintf(fout,"\tA%3.3d [label=\"write\"]\n",n->digraphNode);
                                                break;
                                        case OPER_DEF_FONCTION:
                                                generateDigraphNameNode(n->t_oper.op[0],fout);
                                                generateDigraphNameNode(n->t_oper.op[1],fout);
						n->digraphNode=countDigraph++;
                                		fprintf(fout,"\tA%3.3d [label=\"def_fonction\"]\n",n->digraphNode);
                                                break;
                                        case OPER_ASSIGN:
                                                generateDigraphNameNode(n->t_oper.op[0],fout);
                                                generateDigraphNameNode(n->t_oper.op[1],fout);
						n->digraphNode=countDigraph++;
                                		fprintf(fout,"\tA%3.3d [label=\"=\"]\n",n->digraphNode);
                                                break;
                                        case OPER_WHILE:
                                                generateDigraphNameNode(n->t_oper.op[0],fout);
                                                generateDigraphNameNode(n->t_oper.op[1],fout);
						n->digraphNode=countDigraph++;
                                		fprintf(fout,"\tA%3.3d [label=\"while\"]\n",n->digraphNode);
                                                break;
                                        case OPER_IF:
                                                generateDigraphNameNode(n->t_oper.op[0],fout);
                                                generateDigraphNameNode(n->t_oper.op[1],fout);
						if (n->t_oper.nOperands==3)
                                                	generateDigraphNameNode(n->t_oper.op[2],fout);
						n->digraphNode=countDigraph++;
                                		fprintf(fout,"\tA%3.3d [label=\"if\"]\n",n->digraphNode);
                                                break;
                                        case OPER_SEQUENCE:
                                                generateDigraphNameNode(n->t_oper.op[0],fout);
                                                generateDigraphNameNode(n->t_oper.op[1],fout);
						n->digraphNode=countDigraph++;
                                		fprintf(fout,"\tA%3.3d [label=\";\"]\n",n->digraphNode);
                                                break;
                                        case OPER_RESERVE_SPACE:
						n->digraphNode=countDigraph++;
                                		fprintf(fout,"\tA%3.3d [label=\"reserve_space\"]\n",n->digraphNode);
                                                break;
                                        case OPER_MAIN:
                                                generateDigraphNameNode(n->t_oper.op[0],fout);
						n->digraphNode=countDigraph++;
                                		fprintf(fout,"\tA%3.3d [label=\"main\"]\n",n->digraphNode);
                                                break;
                                        default:
                                                break;
                                }
                        }
                        break;
        }
*/
}

void generateDigraphEdges(nodeType *n,FILE *fout)
{
        if (n==NULL)
                return;

        switch (n->type)
        {
                case typeNumeric:
                case typeIdentifier:
                        break;
                case typeOperator:
                        {
				//printf("oper=%d\n",n->t_oper.oper);
                                switch (n->t_oper.oper)
                                {
                                        case OPER_ADD:
                                        case OPER_SUB:
                                        case OPER_MULT:
                                        case OPER_DIV:
                                        case OPER_INF:
                                        case OPER_SUP:
                                        case OPER_EQ:
                                        case OPER_NE:
                                        case OPER_WHILE:
                                        case OPER_APPEL_FONCTION:
                                        case OPER_DEF_FONCTION:
                                        case OPER_ASSIGN:
                                        case OPER_SEQUENCE:
                                        case OPER_RETURN:
                                                fprintf(fout,"\tA%3.3d -> A%3.3d\n", n->digraphNode,n->t_oper.op[0]->digraphNode);
                                                fprintf(fout,"\tA%3.3d -> A%3.3d\n", n->digraphNode,n->t_oper.op[1]->digraphNode);
                                                generateDigraphEdges(n->t_oper.op[0],fout);
                                                generateDigraphEdges(n->t_oper.op[1],fout);
                                                break;
                                        case OPER_IF:
                                                fprintf(fout,"\tA%3.3d -> A%3.3d\n", n->digraphNode,n->t_oper.op[0]->digraphNode);
                                                fprintf(fout,"\tA%3.3d -> A%3.3d\n", n->digraphNode,n->t_oper.op[1]->digraphNode);
						if (n->t_oper.nOperands==3)
                                                	fprintf(fout,"\tA%3.3d -> A%3.3d\n", n->digraphNode,n->t_oper.op[2]->digraphNode);
                                                generateDigraphEdges(n->t_oper.op[0],fout);
                                                generateDigraphEdges(n->t_oper.op[1],fout);
						if (n->t_oper.nOperands==3)
                                                	generateDigraphEdges(n->t_oper.op[2],fout);
                                                break;
                                        case OPER_WRITE:
                                        case OPER_NOT:
					case OPER_MAIN:
                                                fprintf(fout,"\tA%3.3d -> A%3.3d\n", n->digraphNode,n->t_oper.op[0]->digraphNode);
                                                generateDigraphEdges(n->t_oper.op[0],fout);
                                                break;
					case OPER_SKIP:
					case OPER_RESERVE_SPACE:
						break;
                                        default:
                                                break;
                                }
                        }
                        break;
        }
}

void generateDigraph(nodeType *n)
{
/*
	FILE *fout;

	fout=fopen("res.dot","w");
	countDigraph=0;
	fprintf(fout,"digraph {\n");
	printf("generateDigraphNameNode\n");
	generateDigraphNameNode(n,fout);
	printf("generateDigraphEdges\n");
	generateDigraphEdges(n,fout);
	fprintf(fout,"}\n");
	fclose(fout);
	system("dot -Tpng res.dot -o res.png");
*/
}
