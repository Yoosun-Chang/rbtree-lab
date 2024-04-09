#include "rbtree.h"
#include <stdlib.h>

void rbtree_insert_fixup(rbtree *t,node_t *z);
void rbtree_transplant(rbtree *t, node_t *u, node_t *v);
node_t *rbtree_successor(rbtree *t, node_t *x);
node_t *rbtree_find(const rbtree *t, const key_t key);
void rbtree_erase_fixup(rbtree *t, node_t *x);
void delete_node(rbtree *t, node_t *node);
int inorder(node_t *x, const rbtree *t, key_t *arr, int i);
void left_rotate(rbtree *t, node_t *x);
void right_rotate(rbtree *t, node_t *x);


/* 1. RB tree 구조체 생성 */
// 트리를 생성하는 함수
rbtree *new_rbtree(void) {
  rbtree *t = (rbtree *)calloc(1, sizeof(rbtree));
  node_t *nil_node = (node_t *)calloc(1,sizeof(node_t));
  nil_node->color = RBTREE_BLACK;

  t->nil = nil_node; 
  t->root = nil_node; 
  return t;
}

/* 2. RB tree 구조체가 차지했던 메모리 반환 */
// 트리와 노드의 메모리를 해제하는 함수
void delete_node(rbtree *t, node_t *node){
  if(node->left != t->nil)
    delete_node(t, node->left); 
  if(node->right != t->nil)
    delete_node(t, node->right); 
  free(node);
}

// 트리를 삭제 시 순회하면서 각 노드의 메모리를 반환하는 함수
void delete_rbtree(rbtree *t) {
  node_t *node = t->root;
  if(node != t->nil)
    delete_node(t,node);
  free(t->nil);
  free(t);
}

/* 3. key 추가 */
// 새로운 키를 RB 트리에 추가하는 함수
node_t *rbtree_insert(rbtree *t, const key_t key) {
  node_t *addnode = (node_t *)malloc(sizeof(node_t));
  addnode->key = key;
  addnode->left = t->nil;
  addnode->right = t->nil;
  addnode->color = RBTREE_RED;

  node_t *cur = t->root; 
  node_t *parent = t->nil; 

  while (cur != t->nil) {
    parent = cur;
    if (cur->key > key) {
      cur = cur->left;
    } else {
      cur = cur->right;
    }
  }

  addnode->parent = parent; 

  if (parent == t->nil) {
    t->root = addnode;
  } else if (key < parent->key) {
    parent->left = addnode;
  } else {
    parent->right = addnode;
  }

  rbtree_insert_fixup(t,addnode);
  return addnode;
}

// 새로운 노드 삽입 후 발생한 불균형을 복구하는 함수
void rbtree_insert_fixup(rbtree *t,node_t *z) {
  while(z != t->root && z->parent->color == RBTREE_RED) {
    if(z->parent->parent->left == z->parent) {
      node_t *y = z->parent->parent->right;
      if(y->color == RBTREE_RED) {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      } else {
        if(z == z->parent->right) {
          z = z->parent;
          left_rotate(t,z);
        }
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        right_rotate(t,z->parent->parent);
      }
    } else {
      node_t *y = z->parent->parent->left;
      if(y->color == RBTREE_RED) {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      } else {
        if(z == z->parent->left) {
          z = z->parent;
          right_rotate(t,z);
        }
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        left_rotate(t,z->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;
}

// 왼쪽으로 회전하는 함수
void left_rotate(rbtree *t, node_t *x) {
  node_t *y = x->right; 
  x->right = y->left; 

  if (y->left != t->nil) {
    y->left->parent = x;
  }

  y->parent = x->parent;

  if(x->parent == t->nil)
    t->root = y;

  else if (x == x->parent->left)
    x->parent->left = y;
  
  else 
    x->parent->right = y;

  y->left = x;

  x->parent = y;
}


// 오른쪽으로 회전하는 함수 (left_rotate와 대칭)
void right_rotate(rbtree *t, node_t *x) {
  node_t *y = x->left;
  x->left = y->right;
  if (y->right != t->nil) {
    y->right->parent = x;
  }
  y->parent = x->parent;
  if(x->parent == t->nil)
    t->root = y;
  else if (x == x->parent->right)
    x->parent->right = y;
  else x->parent->left = y;
  y->right = x;
  x->parent = y;
}

/* 4. key 탐색 */
// 4-1. 주어진 키 값에 해당하는 노드를 탐색하여 반환하는 함수
node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *p = t->root; // 루트 노드부터 탐색 시작

  while(p != t->nil) {
    if(p->key == key)
      return p; 
    else if(p->key > key)
      p = p->left; 
    else 
      p = p-> right; 
  }

  return NULL; 
}

// 4-2. 주어진 노드의 다음 노드를 반환하는 함수 (후계자 찾기)
node_t *rbtree_successor(rbtree *t, node_t *x) {
    while(x->left != t->nil) {
      x = x->left;
    }
    return x; 
}

// 4-3. 트리에서 최소값을 가진 노드를 탐색하여 반환하는 함수
node_t *rbtree_min(const rbtree *t) {
  node_t* x = t->root; 

  while(x->left != t->nil) {
    x = x->left; 
  }
  return x; 
}

// 4-4. 트리에서 최대값을 가진 노드를 탐색하여 반환하는 함수
node_t *rbtree_max(const rbtree *t) {
  node_t* x = t->root; 

  while(x->right != t->nil) {
    x = x->right; 
  }
  return x;
}

/* 5. 노드 삭제 */
// 노드를 삭제하는 함수
int rbtree_erase(rbtree *t, node_t *z) {
  node_t* y = z; 
  color_t y_original_color = y->color; 
  node_t *x; 
  
  if (z->left == t->nil) {
    x = z->right; 
    rbtree_transplant(t, z, z->right); 

  } else if (z->right == t->nil) {
    x = z->left; 
    rbtree_transplant(t, z, z->left); 
  } else {
    y = rbtree_successor(t, z->right); 
    y_original_color = y->color; 
    x = y->right; 
    
    if (y->parent == z)
      x->parent = y;
    else {
      rbtree_transplant(t, y, y->right);
      y->right = z->right; 
      y->right->parent = y; 
    }
    
    rbtree_transplant(t, z, y); 
    y->left = z->left;
    y->left->parent = y; 
    y->color = z->color; 
  }
  free(z); 

  if (y_original_color == RBTREE_BLACK){
    rbtree_erase_fixup(t, x);
  }
  return 0; 
}

// 노드 v의 부모를 노드 u의 부모로 교체하는 함수
void rbtree_transplant(rbtree *t, node_t *u, node_t *v) {
  if (u->parent == t->nil) {
    t->root = v;
  }
  else if (u == u->parent->left) {
    u->parent->left = v;
  }
  else u->parent->right = v;
  v->parent = u->parent;
}


// 노드 삭제 후 발생한 불균형을 복구하는 함수
void rbtree_erase_fixup(rbtree *t, node_t *x){
  while (x != t->root && x->color==RBTREE_BLACK)   
  {
    if(x == x->parent->left){                       
      node_t *w = x->parent->right; 
      
      if(w->color == RBTREE_RED){                   
        w->color = RBTREE_BLACK; 
        x->parent->color = RBTREE_RED; 
        left_rotate(t, x->parent); 
        w = x->parent->right; 
      }                                             
      
      if(w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK){
        w->color = RBTREE_RED; 
        x = x->parent; 
      }else{                                        
        if (w->right->color == RBTREE_BLACK){
          w->left->color = RBTREE_BLACK; 
          w->color = RBTREE_RED; 
          right_rotate(t, w); 
          w = x->parent->right; 
        }
        w->color = x->parent->color; 
        x->parent->color = RBTREE_BLACK; 
        w->right->color = RBTREE_BLACK;
        left_rotate(t, x->parent); 
        x = t->root;
      }
    }else{                                  
      node_t *w = x->parent->left;
      if(w->color == RBTREE_RED){
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t, x->parent);
        w = x->parent->left;
      }
      if(w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK){
        w->color = RBTREE_RED;
        x = x->parent;
      }else{ 
        if (w->left->color == RBTREE_BLACK){
          w->right->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          left_rotate(t, w);
          w = x->parent->left;
        }
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        right_rotate(t, x->parent);
        x = t->root;
      }      
    }
  }
  x->color = RBTREE_BLACK;
}

/* 6. array로 변환 */
// 트리의 노드들을 배열에 저장하는 함수
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  if (t->root != t->nil) {
    inorder(t->root, t, arr,0);
  }
  return 0;
}

// 중위 순회하며 트리의 노드들을 배열에 저장하는 함수
int inorder(node_t *x, const rbtree *t, key_t *arr, int i){
  if(x == t->nil) 
    return i;
  i = inorder(x->left ,t,arr,i); 
  arr[i++] = x->key; 
  i = inorder(x->right,t,arr,i); 
  return i;
}