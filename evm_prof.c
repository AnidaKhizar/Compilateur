#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "vm_codops.h"

#define MAX_CODESEGMENT_SIZE 1000
#define MAX_STACK_SIZE 100

// Q1 : Les registres de la machine virtuelle

int pc;
int sp;
int bp;

// Q2 : Declaration du segment de code

int codeSegment[MAX_CODESEGMENT_SIZE];

// Q3 : Declaration de la pile d'execution (tableau de float)

float pile[MAX_STACK_SIZE];

// Q4 : Fonction de lecture d'un fichier langage machine
//      ouvert et dont le pointeur de fichier est fin

void readAssembly(FILE *fin)
{
	int nbinst;
	char line[100];
	int i;
	int pc,v;

	fgets(line,100,fin);
	sscanf(line,"%d",&nbinst);
	for (i=0;i<nbinst;i++)
	{
		fgets(line,100,fin);
		sscanf(line,"%d:%d",&pc,&v);
		codeSegment[i]=v;
	}
}

// Q5 : Fonction de debug qui affiche la pile jusqu'a son sommet

void printPile()
{
	int i;

	printf("sp=%d\n",sp);
	for (i=0;i<=20;i++)
		printf("%d:%f ",i,pile[i]);
	printf("\n");
}

// Q6 : Boucle principale d'exécution de la machine virtuelle
//      On sort de cette fonction si l'instruction pointee
//      par pc est OP_HALT. On traitera les erreurs et on
//	pensera a ajouter des traces et des moyens de mise
//	au point

void run()
{
	float fv;
	int /*vm1,*/v;
    	int i/*,depile*/,n,orig,dest;
	char strReal[256];
	int strRealLength,pos;

	while (codeSegment[pc]!=OP_HALT)
	{
		/*
		printPile();
		printf("PC=%d BP=%d ",pc,bp);
		printf("%d \n",codeSegment[pc]);
		getchar();
		*/
		switch (codeSegment[pc])
		{
			case OP_ADD:
				pile[sp-1]=pile[sp-1]+pile[sp];
				sp-- ; pc++ ; break;
			case OP_SUB:
				pile[sp-1]=pile[sp-1]-pile[sp];
				sp-- ; pc++ ; break;
			case OP_MULT:
				pile[sp-1]=pile[sp-1]*pile[sp];
				sp-- ; pc++ ; break;
			case OP_DIV:
				pile[sp-1]=pile[sp-1]/pile[sp];
				sp-- ; pc++ ; break;
			case OP_DIVI:
				pile[sp-1]=(float)((int)pile[sp-1]/(int)pile[sp]);
				sp-- ; pc++ ; break;
			case OP_NEG:
				pile[sp]=-pile[sp];
				pc++ ; break;
			case OP_AND:
				pile[sp-1]=(float)((int)pile[sp-1] & (int)pile[sp]);
				sp-- ; pc++ ; break;
			case OP_OR:
				pile[sp-1]=(float)((int)pile[sp-1] | (int)pile[sp]);
				sp-- ; pc++ ; break;
			case OP_NOT:
			        //pile[sp]=(float)(1-(int)pile[sp]);
				pile[sp-1]=(pile[sp-1] != pile[sp]) ? 1.0 : 0.0;
			        sp--; pc++ ; break;
			case OP_EQ:
				pile[sp-1]=(pile[sp-1]==pile[sp]) ? 1.0 : 0.0;
				sp-- ; pc++ ; break;
			case OP_LS:
				//printf("av LS=%f %f\n",pile[sp-1],pile[sp]);
				pile[sp-1]=(pile[sp-1]<pile[sp]) ? 1.0 : 0.0;
				//printf("ap LS=%f\n",pile[sp-1]);
				sp-- ; pc++ ; break;
			case OP_GT:
				pile[sp-1]=(pile[sp-1]>pile[sp]) ? 1.0 : 0.0;
				sp-- ; pc++ ; break;
			case OP_INC:
				sp=sp+codeSegment[pc+1];
				pc+=2 ; break;
			case OP_DEC:
				sp=sp-codeSegment[pc+1];
				pc+=2 ; break;
			case OP_PUSH:
				sp++;
				pile[sp]=codeSegment[pc+1];
				pc=pc+2; break;
			case OP_PUSHR:
				strRealLength=0;
				pos=pc+1;
				while (codeSegment[pos]!=0)
				{
					//printf("%c\n",(char)codeSegment[pos]);
					strReal[strRealLength]=(char)codeSegment[pos];
					strRealLength++;
					pos++;
				}
				strReal[strRealLength]='\0';
				//printf("%s\n",strReal);
				sscanf(strReal,"%f",&fv);
				//printf("fv=%f\n",fv);
				sp++;
				pile[sp]=fv;
				pc=pos+1;
				//printf("pc=%d\n",pc);
				break;
			case OP_LIBP:
				sp++;
				pile[sp]=bp+codeSegment[pc+1]; //on stocke l'adresse de retour
				pc=pc+2; break;
		        case OP_DUPL: //duplique la valeur du sommet
				sp++;
				pile[sp]=pile[sp-1];
				pc++; break;
			case OP_CONT:
				pile[sp]=pile[(int)pile[sp]];
				pc++; break;
		        case OP_MOVE: //bouge les n valeurs dans la pile et écrase les valeurs precedentes
				n=codeSegment[pc+1];
				orig=sp-n+1;
				dest=(int)pile[sp-n];
				for (i=1;i<=n;i++)
					pile[dest++]=pile[orig++];
				sp=sp-n-1;
				pc=pc+2; break;
			case OP_STM:
				v=pile[sp];
				dest=(int)pile[sp-1];
				pile[dest]=v;
				pc++; sp-=2;
				break;
		        case OP_COPY: // copie les n valeurs du sommet de la pile
				n=codeSegment[pc+1];
				dest=sp;
				orig=(int)pile[sp];
				for (i=1;i<=n;i++)
					pile[dest++]=pile[orig++];
				sp=sp+n-1;
				pc=pc+2; break;
			case OP_MTS:
				dest=sp;
				orig=(int)pile[sp];
				pile[dest]=pile[orig];
				pc++;
				break;
			case OP_JP:
				pc=codeSegment[pc+1];
				break;
			case OP_JF:
				if (pile[sp]==0.0)
					pc=codeSegment[pc+1];
				else
					pc=pc+2;
				sp--;
				break;
		        case OP_JL: // on effectue le jump seulement si la valeur au sommet de la pile est plus grande que celle empilee avant
				if (pile[sp-1]<pile[sp])
					pc=codeSegment[pc+1];
				else
					pc=pc+2;
				sp--;
				break;
			case OP_JG: // on effectue le jump seulement si la valeur au sommet de la pile est plus petite que celle empilée avant
				if (pile[sp-1]>pile[sp])
					pc=codeSegment[pc+1];
				else
					pc=pc+2;
				sp--;
				break;
		        case OP_CALL: //on appelle une fonction
				sp++;
				pile[sp]=pc+2; //on empile le pc vers lequel on reviendra quand on aura fini d'exécuter le code la fonction
				pc=codeSegment[pc+1]; //on exécute le code de la fonction
				break;
		        case OP_RET:
			        pc=(int)pile[sp]; //à l'appel de la fonction, on avait stocké dans la pile le numero de la ligne où l'on devait se rendre une fois la fonction terminéé
				sp--; //on dépile ce numero de ligne
				break;
			case OP_INPUT:
				scanf("%f",&fv);
				pile[(int)pile[sp]]=fv;
				sp--;
				pc++;break;
			case OP_OUTPUT:
				printf("%f\n",pile[sp]);
				sp--;
				pc++;break;
                        case OP_OUTCHAR:
                                strRealLength=0;
                                pos=pc+1;
                                while (codeSegment[pos]!=0)
                                {
                                        strReal[strRealLength]=(char)codeSegment[pos];
                                        strRealLength++;
                                        pos++;
                                }
                                strReal[strRealLength]='\0';
                                printf("%s\n",strReal);
                                pc=pos+1;
                                break;
		        case OP_SAVEBP: //on empile l'ancien BP, et on met à jour le BP
				sp++;
				pile[sp]=bp;
				bp=sp+1;
				pc++; break;
		        case OP_RSTRBP: //on revient à l'ancienne valeur de BP que l'on avait stocké dans la pile
				bp=(int)pile[sp];
				sp--;
				pc++; break;
			default:
			  printf("Instruction non reconnue: %d\n", codeSegment[pc]);
				exit(1);
		}
		//printExecStack();
	}
}

// Q7 : Programme principal. Apres les tests de nombre et de qualite
//	des arguments, ouvrir le fichier langage machine, appeler
//	la fonction readAssembly, et fermer le fichier ouvert. Ensuite
//	initialiser pc a 0, sp et bp a -1. Enfin, lancer la machine
//	virtuelle avec la fonction run().

int main(int argc, char **argv)
{
	int i;

	if (argc!=2)
	{
		printf("Usage : vm infile.bin\n");
	}
	FILE *fin=fopen(argv[1],"r");
	if (fin==NULL)
	{
		printf("Error opening read file %s\n",argv[1]);
		exit(1);
	}

	readAssembly(fin);
	fclose(fin);
	pc=0;
    	sp=-1;
    	bp=0;

	run();

	for (i=0;i<3;i++)
		printf("%d: %f\n",i,pile[i]);

	return 0;
}
