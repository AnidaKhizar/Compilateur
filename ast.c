#include "ast.h"
#include "symboles.h"
//void generateDigraphNameNode(nodeType *n,FILE *fout);
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
int level=0;
int num_func=table_nbre_variables_globales[0];
void generateAsmRec(nodeType *n, FILE *fout, char* nom)
{
    int label1, label2; // variables locales de la fonction recursive
    char tmpstr[256];
    sprintf(tmpstr,"%s_%d",nom,level++);
    nom=tmpstr;
	if (n==NULL)
		return;

	//printf("ENFANT DU NOEUD: %s\n", nom);
	switch(n->type)
	  {
	  case typeNumeric:
	    {
	      fprintf(fout, "\tpushr %f\n", n -> t_numeric.valeur);
	      break;
	    }
	  case typeOperator:
	    {
            switch (n -> t_oper.oper)
            {
                    case OPER_ADD:
                      {
                        //printf("DANS OPER_ADD\n");
                        generateAsmRec(n -> t_oper.op[0],fout, "OPER_ADD gauche");
                        generateAsmRec(n -> t_oper.op[1],fout, "OPER_ADD droite");
                        fprintf(fout, "\tadd\n");

                        break;
                      }

                    case OPER_SUB:
                      {
                        generateAsmRec(n -> t_oper.op[0],fout, "OPER_SUB gauche");
                        generateAsmRec(n -> t_oper.op[1],fout, "OPER_SUB droite");

                        fprintf(fout, "\tsub\n");

                        break;
                      }

                    case OPER_MULT:
                      {
                        generateAsmRec(n -> t_oper.op[0],fout, "OPER_MULT gauche");
                        generateAsmRec(n -> t_oper.op[1],fout, "OPER_MULT droite");

                        fprintf(fout, "\tmult\n");

                        break;
                      }

                    case OPER_DIV:
                      {
                        int i;
                        for(i=0; i < n -> t_oper.nOperands; i++)
                          {
                        generateAsmRec(n -> t_oper.op[i],fout, "");
                          }

                        fprintf(fout, "\tdiv\n");

                        break;
                      }

                    case OPER_WRITE:
                      {
                        //printf("DANS WRITE\n");
                        generateAsmRec(n -> t_oper.op[0],fout, "OPER_WRITE");
                        fprintf(fout, "\toutput\n");

                        break;
                      }


                    case OPER_ASSIGN:
                      {
                        //printf("DANS OPER_ASSIGN\n");
                        fprintf(fout, "\tpush %d\n", n -> t_oper.op[0] -> t_identifier.index);
                        generateAsmRec(n -> t_oper.op[1],fout, "OPER_ASSIGN droite");
                        fprintf(fout, "\tstm\n");

                        break;
                      }
                    case OPER_SEQUENCE:
                      {
                        //printf("DANS OPER_SEQUENCE: %d\n", n -> t_oper.nOperands);
                        generateAsmRec(n -> t_oper.op[0],fout, "OPER_SEQUENCE gauche");
                        generateAsmRec(n -> t_oper.op[1],fout, "OPER_SEQUENCE droite");

                        break;
                      }

                    case OPER_RESERVE_SPACE:
                      {
                        //printf("DANS RESERVE_SPACE\n");
                        break;
                      }
                    case OPER_MAIN:
                      {
                        //printf("DANS MAIN: %d\n", n-> t_oper.nOperands);
                        fprintf(fout, "main : ");
                        generateAsmRec( n -> t_oper.op[0], fout, "MAIN");
                        break;
                      }
                      case OPER_DEF_FONCTION:
                      {
                        //printf("DANS MAIN: %d\n", n-> t_oper.nOperands);
                        fprintf(fout, "%s : ",table_ident_fonctions[0][++num_func].ident);
                        generateAsmRec( n -> t_oper.op[0], fout, "Func1");
                        generateAsmRec( n -> t_oper.op[1], fout, "Func1");
                        break;
                      }
                    case OPER_APPEL_FONCTION:
                      {
                        break;
                      }

                    case OPER_INF:
                      {
                        //printf("DANS OPER_INF: %d\n", n -> t_oper.nOperands);
//                        if (n -> t_oper.nOperands < 2)
//                        {
//                            printf("\tParent INF %s\n",nom);
//                            FILE *fout2;
//
//                            fout2=fopen("res2.dot","w");
//                            countDigraph=0;
//                            fprintf(fout2,"digraph {\n");
//                            printf("generateDigraphNameNode\n");
//                            generateDigraphNameNode(n,fout2);
//                            printf("generateDigraphEdges\n");
//                            generateDigraphEdges(n,fout2);
//                            fprintf(fout2,"}\n");
//                            fclose(fout2);
//                            system("dot -Tpng res2.dot -o res2.png");
//                        }
                        //exit(1);
                        generateAsmRec(n -> t_oper.op[0],fout, "OPER_INF gauche");
                        generateAsmRec(n -> t_oper.op[1],fout, "OPER_INF droite");
                        fprintf(fout, "\tls\n");
                        break;
                      }
                    case OPER_SUP:
                      {
                        //printf("DANS OPER_SUP\n");
                        generateAsmRec(n -> t_oper.op[0],fout, "OPER_SUP gauche");
                        generateAsmRec(n -> t_oper.op[1],fout, "OPER_SUP droite");
                        fprintf(fout, "\tgt\n");
                        break;
                      }
                    case OPER_EQ:
                      {
                        int i;
                        for(i=0; i < n -> t_oper.nOperands; i++)
                          {
                            generateAsmRec(n -> t_oper.op[i],fout, "OPER_EQ");
                          }
                        fprintf(fout, "\teq\n");
                        break;
                      }
                    case OPER_NE:
                      {
                        int i;
                        for(i=0; i < n -> t_oper.nOperands; i++)
                          {
                        generateAsmRec(n -> t_oper.op[i],fout,"");
                          }
                        fprintf(fout, "\tnot\n");
                        break;
                      }

                    case OPER_IF:
                      {
                        label1 = currentLabel;
                        generateAsmRec(n -> t_oper.op[0],fout, "OPER_IF gauche");
                        fprintf(fout, "\tjf L%.3d\n",currentLabel++);
                        generateAsmRec(n -> t_oper.op[1],fout, "OPER_IF milieu");
                        label2 = currentLabel;
                        if (n -> t_oper.op[2])
                          fprintf(fout, "\tjp L%.3d\n",currentLabel++);
                        fprintf(fout, "L%.3d : ",label1);
                        generateAsmRec(n -> t_oper.op[2],fout, "OPER_IF droite");
                        if (n -> t_oper.op[2])
                          fprintf(fout, "L%.3d : ",label2);
                        break;
                      }

                    case OPER_WHILE:
                      {
                        //printf("DANS BOUCLE WHILE\n");
                        label1 = currentLabel;
                        fprintf(fout, "L%.3d : ",currentLabel++);
                        generateAsmRec(n -> t_oper.op[0],fout, "OPER_WHILE gauche");
                        label2 = currentLabel;
                        fprintf(fout, "\tjf L%.3d\n",currentLabel++);
                        generateAsmRec(n -> t_oper.op[1],fout, "OPER_WHILE droite");
                        fprintf(fout, "\tjp L%.3d\n",label1);
                        fprintf(fout, "L%.3d : ", label2);
                        break;
                      }

                    default:
                      {
                        printf("operation: %d\n", n -> t_oper.oper );
                        printf("OPERATION INCONNUE!\n");
                        exit(1);
                      }
                    }
	      break;
	    }
	  case typeIdentifier:
	    {
	      fprintf(fout, "\tpush %d\n", n -> t_identifier.index);
	      fprintf(fout,"\tmts\n");
	      break;
	    }
	  default:
	    {
	      printf("ERREUR INCONNUE!\n");
	      exit(1);
	    }


	  }

	//printf("FIN DU NOEUD %s\n \n", nom);
	level--;
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
	fprintf(fout, "\tinc %d\n", table_nbre_variables_globales[0]);
	fprintf(fout, "\tjp main\n");
	generateAsmRec(n,fout, "");
	fprintf(fout,"\thalt\n");
	fprintf(fout,"\tend\n");
	fclose(fout);
}

void generateDigraphNameNode(nodeType *n,FILE *fout)
{

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

}
