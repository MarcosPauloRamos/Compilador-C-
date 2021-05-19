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
  FILE *arvore, *tabela, *intermed, *temp;
  arvore = fopen(ArvSint,"w");
  tabela = fopen(TabSimb,"w");
  intermed = fopen(interCode,"w");
	temp = listing;
  
  
  
  char pgm[120]; /* nome do arquivo do código fonte */
  char path[120];

  if (argc < 3) {
    fprintf(stderr,"Arquivo não especificado.\nUso: %s <nome do arquivo> <flag>\n",argv[0]);
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
    fprintf(stderr,"Arquivo %s não encontrado.\n",path);
    exit(-1);
  }
  listing = stdout; /* send listing to screen */
  fprintf(listing,"\nCOMPILAÇÃO DO ARQUIVO C-\n");
  fprintf(listing,"Fonte: ./%s\n",path);
  nomeiaArquivos(pgm);

#if NO_PARSE
  while (getToken()!=ENDFILE);
#else
  syntaxTree = parse();
  if(Error == TRUE){
    printf( "Impossível concluir a compilação!\n\n");
    exit(-1);
    }
  if (TraceParse) {
    listing = arvore;
    fprintf(listing,"Árvore Sintática:\n");
    printTree(syntaxTree);
  }

#if !NO_ANALYZE
  if (TraceAnalyze) fprintf(listing,"Construindo Tabela de Simbolos...\n");
  listing = tabela;
  buildSymtab(syntaxTree);
  if (TraceAnalyze) fprintf(listing,"\nAnálise Concluida!\n"); 
  if(Error){
    printf( "Impossível concluir a compilação!\n\n");
    exit(-1);
  }
#if !NO_CODE
  if(TraceCode) fprintf(listing,"Criando código intermediário...\n");
  listing = intermed;
  codeGen(syntaxTree);  //GERADOR DE COD. INTERMED.
  listing = temp;
  listing = stdout;
  fprintf(listing,  "Compilação concluida com sucesso!\n\n" );
	
  fclose(arvore);
  fclose(tabela);
  fclose(intermed);
#endif
#endif
#endif
  fclose(source);
  return 0;
}
