//Name: Joshua Rony
//UserID: jrony
//Assignment Name: pa2


#ifndef List_H

#define List_H

//This is the type that gets "exported" to the main.  This is how you make an object constructor in a procedural language like plain C.

typedef struct ListObject* List; //"typedef" allows you to make your own 'type' in plain C.  This is being exported to the main (lex.c) because Lex.c includes List.h, but not List.c
								//You now have a struct pointer of "ListObject" type that is called "List", and we know that List has two attributes - "head" and "tail", 
								//and that those two attributes in turn have three attributes of their own because they are structs themselves of the NodeObject type - so they have 
struct NodeObject;				//an "int data" attrubute", a "next" attribute, which is a recursively defined NodeObject type itself, and a "previous" attribute which is also a recursively defined
								//NodeObect type itself - and so thse two attributes, (next and previous) also have three attributes themselves, int data, next, and previous, and so on.
								//Very clever design.

struct ListObject;

List newList(void);

void append(List L, void *num);

void prepend(List L, void *num);

void Print(List L);

void moveBack(List L);

void moveFront(List L);

void * get(List L);

void * front(List L);

void * back(List L);

void insertBefore(List L, void *data);

void insertAfter(List L, void *data);

void moveNext(List L);

int find(List L, void *var);

void findDelete(List L, void *var);

void movePrevious(List L);

void deleteFront(List L);

int deleteBack(List L);

int delete(List L);

void freeList(List *pL);

int Index(List L);

int length(List L);

int equals(List A, List B);

void clear(List L);

void movePrev(List L);

void moveNext(List L);

List copyList(List L);

void printList(List L);

int moveTo(List L, int i);


#endif