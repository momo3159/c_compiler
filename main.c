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
};

Token *token;

void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// token が記号かつop に等しいかどうか
bool consume(char op) {
  if (token->kind != TK_RESERVED || token->str[0] != op) {
    return false;
  }

  token = token->next;
  return true;
}

// token が記号かつopに等しい→一つ進める
void expect(char op) {
  if (token->kind != TK_RESERVED || token->str[0] != op) {
    error("'%c'ではありません", op);
  }

  token = token->next;
}

int expect_number() {
  if (token->kind != TK_NUM) {
    error("数ではありません");
  }

  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof() {
  return token->kind == TK_EOF;
}

Token *new_token(TokenKind kind, Token *cur, char *str) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  cur->next = tok;
  return tok;
}

Token* tokenize(char *p) {
  Token head;
  head.next = NULL;
  Token *cur = &head; // 現在着目しているTokenへのポインタ

  while (*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (*p == '+' || *p == '-') {
      cur = new_token(TK_RESERVED, cur, p++);
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    error("トークナイズできません");
  }

  new_token(TK_EOF, cur, p);
  return head.next;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }

  // 入力をトークン列に変換する
  token = tokenize(argv[1]);
  
  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");
  printf("  mov rax, %ld\n", expect_number());

  while (!at_eof()) {
    if (consume('+')) {
      printf("  add rax, %d\n", expect_number());
      continue;
    }

    expect('-');
    printf("  sub rax, %d\n", expect_number());
  }


    /*
    if (*p == ' ') {
      p++;
      continue;
    }
    */
  printf("  ret\n");
  return 0;
}