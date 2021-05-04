/********************************************************************************/ 
/* File: sintatico.y                                                            */
/* Lex specification for Compiler                                               */
/* References: Compiler discipline, teacher Galvão, Unifesp - Brazil -2019      */
/* Marcos Paulo Ramos - mp001649@gmail.com                                      */
/********************************************************************************/
%{
#define YYPARSER /* distinguishes Yacc output from other code files */

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

#define YYSTYPE TreeNode *
static TreeNode * savedTree; /* stores syntax tree for later return */
static int yylex(void);
int yyerror(char *msg);


%}
%token NUM ID
%token IF ELSE WHILE RETURN VOID
%right INT
%token ERROR ENDFILE
%token SOM SUB MUL DIV
%token ME MEIG MA MAIG CMP DIF IGL
%token PEV VIR APR FPR ACO FCO ACH FCH


/*daqui pra baixo tem new coisas*/
%nonassoc FPR
%nonassoc ELSE

%% /* Grammar for TINY */

programa: declaracao_lista 
                 { savedTree = $1;}
            ;

declaracao_lista: declaracao_lista declaracao
          {
            YYSTYPE t = $1;
              if (t != NULL){
                while (t->sibling != NULL)
                   t = t->sibling;
                t->sibling = $2;
                $$ = $1;
              }
              else $$ = $2;
          }
	      | declaracao { $$ = $1; }
	      ;

declaracao: var_declaracao  { $$ = $1 ;}
	    | fun_declaracao { $$ = $1; }
	    ;

var_declaracao: INT identificador PEV
          {
            $$ = newExpNode(TypeK);
            $$->attr.name = "INT";
            $$->size = 1;
            $$->child[0] = $2;
            $2->kind.exp = VarDeclK;
            $2->type = INTTYPE;
          }
	      | INT identificador ACO numero FCO PEV
            {
              $$ = newExpNode(TypeK);
              $$->attr.name = "INT";
              $$->size = $4->attr.val;
              $$->child[0] = $2;
              $2->kind.exp = VarDeclK;
              $2->type = INTTYPE;
            }
	      ;

tipo_especificador  : INT
              {
                $$ = newExpNode(TypeK);
                $$->attr.name = "INT";
                $$->type = INTTYPE;
                $$->size = 1;
              }
            | VOID
              {
                $$ = newExpNode(TypeK);
                $$->attr.name = "VOID";
                $$->type = INTTYPE;
                $$->size = 1;
              }
            ;

fun_declaracao: INT identificador APR params FPR composto_decl
            {
              $$ = newExpNode(TypeK);
              $$->attr.name = "INT";
              $$->child[0] = $2;
              $2->kind.exp = FunDeclK;
              $2->lineno = $$->lineno;
              $2->type = INTTYPE;
              $2->child[0] = $4;
              $2->child[1] = $6;
            }
        | VOID identificador APR params FPR composto_decl
                    {
                      $$ = newExpNode(TypeK);
                      $$->attr.name = "VOID";
                      $$->child[0] = $2;
                      $2->type = VOIDTYPE;
                      $2->kind.exp = FunDeclK;
                      $2->lineno = $$->lineno;
                      $2->child[0] = $4;
                      $2->child[1] = $6;
                    }
        ;

params: param_lista { $$ = $1; }
       | VOID
          {
            $$ = newExpNode(TypeK);
            $$->attr.name = "VOID";
            $$->size = 1;
            $$->child[0] = NULL;
          }
       ;

param_lista: param_lista VIR param_lista
              {
                YYSTYPE t = $1;
                if (t != NULL){
                  while (t->sibling != NULL)
                       t = t->sibling;
                  t->sibling = $3;
                  $$ = $1;
                }
                else $$ = $3;
              }
           | param { $$ = $1; }
           ;

param : tipo_especificador identificador
        {
          $$ = $1;
          $$->child[0] = $2;
          $2->kind.exp = ParamK;

        }
      | tipo_especificador identificador ACO FCO
        {
          $$ = $1;
          $$->size = 0;
          $$->child[0] = $2;
          $2->kind.exp = ParamK;
        }
      ;

composto_decl: ACH local_declaracoes statement_lista FCH
              {
                YYSTYPE t = $2;
                  if (t != NULL){
                    while (t->sibling != NULL)
                       t = t->sibling;
                    t->sibling = $3;
                    $$ = $2;
                  }
                  else $$ = $3;
              }
             | ACH FCH {}
             | ACH  local_declaracoes FCH { $$ = $2; }
             | ACH statement_lista FCH { $$ = $2; }
             ;

local_declaracoes: local_declaracoes var_declaracao
            {
              YYSTYPE t = $1;
                if (t != NULL){
                  while (t->sibling != NULL)
                     t = t->sibling;
                  t->sibling = $2;
                  $$ = $1;
                }
                else $$ = $2;
            }
          | var_declaracao { $$ = $1; }
          ;

statement_lista: statement_lista statement
            {
              YYSTYPE t = $1;
              if (t != NULL){
                while (t->sibling != NULL)
                t = t->sibling;
                t->sibling = $2;
                $$ = $1;
              }
              else $$ = $2;
            }
          | statement { $$ = $1; }
          ;

statement: expressap_decl { $$ = $1; }
     | composto_decl { $$ = $1; }
     | selecao_decl { $$ = $1; }
     | iteracao_decl { $$ = $1; }
     | retorno_decl { $$ = $1; }
     ;

expressap_decl: expressao PEV { $$ = $1; }
        |  PEV {}
        ;

selecao_decl:  IF APR expressao FPR statement
          {
            $$ = newStmtNode(IfK);
            $$->child[0] = $3;
            $$->child[1] = $5;
          }
        | IF APR expressao FPR statement ELSE statement
          {
            $$ = newStmtNode(IfK);
            $$->child[0] = $3;
            $$->child[1] = $5;
            $$->child[2] = $7;
          }
        ;

iteracao_decl: WHILE APR expressao FPR statement
        {
          $$ = newStmtNode(WhileK);
          $$->child[0] = $3;
          $$->child[1] = $5;
        }
        ;

retorno_decl: RETURN PEV { $$ = newStmtNode(ReturnK); }
            | RETURN expressao PEV
              {
                $$ = newStmtNode(ReturnK);
                $$->child[0] = $2;
              }
            ;

expressao: var IGL expressao
      {
        $$ = newStmtNode(AssignK);
        $$->child[0] = $1;
        // $$->child[0]->kind.exp = AssignElK;
        $$->child[1] = $3;
        // $$->child[1]->kind.exp = AssignElK;
      }
    | simples_expressao { $$ = $1; }
    ;

var: identificador { $$ = $1; }
    | identificador ACO expressao FCO
      {
        $$ = $1;
        $$->type = INTTYPE;
        $$->child[0] = $3;
      }
    ;

simples_expressao: soma_expressao relacional soma_expressao
              {
                  $$ = $2;
                  $$->child[0] = $1;
                  $$->child[1] = $3;
              }
            | soma_expressao { $$ = $1; }
            ;

relacional: ME
              {
                $$ = newExpNode(OpK);
                $$->attr.op = ME;
              }
           | MEIG
              {
                $$ = newExpNode(OpK);
                $$->attr.op = MEIG;
              }
           | MA
              {
                $$ = newExpNode(OpK);
                $$->attr.op = MA;
              }
           | MAIG
              {
                $$ = newExpNode(OpK);
                $$->attr.op = MAIG;
              }
           | CMP
              {
                $$ = newExpNode(OpK);
                $$->attr.op = CMP;
              }
           | DIF
              {
                $$ = newExpNode(OpK);
                $$->attr.op = DIF;
              }
           ;

soma_expressao: soma_expressao soma termo {
            $$ = $2;
            $$->child[0] = $1;
            $$->child[1] = $3;
         }
         | termo { $$ = $1; }
         ;

soma : SOM
       {
         $$ = newExpNode(OpK);
         $$->attr.op = SOM;
       }
     | SUB
        {
          $$ = newExpNode(OpK);
          $$->attr.op = SUB;
        }
     ;

termo: termo mult fator
            {
              $$ = $2;
              $$->child[0] = $1;
              $$->child[1] = $3;
            }
      | fator { $$ = $1; }
      ;

mult : MUL
       {
         $$ = newExpNode(OpK);
         $$->attr.op = MUL;
       }
     | DIV
        {
          $$ = newExpNode(OpK);
          $$->attr.op = DIV;
        }
     ;

fator: APR expressao FPR { $$ = $2; }
      | var { $$ = $1; }
      | ativacao { $$ = $1; }
      | numero { $$ = $1; }
      ;

ativacao: identificador APR arg_lista FPR
        {
          $$ = newExpNode(AtivK);
          $$->attr.name = $1->attr.name;
          $$->child[0] = $3;

        }
        | identificador APR FPR
         {
           $$ = newExpNode(AtivK);
           $$->attr.name = $1->attr.name;
         }
     ;


arg_lista: arg_lista VIR expressao
            {
              YYSTYPE t = $1;
              if (t != NULL){
                while (t->sibling != NULL)
                t = t->sibling;
                t->sibling = $3;
                $$ = $1;
              }
              else $$ = $3;
            }
         | expressao { $$ = $1; }
         ;
identificador : ID
                {
                  $$ = newExpNode(IdK);
                  $$->attr.name = copyString(tokenString);
                }
              ;
numero : NUM
          {
            $$ = newExpNode(ConstK);
            $$->type = INTTYPE;
            $$->attr.val = atoi(tokenString);
          }

%%

int yyerror(char * message)
{ fprintf(listing,"Syntax error at line %d: %s\n",lineno,message);
  fprintf(listing,"Current token: ");
  printToken(yychar,tokenString);
  Error = TRUE;
  return 0;
}

/* yylex calls getToken to make Yacc/Bison output
 * compatible with ealier versions of the TINY scanner
 */
static int yylex(void)
{ return getToken(); }

TreeNode * parse(void)
{ yyparse();
  return savedTree;
}
