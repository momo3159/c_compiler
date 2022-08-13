#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>

typedef enum {
  TK_RESERVED,
  TK_NUM,
  TK_EOF,
} TokenKind;

typedef struct Token Token;

// 入力は Token の連結リストに変換される
struct Token {
  TokenKind kind; // Token の種別
  Token *next;
  int val; // TK_NUM の場合はここに数値
  char *str; // Token に対応する文字列
  int len;
};

extern Token *token;
extern char *user_input;

typedef enum {
  ND_ADD,
  ND_SUB,
  ND_MUL,
  ND_DIV,
  ND_NUM,
  ND_EQ,
  ND_NOT_EQ,
  ND_LT,
  ND_GT,
  ND_LTE,
  ND_GTE,
} NodeKind;

typedef struct Node Node;

struct Node {
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  int val;
};

void gen(Node *node);

Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();
bool consume(char *op);
void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
void expect(char *op); 
Node *new_node(NodeKind kind, Node *lhs, Node *rhs); 
bool at_eof();
Token *new_token(TokenKind kind, Token *cur, char *str, int len); 
Node *new_node_num(int val);
Token* tokenize(char *p);



