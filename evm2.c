#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "vm_codops.h"

#define MAX_CODESEGMENT_SIZE 1000
#define MAX_STACK_SIZE 100

// Les registres de la machine virtuelle

int pc;
int sp;
int bp;

// Declaration du segment de code

int codeSegment[MAX_CODESEGMENT_SIZE];

// Declaration de la pile d'execution (tableau de float)

float pile[MAX_STACK_SIZE];

// Toutes les fonctions representant les instructions de la MV

void inst_add()
{
	pile[sp-1]=pile[sp-1]+pile[sp]; 
	sp-- ; pc++ ;

	printf("add\n");
}

void inst_sub()
{
	pile[sp-1]=pile[sp-1]-pile[sp]; 
	sp-- ; pc++ ;
}

void inst_mult()
{
	pile[sp-1]=pile[sp-1]*pile[sp]; 
	sp-- ; pc++ ;

	printf("mult\n");
}

void inst_div()
{
	pile[sp-1]=pile[sp-1]/pile[sp]; 
	sp-- ; pc++ ;
}

void inst_divi()
{
	pile[sp-1]=(float)((int)pile[sp-1]/(int)pile[sp]); 
	sp-- ; pc++ ;
}

void inst_neg()
{
	pile[sp]=-pile[sp]; 
	pc++ ;
}

void inst_and()
{
	pile[sp-1]=(float)((int)pile[sp-1] & (int)pile[sp]); 
	sp-- ; pc++ ;
}

void inst_or()
{
	pile[sp-1]=(float)((int)pile[sp-1] | (int)pile[sp]); 
	sp-- ; pc++ ;
}

void inst_not()
{
	pile[sp]=(float)(1-(int)pile[sp]); 
	pc++ ;
}

void inst_eq()
{
	pile[sp-1]=(pile[sp-1]==pile[sp]) ? 1.0 : 0.0; 
	sp-- ; pc++ ;
}

void inst_ls()
{
	pile[sp-1]=(pile[sp-1]<pile[sp]) ? 1.0 : 0.0; 
	sp-- ; pc++ ;
}

void inst_gt()
{
	pile[sp-1]=(pile[sp-1]>pile[sp]) ? 1.0 : 0.0; 
	sp-- ; pc++ ;
}

void inst_inc()
{
	sp=sp+codeSegment[pc+1]; 
	pc+=2 ;
}

void inst_dec()
{
	sp=sp-codeSegment[pc+1]; 
	pc+=2 ;
}

void inst_push()
{
	sp++;
	pile[sp]=codeSegment[pc+1];
	pc=pc+2;
}

void inst_pushr()
{
	char strReal[256];
	int strRealLength,pos;
	float fv;

	strRealLength=0;
	pos=pc+1;
	while (codeSegment[pos]!=0)
	{
		strReal[strRealLength]=(char)codeSegment[pos];
		strRealLength++;
		pos++;
	}
	strReal[strRealLength]='\0';
	sscanf(strReal,"%f",&fv);
	sp++;
	pile[sp]=fv;

	printf("pushr %f\n",fv);

	pc=pos+1;
}

void inst_stm()
{
	float fv;
	int dest;

	fv=pile[sp];
	dest=(int)pile[sp-1];	
	pile[dest]=fv;
	pc++; sp-=2;
}

void inst_mts()
{
	int dest, orig;

	dest=sp;
	orig=(int)pile[sp];
	pile[dest]=pile[orig];
	pc++;
}

void inst_jp()
{
	pc=codeSegment[pc+1];
}

void inst_jf()
{
	if (pile[sp]==0.0)
		pc=codeSegment[pc+1];
	else
		pc=pc+2;
	sp--;
}

void inst_input()
{
	float fv;

	scanf("%f",&fv);
	pile[(int)pile[sp]]=fv;
	sp--;
	pc++;
}

void inst_output()
{
	printf("%f\n",pile[sp]);
	sp--;
	pc++;
}

void inst_outchar()
{
	char strReal[256];
	int strRealLength,pos;

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
}

void (*instructions[])()=
{
	NULL,
	inst_add,
	inst_sub,
	inst_mult,
	inst_div,
	inst_divi,
	inst_neg,
	inst_and,
	inst_or,
	inst_not,
	inst_eq,
	inst_ls,
	inst_gt,
	inst_inc,
	inst_dec,
	inst_push,
	inst_pushr,
	inst_stm,
	inst_mts,
	inst_jp,
	inst_jf,
	inst_input,
	inst_output,
	inst_outchar,
};

// Fonction de lecture d'un fichier langage machine
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

// Boucle principale d'exécution de la machine virtuelle 
//      On sort de cette fonction si l'instruction pointee
//      par pc est OP_HALT. On traitera les erreurs et on
//	pensera a ajouter des traces et des moyens de mise
//	au point

void run()
{
	int inst;

	inst=codeSegment[pc];
	while (inst!=OP_HALT)
	{
		(*instructions[inst])();
		inst=codeSegment[pc];
	}
}

// Programme principal. Apres les tests de nombre et de qualite
//	des arguments, ouvrir le fichier langage machine, appeler
//	la fonction readAssembly, et fermer le fichier ouvert. Ensuite
//	initialiser pc a 0, sp et bp a -1. Enfin, lancer la machine
//	virtuelle avec la fonction run().

int main(int argc, char **argv)
{
  //int i;

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

	return 0;
}
