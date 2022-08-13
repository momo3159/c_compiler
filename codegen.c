#include "9cc.h"

void gen(Node *node) {
  if (node->kind == ND_NUM) {
    printf("  push %d\n", node->val);
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind) {
    case ND_ADD:
      printf("  add rax, rdi\n");
      break;
    case ND_SUB:
      printf("  sub rax, rdi\n");
      break;
    case ND_MUL:
      printf("  imul rax, rdi\n");
      break;
    case ND_DIV:
      printf("  cqo\n");
      printf("  idiv rdi\n");
      break;
    case ND_EQ:
      printf("  pop rdi\n");
      printf("  pop rax\n");
      printf("  cmp rax, rdi");
      printf("  sete al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_NOT_EQ:
      printf("  pop rdi\n");
      printf("  pop rax\n");
      printf("  cmp rax, rdi");
      printf("  setne al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_LT:
      printf("  pop rdi\n");
      printf("  pop rax\n");
      printf("  cmp rax, rdi");
      printf("  setl al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_LTE:
      printf("  pop rdi\n");
      printf("  pop rax\n");
      printf("  cmp rax, rdi");
      printf("  setle al\n");
      printf("  movzb rax, al\n");
      break;
  }

  printf("  push rax\n");
}

