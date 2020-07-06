//Name:  Joshua Rony
//UserID: jrony


#include<stdlib.h>
#include<stdio.h>
#include <string.h>
#include <pthread.h>
#include "List.h"

pthread_mutex_t searchMutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct NodeObject{
	
	void *data;
	
	struct NodeObject* next; //each of these structs are themselves NodeObjects, and so they have their own struct NodeObject "next", "previous", and "data" attribiutes.
	
	struct NodeObject* previous;

}NodeObject;

typedef NodeObject* Node; 

Node getNewNode(void *num){
	
	Node temp = malloc(sizeof(NodeObject));
	
	temp->data = num;
	
	temp->next = NULL;
	
	temp->previous = NULL;
	
	return(temp);
}

typedef struct ListObject{
	
	Node head;
	
	Node tail;
	
	Node cursor;
	
	int length;
	
	int Index;

	
} ListObject;



//Constructors-Destructors----------------------------------------------------------------------------------------------------------------------------------

void freeNode(Node *pN){ //Looked at Prof. Tantalo's handouts for this function.  New addition from previous PA2 version of List.  Also had some help writing it.  WORKS SO WELL
	
	if(pN != NULL && *pN != NULL){
		
		free(*pN);
		
		*pN = NULL;
	}
}


List newList(void){
	
	List L;
	
	L = malloc(sizeof(*L));

	L->head = NULL;
	L->tail = NULL;
	L->cursor = NULL;
	L->Index = -1;
	L->length = 0;
	
		
	return(L);
}

void freeList(List *pL){
	
 if(pL != NULL && *pL != NULL){
 
	
 	Node temp = (*pL)->head;
	
	for(int x = 0; x < length((*pL)); x++){
		
		Node temp2 = temp;
		
		temp = temp->next;
		
		freeNode(&temp2);
	}
	
	free(*pL);
	
	*pL = NULL;
	
	}
	
}

//Access functions---------------------------------------------------------------------------------------------------------------------------

int length(List L){
	
	return(L->length);
	
}

int Index(List L){
	
	return(L->Index);
	
}


void * front(List L){
	
	if(L->length < 1){
		
		printf("List module front() function cannot retrieve data element from node in empty List.\n");
		
		exit(1);
	}
	
	else{
	
		return(L->head->data);
		
	}
}

void * back(List L){
	
	if(L->length < 1){
		
		//printf("List module back() function cannot retrieve data element from node in empty List.\n");
		
        return NULL;
		//exit(1);
	}
	
	else{
	
	
		return(L->tail->data);
	
	}
}

void * get(List L){
	
	if(L->length < 1){
		
		printf("List module get() function cannot retrieve data element from empty List.\n");

        return NULL;

		//exit(1);
	}
	
	if(L->Index < 0){
		
		printf("List module get() function cannot retrieve data element from undefined node.\n");
		
        return NULL;

		//exit(1);
	}  
	
	
	else if(L->cursor != NULL && L->Index > -1){
	
		return(L->cursor->data);
		
	}
	
	return NULL;
}

int equals(List A, List B){
	
	if(A->length == 0 && B->length == 0){
		
		return(1);
	}
	
	if(A->length != B->length){
		
		return(0);
	}
	
	else{
		
		Node temp = A->head;
		
		Node temp2 = B->head;
		
		for(int x =0; x < A->length; x++){
			
			if(temp->data != temp2->data){
				
				return(0);
			}
			
			temp = temp->next;
			
			temp2 = temp2->next;
		}
	
		
	}
	
	return(1);
}


//Manipulation procedures-------------------------------------------------------------------------------------------------------------------------

void clear(List L){
	
	if(L->length > 0){
		
		if(L->length == 1){
			
			L->head = NULL;
			
			L->tail = NULL;
			
			L->length = 0;
			
			L->cursor = NULL;
			
			L->Index = -1;
		}
		
		else{
			
			Node temp = L->head;
			
			for(int x = 0; x < L->length; x++){
				
				Node temp2 = temp;
				
				temp = temp->next;
				
				free(temp2);	
			}
			
			free(temp);
			
			L->head = NULL;
			
			L->tail = NULL;
			
			L->Index = -1;
			
			L->length = 0;
			
			L->cursor = NULL;
	
		}
	
	}
}

void moveFront(List L){
	
	if(L->length > 0){
		
		L->cursor = L->head;
		
		L->Index = 0;
	}
}

void moveBack(List L){
	
	if(L->length > 0){
		
		L->cursor = L->tail;
		
		L->Index = L->length - 1;
	}
}

void movePrev(List L){
	
	if(L->Index == 0){
		
		L->cursor = NULL;
		
		L->Index = -1;	
			
	}
		
	else if(L->cursor != NULL && L->Index > 0){
		
		L->cursor = L->cursor->previous;
		
		L->Index = L->Index - 1;
	}
	
}

void moveNext(List L){
	
	if(L->cursor != NULL && L->Index == L->length - 1){
		
		L->cursor = NULL;
		
		L->Index = -1;
	}
	
	else if(L->cursor != NULL && L->Index < L->length - 1){
		
		L->cursor = L->cursor->next;
		
		L->Index = L->Index + 1;
	}
}

void prepend(List L, void *num){
	
	if(L == NULL){
		
		printf("List module prepend() function cannot prepend node to undefined List.\n");
		
		exit(1);
	}
	
	Node nuNode = getNewNode(num);
	
	if(L->length == 0){
		
		L->head = nuNode;
		
		L->tail = nuNode;
		
		L->length = 1;
		
	}

	
	else{
		
		if(L->cursor != NULL){
			
			L->Index = L->Index + 1;
		}
		
		L->head->previous = nuNode;
		
		nuNode->next = L->head;
		
		L->head = nuNode;
		
		L->length = L->length + 1;
	}
	
}

void append(List L, void *num){
	
	if(L == NULL){
		
		printf("List module append() function cannot append node to undefined List.\n");
		
		exit(1);
	}
	
	Node nuNode = getNewNode(num);
	
	if(L->length == 0){
	
		L->head = nuNode;
	
	    L->tail = nuNode;
	
		L->length = 1;
	
	}
	
	
	
	
	else{
	
		L->tail->next = nuNode;
	
		nuNode->previous = L->tail;
	
		L->tail = nuNode;
	
		L->length = L->length + 1;		
	}
		
}

void insertBefore(List L, void *data){
	
	if(L->length <= 0){
		
		printf("List module insertBefore() function cannot insert element in empty List.\n");
		
		exit(1);
	}
	
	if(L->Index < 0){
		
		printf("List module insertBefore() function cannot insert element before undefined node.\n");
		
		exit(1);
	}
	
	if(L->Index == 0){
		
		prepend(L, data);

	}
	
	else{
	
		
		Node nuNode = getNewNode(data);
			
		L->cursor->previous->next = nuNode;
		
		nuNode->previous = L->cursor->previous;
		
		nuNode->next = L->cursor;
		
		L->cursor->previous = nuNode;
		
		L->length = L->length + 1;
		
		L->Index = L->Index + 1;
		
	}
}

void insertAfter(List L, void *data){
	
	if(L->length <= 0){
		
		printf("List module insertAfter() function cannot insert element in empty List.\n");
		
		exit(1);
	}
	
	if(L->Index < 0){
		
		printf("List module insertAfter() function cannot insert element after undefined node.\n");
		
		exit(1);
		
	}
	
	if(L->Index == L->length - 1){
		
		append(L, data);
	}
	
	
	
	else{
		
		Node nuNode = getNewNode(data);
		
		L->cursor->next->previous = nuNode;
		
		nuNode->next = L->cursor->next;
		
		nuNode->previous = L->cursor;
		
		L->cursor->next = nuNode;
		
		L->length = L->length + 1;
	}
	
}

void deleteFront(List L){
	
	if(L->length <= 0){
		
		printf("List module deleteFront() function cannot delete front element from empty List.\n");
		
		exit(1);
	}
	
	else if(L->length == 1){
		
		if(L->Index == 0){
			
			L->cursor = NULL;
			
			L->Index = -1;
		}
		
		L->length = 0;
		
		freeNode(&L->head);
	}
	
	else{
		
		if(L->cursor != NULL && L->cursor == L->head){
			
			L->cursor = NULL;
			
			L->Index = -1;
		}
		
		if(L->cursor != NULL && L->cursor != L->head){
			
			L->Index = L->Index - 1;
		}
		
		Node temp = L->head;
		
		L->head = L->head->next;
		
		L->head->previous = NULL;
		
		freeNode(&temp);
		
		L->length = L->length - 1;
	
		
	}
	
}

int deleteBack(List L){
	
	if(L->length < 1){
		
		printf("List module deleteBack() function cannot delete back element from empty List.\n");
		
        return -1;
		//exit(1);
	}
	
	else if(L->length == 1){
		
		if(L->cursor == L->tail || L->cursor == L->head){
			
			L->cursor = NULL;
			
			L->Index = -1;
		}
		
		
		
		freeNode(&L->head);
	
		L->length = 0;
	}
	
	else if(L->length > 1){
		
		if(L->cursor == L->tail){
			
			L->cursor = NULL;
			
			L->Index = -1;
		}
		
		Node temp = L->tail;
		
		L->tail = L->tail->previous;
		
		L->tail->next = NULL;
		
		freeNode(&temp);
		
		L->length = L->length - 1;
	
	}

    return 0;
		
}

int delete(List L){
	
	if(L->length == 0){
		
		printf("List module delete() function cannot delete node from empty List.");
		
        return -1;
		//exit(1);
	}

	
	if(L->Index == 0 && 0 != L->length - 1){//condition 1:  the cursor is at the head of the list, and the list has more than one element in it.
		
		
		Node temp = L->head;
		
		L->head = L->head->next;
		
		L->head->previous = NULL;
		
		freeNode(&temp);
		
		L->length = L->length - 1;
		
		L->cursor = NULL;
		
		L->Index = -1;
	}
	
	else if(L->Index == L->length - 1 && L->length != 1){ //condition 2: the cursor is at the end of the list, and the list has more than one element in it.
		
		Node temp = L->tail;
		
		L->tail = L->tail->previous;
		
		L->tail->next = NULL;
		
		freeNode(&temp);
		
		L->length = L->length - 1;
		
		L->cursor = NULL;
		
		L->Index = -1;
	}
	
	else if(L->Index != 0 && L->Index != L->length - 1){//condition 3:  the cursor is somewhere in the interior of the list.
		
		Node temp2 = L->cursor;
		
		L->cursor->next->previous = L->cursor->previous;
		
		L->cursor->previous->next = L->cursor->next;
		
		L->cursor = NULL;
		
		freeNode(&temp2);
		
		L->Index = -1;
		
		L->length = L->length - 1;
	}
	
	else if(L->length == 1){//condition 4:  the list only has one element in it.
		
		clear(L);
	}

    return 0;
	
}

void Print(List L){
	
	if(length(L) == 0){
		
		printf("Error.  Cannot print contents of empty List.\n");
		
		return;
	}
	
	Node temp = L->head;
	
	for(int x = 0; x < L->length; x++){
		
		printf("%ls%s", (int *)temp->data, " ");
		
		temp = temp->next;
	}
	
}

//Other operations------------------------------------------------------------------------------------------------------------------------

void printList(List L){
	
	if(L->head == NULL && L->tail == NULL){
		
		printf("List module printList function cannot print contents of empty List.\n");
		
		exit(1);
	}
	
	printf("%s\n", "Here's the list: ");

	Node temp = L->head;
	
	for(int x = 0; x < L->length; x++){
		
		printf("%p%s", temp->data, " ");
		
		temp = temp->next;
	}
	
	freeNode(&temp);
	
}

List copyList(List L){
	
	List copyList = newList();
	
	if(L->length == 0){
		
		return(copyList);
	}
	
	else{
		
		Node temp = L->head;
		
		for(int x = 0; x < L->length; x++){
			
			append(copyList, temp->data);
			
			temp = temp->next;
		}
		
	}
	
	return(copyList);
	
}

int find(List L, void *var){ //only used for string ops

	
	
	
	if(L->length == 0){

		printf("%s\n","List function find() cannot find string in empty list.");

		return -1;
	
	}

	else{

		

		L->cursor = L->head;
		L->Index = 0;

		for(int x = 0; x < L->length; x++){

			/*if(L->cursor == NULL){

				return -1;
			}*/
			//printf("%s%s\n", "Here is L->cursor->data from find() call: ", (char *)L->cursor->data);
			//printf("%s%s\n", "Here is var from find() call: ", (char *)var);
			//printf("%s%d\n", "Here is L->length from find() call:", L->length);

			if(x == (L->length - 1) && strcmp((char *)L->cursor->data, (char *)var) != 0){
				

				return -1;
			}

			if(strcmp((char *)L->cursor->data, (char *)var) == 0){

		

				//printf("%s%s\n", "Here is L->cursor->data: ", (char *)L->cursor->data);

				//printf("%s%ld\n", "here is strlen(L->cursor->data) : ", strlen(L->cursor->data));

				return L->Index;

			}

			L->cursor = L->cursor->next;

			L->Index++;
		}

	

	}

	return -1;
}

void findDelete(List L, void *var){ //only used for string ops

	
	
	if(L->length == 0){

		printf("%s\n", "findDelete() function cannot find nor delete element in empty list.");
		return;
	}

	else{

		L->cursor = L->head;
		L->Index = 0;

		for(int x = 0; x < L->length; x++){

			//printf("%s%d\n", "Here is L->length from findDelete() call: ", L->length);

			/*if(L->cursor->data == NULL){

				return -1;
			}*/

			//printf("%s%s\n", "Here is L->cursor->data from findDelete() call: ", (char *)L->cursor->data);
			//printf("%s%s\n", "Here is var from findDelete() call: ", (char *)var);

			if( (x == (L->length - 1)) && ((int *)L->cursor->data != (int *)var) ){
				
				break;
			}

			if((int *)L->cursor->data == (int *)var){

				delete(L);
				break;

			}

			L->cursor = L->cursor->next;

			L->Index++;
		}

	}

	

}

int moveTo(List L, int i){

	if(L->length == 0 || L->length < (i + 1)){

		return -1;
	}

	else if(i == 0){

		L->cursor = L->head;

		L->Index = 0;
	}
	else{

		L->cursor = L->head;

		L->Index = 0;

		for(int x = 0; x < i; x++){

			L->cursor = L->cursor->next;

			L->Index++;
		}

		
	}

	

	return 0;
}


