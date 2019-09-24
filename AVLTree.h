#ifndef AVLTREE_H_
#define AVLTREE_H_

#include <stdlib.h>

#define MAX(a, b) (((a) >= (b))?(a):(b))

typedef struct node{
	void* elem;
	void* info;
	struct node *pt;
	struct node *lt;
	struct node *rt;
	struct node* next;
	struct node* prev;
	struct node* end;
	long height;
}TreeNode;

typedef struct TTree{
	TreeNode *root;
	TreeNode *nil;
	void* (*createElement)(void*);
	void (*destroyElement)(void*);
	void* (*createInfo)(void*);
	void (*destroyInfo)(void*);
	int (*compare)(void*, void*);
	long size;
}TTree;


TTree* createTree(void* (*createElement)(void*), void (*destroyElement)(void*),
		void* (*createInfo)(void*), void (*destroyInfo)(void*),
		int compare(void*, void*)){
	
	TTree *newTree = (TTree *)malloc(sizeof(TTree));
	
	newTree->nil         = (TreeNode *)malloc(sizeof(TreeNode));
	newTree->nil->pt     = newTree->nil;
	newTree->nil->lt     = newTree->nil;
	newTree->nil->rt     = newTree->nil;
	newTree->nil->next   = newTree->nil;
	newTree->nil->prev   = newTree->nil;
	newTree->nil->end    = newTree->nil;
	newTree->nil->height = 0;
	newTree->nil->elem   = NULL;
	newTree->nil->info   = NULL;

	newTree->root         = newTree->nil;

	newTree->createElement  = createElement;
	newTree->destroyElement = destroyElement;
	newTree->createInfo     = createInfo;
	newTree->destroyInfo    = destroyInfo;
	newTree->compare        = compare;
	
	newTree->size = 0;

	return newTree;
}

TreeNode* createTreeNode(TTree *tree, void* value, void* info){

	TreeNode *newNode = (TreeNode *)malloc(sizeof(TreeNode));

	newNode->pt     = tree->nil;
	newNode->lt     = tree->nil;
	newNode->rt     = tree->nil;
	newNode->next   = tree->nil;
	newNode->prev   = tree->nil;
	newNode->end    = newNode;
	newNode->height = 1;
	newNode->elem   = tree->createElement(value);
	newNode->info   = tree->createInfo(info);

	return newNode;
}

void destroyTreeNode(TTree *tree, TreeNode* node) {

	if( tree->root == tree->nil ) 
	
		return;

	node->prev->next = node->next;
	node->next->prev = node->prev;
	tree->destroyElement(node->elem);
	tree->destroyInfo(node->info);

	free(node);

}

int isEmpty( TTree* tree ) {

	return( tree->root == tree->nil );
       	
}

TreeNode* search( TTree* tree, TreeNode* x, void* elem ) {

	if( isEmpty(tree) == 1 )
		
		return NULL;

	while( x != tree->nil && tree->compare( x->elem, elem ) != 0 ) {
	
		if( tree->compare( x->elem, elem ) > 0 ) 

			x = x->lt;

		else 
			x = x->rt;

	}

	return x;

}

TreeNode* minimum( TTree*tree, TreeNode* x ) {

	if( isEmpty(tree) == 1 ) 
	
		return tree->nil;

	while( x->lt != tree->nil )

		x = x->lt;

	return x;

}

TreeNode* maximum( TTree* tree, TreeNode* x ) {
	
	if( isEmpty(tree) == 1 ) 
		
		return tree->nil;

	while( x->rt != tree->nil )

		x = x->rt;

	return x;

}

TreeNode* successor( TTree* tree, TreeNode* x ) {

	if( x == tree->nil ) 
	
		return tree->nil;

	if( x->rt != tree->nil )

		return minimum(tree, x->rt);

	TreeNode *y = x->pt;
	
	while( y != tree->nil && x == y->rt ) {

		x = y;
		y = y->pt;

	}
	
	return y;

}

TreeNode* predecessor( TTree* tree, TreeNode* x ) {

	if( x == tree->nil ) 
	
		return tree->nil;

	if( x->lt != tree->nil )

		return maximum(tree, x->lt);

	TreeNode *y = x->pt;

	 while( y != tree->nil && x == y->lt) {

		x = y;
		y = y->pt;

	}

	return y;

}

void avlRotateLeft( TTree* tree, TreeNode* x ) {

	TreeNode *y = x->rt;

	x->rt = y->lt;

	if( y->lt != tree->nil )

		y->lt->pt = x;

	y->pt = x->pt;

	if( x->pt == tree->nil )

		tree->root = y;

	else if( x->pt->lt == x )

		x->pt->lt = y;

	else
		x->pt->rt = y;

	y->lt = x; 
	x->pt = y;

	y->height = MAX(y->lt->height, y->rt->height) + 1;
	x->height = MAX(x->lt->height, x->rt->height) + 1;

}

void avlRotateRight( TTree* tree, TreeNode* y ) {

	TreeNode *x = y->lt;

	y->lt = x->rt;
	if( x->rt != tree->nil )

		x->rt->pt = y;

	x->pt = y->pt;

	if( y->pt == tree->nil )

		tree->root = x;

	else if( y->pt->lt == y )

		y->pt->lt = x;

	else
		y->pt->rt = x;

	x->rt = y;
	y->pt = x;

	y->height = MAX(y->lt->height, y->rt->height) + 1;
	x->height = MAX(x->lt->height, x->rt->height) + 1;
}

int avlGetHeight(TTree *tree, TreeNode *x) {

	int leftHeight = 0;
	int rightHeight = 0;

	if( x->lt != tree->nil )

		leftHeight = avlGetHeight( tree, x->lt );

	if( x->rt != tree->nil )

		rightHeight = avlGetHeight( tree, x->rt );

	if( leftHeight > rightHeight ) {

		leftHeight++;
		return leftHeight;

	} else {
	
		rightHeight++;
		return rightHeight;

	}

}

int avlGetBalance(TTree* tree, TreeNode *x){

	int balance = 0;
	
	if( x == tree->nil )

		return 0;

	if( x->lt == tree->nil && x->rt == tree->nil )
		
		return 0;

	if( x->rt != tree->nil ) {
		balance = balance + avlGetHeight(tree, x->rt);
	}

	if( x->lt != tree->nil ) {

		balance = balance - avlGetHeight(tree, x->lt);
	}


	return balance;
}


void avlFixUp(TTree* tree, TreeNode* y) {
	
	if( isEmpty(tree) == 1 )
		
		return;


	if( y == tree->nil )

		return;

	
	while( y != tree->nil ) {

		int balance = avlGetBalance(tree, y);
		
		if( balance == -2 ) {

			int balance_left = avlGetBalance(tree, y->lt);

			if( balance_left != -1 && balance_left != 0 ) {

				avlRotateLeft(tree, y->lt);
				avlRotateRight(tree, y);

			} else {

				avlRotateRight(tree, y);
			}

		} else if( balance == 2 ) {

			int balance_right = avlGetBalance(tree, y->rt);

			if( balance_right != 1 && balance_right != 0 ) {

				avlRotateRight(tree, y->rt);
				avlRotateLeft(tree, y);

			} else {
			
				avlRotateLeft(tree, y);

			}

		}
		
		y = y->pt;  

	}

}

void insert(TTree* tree, void* elem, void* info) {

	if( tree->root == tree->nil ) { 
		
		tree->root = createTreeNode( tree, elem, info );
		tree->root->end = tree->root;
		tree->size = tree->size + 1;

	} else {
		
		TreeNode *newNode  = createTreeNode( tree, elem, info );
		TreeNode *node     = tree->root;
		TreeNode *node_pt;

		while( node != tree->nil ) {
		
			if( tree->compare( elem, node->elem ) < 0 ) {
				
				node_pt = node;
				node    = node->lt;

			} else if( tree->compare( elem, node->elem ) > 0 ) {

				node_pt = node;
				node    = node->rt;
				
			} else if( tree->compare( elem, node->elem ) == 0 ) { 
			
				node->end->next = newNode;
				newNode->prev   = node->end;
				node->end       = newNode;
				newNode->end    = newNode;
				newNode->next   = successor(tree, node);
				successor(tree,node)->prev = newNode;
				return;
			}
		}
		
		if( tree->compare( elem, node_pt->elem ) < 0 ) {

			node_pt->lt = newNode;
			newNode->pt = node_pt;
			newNode->next = successor(tree, newNode);
			successor(tree, newNode)->prev = newNode;
			TreeNode *aux = predecessor(tree, newNode);

			if( aux != tree->nil ) {

				if( aux->end != aux ) {

					newNode->prev  = aux->end;
					aux->end->next = newNode;
				
				} else {

					newNode->prev = aux;
					aux->next     = newNode;

				}

			} else {
				  newNode->prev = aux;
			}


		} else {

			node_pt->rt = newNode;
			newNode->pt = node_pt;
		
			newNode->next = successor(tree, newNode);
			successor(tree, newNode)->prev = newNode;
			TreeNode *aux = predecessor(tree, newNode);
			
			if( aux != tree->nil ) {

		        	if( aux->end != aux ) {

					newNode->prev  = aux->end;
					aux->end->next = newNode;
				
				} else {

					newNode->prev = aux;
					aux->next     = newNode;

				}

			} else {
				newNode = aux;

			}

		}

		tree->size = tree->size + 1;
		avlFixUp( tree, node_pt );

	}

}

void avlList(TTree *tree, TreeNode *x) {

	if( x != tree->nil ) {
	
		x->next = successor(tree, x);
		successor(tree, x)->prev = x->end;
		TreeNode *aux = predecessor(tree, x);

		if( aux != tree->nil ) {

			if( aux->end != aux ) {

				x->prev  = aux->end;
				aux->end->next = x;
				
			} else {

					x->prev   = aux;
					aux->next = x;

				}

		} else {
			  x->prev = aux;
		}

	avlList( tree, x->lt );
	avlList( tree, x->rt );

	}
	else return;
}

void delete(TTree* tree, void* elem){

	TreeNode *z;
	TreeNode *y;
	TreeNode *x;

	z = search( tree, tree->root, elem );
	
	if( z == tree->nil )

		return;

	if( z->end != z ) {

		TreeNode *aux = z->end;

		if(z->end->prev != z ) {

			z->end = z->end->prev;
			z->end->next = successor(tree, z);
			successor(tree, z)->prev = z->end;
			destroyTreeNode(tree,aux);

		} else {
			z->end = z;
			z->end->next = successor(tree, z);
			successor(tree, z)->prev = z->end;
			destroyTreeNode(tree,aux);
		}
	
		return;
	}

	if( z->lt == tree->nil || z->rt == tree->nil )

		y = z;

	else
		y = successor(tree, z);
	
	if( y->lt == tree->nil )
	
		x = y->rt;
	else 
		x = y->lt;

	if( x != tree->nil )

		x->pt = y->pt;

	if( y->pt == tree->nil )
	
		tree->root = x;

	else if( y == y->pt->lt )

		y->pt->lt = x;

	else 
		y->pt->rt = x;

	TreeNode *t = z;

	if (t->lt && t->rt) {

		avlFixUp(tree, successor(tree, t));

	} else {

		avlFixUp(tree, t->pt);
	}

	if( y != z ) {

		z->elem = tree->createElement(y->elem);
		z->info = tree->createInfo(y->info);

		z->next = successor(tree, y);
		successor(tree, y)->prev = z->end;
		TreeNode *aux = predecessor(tree, y);

		z->prev = aux->end;
		aux->end->next = z;

		if( y->next->elem != y->elem ){

			z->end  = z;

		} else
			z->end = y->end;

	} 

	destroyTreeNode( tree, y );
	avlList( tree, tree->root);

}

void destroyTree(TTree* tree){

	TreeNode *aux = minimum(tree, tree->root);

	while( aux != tree->nil) {

		if (aux == tree->root) {
			aux = aux->next;
			continue;
		}

		TreeNode *x = aux;
		aux = aux->next;
		destroyTreeNode(tree, x);

	}

	destroyTreeNode(tree, tree->root);
	destroyTreeNode(tree, tree->nil);

	tree->createElement  = NULL;
	tree->createInfo     = NULL;
	tree->destroyElement = NULL;
	tree->destroyInfo    = NULL;
	tree->compare        = NULL;

	free(tree);

}


#endif /* AVLTREE_H_ */
