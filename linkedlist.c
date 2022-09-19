#include "value.h"
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "talloc.h"

// Create a new NULL_TYPE value node.
Value *makeNull(){
  Value* value = talloc(sizeof(Value));
  value->type = NULL_TYPE; 
  return value;
}

// Create a new CONS_TYPE value node.
Value *cons(Value *newCar, Value *newCdr) {
    Value* value = talloc(sizeof(Value));
    value->type = CONS_TYPE;
    value->c.car = newCar;
    value->c.cdr = newCdr;
    return value;
}

// Display the contents of the linked list to the screen in some kind of
// readable format
void display(Value *list){
  switch(list->type){
    case CONS_TYPE:
      display(list->c.car);
      display(list->c.cdr);
      break;
    case INT_TYPE: 
      printf("%i ", list->i);
      break;
    case DOUBLE_TYPE:
      printf("%lf ", list->d);
      break;
    case STR_TYPE:
      printf("%s ", list->s);
      break;
    case NULL_TYPE:
      printf("\n");
      break;
    default:
      break;
  }
}

// Return a new list that is the reverse of the one that is passed in. All
// content within the list should be duplicated; there should be no shared
// memory whatsoever between the original list and the new one.
//
// FAQ: What if there are nested lists inside that list?
// ANS: There won't be for this assignment. There will be later, but that will
// be after we've got an easier way of managing memory.
Value *reverse(Value *list) {
    if (list->type != CONS_TYPE) { 
      return list;
    } else {
      Value* reversed = makeNull();
      while(list->type != NULL_TYPE){
        reversed = cons(list->c.car, reversed);
        list = list->c.cdr;
      }
      return reversed;
    }
}   

// Utility to make it less typing to get car value. Use assertions to make sure
// that this is a legitimate operation.
Value *car(Value *list) {
    assert(list->type == CONS_TYPE);
    return list->c.car;
}

// Utility to make it less typing to get cdr value. Use assertions to make sure
// that this is a legitimate operation.
Value *cdr(Value *list){
    assert(list->type == CONS_TYPE);
    return list->c.cdr;
}

// Utility to check if pointing to a NULL_TYPE value. Use assertions to make sure
// that this is a legitimate operation.
bool isNull(Value *value) {
    assert(value);
    if (value->type == NULL_TYPE) {
        return true;
    } else {
        return false;
    }
}

// Measure length of list. Use assertions to make sure that this is a legitimate
// operation.
int length(Value *value){
  assert(value->type == CONS_TYPE || value->type == NULL_TYPE); 
  if (value->type == NULL_TYPE)
    return 0; 
  else {
    return 1 + length(value->c.cdr);
  }
}