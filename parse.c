#include "9cc.h"

// token が記号かつop に等しいかどうか
bool consume(char *op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len)) {
    return false;
  }

  token = token->next;
  return true;
}

void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  int pos = loc - user_input; // 文字列の途中のポインタ - 文字列の先頭のポインタ = 先頭からのバイト数
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, " ");
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}


// token が記号かつopに等しい→一つ進める
void expect(char *op) {
  if (token->kind != TK_RESERVED || token->len != strlen(op) || memcmp(token->str, op, token->len)) {
    error_at(token->str, "'%c'ではありません", op);
  }

  token = token->next;
}

int expect_number() {
  if (token->kind != TK_NUM) {
    error_at(token->str, "数ではありません");
  }

  int val = token->val;
  token = token->next;
  return val;
}

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}



bool at_eof() {
  return token->kind == TK_EOF;
}

Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  cur->next = tok;
  return tok;
}


Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}




Node *expr() {
  return equality();
}

Node *equality() {
  Node *node = relational();

  for (;;) {
    if (consume("==")) {
      node = new_node(ND_EQ, node, relational());
    } else if (consume("!=")) {
      node = new_node(ND_NOT_EQ, node, relational());
    } else {
      return node;
    }   
  }
}

Node *relational() {
  Node *node = add();

  for (;;) {
    if (consume("<")) {
      node = new_node(ND_LT, node, add());
    } else if (consume(">")) {
      node = new_node(ND_LT, add(), node);
    } else if (consume("<=")) {
      node = new_node(ND_LTE, node, add());
    } else if (consume("=>")) {
      node = new_node(ND_LTE, add(), node);
    } else {
      return node;
    }
  }
}

Node *add() {
  Node *node = mul();

  for(;;) {
    // トークンを一つ先読みして，次の挙動を決めている→LL(1)パーサー
    if (consume("+")) {
      node = new_node(ND_ADD, node, mul());
    } else if(consume("-")) {
      node = new_node(ND_SUB, node, mul());
    }
    else {
      return node;
    }
  }
}

Node *mul() {
  Node *node = unary();
  for (;;) {
    if (consume("*")) {
      node = new_node(ND_MUL, node, unary());
    } else if(consume("/")) {
      node = new_node(ND_DIV, node, unary());
    } else {
      return node;
    }
  }
}

Node *unary() {

  if (consume("+")) {
    return primary();
  } 
  
  if (consume("-")) {
    return new_node(ND_SUB, new_node_num(0), primary());
  } 
  
  return primary();
}

Node *primary() {
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  } 
  return new_node_num(expect_number()); 
}

bool start_with(char *str, char *word) {
  return memcmp(str, word, strlen(word)) == 0;
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

    if (strchr("+-*/()<>", *p)) {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }

    if (start_with(p, "==") || start_with(p, "!=") || start_with(p, "<=") || start_with(p, ">=")) {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p, 0);
      char *q = p;
      cur->val = strtol(p, &p, 10);
      cur->len = p - q;
      continue;
    }


    error_at(token->str, "invalid token");
  }

  new_token(TK_EOF, cur, p, 0);
  return head.next;
}