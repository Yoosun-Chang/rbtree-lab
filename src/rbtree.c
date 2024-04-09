#include "rbtree.h"
#include <stdlib.h>

void rbtree_insert_fixup(rbtree *t,node_t *z);
void rbtree_transplant(rbtree *t, node_t *u, node_t *v);
node_t *rbtree_successor(rbtree *t, node_t *x);
node_t *rbtree_find(const rbtree *t, const key_t key);
void rbtree_erase_fixup(rbtree *t, node_t *x);


rbtree *new_rbtree(void) {
  // rbtree 구조체에 대한 동적 할당
  rbtree *t = (rbtree *)calloc(1, sizeof(rbtree));

  // nil 노드 생성 및 초기화
  node_t *nil_node = (node_t *)calloc(1,sizeof(node_t)); // nil 노드 동적 할당

  // nil 노드의 각 멤버 초기화
  // nil_node->parent = NULL;
  // nil_node->left = NULL; 
  // nil_node->key = NULL;
  nil_node->color = RBTREE_BLACK;

  t->nil = nil_node; // rbtree 구조체의 nil 포인터에 nil 노드 할당
  t->root = nil_node; // rbtree 구조체의 root 포인터에 nil 설정 (빈 상태의 트리)
  return t;
}


// 왼쪽 회전
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
  else x->parent->right = y;

  y->left = x;
  x->parent = y;
}

// 오른쪽 회전
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


void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  free(t);
}

node_t *rbtree_insert(rbtree *t, const key_t key) {

    // 새로운 노드 추가
  node_t *addnode = (node_t *)malloc(sizeof(node_t));
  addnode->key = key;
  addnode->left = t->nil;
  addnode->right = t->nil;
  addnode->color = RBTREE_RED;


  node_t *cur = t->root;
  node_t *parent = t->nil;

  // 탐색하면서 추가할 위치 찾기
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
    // 빈 트리인 경우 새로운 노드를 루트로 설정
    t->root = addnode;
  } else if (key < parent->key) {
    parent->left = addnode;
  } else {
    parent->right = addnode;
  }
  rbtree_insert_fixup(t,addnode);
  return addnode; // 추가된 노드 반환
}

void rbtree_insert_fixup(rbtree *t,node_t *z)
{
  while(z != t->root && z->parent->color == RBTREE_RED)
  {
    if(z->parent->parent->left == z->parent)
    {
      node_t *y = z->parent->parent->right;
      if(y->color == RBTREE_RED)
      {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }
      else
      {
        if(z == z->parent->right)
        {
          z = z->parent;
          left_rotate(t,z);
        }
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        right_rotate(t,z->parent->parent);
      }
    }
    else
    {
      node_t *y = z->parent->parent->left;
      if(y->color == RBTREE_RED)
      {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }
      else
      {
        if(z == z->parent->left)
        {
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

node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *p = t->root;

  while(p != t->nil)
  {
    if(p->key == key)
      return p;
    else if(p->key > key)
      p = p->left;
    else
      p = p-> right;
  }

  return NULL;
}


node_t *rbtree_min(const rbtree *t) {
  node_t* x = t->root;
  while(x->left != t->nil) {
    x = x->left;
  }
  return x;
}

node_t *rbtree_max(const rbtree *t) {
    node_t* x = t->root;
  while(x->right != t->nil) {
    x = x->right;
  }
  return x;
}

  // 부모 교체
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

// 키 값을 기준으로 다음 노드를 반환하는 함수
node_t *rbtree_successor(rbtree *t, node_t *x){
    while(x->left != t->nil) {
      x = x->left;
    }
    return x;
}

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


void rbtree_erase_fixup(rbtree *t, node_t *x){
  while (x != t->root && x->color==RBTREE_BLACK)   
  {
    if(x == x->parent->left){                       // 왼쪽에 붙어 있을때 
      node_t *w = x->parent->right;                 // 임시 노드에 형제 노드 할당
      if(w->color == RBTREE_RED){                   // 형제가 red일때
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(t, x->parent);
        w = x->parent->right;
      }                                             // 형제 왼쪽 자식이 블랙이면서 오른쪽 자식도 블랙일때
      if(w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK){
        w->color = RBTREE_RED;
        x = x->parent;
      }else{                                        // 형제 자식 중 하나라도 레드면
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

int inorder(node_t *x, const rbtree *t, key_t *arr, int i){
  if(x == t->nil)
    return i;
  i = inorder(x->left ,t,arr,i);
  arr[i++] = x->key;
  // i++;
  i = inorder(x->right,t,arr,i);
  return i;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  const rbtree *x = t;
  inorder(t->root, x, arr,0);
  return 0;
}