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
      return 13;
    else if(strcmp("add",asm_cmd)==0)
      return 1;
    else if(strcmp("push",asm_cmd)==0)
      return 15;
    else if (strcmp("sub",asm_cmd)==0)
      return 2;
    else if (strcmp("mult",asm_cmd)==0)
      return 3;
    else if (strcmp("div",asm_cmd)==0)
      return 4;
    else if (strcmp("divi",asm_cmd)==0)
      return 5;
    else if (strcmp("neg",asm_cmd)==0)
      return 6;
    else if (strcmp("and",asm_cmd)==0)
      return 7;
    else if (strcmp("or",asm_cmd)==0)
      return 8;
    else if (strcmp("not",asm_cmd)==0)
      return 9;
    else if (strcmp("eq",asm_cmd)==0)
      return 10;
    else if (strcmp("ls",asm_cmd)==0)
      return 11;
    else if (strcmp("gt",asm_cmd)==0)
      return 12;
    else if (strcmp("dec",asm_cmd)==0)
      return 14;
    else if (strcmp("pushr",asm_cmd)==0)
      return 16;
    else if (strcmp("stm",asm_cmd)==0)
      return 17;
    else if (strcmp("mts",asm_cmd)==0)
      return 18;
    else if (strcmp("jp",asm_cmd)==0)
     return 19; 
    else if (strcmp("jf",asm_cmd)==0)
      return 20;
    else if (strcmp("input",asm_cmd)==0)
      return 21;
    else if (strcmp("output",asm_cmd)==0)
      return 22;
    else if (strcmp("outchar",asm_cmd)==0)
      return 23;
    else if (strcmp("end",asm_cmd)==0)
      return 24;
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
	  tabCodr[ table_ref[i].addr] = table_etiquette[i].addr;
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
	      /* L'instruction qui suit le label est passée à la ligne */
	      /* IL n'y a donc aucune instruction sur la même ligne que le label */
	      char nom[256];
	      char tmp; //pour stocker :
	      sscanf(line, "%s %c %s %s", nom, &tmp, instruction, argument);
	      addLabel(nom, tabCodr_ind);
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
		  tabCodr[tabCodr_ind++] = table_etiquette[indLabel].addr;
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

