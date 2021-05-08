/****************************************************/
/* File: main.c                                     */
/* Semantic analyzer interface for TINY compiler    */
/* Remodelado para utilizacao no projeto            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/
#include "globals.h"

/* set NO_PARSE to TRUE to get a scanner-only compiler */
#define NO_PARSE FALSE
/* set NO_ANALYZE to TRUE to get a parser-only compiler */
#define NO_ANALYZE FALSE

/* set NO_CODE to TRUE to get a compiler that does not
 * generate code
 */
#define NO_CODE FALSE

#include "util.h"
#if NO_PARSE
#include "scan.h"
#else
#include "parse.h"
#if !NO_ANALYZE
#include "analyze.h"
#if !NO_CODE
#include "cgen.h"
#endif
#endif
#endif

/* allocate global variables */
extern int check_return;
int lineno = 1;
int init_code;
FILE * source;
FILE * listing;

/* allocate and set tracing flags */
FlagType TraceScan = FALSE; // Imprimir tokens
FlagType TraceParse = FALSE; // Imprimir árvore sintática
FlagType TraceAnalyze = FALSE; // Imprimir tabela de simbolos
FlagType TraceCode = FALSE; // Imprimir nós da geração de código
FlagType PrintCode = FALSE; // Imprimir os códigos gerados
FlagType CreateFiles = FALSE; // Criar arquivos de compilação
FlagType Error = FALSE; // Flag que marca a existência de erros
FlagType SO; // Indica se a compilação é de um SO


int main( int argc, char * argv[] ) {
  char pgm[120]; /* nome do arquivo do código fonte */
  char path[120];

  if (argc < 3) {
    fprintf(stderr,N_VERM"Arquivo não especificado.\nUso: %s <nome do arquivo> <flag>\n"RESET,argv[0]);
    exit(-1);
  }
  
  if(strcmp(argv[2],"-so") == 0){
    init_code = 0;
    SO = TRUE;
  }else{
    init_code = atoi(argv[2]);
    SO = FALSE;
  }
  
  strcpy(path,"codigos/");
  strcpy(pgm,argv[1]) ;
  if (strchr (pgm, '.') == NULL)
     strcat(pgm,".cm");
  strcat(path,pgm);
  source = fopen(path,"r");
  if (source==NULL) {
    fprintf(stderr,N_VERM"Arquivo %s não encontrado.\n"RESET,path);
    exit(-1);
  }
  listing = stdout; /* send listing to screen */
  fprintf(listing,N_BRC"\nCOMPILAÇÃO DO ARQUIVO C-\n"RESET);
  fprintf(listing,"Fonte: "VERD"./%s\n"RESET,path);
  nomeiaArquivos(pgm);

#if NO_PARSE
  while (getToken()!=ENDFILE);
#else
  syntaxTree = parse();
  if(Error == TRUE){
    printf(N_VERM "Impossível concluir a compilação!\n\n");
    exit(-1);
    }
  if (TraceParse) {
    fprintf(listing,N_AZ"Árvore Sintática:\n"RESET);
    printTree(syntaxTree);
  }

#if !NO_ANALYZE
  if (TraceAnalyze) fprintf(listing,AZ"Construindo Tabela de Simbolos...\n"RESET);
  buildSymtab(syntaxTree);
  if (TraceAnalyze) fprintf(listing,N_VERD"\nAnálise Concluida!\n"RESET); 
  if(Error){
    printf(N_VERM "Impossível concluir a compilação!\n\n");
    exit(-1);
  }
#if !NO_CODE
  if(TraceCode) fprintf(listing,AZ"Criando código intermediário...\n"RESET);
  codeGen(syntaxTree);  //GERADOR DE COD. INTERMED.
  if(!PrintCode) listing = NULL;
  listing = stdout;
  fprintf(listing, N_VERD "Compilação concluida com sucesso!\n\n" RESET);

#endif
#endif
#endif
  fclose(source);
  return 0;
}
