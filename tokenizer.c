#include "value.h"
#include "talloc.h"
#include "linkedlist.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Value* tokenizeString(){
  Value* newValue = talloc(sizeof(Value));
  newValue->type = STR_TYPE; 
  char accumulator[305];
  int length = 1; 
  accumulator[0] = '"';
  char c;
  do {
    c = (char) fgetc(stdin);
    if (c == EOF){
      printf("Syntax Error! \n");
      texit(0);
    }
    accumulator[length] = c;
    length++;
  } while (c != '"');

  accumulator[length] = '\0';
  newValue->s = talloc((length+1) * sizeof(char));
  strcpy(newValue->s, accumulator);
  return newValue; 
}

int isNumber(char c){
  if (0x30 <= c && c<= 0x39) return 1; 
  else return 0;
}

int isLetter(char c){
  if ((0x61 <= c && c<= 0x7A )|| 
    (0x41 <= c && c<= 0x5A)) 
    return 1; 
  else return 0;
}    

int isSpecial(char c){
  if (c== '!' || c == '$' || c == '%' || c == '&' || 
    c == '*' || c == '/' || c == ':' || c == '<' ||
    c == '=' || c == '>' || c == '?' || c == '~' || 
    c == '_' || c == '^') return 1; 
  else return 0; 
}

int isSymbolSubsequent (char c){
  if (isNumber(c)) return 1; 
  else if (isSpecial(c)) return 1; 
  else if (isLetter(c)) return 1; 
  else if (c == '.' || c == '+' || c == '-') return 1;
  else return 0;
}

int isSymbolInitial (char c){
  if (isLetter(c) || isSpecial(c)) return 1;
  else return 0; 
}

Value* createParenthesisValue(valueType parenthesisType, char c){
  Value* newValue = talloc(sizeof(Value));
  newValue->type = parenthesisType;
  newValue->s = talloc(sizeof(char));
  *(newValue->s) = c; 
  return newValue; 
}

Value* createBoolValue(int boolean){
  Value* newValue = talloc(sizeof(Value));
  newValue->type = BOOL_TYPE;
  newValue->i = boolean; 
  return newValue; 
}

Value* createPlusMinusValue(char c){
  Value* newValue = talloc(sizeof(Value));
  newValue->type = SYMBOL_TYPE;
  newValue->s = talloc(2 * sizeof(char));
  newValue->s[0] = c;
  newValue->s[1] = '\0';
  return newValue; 
}

Value *handleNumber(char sign, char firstNumber) {
  Value* newValue = talloc(sizeof(Value));
  
  char accumulator[305]; 
  int length = 1;
  accumulator[0] = sign; 
  char c = firstNumber;
  int periods = 0; 
  int numbers = 0;
  

  while(isNumber(c) || c == '.'){
    if (isNumber(c)) numbers++;
    if (c == '.') periods++;
    accumulator[length] = c;
    length++;
    c = (char)fgetc(stdin);
  } 
  accumulator[length] = '\0';
  if (periods > 1 || numbers == 0){
    printf("Syntax Error! \n");
    texit(0);
  } else if (periods == 1){
    newValue->type = DOUBLE_TYPE;
    char * nullPtr; 
    newValue->d = strtod(accumulator, &nullPtr);
  } else {
    newValue->type = INT_TYPE; 
    char * nullPtr; 
    newValue->i = strtol(accumulator, &nullPtr, 10); 
  }
  ungetc(c, stdin);
  return newValue; 
}

void handleComment(){
  char charRead = (char)fgetc(stdin);
  while(charRead != '\n' && charRead != EOF) 
    charRead = (char)fgetc(stdin);
} 

Value * handleSymbol(char initial){
  Value* newValue = talloc(sizeof(Value));
  
  char accumulator[305]; 
  int length = 0;
  char c = initial;

  while(isSymbolSubsequent(c)){
    accumulator[length] = c;
    length++;
    c = (char)fgetc(stdin);
  } 
  accumulator[length] = '\0';
  newValue->type = SYMBOL_TYPE; 
  newValue->s = talloc((length+1)*sizeof(char));
  strcpy(newValue->s, accumulator);
  ungetc(c, stdin);
  return newValue; 
}

// Read all of the input from stdin, and return a linked list consisting of the
// tokens.
Value *tokenize(){
  char charRead;
  Value *list = makeNull();
  charRead = (char)fgetc(stdin);
  
  while (charRead != EOF) {
    if (charRead == '('){
      list = cons(createParenthesisValue(OPEN_TYPE, charRead), list);
    } else if (charRead == ')'){
      list = cons(createParenthesisValue(CLOSE_TYPE, charRead), list);
    } else if (charRead == '\n' || charRead == ' '){
      charRead = (char)fgetc(stdin);
      continue; 
    } else if (charRead == '#') {
        char next = (char)fgetc(stdin);
        if (next == 'f') {
          list = cons(createBoolValue(0), list);
        } else if (next == 't') {
          list = cons(createBoolValue(1), list);
        } else {
            printf("Syntax Error!\n");
            texit(0);
        }
    } else if (charRead == ';') {
      handleComment(); 
    } else if (charRead == '"') {
        list = cons(tokenizeString(), list);
    } else if (charRead == '+' || charRead == '-') {
        char sign = charRead;
        charRead = (char)fgetc(stdin);
        if (isNumber(charRead) || charRead == '.') {
          list = cons(handleNumber(sign, charRead), list);
        } else if (charRead == ')' || charRead == '(' 
          || charRead == '\n' || charRead == ' ') {
          list = cons(createPlusMinusValue(sign), list);
          continue;
        }
        else {
          printf("Syntax Error! '+' or '-' followed by an invalid character");
          texit(0);
        }
    } else if (isNumber(charRead)){
      list = cons(handleNumber('+', charRead), list);
    } else if (isSymbolInitial(charRead)){
        list = cons(handleSymbol(charRead), list);
    } else {
      printf("Syntax Error!\n");
      texit(0);
    }
    charRead = (char)fgetc(stdin);
  }
  Value *revList = reverse(list); 
  return revList; 
}

// Displays the contents of the linked list as tokens, with type information
void displayTokens(Value *list) {
    if (list->type == NULL_TYPE) return; 
    switch (list->c.car->type){
      case INT_TYPE: 
        printf("%i:integer\n", list->c.car->i);
        break;
      case DOUBLE_TYPE:
        printf("%lf:double\n", list->c.car->d);
        break;
      case STR_TYPE:
        printf("%s:string\n", list->c.car->s);
        break;
      case BOOL_TYPE:
        if(list->c.car->i == 0)
          printf("#f:boolean\n");
        if(list->c.car->i == 1)
          printf("#t:boolean\n");
        break;
      case OPEN_TYPE:
        printf("(:open\n");
        break;
      case CLOSE_TYPE:
        printf("):close\n");
        break;
      case SYMBOL_TYPE:
        printf("%s:symbol\n", list->c.car->s);
        break;
      default:
        break;
    }
    displayTokens(list->c.cdr);
}
