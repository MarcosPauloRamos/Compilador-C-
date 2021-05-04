
%{
#include "globals.h"
#include "util.h"
#include "scan.h"
/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN+1];
static int yylex(void);
%}

digito       [0-9]
numero      {digito}+
letra      [a-zA-Z]
identificador  {letra}+
newline     \n
whitespace  [ \t\r]+

%%

"void"			{return VOID;}
"if"            {return IF;}
"int"           {return INT;}
"return"        {return RETURN;}
"while"         {return WHILE;}
"else"          {return ELSE;}

"="             {return IGL;}
"=="            {return CMP;}
"!="            { return DIF;}
"<"             {return ME;}
">"	          	{return MA;}
">="	        {return MAIG;}
"<="	        {return MEIG;}
"+"             {return SOM;}
"-"             {return SUB;}
"*"             {return MUL;}
"/"             {return DIV;}
"("             {return APR;}
")"             {return FPR;}
"["             {return ACO;}
"]"             {return FCO;}
"{"             {return ACH;}
"}"             {return FCH;}
";"             {return PEV;}
","             {return VIR;}
{numero}        {return NUM;}
{identificador} {return ID;}
{newline}       {lineno++;}
{whitespace}    {/* skip whitespace */}
"/*"             { char c;
                    int f = 1;
                  do
                  { c = input();
                    if (c == EOF) break;
                    if (c == '\n') lineno++;
                    while (c == '*'){
                      if((c = input()) == '/') {
                      f = 0;
                      break;
                      }
                    }

                  } while (f);
                }
.               {return ERROR;}

%%

TokenType getToken(void)
{ static int firstTime = TRUE;
  TokenType currentToken;
  if (firstTime)
  { firstTime = FALSE;
    lineno++;
    yyin = source;
    yyout = listing;
  }
  currentToken = yylex();
  strncpy(tokenString,yytext,MAXTOKENLEN);
  if (TraceScan) {
    fprintf(listing,"\t%d: ",lineno);
    printToken(currentToken,tokenString);
  }
  return currentToken;
}
