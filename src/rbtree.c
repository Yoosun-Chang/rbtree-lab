#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>

void rbtree_insert_fixup(rbtree *t, node_t *z);

void left_rotate(rbtree *tree, node_t *x);

void right_rotate(rbtree *tree, node_t *x);

node_t *rbtree_find(const rbtree *t, const key_t key);

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
  else x->parent->left = y;
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
  return t->root; // 추가된 노드 반환
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
  node_t* p = t->root;

  while(p != NULL)
  {
    if(p->key == key)
      return p;
    else if (p->key > key)
      p = p->left;
    else 
      p = p->right;
  }
  return NULL; // 찾고자 하는 키 값이 존재하지 않을 경우 NULL 반환
}


node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  return t->root;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  return t->root;
}

void rbtree_transplant(rbtree *t, node_t *u, node_t *v) {
  
}

int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  return 0;
}
