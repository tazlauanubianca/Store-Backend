#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AVLTree.h"

#define BUFLEN 1024
#define ELEMENT_TREE_LENGTH 3

/* Range of models*/
typedef struct Range{
	int *index; // Array of models in the range
	int size; // Number of array elements
	int capacity; // Array capacity
}Range;

void* createStrElement(void* str) {

	char *l = malloc(sizeof(char));
	strncpy(l,(char*)str,3);
	return l;
}

void destroyStrElement(void* elem) {

	free((char*)elem);
}


void* createPriceElement(void* price) {

	long *l = malloc(sizeof(long));
	*l = *((long*) (price));
	return l;
}

void destroyPriceElement(void* price) {

	free((long*)price);
}

void* createIndexInfo(void* index) {
	
	int *l = malloc(sizeof(int));
	*l = *((int*) (index));
	return l;
}

void destroyIndexInfo(void* index) {

	free((int*)index);
}

int compareStr(void* str1, void* str2) {

	if( strcmp((char*)str1 , (char*)str2 ) < 0)
		return -1;
	if( strcmp((char*)str1 , (char*)str2 ) > 0)
		return 1;
	return 0;
}

int comparePrice(void* price1, void* price2) {

	if(*((long*)price1) < *((long*)price2))
		return -1;
	if(*((long*)price1) > *((long*)price2))
		return 1;
	return 0;
}

void buildTreesFromFile(char* fileName, TTree* modelTree, TTree* priceTree) {

	if(fileName == NULL) return;

	FILE * file = fopen(fileName,"r");
	if (file == NULL)

		return;

	char *buf = (char*) malloc(BUFLEN+1);
	int index = ftell(file);

	while( fgets( buf,BUFLEN,file ) != NULL ) {

		buf[strlen(buf) - 1] = '\0';
		char* model = strtok(buf,",");
		long price  = atol(strtok(NULL,","));

		insert( modelTree, model, &index );
		insert( priceTree, &price, &index);
		
		index = ftell(file);
	}

}

Range* modelGroupQuery(TTree* tree, char* q){
	
	Range *range;
	range = (Range*)malloc(sizeof(Range));

	range->index = (int *)malloc(512*sizeof(int));
	range->size  = 0;
	range->capacity = 512;

	int p = strlen(q);

	TreeNode *x = minimum(tree,tree->root);
	int compare = strncmp( (char*)x->elem,q,strlen(q));

	while( compare != 0 ){
	
		x = successor(tree,x);

		compare = strncmp((char*)x->elem,q,strlen(q));

	}

	while( strncmp((char*)x->elem,q,strlen(q)) == 0 ) {

		if( range->size + 1 <= range->capacity ) {
			
			range->index[range->size] = *((int*)x->info);
			range->size++;
		} else {
			range->index = realloc( range->index, 2*range->capacity);
			range->capacity = range->capacity;
			range->index[range->size] = *((int*)x->info);
			range->size++;
		} 
			
		x = x->next;		
	}

	return range;

}

Range* modelRangeQuery(TTree* tree, char* q, char* p){

	Range *range;
	range = (Range*)malloc(sizeof(Range));

	range->index = (int *)malloc(512*sizeof(int));
	range->size  = 0;
	range->capacity = 512;

	TreeNode *x = minimum(tree,tree->root);

	int compare = strncmp( (char*)x->elem,q,strlen(q));

	while( compare != 0 ){
	
		x = successor(tree,x);

		compare = strncmp((char*)x->elem,q,strlen(q));

	}


	while( strncmp((char*)x->elem,p,strlen(p)) <= 0 ) {

		if( range->size + 1 <= range->capacity ) {
			
			range->index[range->size] = *((int*)x->info);
			range->size++;

		} else {

			range->index = realloc( range->index, 2*range->capacity);
			range->capacity = range->capacity;
			range->index[range->size] = *((int*)x->info);
			range->size++;
		} 
			
		x = x->next;		
	}

	return range;

}
Range* priceRangeQuery(TTree* tree, long q, long p){

	Range *range;
	range = (Range*)malloc(sizeof(Range));

	range->index = (int *)malloc(512*sizeof(int));
	range->size  = 0;
	range->capacity = 512;

	TreeNode *x = minimum(tree,tree->root);

	while( x != tree->nil && *((long*)x->elem) < q  ) {

		if(x != tree->nil )
	
		x = successor(tree,x);

	}
	if(x != tree->nil )

	while( *((long*)x->elem) <= p ) {
	
		if( range->size + 1 <= range->capacity ) {
			
			range->index[range->size] = *((int*)x->info);
			range->size++;

		} else {
			range->index = realloc( range->index, 2*range->capacity);
			range->capacity = range->capacity;
			range->index[range->size] = *((int*)x->info);
			range->size++;
		} 
			
		x = x->next;

	}
	
	return range;
}
Range* modelPriceRangeQuery(char* fileName, TTree* tree, char* m1, char* m2, long p1, long p2){
	
	TTree* priceTree = createTree(createPriceElement, destroyPriceElement, createIndexInfo, destroyIndexInfo, comparePrice);

	if(fileName == NULL) return NULL;
	FILE * file = fopen(fileName,"r");
	if (file == NULL)
		return NULL;

	char *buf = (char*) malloc(BUFLEN+1);
	int index = ftell(file);
	while(fgets(buf,BUFLEN,file) != NULL){

		buf[strlen(buf) - 1] = '\0';
		char* model = strtok(buf,",");
		long price = atol(strtok(NULL,","));

		insert(priceTree,&price,&index);
		
		index = ftell(file);
	}

	Range *range;

	range = (Range*)malloc(sizeof(Range));

	range->index = (int *)malloc(512*sizeof(int));
	range->size  = 0;
	range->capacity = 512;

	Range *range1 = modelRangeQuery(tree,m1,m2);
	Range *range2 = priceRangeQuery(priceTree,p1,p2);
	
	int i,j;
	for( i = 0; i<=range1->size; i++){
		
		for( j = 0; j<range2->size; j++) {
		
			if( range1->index[i] == range2->index[j] ) {

				if( range->size + 1 <= range->capacity ) {
			
					range->index[range->size] = range1->index[i];
					range->size++;
				}	
			}
		}
	}
				
				
	return range;
}


void printFile(char* fileName);
void inorderModelTreePrint(TTree* tree, TreeNode* node);
void inorderPriceTreePrint(TTree* tree, TreeNode* node);
void printRange(Range* range, char* fileName);


int main(void) {

	TTree* modelTree = createTree(createStrElement, destroyStrElement, createIndexInfo, destroyIndexInfo, compareStr);
	TTree* priceTree = createTree(createPriceElement, destroyPriceElement, createIndexInfo, destroyIndexInfo, comparePrice);

	buildTreesFromFile("input.csv", modelTree, priceTree);

	printf("Model Tree In Order:\n");
	inorderModelTreePrint(modelTree, modelTree->root);
	printf("\n\n");

	printf("Price Tree In Order:\n");
	inorderPriceTreePrint(priceTree, priceTree->root);
	printf("\n\n");

	printf("Group Model Search:\n");
	Range *range = modelGroupQuery(modelTree,"MG3");
	printRange(range,"input.csv");
	printf("\n\n");

	printf("Price Range Search:\n");
	Range *range2 = priceRangeQuery(priceTree,100,400);
	printRange(range2,"input.csv");
	printf("\n\n");

	printf("Model Range Search:\n");
	Range *range3 = modelRangeQuery(modelTree,"L2","M");
	printRange(range3,"input.csv");
	printf("\n\n");

	printf("Model Price Range Search:\n");
	Range *range4 = modelPriceRangeQuery("input.csv",modelTree,"L2","M", 300, 600);
	printRange(range4,"input.csv");

	free(range->index);
	free(range);
	free(range2->index);
	free(range2);
	free(range3->index);
	free(range3);
	free(range4->index);
	free(range4);
	destroyTree(priceTree);
	destroyTree(modelTree);
	return 0;
}



void inorderModelTreePrint(TTree* tree, TreeNode* node){
	if(node != tree->nil){
		inorderModelTreePrint(tree, node->lt);
		TreeNode* begin = node;
		TreeNode* end = node->end->next;
		while(begin != end){
			printf("%d:%s  ",*((int*)begin->info),(char*)begin->elem);
			begin = begin->next;
		}
		inorderModelTreePrint(tree, node->rt);
	}
}

void inorderPriceTreePrint(TTree* tree, TreeNode* node){
	if(node != tree->nil){
		inorderPriceTreePrint(tree, node->lt);
		TreeNode* begin = node;
		TreeNode* end = node->end->next;
		while(begin != end){
			printf("%d:%ld  ",*((int*)begin->info),*((long*)begin->elem));
			begin = begin->next;
		}
		inorderPriceTreePrint(tree, node->rt);
	}
}

void printRange(Range* range, char* fileName){
	if(fileName == NULL) return;
	FILE * file = fopen(fileName,"r");
	if (file == NULL) return;

	char *buf = (char*) malloc(BUFLEN+1);

	for(int i = 0; i < range->size;i++){
		fseek(file,range->index[i],SEEK_SET);
		if(fgets(buf,BUFLEN,file) != NULL){
			char* model = strtok(buf,",");
			long price = atol(strtok(NULL,","));
			printf("%s:%ld  ", model, price);
		}
	}
	printf("\n");
	free(buf);
	fclose(file);
}

void printFile(char* fileName){
	printf("---------\n");
	if(fileName == NULL) return;
	FILE * file = fopen(fileName,"r");
	if (file == NULL)
		return;

	char *buf = (char*) malloc(BUFLEN+1);

	while(fgets(buf,BUFLEN,file) != NULL){
		buf[strlen(buf) - 1] = '\0';
		printf("%s\n",buf);
	}

	printf("\n");
	printf("---------\n");
	free(buf);
	fclose(file);
}
