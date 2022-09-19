#include <stdio.h>
#include "value.h"
#include "talloc.h"
#include "linkedlist.h"
#include "interpreter.h"
#include <string.h>


/* ---- General error checking ----*/ 
#define LOOK_UP_ERROR 0
#define PROCEDURE_ERROR 1
#define IF_ERROR 2
#define LET_ERROR 3
#define QUOTE_ERROR 4
#define DEFINE_ERROR 5
#define LAMBDA_ERROR 6
#define EXECUTE_ERROR 7
#define SUM_ERROR 8
#define IS_NULL_ERROR 9
#define CAR_ERROR 10
#define CDR_ERROR 11
#define CONS_ERROR 12
#define SET_ERROR 13
#define AND_ERROR 14
#define OR_ERROR 15
#define SUBTRACT_ERROR 16
#define LESS_THAN_ERROR 17
#define GREATER_THAN_ERROR 18
#define EQUAL_ERROR 19
#define MULTIPLICATION_ERROR 20
#define DIVISION_ERROR 21
#define MODULO_ERROR 22
#define COND_ERROR 23


void error(int errorType){ 
  switch(errorType){
    case LOOK_UP_ERROR: 
      printf("Evaluation Error: Unbound variable error\n");
      break;
    case PROCEDURE_ERROR: 
      printf("Evaluation Error: Unable to evaluate this type\n");
      break;
    case IF_ERROR: 
      printf("Evaluation Error: Error in special type 'if'!\n");
      break;
    case LET_ERROR:
      printf("Evaluation Error: Error in special type 'let'!\n");
      break;
    case QUOTE_ERROR:
      printf("Evaluation Error: Error in special type 'quote'!\n");
      break;
    case DEFINE_ERROR:
      printf("Evaluation Error: Error in special type 'define'!\n");
      break;
    case LAMBDA_ERROR:
      printf("Evaluation Error: Error in special type 'lambda'!\n");
      break;
    case EXECUTE_ERROR: 
      printf("Evaluation Error: Error in function execution\n");
      break;
    case SUM_ERROR: 
      printf("Evaluation Error: Error in primitive function sum\n");
      break;
    case IS_NULL_ERROR: 
      printf("Evaluation Error: Error in primitive function null?\n");
      break;
    case CAR_ERROR: 
      printf("Evaluation Error: Error in primitive function car\n");
      break;
    case CDR_ERROR: 
      printf("Evaluation Error: Error in primitive function cdr\n");
      break;
    case CONS_ERROR: 
      printf("Evaluation Error: Error in primitive function cons\n");
      break;
    case SET_ERROR: 
      printf("Evaluation Error: Error in special type 'set!'\n");
      break;
    case AND_ERROR: 
      printf("Evaluation Error: Error in special type 'and'\n");
      break;
    case OR_ERROR: 
      printf("Evaluation Error: Error in special type 'or'\n");
      break;
    case SUBTRACT_ERROR: 
      printf("Evaluation Error: Error in primitive function subtract\n");
      break;
    case LESS_THAN_ERROR: 
      printf("Evaluation Error: Error in primitive function <\n");
      break;
    case GREATER_THAN_ERROR: 
      printf("Evaluation Error: Error in primitive function >\n");
      break;
    case EQUAL_ERROR: 
      printf("Evaluation Error: Error in primitive function =\n");
      break;
    case MULTIPLICATION_ERROR: 
      printf("Evaluation Error: Error in primitive function multiplication\n");
      break;
    case DIVISION_ERROR: 
      printf("Evaluation Error: Error in primitive function division\n");
      break;
    case MODULO_ERROR: 
      printf("Evaluation Error: Error in primitive function modulo\n");
      break;
    case COND_ERROR: 
      printf("Evaluation Error: Error in special type 'cond'\n");
      break;
    default:
      break; 
  }
  texit(0); 
}


/* ---- Printing ----*/ 
void printNonList(Value * value){
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
    case CLOSURE_TYPE: 
      printf("#<procedure>"); 
      break; 
    case PRIMITIVE_TYPE: 
      printf("#<procedure>"); 
      break; 
    default:
      break;
  }
}

void printList(Value * head){
  Value* cur = head;
  while(cur->type == CONS_TYPE){
    if(car(cur)->type == CONS_TYPE || 
        car(cur)->type == NULL_TYPE) {
      printf("(");
      printList(car(cur));
      printf(")");
    } else {
      printNonList(car(cur)); 
    }
    cur = cdr(cur);
    if (cur->type != NULL_TYPE) printf(" ");
  }

  if(cur->type != NULL_TYPE){
    printf(". ");
    printNonList(cur); 
  }
}

void printLineList(Value * list) {
  printf("(");
  printList(list);
  printf(")\n");
}

void printLineNonList(Value * list) {
  printNonList(list);
  printf("\n");
}

void displayResult(Value *value) {
  if (value->type == CONS_TYPE || value->type == NULL_TYPE)
    printLineList(value);
  else if (value->type != VOID_TYPE)
    printLineNonList(value); 
}

/* --- General use functions, isPrimitive, evalBodies, addNewBinding, inits --- */

int isPrimitive(Value * expr){ 
  if (expr->type == INT_TYPE ||
     expr->type == BOOL_TYPE || 
     expr->type == STR_TYPE || 
     expr->type == DOUBLE_TYPE)
    return 1;
  else return 0; 
}

/* For use in let and lambda body evaluation */
Value *evalBodies(Value * bodies, Frame * frame){ 
  Value * out; 
  while(bodies->type != NULL_TYPE){
    out = eval(car(bodies), frame);
    bodies = cdr(bodies);  
  }
  return out; 
}

/* Binds symbol and value to bindingsList*/
Value* addNewBinding(Value * bindingsList, Value * symbol, Value * value){
  return cons(cons(symbol, value), bindingsList); 
}

Frame* initFrame(Value * bindings, Frame * parent){
    Frame * newFrame = talloc(sizeof(Frame));
    newFrame->parent = parent;
    newFrame->bindings = bindings;
    return newFrame; 
}

Value * initClosure(Value * args, Value * body, Frame* frame){
  Value * closure = makeNull();

  closure->type = CLOSURE_TYPE; 
  closure->cl.frame = frame; 
  closure->cl.paramNames = args; 
  closure->cl.functionCode = body; 

  return closure;  
}

Value * initBool(int boolInt){
  Value * newBool = makeNull(); 
  newBool->type = BOOL_TYPE; 
  newBool->i = boolInt; 
  return newBool;
}

Value* initInt(int val){
  Value * integer = makeNull(); 
  integer->type = INT_TYPE;
  integer->i = val;
  return integer; 
}

Value * initSymbol(char *name){
  Value * symbol = makeNull(); 
  symbol->type = SYMBOL_TYPE; 
  symbol->s = name; 
  return symbol;
}

Value * initPrimitive(Value *(*function)(Value *)){
  Value * primitive = makeNull(); 
  primitive->type = PRIMITIVE_TYPE; 
  primitive->pf = function; 
  return primitive; 
}

Value * initLetErrorType(){
  Value * letError = makeNull(); 
  letError->type = CLOSE_TYPE; 
  return letError; 
}

Value * initVoidType(){
  Value * voidType = makeNull(); 
  voidType->type = VOID_TYPE; 
  return voidType; 
}

/* --- Symbol Look Up --- */ 

Value* lookUpSymbolInLocalBinding(Value* target, Value *bindings) {
    char * targetSymbol = target->s; 
    while(bindings->type != NULL_TYPE) {
        char * curSymbol = car(car(bindings))->s;
        if(!strcmp(curSymbol, targetSymbol))
          return cdr(car(bindings));
        bindings = cdr(bindings);
    }
    return NULL; 
}

Value* lookUpSymbol(Value* target, Frame* localFrame){
  Value * value = lookUpSymbolInLocalBinding(target, localFrame->bindings);

  if (value == NULL){
    if(localFrame->parent == NULL) error(LOOK_UP_ERROR);
    return lookUpSymbol(target, localFrame->parent);
  } else {
    if(value->type == CLOSE_TYPE){
      error(LET_ERROR); 
    }
    return value;
  }
}

/* --- Special Types --- */ 

/* IF */

void ifIsSize3List(Value* list){
  if(list->type == NULL_TYPE || 
    cdr(list)->type == NULL_TYPE || 
    cdr(cdr(list))->type == NULL_TYPE || 
    cdr(cdr(cdr(list)))->type != NULL_TYPE)
    error(IF_ERROR); 
}

Value *evalIf(Value *expr, Frame* frame){
  
  ifIsSize3List(expr); 
  
  int test = eval(car(expr), frame)->i;
  if (test == 1)
    return eval(car(cdr(expr)), frame);
  else 
    return eval(car(cdr(cdr(expr))), frame); 
}

/* LET */

void letIsNonNullList(Value * list){
  if(list->type == NULL_TYPE) error(LET_ERROR); 
}

void letIsSize2List(Value * list){
  if(list->type != CONS_TYPE || 
      cdr(list)->type != CONS_TYPE || 
      cdr(cdr(list))->type != NULL_TYPE ||
      car(list)->type != SYMBOL_TYPE)
      error(LET_ERROR); 
}

void letIsRepeatedSymbol(Value * newSymbol, Value * bindings){
  if (lookUpSymbolInLocalBinding(newSymbol, bindings) != NULL) 
    error(LET_ERROR);
}

void letIsList(Value * list){
  if(list->type != CONS_TYPE &&
    list->type != NULL_TYPE) 
    error(LET_ERROR);
}

/* Frame management in let */

/* Returns a LIST of bindings in Frame->bindings format.
 * Frame->bindings format is a list of cons pairs,
 * car is the symbol, cdr is the value. 
 * The input is the argument for the let special type,
 * without evaluation. 
*/
Value * buildBindings(Value * letBindings, Frame* frame){
  Value * bindings = makeNull(); 

  Value* toBind; 
  Value* symbol; 
  Value* value; 
  
  while(letBindings->type != NULL_TYPE){
    toBind = car(letBindings); 
    
    letIsSize2List(toBind); 
    
    symbol = car(toBind); 
    letIsRepeatedSymbol(symbol, bindings);
    
    value = eval(car(cdr(toBind)), frame); 
    
    bindings = addNewBinding(bindings, symbol, value); 
    letBindings = cdr(letBindings);      
  }

  return bindings;
}

Frame * buildFrame(Value* letBindings, Frame * frame){
  Value * frameBindings = buildBindings(letBindings, frame);
  Frame * localFrame = initFrame(frameBindings, frame); 

  return localFrame; 
} 

Value *evalLet(Value *expr, Frame* frame){

  Value * letBindings = car(expr);
  letIsList(letBindings); 
  Frame * localFrame = buildFrame(letBindings, frame); 
  
  Value * bodiesExpr = cdr(expr);  
  letIsNonNullList(bodiesExpr); 

  return evalBodies(bodiesExpr, localFrame); 
}

/* LET* */ 

Frame * buildLetStarFrame(Value * letBindings, Frame * frame){
  Value * cur = letBindings; 
  Frame * prevFrame = frame; 
  Value * toBind;
  Value * symbol;
  Value * value;  

  while(cur->type != NULL_TYPE){
    toBind = car(cur); 
    letIsSize2List(toBind); 

    symbol = car(toBind); 
    value = eval(car(cdr(toBind)), prevFrame);
    prevFrame = initFrame(addNewBinding(makeNull(), symbol, value), prevFrame); 

    cur = cdr(cur); 
  }

  return prevFrame; 
}

Value * evalLetStar(Value * expr, Frame * frame){
  Value * letBindings = car(expr);
  letIsList(letBindings); 

  Frame * localFrame = buildLetStarFrame(letBindings, frame); 

  Value * bodiesExpr = cdr(expr);  
  letIsNonNullList(bodiesExpr); 

  return evalBodies(bodiesExpr, localFrame); 
}

/* LETREC */

void buildBindingsLetRec(Value * bindings, Frame * frame){
  Value * cur = bindings; 
  Value * toBind; 
  Value * value; 
  Value * symbol;
  Value * dummyBindings = makeNull();
  Value * letError = initLetErrorType(); 

  while(cur->type != NULL_TYPE){
    toBind = car(cur); 
    letIsSize2List(toBind); 
    symbol = car(toBind); 
    letIsRepeatedSymbol(symbol, dummyBindings);
    
    dummyBindings = addNewBinding(dummyBindings, symbol, letError);

    cur = cdr(cur);
  }

  frame->bindings = dummyBindings; 
  
  Value * trueBindings = makeNull();
  cur = bindings;
  while(cur->type != NULL_TYPE){    
    value = car(cdr(car(cur)));
    value = eval(value, frame);  
    symbol = car(car(cur)); 
    trueBindings = addNewBinding(trueBindings, symbol, value); 
    cur = cdr(cur); 
  }

  frame->bindings = trueBindings; 
}

Value * evalLetRec(Value * expr, Frame * frame){
  Value * letBindings = car(expr);
  letIsList(letBindings); 

  Frame * localFrame = initFrame(NULL, frame);
  buildBindingsLetRec(letBindings, localFrame); 

  Value * bodiesExpr = cdr(expr);  
  letIsNonNullList(bodiesExpr); 
  
  return evalBodies(bodiesExpr, localFrame); 
}

/* QUOTE */

void quoteIsSize1List(Value *list){
  if(list->type !=CONS_TYPE || cdr(list)->type != NULL_TYPE)
    error(QUOTE_ERROR); 
}

Value *evalQuote(Value *arg){
  quoteIsSize1List(arg); 
  return car(arg); 
}

/* DEFINE */

void defineIsSymbolBind(Value *list){
  if(list->type != CONS_TYPE || 
     cdr(list)->type != CONS_TYPE || 
     cdr(cdr(list))->type != NULL_TYPE ||
     car(list)->type != SYMBOL_TYPE) 
    error(DEFINE_ERROR);  
}

Value *evalDefine(Value *args, Frame *frame) {
  defineIsSymbolBind(args); 
  
  Value *value = eval(car(cdr(args)), frame); 
  Value *binding = cons(car(args), value);
  frame->bindings = cons(binding, frame->bindings);
  
  return initVoidType(); 
}

/* SET! */

void setIsSymbolBind(Value *list){
  if(list->type != CONS_TYPE || 
     cdr(list)->type != CONS_TYPE || 
     cdr(cdr(list))->type != NULL_TYPE ||
     car(list)->type != SYMBOL_TYPE) 
    error(SET_ERROR);  
}

void lookUpAndChange(Value * target, Value * value, Frame* frame){ 
  Value * bindings = frame->bindings; 
  char * targetSymbol = target->s; 
  while(bindings->type != NULL_TYPE) {
    char * curSymbol = car(car(bindings))->s;
    if(!strcmp(curSymbol, targetSymbol)){
      bindings->c.car->c.cdr = value;
      return; 
    }
    bindings = cdr(bindings);
  }
  if (frame->parent == NULL)
    error(LOOK_UP_ERROR);
  else 
    lookUpAndChange(target, value, frame->parent); 
}

Value *evalSet(Value *args, Frame *frame) {
  setIsSymbolBind(args); 
  
  Value *value = eval(car(cdr(args)), frame); 
  Value *symbol = car(args); 

  lookUpAndChange(symbol, value, frame); 
   
  return initVoidType(); 
}

/* LAMBDA */

void lambdaIsListBiggerThan2(Value *list){
  if (list->type != CONS_TYPE || cdr(list)->type != CONS_TYPE)
    error(LAMBDA_ERROR); ; 
}

void lambdaIsUniqueSymbol(Value * list){
  if (list->type != NULL_TYPE){
    Value * cur = cdr(list); 
    char* symbol = car(list)->s; 

    while(cur->type != NULL_TYPE){
      if(!strcmp(symbol, car(cur)->s))
        error(LAMBDA_ERROR);
      cur = cdr(cur); 
    }
    lambdaIsUniqueSymbol(cdr(list)); 
  }
}

void lambdaIsAllSymbols(Value *list){
  Value * cur = list; 
  while(cur->type != NULL_TYPE){
    if(car(cur)->type != SYMBOL_TYPE) error(LAMBDA_ERROR); 
    cur = cdr(cur); 
  }
}


Value * evalLambda(Value* exprArgs, Frame *frame){
  lambdaIsListBiggerThan2(exprArgs); 
  lambdaIsAllSymbols(car(exprArgs)); 
  lambdaIsUniqueSymbol(car(exprArgs)); 
  return initClosure(car(exprArgs), cdr(exprArgs), frame); 
}

/* BEGIN */

Value* evalBegin(Value* expr, Frame *frame){
  Value * last = initVoidType(); 
  Value * cur = expr; 
  while(cur->type != NULL_TYPE){
    last = eval(car(cur), frame);
    cur = cdr(cur);
  }
  return last; 
}

/* AND */

void andIsSizeAtLeast1List(Value * list){
  if(list->type == NULL_TYPE)
    error(AND_ERROR); 
}

Value * evalAnd(Value *args, Frame * frame) {
  andIsSizeAtLeast1List(args);
  Value * cur = args;
  Value * last;
  while (cur->type != NULL_TYPE) {
    last = eval(car(cur), frame);
    if (last->type == BOOL_TYPE && last->i == 0) {
      return last;
    }
    cur = cdr(cur); 
  }
  return last;
}

/* OR */

void orIsSizeAtLeast1List(Value * list){
  if(list->type == NULL_TYPE)
    error(OR_ERROR); 
}

Value * evalOr(Value *args, Frame * frame) {
  orIsSizeAtLeast1List(args);
  Value * cur = args; 
  Value * falseValue = initBool(0);
  while (cur->type != NULL_TYPE) {
    Value * last = eval(car(cur), frame);
    if (last->type != BOOL_TYPE || last->i != 0) {
      return last;
    }
    cur = cdr(cur); 
  }
  return falseValue;
}

/* COND */

void condIsLast(Value *list){
  if(cdr(list)->type != NULL_TYPE)
    error(COND_ERROR); 
}

Value * evalCond(Value * expr, Frame * frame){
  Value * cur = expr; 
  Value * condPair; 
  Value * cond; 
  Value * op; 
  while(cur->type != NULL_TYPE){
    condPair = car(cur); 
    cond = car(condPair); 
    int isElse = cond->type == SYMBOL_TYPE && !strcmp("else", cond->s);  
    if(isElse || eval(cond, frame)->i == 1){
      if(isElse) condIsLast(cur); 
      op = car(cdr(condPair));
      return eval(op, frame); 
    }
    cur = cdr(cur); 
  }
  return initVoidType(); 
}

/* --- Primitive Functions ---*/

int isTwoNumbers(Value *args) {
  Value *cur = args;
  if (cur->type != CONS_TYPE || 
      cdr(cur)->type != CONS_TYPE ||
      cdr(cdr(cur))->type != NULL_TYPE)
    return 0;

  if ((car(cur)->type != INT_TYPE && 
      car(cur)->type != DOUBLE_TYPE) ||
      (car(cdr(cur))->type != INT_TYPE &&
      car(cdr(cur))->type != DOUBLE_TYPE))
    return 0;
  
  return 1; 
}

/* SUM */
Value *sumPrimitive(Value * args){
    
  Value *sum = initInt(0); 
  Value * cur = args;
  while(cur->type != NULL_TYPE) {
    if (car(cur)->type == DOUBLE_TYPE) {
      if (sum->type == INT_TYPE) {
        sum->type = DOUBLE_TYPE;
        sum->d = (double) sum->i; 
      }
      sum->d += car(cur)->d;
    } else if (car(cur)->type == INT_TYPE) {
      if (sum->type == INT_TYPE)
        sum->i += car(cur)->i;
      else sum->d += (double) car(cur)->i;
    } else {
      error(SUM_ERROR);
    }
    
    cur =cdr(cur); 
  }
  return sum; 
}

/* SUBTRACT */

void subtractIsTwoNumbers(Value * args){
  if(!isTwoNumbers(args))
    error(SUBTRACT_ERROR);
  }

Value *subtractPrimitive(Value *args) {
  subtractIsTwoNumbers(args);
  Value *cur = args; 
  Value *ans = makeNull(); 
  Value *first = car(cur);
  Value *second = car(cdr(cur));
  if (first->type == INT_TYPE) {
    if (second->type == INT_TYPE) {
      ans->type = INT_TYPE; 
      ans->i = first->i - second->i;
    } else { /* second is DOUBLE_TYPE */
      ans->type = DOUBLE_TYPE;
      ans->d = ((double) first->i) - second->d;
    }
  } else { /* first is DOUBLE_TYPE */
    ans->type = DOUBLE_TYPE; 
    ans->d = first->d;
    if (second->type == INT_TYPE) {
      ans->d -= (double) second->i;
    } else { /* second is DOUBLE_TYPE */
      ans->d -= second->d;
    }
  }
  return ans;
}

/* MULTIPLICATION */

void multiplicationIsListBiggerThan2(Value * list){
  if(list->type != CONS_TYPE || cdr(list)->type != CONS_TYPE)
    error(MULTIPLICATION_ERROR);
}

Value *multiplicationPrimitive(Value * args){
    
  Value *ans = initInt(1); 
  Value * cur = args;
  while(cur->type != NULL_TYPE) {
    if (car(cur)->type == DOUBLE_TYPE) {
      if (ans->type == INT_TYPE) {
        ans->type = DOUBLE_TYPE;
        ans->d = (double) ans->i; 
      }
      ans->d *= car(cur)->d;
    } else if (car(cur)->type == INT_TYPE) {
      if (ans->type == INT_TYPE)
        ans->i *= car(cur)->i;
      else ans->d *= (double) car(cur)->i;
    } else {
      error(SUM_ERROR);
    }
    
    cur =cdr(cur); 
  }
  return ans; 
}

/* DIVISION */

void divisionIsTwoNumbers(Value * args){
  if(!isTwoNumbers(args))
    error(SUBTRACT_ERROR);
  }

Value *divisionPrimitive(Value *args) {
  divisionIsTwoNumbers(args);
  Value *cur = args; 
  Value *ans = makeNull(); 
  Value *first = car(cur);
  Value *second = car(cdr(cur));
  if (first->type == INT_TYPE) {
    if (second->type == INT_TYPE) {
      int firstInt = first->i; 
      int secondInt = second->i; 
      if (firstInt % secondInt == 0){
        ans->type = INT_TYPE; 
        ans->i = firstInt / secondInt; 
      } else {
        ans->type = DOUBLE_TYPE; 
        ans->d = ((double) firstInt) / ((double) secondInt);
      }
    } else { /* second is DOUBLE_TYPE */
      ans->type = DOUBLE_TYPE;
      ans->d =  first->i / second->d;
    }
  } else { /* first is DOUBLE_TYPE */
    ans->type = DOUBLE_TYPE; 
    ans->d = first->d;
    if (second->type == INT_TYPE) {
      ans->d = ans->d / second->i;
    } else { /* second is DOUBLE_TYPE */
      ans->d = ans->d / second->d;
    }
  }
  return ans;
}

/* MODULO */

void moduloIsTwoNumbers(Value * args){
  if(!isTwoNumbers(args))
    error(MODULO_ERROR);
  }
void moduloIsNonZero(int nonZero){
  if (nonZero == 0)
    error(MODULO_ERROR);
}

Value *moduloPrimitive(Value *args) {
  moduloIsTwoNumbers(args);
  Value *cur = args;  
  Value *first = car(cur);
  Value *second = car(cdr(cur));
  int firstInt = first->i; 
  int secondInt = second->i;
  moduloIsNonZero(secondInt); 
  int modulo = first->i % second->i; 
  
  if(modulo == 0) return initInt(0);
  
  if( (firstInt > 0 && secondInt < 0) || 
    (firstInt < 0 && secondInt > 0) ){
    modulo += secondInt;
  } 
    return initInt(modulo); 
}

/* LESS THAN */

void lessThanIsTwoNumbers(Value * args){
  if(!isTwoNumbers(args))
    error(LESS_THAN_ERROR);
  }

Value *lessThanPrimitive(Value *args) {
  lessThanIsTwoNumbers(args);
  int lessThan = 0; 
  Value *cur = args;  
  Value *first = car(cur);
  Value *second = car(cdr(cur));
  if (first->type == INT_TYPE) {
    if (second->type == INT_TYPE) {
      if (first->i < second->i) lessThan = 1; 
    } else { /* second is DOUBLE_TYPE */
      if (first->i < second->d) lessThan = 1; 
    }
  } else { /* first is DOUBLE_TYPE */
    if (second->type == INT_TYPE) {
      if (first->d < second->i) lessThan = 1; 
    } else { /* second is DOUBLE_TYPE */
      if (first->d < second->d) lessThan = 1; 
    }
  }
  return initBool(lessThan);
}

/* GREATER THAN */

void greaterThanIsTwoNumbers(Value * args){
  if(!isTwoNumbers(args))
    error(GREATER_THAN_ERROR);
  }

Value *greaterThanPrimitive(Value *args) {
  greaterThanIsTwoNumbers(args);
  int greaterThan = 0; 
  Value *cur = args;  
  Value *first = car(cur);
  Value *second = car(cdr(cur));
  if (first->type == INT_TYPE) {
    if (second->type == INT_TYPE) {
      if (first->i > second->i) greaterThan = 1; 
    } else { /* second is DOUBLE_TYPE */
      if (first->i > second->d) greaterThan = 1; 
    }
  } else { /* first is DOUBLE_TYPE */
    if (second->type == INT_TYPE) {
      if (first->d > second->i) greaterThan = 1; 
    } else { /* second is DOUBLE_TYPE */
      if (first->d > second->d) greaterThan = 1; 
    }
  }
  return initBool(greaterThan);
}

/* EQUAL */

void equalIsTwoNumbers(Value * args){
  if(!isTwoNumbers(args))
    error(EQUAL_ERROR);
  }

Value *equalPrimitive(Value *args) {
  equalIsTwoNumbers(args);
  int equal = 0;
  Value *cur = args;  
  Value *first = car(cur);
  Value *second = car(cdr(cur));
  
  if (first->type == INT_TYPE) {
    if (second->type == INT_TYPE) {
      if (first->i == second->i) equal = 1; 
    } else { /* second is DOUBLE_TYPE */
      if (first->i == second->d) equal = 1;
    }
  } else { /* first is DOUBLE_TYPE */
    if (second->type == INT_TYPE) { 
      if (first->d == second->i) equal = 1;
    } else { /* second is DOUBLE_TYPE */
      if (first->d == second->d) equal = 1;
    }
  }
  return initBool(equal);
}

/* NULL? */
void nullIsSize1List(Value * args){
  if(args->type != CONS_TYPE || cdr(args)->type != NULL_TYPE)
    error(IS_NULL_ERROR);
}

Value * isNullPrimitive(Value * args){
  nullIsSize1List(args);
  int isNull = 0; 

  if(car(args)->type ==  NULL_TYPE)
    isNull = 1; 
    
  return initBool(isNull); 
}

/* CAR */
void carIsValid(Value * args) {
  if(args->type != CONS_TYPE || 
    cdr(args)->type != NULL_TYPE || 
    car(args)->type != CONS_TYPE)
    error(CAR_ERROR);
}

Value * carPrimitive(Value * args) {
  carIsValid(args);
  return car(car(args));
}

/* CDR */
void cdrIsValid(Value * args) {
  if(args->type != CONS_TYPE || 
    cdr(args)->type != NULL_TYPE || 
    car(args)->type != CONS_TYPE)
    error(CDR_ERROR);
}

Value * cdrPrimitive(Value * args) {
  cdrIsValid(args);
  return cdr(car(args));
}

/* CONS */
void consIsValid(Value * args) {
  if(args->type != CONS_TYPE || 
    cdr(args)->type != CONS_TYPE ||
    cdr(cdr(args))->type != NULL_TYPE)
    error(CONS_ERROR);
}

Value * consPrimitive(Value * args) {
  consIsValid(args);
  return cons(car(args), car(cdr(args)));
}

/* Primitive functions binding in global frame */

void bind(char *name, Value *(*function)(Value *), Frame *frame) {
    Value *symbol = initSymbol(name); 
    Value *primitive = initPrimitive(function); 
    Value *bind = cons(symbol, primitive); 
    frame->bindings = cons(bind, frame->bindings); 
}

void bindPrimitives(Frame * frame){
  bind("-", &subtractPrimitive, frame); 
  bind("+", &sumPrimitive, frame); 
  bind("*", &multiplicationPrimitive, frame); 
  bind("/", &divisionPrimitive, frame);
  bind("modulo", &moduloPrimitive, frame);
  bind("<", &lessThanPrimitive, frame);
  bind(">", &greaterThanPrimitive, frame);
  bind("=", &equalPrimitive, frame);
  bind("cons", &consPrimitive, frame); 
  bind("cdr", &cdrPrimitive, frame); 
  bind("car", &carPrimitive, frame); 
  bind("null?", &isNullPrimitive, frame); 
}

/* --- Function execution --- */

/* Lambda evaluation */

/* Returns a LIST matching formal and real parameters, 
 * in the same format as in Frame->bindings
 */
Value * bindArguments(Value* symbols, Value *args){
  Value* bindings = makeNull(); 

  while(symbols->type != NULL_TYPE) {
      if(args->type == NULL_TYPE) error(EXECUTE_ERROR); 
      
      Value * value = car(args);
      Value * symbol = car(symbols);

      bindings = addNewBinding(bindings, symbol, value);
      symbols = cdr(symbols);
      args = cdr(args);
  }

  if(args->type != NULL_TYPE) error(EXECUTE_ERROR); 
  return bindings; 
}

/* Applies the given lambda with args. Args is a list 
 * of evaluated arguments */
Value * apply(Value* function, Value* args){
  Value * bindings = bindArguments(function->cl.paramNames, args); 
  Frame * closureFrame = initFrame(bindings, function->cl.frame); 
  return evalBodies(function->cl.functionCode, closureFrame);
}

/* General Execution */

/* Evaluates arguments and puts them in a new 
 * list with the same order*/
Value * evalArgs(Value *args, Frame* frame){
  Value * evaluated = makeNull(); 
  Value * cur = args; 
  while(cur->type != NULL_TYPE){
    evaluated = cons(eval(car(cur), frame), evaluated);
    cur = cdr(cur); 
  }
  evaluated = reverse(evaluated); 
  return evaluated; 
}

/* Evaluation of CONS_TYPE expressions */
void executeIsClosureOrPrimitive(Value *expr){ 
  if(expr->type != CLOSURE_TYPE && expr->type != PRIMITIVE_TYPE)
    error(EXECUTE_ERROR); 
}

/* EVALUATION OF PRIMITIVES AND LAMBDAS */
Value * executeExpression(Value *expr, Frame* frame){
  expr = evalArgs(expr, frame); 
  
  Value * function = car(expr);
  Value * args = cdr(expr);  
  
  executeIsClosureOrPrimitive(function); 

  Value * result; 
  if(function->type == CLOSURE_TYPE) 
    result = apply(function, args);
  else
    result = function->pf(args); 
  return result; 
}

/* Extension of eval. 
 * Handles cases within the CONS type case  in eval.*/
Value * evalExpression(Value *expr, Frame* frame){
    Value *args = cdr(expr);
    Value *func = car(expr);
    
    Value * result;
    if(func->type == SYMBOL_TYPE) {
      char * procedureSymbol = func->s; 
      if (!strcmp(procedureSymbol,"if")) 
        result = evalIf(args,frame);
      else if (!strcmp(procedureSymbol,"let"))
        result = evalLet(args,frame);
      else if (!strcmp(procedureSymbol,"let*")) 
        result = evalLetStar(args, frame);
      else if (!strcmp(procedureSymbol,"letrec")) 
        result = evalLetRec(args, frame);
      else if (!strcmp(procedureSymbol,"quote")) 
        result = evalQuote(args);
      else if (!strcmp(procedureSymbol,"define")) 
        result = evalDefine(args, frame);
      else if (!strcmp(procedureSymbol,"set!")) 
        result = evalSet(args, frame);
      else if (!strcmp(procedureSymbol,"lambda")) 
        result = evalLambda(args, frame);
      else if (!strcmp(procedureSymbol,"begin")) 
        result = evalBegin(args, frame);
      else if (!strcmp(procedureSymbol,"and")) 
        result = evalAnd(args, frame);
      else if (!strcmp(procedureSymbol,"or")) 
        result = evalOr(args, frame);
      else if (!strcmp(procedureSymbol,"cond")) 
        result = evalCond(args, frame);
      else 
        result = executeExpression(expr, frame);
    } else
      result = executeExpression(expr, frame);
    return result; 
}

/* ---- EVAL AND INTERPRET ---- */

Value *eval(Value *expr, Frame *frame){
  Value * result;
  if(isPrimitive(expr))
    result = expr; 
  else if (expr->type == SYMBOL_TYPE)
    result = lookUpSymbol(expr, frame); 
  else if (expr->type == CONS_TYPE) 
    result = evalExpression(expr, frame); 
  else
    error(PROCEDURE_ERROR);
  return result; 
}

void interpret(Value *tree){
  Frame * globalFrame =  initFrame(makeNull(), NULL); 

  bindPrimitives(globalFrame); 
  Value * curTree = tree; 
  while(curTree->type != NULL_TYPE){
    displayResult(eval(car(curTree), globalFrame)); 
    curTree = cdr(curTree); 
  }
}
