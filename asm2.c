#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include "vm_codops.h"
#define SIZE 500

int tabCodr[SIZE];
int tabCodr_ind = 0;

typedef struct label
{
  int addr;
  char* nom;
} Label;

typedef struct ref
{
  int addr;
  char *nom;
} Reference;

Label table_etiquette[SIZE];
Reference table_ref[SIZE];
int tab_ind_etiquette = 0;
int tab_ind_ref = 0;

int instruction_to_codops(char* asm_cmd)
{
    if(strcmp("inc",asm_cmd)==0)
      return OP_INC;
    else if(strcmp("add",asm_cmd)==0)
      return OP_ADD;
    else if(strcmp("push",asm_cmd)==0)
      return OP_PUSH;
    else if (strcmp("sub",asm_cmd)==0)
      return OP_SUB;
    else if (strcmp("mult",asm_cmd)==0)
      return OP_MULT;
    else if (strcmp("div",asm_cmd)==0)
      return OP_DIV;
    else if (strcmp("divi",asm_cmd)==0)
      return OP_DIVI;
    else if (strcmp("neg",asm_cmd)==0)
      return OP_NEG;
    else if (strcmp("and",asm_cmd)==0)
      return OP_AND;
    else if (strcmp("or",asm_cmd)==0)
      return OP_OR;
    else if (strcmp("not",asm_cmd)==0)
      return OP_NOT;
    else if (strcmp("eq",asm_cmd)==0)
      return OP_EQ;
    else if (strcmp("ls",asm_cmd)==0)
      return OP_LS;
    else if (strcmp("gt",asm_cmd)==0)
      return OP_GT;
    else if (strcmp("dec",asm_cmd)==0)
      return OP_DEC;
    else if (strcmp("pushr",asm_cmd)==0)
      return OP_PUSHR;
    else if (strcmp("stm",asm_cmd)==0)
      return OP_STM;
    else if (strcmp("mts",asm_cmd)==0)
      return OP_MTS;
    else if (strcmp("jp",asm_cmd)==0)
     return OP_JP; 
    else if (strcmp("jf",asm_cmd)==0)
      return OP_JF;
    else if (strcmp("input",asm_cmd)==0)
      return OP_INPUT;
    else if (strcmp("output",asm_cmd)==0)
      return OP_OUTPUT;
    else if (strcmp("outchar",asm_cmd)==0)
      return OP_OUTCHAR;
    else if (strcmp("end",asm_cmd)==0)
      return OP_HALT;
    else if (strcmp("halt",asm_cmd)==0)
      return OP_HALT;
    else if (strcmp("libp",asm_cmd)==0)
      return OP_LIBP;
    else if (strcmp("dupl",asm_cmd)==0)
      return OP_DUPL;
    else if (strcmp("cont",asm_cmd)==0)
      return OP_CONT;
    else if (strcmp("move",asm_cmd)==0)
      return OP_MOVE;
    else if (strcmp("copy",asm_cmd)==0)
      return OP_COPY;
    else if (strcmp("call",asm_cmd)==0)
      return OP_CALL;
    else if (strcmp("ret",asm_cmd)==0)
      return OP_RET;
    else if (strcmp("calls",asm_cmd)==0)
      return OP_CALLS;
    else if (strcmp("savebp",asm_cmd)==0)
      return OP_SAVEBP;
    else if (strcmp("rstrbp",asm_cmd)==0)
      return OP_RSTRBP;
   else      
      return 0;

}

int find_label(char* reference)
{
  int i;
  for(i =0; i<tab_ind_etiquette; i++)
    {
      if (strcmp(reference, table_etiquette[i].nom ) == 0)
	{
	  return i;
	}
    }
  return -1;
}

void ref_resolution()
{
  int i;
  for(i = 0 ; i < tab_ind_etiquette; i++)
    {
      printf("%s %d\n", table_etiquette[i].nom, table_etiquette[i].addr);
    }

  for (i = 0; i < tab_ind_ref; i++)
    {
      int indLabel = find_label(table_ref[i].nom);
      if (indLabel == -1)
	{
	  printf("LE LABEL %s N'EXISTE PAS!!!!\n", table_ref[i].nom );
	  exit(1);
	}
	
      else
	{
	  tabCodr[ table_ref[i].addr] = table_etiquette[indLabel].addr;
	}
    }

  
}

void addLabel(char* nom, int addr){

  table_etiquette[tab_ind_etiquette].nom = strdup(nom);
  table_etiquette[tab_ind_etiquette++].addr = addr;
}

void writeBinary(FILE* fout)
{
  int i;

  fprintf(fout, "%d\n", tabCodr_ind);

  for (i = 0 ; i < tabCodr_ind; i++)
    {
      fprintf(fout, "%d:%d\n", i, tabCodr[i]);
    }

}

void readAssembly(FILE *fin)
{
	char line[100];
	char instruction[256];
	char argument[256];
	int codops;

	do
	{
	  fgets(line,100,fin);

	  if (strstr(line, ":") != NULL)
	    {
	      char nom[256];
	      char tmp; //pour stocker :
	      sscanf(line, "%s %c", nom, &tmp);
	      addLabel(nom, tabCodr_ind);
	      continue;
	    }
	  else
	    {
	      sscanf(line,"%s %s",instruction,argument);
	    }
	  
	  codops = instruction_to_codops(instruction);

	  switch(codops)
	    {
	    case OP_PUSH:
	      {
		tabCodr[tabCodr_ind++] = codops;
		tabCodr[tabCodr_ind++] = atoi(argument);
		break;
	      }
	    case OP_PUSHR:
	      {
		tabCodr[tabCodr_ind++] = codops;
		int i;
		for(i = 0; i < strlen(argument); i++)
		  {
		    tabCodr[tabCodr_ind++] = (int)argument[i];
		  }
		tabCodr[tabCodr_ind++] = 0;
		break;
	      }

	    case OP_INC:
	      {
		tabCodr[tabCodr_ind++] = codops;
		tabCodr[tabCodr_ind++] = atoi(argument);
		break;
	      }
	    case OP_DEC:
	      {
		tabCodr[tabCodr_ind++] = codops;
		tabCodr[tabCodr_ind++] = atoi(argument);
		break;
	      }
	    case OP_JP:
	      {

		int indLabel = find_label(argument);

		tabCodr[tabCodr_ind++] = codops;
		
		if ( indLabel != -1)
		  tabCodr[tabCodr_ind++] = table_etiquette[indLabel].addr;
		else
		  {
		    table_ref[tab_ind_ref].addr = tabCodr_ind;
		    table_ref[tab_ind_ref++].nom = strdup(argument);
		    tabCodr[tabCodr_ind++] = -1;
		  }
		break;
	      }

	    case OP_JF:
	      {

		int indLabel = find_label(argument);

		tabCodr[tabCodr_ind++] = codops;
		
		if ( indLabel != -1)
		  {
		    /* printf("DANS JF!! indice: %d\n", table_etiquette[indLabel].addr); */
		    tabCodr[tabCodr_ind++] = table_etiquette[indLabel].addr;
		  }
		  
		else
		  {
		    table_ref[tab_ind_ref].addr = tabCodr_ind;
		    table_ref[tab_ind_ref++].nom = strdup(argument);
		    tabCodr[tabCodr_ind++] = -1;
		  }
		break;
	      }

	    case -1:
	      {
		printf("ERREUR DE SYNTAXE!!!!\n");
		exit(1);
	      }
	    case OP_LIBP:
	      {
		tabCodr[tabCodr_ind++] = codops;
		tabCodr[tabCodr_ind++] = atoi(argument);
		break;
	      }
	    case OP_MOVE:
	      {
		tabCodr[tabCodr_ind++] = codops;
		tabCodr[tabCodr_ind++] = atoi(argument);
		break;
	      }
	    case OP_COPY:
	      {
		tabCodr[tabCodr_ind++] = codops;
		tabCodr[tabCodr_ind++] = atoi(argument);
		break;
	      }
	    case OP_CALL:
	      {
		int indLabel = find_label(argument);
		tabCodr[tabCodr_ind++] = codops;
		
		if ( indLabel != -1)
		  {
		    /* printf("DANS JF!! indice: %d\n", table_etiquette[indLabel].addr); */
		    tabCodr[tabCodr_ind++] = table_etiquette[indLabel].addr;
		  }
		  
		else
		  {
		    table_ref[tab_ind_ref].addr = tabCodr_ind;
		    table_ref[tab_ind_ref++].nom = strdup(argument);
		    tabCodr[tabCodr_ind++] = -1;
		  }
		break;
	      }
	    default:
	      {
		tabCodr[tabCodr_ind++] = codops;
		break;
	      }

	    }

	} while(strstr(line,"end") == NULL);

	ref_resolution();
}

int main(int argc, char **argv)
{

  if (argc!=3)
    {
      printf("Usage : asm infile.asm outfile.bin\n");
    }

	FILE *fin=fopen(argv[1],"r");
	FILE *fout= fopen(argv[2], "w");

	if (fin==NULL)
	{
		printf("Error opening read file %s\n",argv[1]);
		exit(1);
	}

	readAssembly(fin);
	writeBinary(fout);
	fclose(fin);
	fclose(fout);

	return 0;
}

