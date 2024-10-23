#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include"kvstore.h"
#define RED				1
#define BLACK 			2
#define KEY_CHAR 1
#if KEY_CHAR
//typedef char* KEY_TYPE;
#else
typedef int KEY_TYPE;
#endif

/*typedef struct _rbtree_node {
	unsigned char color;
	struct _rbtree_node *right;
	struct _rbtree_node *left;
	struct _rbtree_node *parent;
	KEY_TYPE key;
	void *value;
} rbtree_node;

typedef struct _rbtree {
	rbtree_node *root;
	rbtree_node *nil;
} rbtree;
*/
rbtree_node *rbtree_mini(rbtree *T, rbtree_node *x) {
	while (x->left != T->nil) {
		x = x->left;
	}
	return x;
}

rbtree_node *rbtree_maxi(rbtree *T, rbtree_node *x) {
	while (x->right != T->nil) {
		x = x->right;
	}
	return x;
}

rbtree_node *rbtree_successor(rbtree *T, rbtree_node *x) {
	rbtree_node *y = x->parent;

	if (x->right != T->nil) {
		return rbtree_mini(T, x->right);
	}

	while ((y != T->nil) && (x == y->right)) {
		x = y;
		y = y->parent;
	}
	return y;
}


void rbtree_left_rotate(rbtree *T, rbtree_node *x) {

	rbtree_node *y = x->right;  // x  --> y  ,  y --> x,   right --> left,  left --> right

	x->right = y->left; //1 1
	if (y->left != T->nil) { //1 2
		y->left->parent = x;
	}

	y->parent = x->parent; //1 3
	if (x->parent == T->nil) { //1 4
		T->root = y;
	} else if (x == x->parent->left) {
		x->parent->left = y;
	} else {
		x->parent->right = y;
	}

	y->left = x; //1 5
	x->parent = y; //1 6
}


void rbtree_right_rotate(rbtree *T, rbtree_node *y) {

	rbtree_node *x = y->left;

	y->left = x->right;
	if (x->right != T->nil) {
		x->right->parent = y;
	}

	x->parent = y->parent;
	if (y->parent == T->nil) {
		T->root = x;
	} else if (y == y->parent->right) {
		y->parent->right = x;
	} else {
		y->parent->left = x;
	}

	x->right = y;
	y->parent = x;
}

void rbtree_insert_fixup(rbtree *T, rbtree_node *z) {

	while (z->parent->color == RED) { //z ---> RED
		if (z->parent == z->parent->parent->left) {
			rbtree_node *y = z->parent->parent->right;
			if (y->color == RED) {
				z->parent->color = BLACK;
				y->color = BLACK;
				z->parent->parent->color = RED;

				z = z->parent->parent; //z --> RED
			} else {

				if (z == z->parent->right) {
					z = z->parent;
					rbtree_left_rotate(T, z);
				}

				z->parent->color = BLACK;
				z->parent->parent->color = RED;
				rbtree_right_rotate(T, z->parent->parent);
			}
		}else {
			rbtree_node *y = z->parent->parent->left;
			if (y->color == RED) {
				z->parent->color = BLACK;
				y->color = BLACK;
				z->parent->parent->color = RED;

				z = z->parent->parent; //z --> RED
			} else {
				if (z == z->parent->left) {
					z = z->parent;
					rbtree_right_rotate(T, z);
				}

				z->parent->color = BLACK;
				z->parent->parent->color = RED;
				rbtree_left_rotate(T, z->parent->parent);
			}
		}
		
	}

	T->root->color = BLACK;
}


void rbtree_insert(rbtree *T, rbtree_node *z) {

	rbtree_node *y = T->nil;
	rbtree_node *x = T->root;

	while (x != T->nil) {
		y = x;
#if KEY_CHAR
		if (strcmp(z->key,x->key)<0) {
			x = x->left;
		} else if (strcmp(z->key,x->key)>0) {
			x = x->right;
		} else { //Exist
			return ;
		}
#else
		if (z->key < x->key) {
			x = x->left;
		} else if (z->key > x->key) {
			x = x->right;
		} else { //Exist
			return ;
		}
#endif
	}

	z->parent = y;
#if KEY_CHAR
	if (y == T->nil) {
		T->root = z;
	} else if (strcmp(z->key , y->key)<0) {
		y->left = z;
	} else {
		y->right = z;
	}
#else
	if (y == T->nil) {
		T->root = z;
	} else if (z->key < y->key) {
		y->left = z;
	} else {
		y->right = z;
	}
#endif
	z->left = T->nil;
	z->right = T->nil;
	z->color = RED;

	rbtree_insert_fixup(T, z);
}

void rbtree_delete_fixup(rbtree *T, rbtree_node *x) {

	while ((x != T->root) && (x->color == BLACK)) {
		if (x == x->parent->left) {

			rbtree_node *w= x->parent->right;
			if (w->color == RED) {
				w->color = BLACK;
				x->parent->color = RED;

				rbtree_left_rotate(T, x->parent);
				w = x->parent->right;
			}

			if ((w->left->color == BLACK) && (w->right->color == BLACK)) {
				w->color = RED;
				x = x->parent;
			} else {

				if (w->right->color == BLACK) {
					w->left->color = BLACK;
					w->color = RED;
					rbtree_right_rotate(T, w);
					w = x->parent->right;
				}

				w->color = x->parent->color;
				x->parent->color = BLACK;
				w->right->color = BLACK;
				rbtree_left_rotate(T, x->parent);

				x = T->root;
			}

		} else {

			rbtree_node *w = x->parent->left;
			if (w->color == RED) {
				w->color = BLACK;
				x->parent->color = RED;
				rbtree_right_rotate(T, x->parent);
				w = x->parent->left;
			}

			if ((w->left->color == BLACK) && (w->right->color == BLACK)) {
				w->color = RED;
				x = x->parent;
			} else {

				if (w->left->color == BLACK) {
					w->right->color = BLACK;
					w->color = RED;
					rbtree_left_rotate(T, w);
					w = x->parent->left;
				}

				w->color = x->parent->color;
				x->parent->color = BLACK;
				w->left->color = BLACK;
				rbtree_right_rotate(T, x->parent);

				x = T->root;
			}

		}
	}

	x->color = BLACK;
}

rbtree_node *rbtree_delete(rbtree *T, rbtree_node *z) {

	rbtree_node *y = T->nil;
	rbtree_node *x = T->nil;

	if ((z->left == T->nil) || (z->right == T->nil)) {
		y = z;
	} else {
		y = rbtree_successor(T, z);
	}

	if (y->left != T->nil) {
		x = y->left;
	} else if (y->right != T->nil) {
		x = y->right;
	}

	x->parent = y->parent;
	if (y->parent == T->nil) {
		T->root = x;
	} else if (y == y->parent->left) {
		y->parent->left = x;
	} else {
		y->parent->right = x;
	}

	if (y != z) {
#if KEY_CHAR
		void* temp=z->key;
		z->key=y->key;
		y->key=temp;
		//strcpy(y->key,temp);

		temp=z->value;
		z->value=y->value;
		y->value=temp;
		//strcpy(y->value,temp);
#else
		z->key = y->key;
		z->value = y->value;
#endif
	}

	if (y->color == BLACK) {
		rbtree_delete_fixup(T, x);
	}

	return y;
}

rbtree_node *rbtree_search(rbtree *T, KEY_TYPE key) {

	rbtree_node *node = T->root;
	while (node != T->nil) {
#if KEY_CHAR
		if (strcmp(key , node->key)<0) {
			node = node->left;
		} else if (strcmp(key , node->key)>0) {
			node = node->right;
		} else {
			return node;
		}
#else
		if (key < node->key) {
			node = node->left;
		} else if (key > node->key) {
			node = node->right;
		} else {
			return node;
		}
#endif
	}
	return T->nil;
}


void rbtree_traversal(rbtree *T, rbtree_node *node) {
	if (node != T->nil) {
		rbtree_traversal(T, node->left);
#if KEY_CHAR
		printf("key:%s, color:%s\n", node->key,(char*)( node->value));
#else
		printf("key:%d, color:%d\n", node->key, node->color);
#endif
		rbtree_traversal(T, node->right);
	}
}

/*int main() {
#if KEY_CHAR
	char* keyArray[10] = {"Aa","Bb","Cc","Dd","Ee","Ff","Gg","Hh","Jj","Ll"};
	char* valueArray[10] = {"1Aa","1Bb","1Cc","1Dd","1Ee","1Ff","1Gg","1Hh","1Jj","1Ll"};
	rbtree *T = (rbtree *)malloc(sizeof(rbtree));
	if (T == NULL) {
		printf("malloc failed\n");
		return -1;
	}
	
	T->nil = (rbtree_node*)malloc(sizeof(rbtree_node));
	T->nil->color = BLACK;
	T->root = T->nil;

	rbtree_node *node = T->nil;
	int i = 0;
	for (i = 0;i < 10;i ++) {
		node = (rbtree_node*)malloc(sizeof(rbtree_node));
		node->key=malloc(strlen(keyArray[i])+1);
		memset(node->key,0,strlen(keyArray[i])+1);
		node->key=keyArray[i];

		node->value=malloc(strlen(valueArray[i])+1);
		memset(node->value,0,strlen(valueArray[i])+1);
		node->value=valueArray[i];

		rbtree_insert(T, node);
		
	}

	rbtree_traversal(T, T->root);
	printf("11----------------------------------------\n");

	for (i = 0;i < 10;i ++) {

		rbtree_node *node = rbtree_search(T, keyArray[i]);
		rbtree_node *cur = rbtree_delete(T, node);
		free(cur);

		rbtree_traversal(T, T->root);
		printf("----------------------------------------\n");
	}
#else
	int keyArray[20] = {24,25,13,35,23, 26,67,47,38,98, 20,19,17,49,12, 21,9,18,14,15};

	rbtree *T = (rbtree *)malloc(sizeof(rbtree));
	if (T == NULL) {
		printf("malloc failed\n");
		return -1;
	}
	
	T->nil = (rbtree_node*)malloc(sizeof(rbtree_node));
	T->nil->color = BLACK;
	T->root = T->nil;

	rbtree_node *node = T->nil;
	int i = 0;
	for (i = 0;i < 20;i ++) {
		node = (rbtree_node*)malloc(sizeof(rbtree_node));
		node->key = keyArray[i];
		node->value = NULL;

		rbtree_insert(T, node);
		
	}

	rbtree_traversal(T, T->root);
	printf("----------------------------------------\n");

	for (i = 0;i < 20;i ++) {

		rbtree_node *node = rbtree_search(T, keyArray[i]);
		rbtree_node *cur = rbtree_delete(T, node);
		free(cur);

		rbtree_traversal(T, T->root);
		printf("----------------------------------------\n");
	}
#endif

	
}*/

typedef struct _rbtree kvs_rbtree_t;
kvs_rbtree_t global_rbtree;
void kvs_rbtree_create(kvs_rbtree_t* inst,int fd){
    if(inst==NULL)return;
    inst->nil = (rbtree_node*)kvs_malloc(sizeof(rbtree_node),fd);
	inst->nil->color = BLACK;
	inst->root = inst->nil;

    return;
}

void kvs_rbtree_destory(kvs_rbtree_t* inst,int fd){
    if(inst==NULL)return;
    rbtree_node* node=NULL;
	while(!(node=inst->root)){
		rbtree_node* mini=rbtree_mini(inst,node);
		rbtree_node* cur=rbtree_delete(inst,mini);
	if(cur->key){
		kvs_free(cur->key,fd);
		cur->key=NULL;
	}
	if(cur->value){
		kvs_free(cur->value,fd);
		cur->value=NULL;
	}
	if(cur){
		kvs_free(cur,fd);
		cur=NULL;
	}
	if(inst->nil){
		kvs_free(inst->nil,fd);
		inst->nil=NULL;
	}
	}
    return;
}
char* kvs_rbtree_get(kvs_rbtree_t* inst,char* key,int fd)
{
	
	if(inst==NULL||key==NULL)return NULL;
	
	rbtree_node *node = rbtree_search(inst, key);
	
	return node->value;
}
int kvs_rbtree_set(kvs_rbtree_t* inst,char* key,char* value,int fd){
		if(inst==NULL||key==NULL){
			
			printf("1\n");
			return -1;
		}
		if(kvs_rbtree_get(inst,key,fd)){
			printf("2\n");
			return -1;
		}
		rbtree_node* node = (rbtree_node*)kvs_malloc(sizeof(rbtree_node),fd);
		if(!node){
			printf("3\n");
			return -1;
		}
		node->key=(char*)kvs_malloc(strlen(key)+1,fd);
		memset(node->key,0,strlen(key)+1);
		strcpy(node->key,key);
		

		node->value=(char*)kvs_malloc(strlen(value)+1,fd);
		memset(node->value,0,strlen(value)+1);
		strcpy(node->value,value);
		

		rbtree_insert(inst, node);
		rbtree_node *node1 = rbtree_search(inst, key);
		return 0;
}
int kvs_rbtree_del(kvs_rbtree_t* inst,char* key,int fd){
	if(inst==NULL||key==NULL)return -1;
	rbtree_node *node = rbtree_search(inst, key);
	rbtree_node *cur = rbtree_delete(inst, node);
	if(cur->key){
		kvs_free(cur->key,fd);
		cur->key=NULL;
	}
	if(cur->value){
		kvs_free(cur->value,fd);
		cur->value=NULL;
	}
	if(cur){
		kvs_free(cur,fd);
		cur=NULL;
	}
	return 0;
}
int kvs_rbtree_mod(kvs_rbtree_t* inst,char* key,char* value,int fd){
	if(inst==NULL||key==NULL)return -1;
	rbtree_node *node = rbtree_search(inst, key);
	if(!node)return 1;
	if(node==inst->nil)return 1;
	char* str=(char*)kvs_malloc(strlen(value)+1,fd);
	memset(str,0,strlen(value)+1);
	memcpy(str,value,strlen(value)+1);
	if(node->value){
		kvs_free(node->value,fd);
		node->value=NULL;
	}
	

	node->value=str;
	return 0;
}
int kvs_rbtree_exist(kvs_rbtree_t* inst,char* key,int fd){
	if(inst==NULL||key==NULL)return -1;
	rbtree_node *node = rbtree_search(inst, key);
	if(node==inst->nil)return 1;
	else return 0;
}
