#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include "asm_codops.h"
#define SIZE 500

int tabCodr[SIZE];
int tabCodr_ind = 0;

typedef struct label
{
  int addr;
  char nom[256];
} Label;

typedef struct resolution_ref
{
  int addr;
  char nom[256];
  fpos_t position;
} Ref_resolue;


Label table_etiquette[SIZE];
Ref_resolue table_ref_non_resolues[SIZE];
int tab_ind_etiquette = 0;
int tab_ind_ref = 0;

/*
Fabriquer les tables de reference et table des labels
 */

int asm_to_bin(char* asm_cmd)
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
      return -1;

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

void readAssembly(FILE *fin, FILE* fout)
{
	char line[100];
	int nb_lines=0;
	char cmd[256];
	char num[256];
	int instruction;
	char tmp; //pour stocker :

	fprintf(fout, "%d\n\n", nb_lines);

	do
	{
	  fgets(line,100,fin);

	  if (strstr(num, ":") != NULL)
	    {
	      sscanf(line, "%s %c %s %s", table_etiquette[tab_ind_etiquette].nom, &tmp, cmd, num);
	      table_etiquette[tab_ind_etiquette++].addr = nb_lines;
	    }
	  else
	    {
	      sscanf(line,"%s %s",cmd,num);
	     
	    }
	  instruction = asm_to_bin(cmd);
	  fprintf(fout,"%d:%d\n",nb_lines,instruction);
	  nb_lines++;
	  
	  // Position de cette instruction a vérifier: doit pas ecrire la ligne d'avant
	  if (instruction ==0)
	    {
	      tabCodr[tabCodr_ind++] = nb_lines;
	    }

	  if (instruction == 15)
	    {
	      fprintf(fout,"%d:%d\n",nb_lines, atoi(num));
	      nb_lines++;
	    }
	  else if (instruction == 16)
	    {	     
	      fprintf(fout,"%d:%d\n",nb_lines, 48 + atoi(num));	  
	      fprintf(fout,"%d:0\n",++nb_lines);
	      nb_lines++;
	    }

	  else if (instruction == 13)
	    {
	      fprintf(fout,"%d:%d\n",nb_lines, atoi(num));
	      nb_lines++;
	    }
	  else if (instruction == 19 || instruction == 20)
	    {	      
	      char reference[256];
	      sprintf(reference, "%s", num);
	      int i = find_label(reference); 
	      if (i != -1)
		{
		  fprintf(fout,"%d:%d\n%d:%d\n", nb_lines, instruction, nb_lines+1, table_etiquette[i].addr);
		  nb_lines++;
		}
	      else
		{
		  fprintf(fout,"%d:%d\n", nb_lines, instruction);
		  fgetpos(fout, &table_ref_non_resolues[tab_ind_ref].position);
		  fprintf(fout,"%d:%d\n", nb_lines+1, -1);
		  table_ref_non_resolues[tab_ind_ref].addr = nb_lines+1;
		  sprintf(table_ref_non_resolues[tab_ind_ref++].nom, "%s", reference); 
		  nb_lines++;
		}
	      
	    }

	} while(strstr(line,"end")==NULL);
	
	/*2eme passe pour resoudre les references non resolues*/
	int i,j;
	for(i=0; i< tab_ind_ref; i++)
	  {

	    j = find_label(table_ref_non_resolues[i].nom);
	    if (j == -1)
	      {
		printf("ERREUR DE LABELS!!\n");
		exit(1);
	      }
	    fsetpos(fout, &table_ref_non_resolues[i].position);
	    fprintf(fout,"%d:%d\n", table_ref_non_resolues[i].addr, table_etiquette[j].addr);
	    
	     
	  }
	

	// ecriture du nombre de ligne en debut de fichier
	rewind(fout);
	fprintf(fout, "%d\n", nb_lines--);
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

	readAssembly(fin, fout);
	fclose(fin);
	fclose(fout);

	return 0;
}

