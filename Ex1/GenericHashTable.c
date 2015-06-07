#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "GenericHashTable.h"

/**
* The function gets the original size and the type of the data in the table elements.
* it initializes the Table struct members. 
* On success, the function returns a pointer to the new created Table, otherwise, it return NULL.
*/
Table* createTable(int size, int dType, int listLength)
{
	int i;
	Table* myHashTable;

	if (size == 0 || listLength == 0)
		return NULL;

	myHashTable = (Table*)malloc(sizeof(Table));
	myHashTable->arr = (Object**)malloc(size*sizeof(Object*));
	for (i=0 ; i<size ; i++) {
		myHashTable->arr[i] = NULL;
	}
	myHashTable->d = 1;	    
	myHashTable->listLength = listLength;
    myHashTable->size = size;
    myHashTable->type = dType;
    return myHashTable;
}


/**
* The function gets a table, in order to create larger one so we'll be able
* to store more data in it. 
*/
int enlargeTable(Table* table)
{
	int arrLength, newArrLength, i;
	Object** newArr;

	if (table == NULL)
		return -1;

	arrLength = table->size*table->d;
	newArrLength = arrLength*2;
	newArr = (Object**)malloc(newArrLength*sizeof(Object*));
	if (newArr == NULL)
		return -1;
	for (i=0 ; i<newArrLength ; i++) 
		newArr[i] = NULL;
	for (i=0 ; i<arrLength ; i++)
		newArr[i*2] = table->arr[i];
	free(table->arr);
	table->arr = newArr;
	table->d *= 2;
	return 0;
}


/**
* The function release all the allocated members of struct Table.
*/
void freeTable(Table* table)
{
	int i;
	Object* temp;
	Object* nextObj;

	if (table == NULL)
		return;

	for (i=0 ; i<table->size*table->d ; i++) 
	{
		if(table->arr[i] != NULL) 
		{
			temp = table->arr[i];
			do {
				nextObj=temp->next;
				freeObject(temp);
				temp = nextObj;
			} while (temp != NULL);
		}
	}
	free(table->arr);
	free(table);
}


/**
* This function creates an object and return the pointer to it or NULL if there is some error.
*/
Object* createObject(void* data, int type)
{ 
	Object* newObj;
	if (data == NULL)
		return NULL;

	newObj = (Object*)malloc(sizeof(Object));

	if (newObj == NULL)
		return NULL;
	if (type == INT_TYPE)
		newObj->data = (int*)malloc(sizeof(int));
	else if (type == STR_TYPE)
		newObj->data = (char*)malloc(strlen((char*)data)*(1+sizeof(char)));

	if (newObj->data == NULL) {		// Check that the memory allocation went successfuly
			free(newObj);			// and if not it free the allocated memory.
			newObj=NULL;
			return NULL;
	}		

	newObj->next = NULL;

	if (type == INT_TYPE)
		*(int*)newObj->data = *(int*)data;
	else if (type == STR_TYPE)
		strcpy((char*)newObj->data, (char*)data);

	return newObj;
}


/**
* This function frees an object,  note the in the case of STR_TYPE, the data
* of the object should also be freed.
*/
void freeObject(Object* obj)
{
	if (obj == NULL) 
		return;
	if (obj->data != NULL)
		free(obj->data);
	obj->data = NULL;
	free(obj);
	obj = NULL;
}


/**
* check the equality of the data of two objects. The implementation is different depending the type of the data.
* the function returns 0 if they are equal or some other value if they are not equal.
*/
int isEqual(int type, void* data1, void* data2)
{
	if (data1 == NULL || data2 == NULL)
		return 1;
	if (type == INT_TYPE)
	{
		if (*((int*)data1) == *((int*)data2))
			return 0;
		else
			return 1;
	}
	else
	{
		if (strcmp((char*)data1, (char*)data2) == 0)
			return 0;
		else
			return 1;
	}
}


/**
* returns the hash value of an integer, which is key mod origSize 
*/
int intHashFun(int* key, int origSize)
{
	int temp;

	if (key == NULL)
		return -1;
	temp = *key % origSize;
	return temp;
}


/**
* returns the hash value of an string, which is m mod origSize, where m is  
* the sum of the ascii value of all the character in key. 
*/
int strHashFun(char* key, int origSize)
{
	int i, asciSum=0;
	if (key == NULL)
		return -1;
	for (i=0 ; i<strlen(key) ; i++) {
		asciSum += key[i];
	}
	asciSum = asciSum % origSize;
	return asciSum;
}


/*
* Returns the hash value of a given data using intHashFun or strHashFun (depends on the type
* of the data). In case of any error it returns -1;
*/
int getHashVal(Table* table, void* data)
{
	int hashVal;
	if (table==NULL || data==NULL)
		return -1;
	if (table->type == INT_TYPE)
		hashVal = intHashFun((int*)data, table->size)*table->d;
	else if (table->type == STR_TYPE)
		hashVal = strHashFun((char*)data, table->size)*table->d;
	return hashVal;
}


/**
* The function adds data to the hashtable (as described in the exe definition)
* On success, the function returns the array index of the added data, otherwise, it return -1.
*/
int add(Table* table, void* data)
{
	int index, j, returnVal, type;;
	int d;
	Object* newObj;

	if (table == NULL || data == NULL)
		return -1;
	if (table->type==INT_TYPE)
		if (*(int*)data < 0)
			return -1;

	type = table->type;	
	newObj = createObject(data, type);

add:d = table->d;
	
	index = getHashVal(table, data);
	if (index == -1)
		return -1;

	for (j=0 ; j<d ; j++) {								// Try adding the data to the hashfunc return index, if
		returnVal = addToList(table, newObj, index+j);	// didn't work, try add it to the next index if possible.
		if (returnVal != -1)		
			return returnVal;
	}

	if (enlargeTable(table)==-1)	// In case an error was recieved during the 
		return -1;					// creation of a bigger table;
	goto add;
}


/*
* The function gets a table, object to add and an index, and add the data to the
* desired list according to the given array index. If succeeded it returns the array 
* index of the added data, and if not it returns -1;
*/
int addToList(Table* table, Object* newObj, int index)
{
	int i=1;
	Object* last;

	if (table==NULL || newObj==NULL)
		return -1;

	last = table->arr[index];

	if(table->arr[index] == NULL) {	  // the needed list is empty.
		table->arr[index] = newObj;
		return index;
	}
	while (last->next != NULL) {	// there are objects in the list so we
		last = last->next;			// need to get to the last object
		i++;
	}
	if (i < table->listLength) {	// there's still room in the list.
		last->next = newObj;	
		return index;
	}
	else 						
		return -1;				// there's no room in the list.
}


/**
* The function search for an object that its data is equal to data and returns a pointer to that object.
* If there is no such object or in a case of an error, NULL is returned.
*/
Object* search(Table* table, void* data)
{
	int d, type, i, hashVal, listIndex;
	Object* temp;
	
	if (data == NULL || table == NULL)
		return NULL;

	d=table->d;
	type = table->type;

	hashVal = getHashVal(table, data);
	if (hashVal == -1)
		return NULL;

	for (i=hashVal ; i<hashVal+d ; i++)
	{
		temp = table->arr[i];
		listIndex=1;
		while (temp != NULL) 
		{
			if (type == INT_TYPE)
			{
				if (isEqual(INT_TYPE, temp->data, data) == 0) {
					printf("%d\t%d\n", i, listIndex);
					return temp;		
				}
			}
			else if (type == STR_TYPE) 
				if (isEqual(STR_TYPE, temp->data, data) == 0) {
					printf("%d\t%d\n", i, listIndex);
					return temp;
				}

			temp = temp->next;
			listIndex++;
		}
	}
	return NULL;
}


/**
* The function print the table (the format is in the exe definition)
*/
void printTable(Table* table)
{
	int i;
	Object* obj;
	if (table == NULL)
		return;
	for (i=0 ; i<table->size*table->d ; i++)
	{
		printf("[%d]\t", i);
		obj = table->arr[i];
		while(obj != NULL)
		{
			if (table->type == INT_TYPE)
				printf("%d\t", *(int*)obj->data);
			else if (table->type == STR_TYPE)
				printf("%s\t", (char*)obj->data);

			obj = obj->next;
			if (obj != NULL)
				printf("-->\t");
		}
		printf("\n");
	}
}


/**
* The function removes the Object which its data equals to data, if there are more than one, it removes the first one.
* On success, the function returns the array index of the removed data, otherwise, it returns -1.
* -1 is also returned in the case where there is no such object.
*/
int removeObj(Table* table, void* data)
{
	int hashVal, i, type, d;
	Object* temp1;
	Object* temp2;

	if (table==NULL || data==NULL)
		return -1;

	type = table->type;
	d = table->d;

	if (search(table, data) == NULL)
		return -1;
	
	hashVal = getHashVal(table, data);
	if (hashVal == -1)
		return -1;

	for (i=hashVal ; i<hashVal+d ; i++)
	{
		temp1 = table->arr[i];
		if (isEqual(type,  temp1->data, data) == 0)
		{
			temp2 = temp1->next;
			table->arr[i] = temp2;
			freeObject(temp1);
			return i;
		}
		temp2 = temp1->next;
		while (temp2 != NULL)
		{
			if (isEqual(type,  temp2->data, data) == 0)
			{
				temp1->next = temp2->next;
				freeObject(temp2);
				return i;
			}
			temp1=temp1->next;
			temp2=temp2->next;
		}
	}
	return -1;
}


/*int main()
{
	int x1=1,x2=2,x3=3,x4=4,x5=5,x6=6,x7=7,x8=8,x9=9,x10=10;;

	Table* tbl = createTable(3, INT_TYPE, 3);
	add(tbl, &x1);
	add(tbl, &x2);
	add(tbl, &x3);
	add(tbl, &x4);
	add(tbl, &x5);
	add(tbl, &x6);
	add(tbl, &x7);
	add(tbl, &x8);
	add(tbl, &x9);
	add(tbl, &x10);

	printTable(tbl);
	search(tbl, &x10);
	freeTable(tbl);
	return 0;
}*/