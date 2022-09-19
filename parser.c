#include <stdio.h>
#include "value.h"
#include "talloc.h"
#include "linkedlist.h"

#define ERROR_MORE 0 
#define ERROR_LESS 1 

void exitWithSyntaxError(int ERROR){
   if(ERROR == ERROR_MORE) 
    printf("Syntax error: too many close parentheses");
   else 
    printf("Syntax error: not enough close parentheses\n");
   texit(0);
}

void printPrimitive(Value *value) {
  switch (value->type){
    case INT_TYPE: 
      printf("%i", value->i);
      break;
    case DOUBLE_TYPE:
      printf("%lf", value->d);
      break;
    case STR_TYPE:
      printf("%s", value->s);
      break;
    case BOOL_TYPE:
      if(value->i == 0)
        printf("#f");
      if(value->i == 1)
        printf("#t");
      break;
    case SYMBOL_TYPE:
      printf("%s", value->s);
      break;
    default:
      break;
  }
}


void printTree(Value * head){
  Value* cur = head;
  while(cur->type != NULL_TYPE){
    if(cur->c.car->type == CONS_TYPE || 
        cur->c.car->type == NULL_TYPE) {
      printf("(");
      printTree(cur->c.car);
      printf(")");
    } else {
      printPrimitive(cur->c.car); 
    }
    cur = cur->c.cdr;
    if (cur->type != NULL_TYPE) printf(" ");
  }
}



Value * buildSubTree(Value* parseStack){
  Value* cur = parseStack;
  Value* tree = makeNull();
  while(cur->type != NULL_TYPE && 
        cur->c.car->type != OPEN_TYPE){
    tree = cons(cur->c.car, tree);
    cur = cur->c.cdr; 
  }
  if(cur->type == NULL_TYPE) { 
    exitWithSyntaxError(ERROR_MORE);
  }
  parseStack = cons(tree, cur->c.cdr);
  return parseStack; 
}


Value *parse(Value *tokens){
  Value* parseStack = makeNull(); 
  Value* cur = tokens; 
  int depth = 0;

  while (cur->type != NULL_TYPE){
    if(cur->c.car->type == CLOSE_TYPE){
      parseStack = buildSubTree(parseStack);
      depth++;
    } else {
      parseStack = cons(cur->c.car, parseStack);
      if(cur->c.car->type == OPEN_TYPE) depth--;
    }
    cur = cur->c.cdr; 
  }
  if (depth != 0){
    if (depth > 0) exitWithSyntaxError(ERROR_MORE); 
    else exitWithSyntaxError(ERROR_LESS);
  }
  Value* reversed = reverse(parseStack);
  return reversed;

}