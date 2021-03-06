/********************************************************************************/ 
/* File: util.c                                                                 */
/*  Utility function implementation                                             */
/* References: Compiler discipline, teacher Galvão, Unifesp - Brazil -2019      */
/* Marcos Paulo Ramos - mp001649@gmail.com                                      */
/********************************************************************************/

/* Variable indentno is used by printTree to
 * store current number of spaces to indent
 */
static int indentno = 0;

/* macros to increase/decrease indentation */
#define INDENT indentno+=2
#define UNINDENT indentno-=2

#include "globals.h"
#include "util.h"
#include "cgen.h"
#include "symtab.h"
#include "assembly.h"
#include "binary.h"

/* Procedure printToken prints a token
 * and its lexeme to the listing file
 */
 void printToken( TokenType token, const char* tokenString )
 { switch (token)
   { case IF:   fprintf(listing,"%s\n",tokenString);break;
     case RET:  fprintf(listing,"%s\n",tokenString);break;
     case ELSE: fprintf(listing,"%s\n",tokenString);break;
     case INT:  fprintf(listing,"%s\n",tokenString);break;
     case WHI:  fprintf(listing,"%s\n",tokenString);break;
     case VOID: fprintf(listing,"%s\n",tokenString);break;
     case IGL: fprintf(listing,"==\n"); break;
     case ATR: fprintf(listing,"=\n"); break;
     case DIF: fprintf(listing,"!=\n"); break;
     case MENO: fprintf(listing,"<\n"); break;
     case MAIO: fprintf(listing,">\n"); break;
     case MEIG: fprintf(listing,"<=\n"); break;
     case MAIG: fprintf(listing,">=\n"); break;
     case APR: fprintf(listing,"(\n"); break;
     case FPR: fprintf(listing,")\n"); break;
     case ACO: fprintf(listing,"[\n"); break;
     case FCO: fprintf(listing,"]\n"); break;
     case ACH: fprintf(listing,"{\n"); break;
     case FCH: fprintf(listing,"}\n"); break;
     case PEV: fprintf(listing,";\n"); break;
     case VIRG: fprintf(listing,",\n"); break;
     case SOM: fprintf(listing,"+\n"); break;
     case SUB: fprintf(listing,"-\n"); break;
     case MUL: fprintf(listing,"*\n"); break;
     case DIV: fprintf(listing,"/\n"); break;
     case FIM: fprintf(listing,"EOF\n"); break;
     case NUM: fprintf(listing,"NUM, val = %s\n",tokenString);break;
     case ID:  fprintf(listing,"ID, nome = %s\n",tokenString);break;
     case ERR:
       fprintf(listing,
           "%s\n",tokenString);
       break;
     default: /* Nunca deve acontecer */
       fprintf(listing,"Token Desconhecido: %d\n",token);
   }
 }

/* Function newStmtNode creates a new statement
 * node for syntax tree construction
 */
TreeNode * newStmtNode(StmtKind kind)
{ TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = StmtK;
    t->kind.stmt = kind;
    t->lineno = lineno;
  }
  return t;
}

/* Function newExpNode creates a new expression
 * node for syntax tree construction
 */
TreeNode * newExpNode(ExpKind kind)
{ TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = ExpK;
    t->kind.exp = kind;
    t->lineno = lineno;
    t->type = Void;
  }
  return t;
}

/* Function copyString allocates and makes a new
 * copy of an existing string
 */
char * copyString(char * s)
{ int n;
  char * t;
  if (s==NULL) return NULL;
  n = strlen(s)+1;
  t = malloc(n);
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else strcpy(t,s);
  return t;
}

/* printSpaces indents by printing spaces */
static void printSpaces(void)
{ int i;
  for (i=0;i<indentno;i++)
    fprintf(listing," ");
}

/* procedure printTree prints a syntax tree to the
 * listing file using indentation to indicate subtrees
 */
 void printTree( TreeNode * tree )
 { int i;
   INDENT;
   while (tree != NULL) {
     printSpaces();
     if (tree->nodekind==StmtK)
     { switch (tree->kind.stmt) {
         case IfK:
           fprintf(listing,"If\n");
           break;
         case WhileK:
           fprintf(listing,"While\n");
           break;
         case AssignK:
           fprintf(listing,"Assign: \n");
           break;
         case ReturnINT:
           fprintf(listing,"Return\n");
           break;
         case ReturnVOID:
           fprintf(listing,"Return\n");
           break;
         default:
           fprintf(listing,"Unknown ExpNode kind\n");
           break;
       }
     }
     else if (tree->nodekind==ExpK)
     { switch (tree->kind.exp) {
         case OpK:
           fprintf(listing,"Op: ");
           printToken(tree->attr.op,"\0");
           break;
         case ConstK:
           fprintf(listing,"Const: %d\n",tree->attr.val);
           break;
         case IdK:
           fprintf(listing,"Id: %s\n",tree->attr.name);
           break;
         case VarDeclK:
           fprintf(listing,"Var: %s\n",tree->attr.name);
           break;
         case FunDeclK:
           fprintf(listing,"Func: %s\n",tree->attr.name);
           break;
         case AtivK:
           fprintf(listing,"Ativação: %s\n",tree->attr.name);
           break;
         case TypeK:
           fprintf(listing,"Tipo: %s*\n",tree->attr.name);
           break;
         case VarParamK:
           fprintf(listing,"Parametro: %s\n",tree->attr.name);
           break; 
         case VetParamK:
           fprintf(listing,"Parametro: %s\n",tree->attr.name);
           break; 
         case VetorK:
           fprintf(listing, "Vetor: %s", tree->attr.name);
	          break;
         default:
           fprintf(listing,"Unknown ExpNode kind\n");
           break;
       }
     }
     else fprintf(listing,"Unknown node kind\n");
     for (i=0;i<MAXCHILDREN;i++)
          printTree(tree->child[i]);
     tree = tree->sibling;
   }
   UNINDENT;
 }
void nomeiaArquivos(char *nome){
   int fnlen = strcspn(nome,".");
   // aloca espaços para nomes
   ArvSint   = (char*)calloc(8+fnlen+5,sizeof(char));
   TabSimb   = (char*)calloc(8+fnlen+4,sizeof(char));
   interCode = (char*)calloc(8+fnlen+4,sizeof(char));
   assCode   = (char*)calloc(8+fnlen+5,sizeof(char));
   binCode   = (char*)calloc(8+fnlen+5,sizeof(char));
   // insere pasta '/gerados' ao caminho
   strcpy(ArvSint,"saida/");
   strcpy(TabSimb,"saida/");
   strcpy(interCode,"saida/");
   strcpy(assCode,"saida/");
   strcpy(binCode,"saida/");
   // insere nome do arquivo ao caminho
   strncat(ArvSint,nome,fnlen);
   strncat(TabSimb,nome,fnlen);
   strncat(interCode,nome,fnlen);
   strncat(assCode,nome,fnlen);
   strncat(binCode,nome,fnlen);
   // insere extensão do arquivo
   strcat(ArvSint,".tree");
   strcat(TabSimb,".tab");
   strcat(interCode,".itm");
   strcat(assCode,".asb");
   strcat(binCode,".bin");
}

void criararquivos(){
  int i;
  FILE *arvore, *tabela, *intermed, *temp, *assembly, *binary;
	
  arvore = fopen(ArvSint,"w");
  tabela = fopen(TabSimb,"w");
  intermed = fopen(interCode,"w");
  assembly = fopen(assCode,"w");
  binary = fopen(binCode,"w");
	
  temp = listing;
	
  listing = arvore;
  printTree(syntaxTree);
	
  listing = tabela;
  printSymTab(listing);
	
  listing = intermed;
  printCode(getIntermediate());
	
  listing = assembly;
  printAssembly();
	
  listing = binary;
  generateBinary();
	
  listing = temp;
	
  fclose(arvore);
  fclose(tabela);
  fclose(intermed);
  fclose(assembly);
  fclose(binary);
}
