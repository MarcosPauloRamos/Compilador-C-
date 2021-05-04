# Compilador-C-
Este repositório se trata de um projeto da disciplina Laboratório de Compiladores da UNIFESP SJC no ano de 2021, onde a proposta foi desenvolver a um compilador para a linguagem C- proposta no livro Compiladores: Princípios e Práticas de Kenneth Louden. O compilador, além de fazer toda análise do código C-, gera um código binário baseado em um processador de 32 bits, feito anteriormente durante o curso, e arquivos de texto com todos os processos de compilação sendo eles a árvore sintática, a tabela de símbolos, o código intermediário e o código assembly.

Para informações mais detalhadas sobre a implementação do compilador confira o relatório presente na pasta Relatório.

# Sobre
O projeto do compilador foi baseado nas premissas de Kenneth Louden em Compiladores: Princípios e Práticas e foi majoritariamente feito na linguagem C, utilizando algumas ferramentas para auxiliar na criação de algumas partes.

A primeira ferramenta foi o compilador Flex, que foi utilizado para facilitar na criação de um Scaner sem ter a necessidade de programar linha por linha do mesmo, e sim inserir Expressões Regulares para a aceitação de cada token.

A segunda, foi o compilador YACC Bison, que foi utilizado em conjunto de uma Gramática Livre de Contexto para gerar um Parser. Essa ferramenta tem uma vantagem que é o fato de ser própria para a utilização em conjunto com o Flex, fazendo a criação de uma rotina de análise se tornar mais simples.

Ambas ferramentas depois de compiladas geram códigos C que podem ser facilmente integrados com as outas partes do projeto que foram desenvolvidas na mesma linguagem.

# Divisão

O projeto foi divido macroscopicamente em duas partes: a Análise e a Síntese, sendo cada uma delas subdivididas em mais 3 partes.

# Análise
A fase de análise é a responsável por percorrer todo o código e verificar sua integridade e a correspondência com a linguagem proposta, no caso C-. Ela foi dividida em análise léxica¹ (scaner.l), análise sintática² (parser.y) e análise semântica³ (analyze.c e symtab.c). A função de 1 é percorrer todo o código fonte analisando se cada palavra escrita se encaixa no escopo da linguagem C-, gerando tokens que são passados para 2 que os verifica em conjunto e analisa se a estrutura do código está correta, gerando uma árvore sintática. Essa última por fim é repassada para 3 que procura por erros de contexto como variáveis não declaradas ou atribuições de tipos conflitantes e caso nenhum seja encontrado uma tabela pe gerada constando todos os itens presentes no código, bem como a posição de memória a ser alocada (respeitando as restrições do processador) e outras informações presentes na árvore sintática.
