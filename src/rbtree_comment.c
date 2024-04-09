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
  // rbtree 구조체에 대한 동적 할당
  rbtree *t = (rbtree *)calloc(1, sizeof(rbtree));

  // nil 노드 생성 및 초기화
  node_t *nil_node = (node_t *)calloc(1,sizeof(node_t)); // nil 노드 동적 할당

  // nil 노드는 언제나 검정
  nil_node->color = RBTREE_BLACK;

  t->nil = nil_node; // rbtree 구조체의 nil 포인터에 nil 노드 할당
  t->root = nil_node; // rbtree 구조체의 root 포인터에 nil 할당 
  return t;
}

/* 2. RB tree 구조체가 차지했던 메모리 반환 */
// 트리와 노드의 메모리를 해제하는 함수
void delete_node(rbtree *t, node_t *node){
  // 노드의 왼쪽 자식이 nil이 아닌 경우
  if(node->left != t->nil)
    delete_node(t, node->left); // 왼쪽 자식 노드부터 재귀적으로 메모리 해제 수행
  // 노드의 오른쪽 자식이 nil이 아닌 경우
  if(node->right != t->nil)
    delete_node(t, node->right); // 오른쪽 자식 노드부터 재귀적으로 메모리 해제 수행
  // 현재 노드의 메모리 해제
  free(node);
}

// 트리를 삭제 시 순회하면서 각 노드의 메모리를 반환하는 함수
void delete_rbtree(rbtree *t) {
  // 트리의 루트 노드를 시작으로 순회
  node_t *node = t->root;
  // 루트 노드가 nil이 아닌 경우에만 순회하고 메모리 해제 수행
  if(node != t->nil)
    delete_node(t,node);
  free(t->nil); // nil 노드의 메모리 해제
  free(t); // 트리 자체의 메모리 해제
}

/* 3. key 추가 */
// 새로운 키를 RB 트리에 추가하는 함수
node_t *rbtree_insert(rbtree *t, const key_t key) {
  // 새로운 노드를 동적으로 할당, 초기화
  node_t *addnode = (node_t *)malloc(sizeof(node_t));
  addnode->key = key;
  addnode->left = t->nil;
  addnode->right = t->nil;
  addnode->color = RBTREE_RED;

  node_t *cur = t->root; // 현재 노드를 루트로 초기화
  node_t *parent = t->nil; // 부모 노드를 nil로 초기화

  // 새로운 노드를 삽입할 위치를 찾기
  while (cur != t->nil) {
    parent = cur;
    if (cur->key > key) {
      cur = cur->left;
    } else {
      cur = cur->right;
    }
  }

  addnode->parent = parent; // 추가된 노드의 부모를 설정

  if (parent == t->nil) {
    // 트리가 비어있는 경우 새로운 노드를 루트로 설정
    t->root = addnode;
  } else if (key < parent->key) {
    parent->left = addnode;
  } else {
    parent->right = addnode;
  }

  // 삽입된 노드에 대해 불균형 복구
  rbtree_insert_fixup(t,addnode);
  return addnode;
}

// 새로운 노드 삽입 후 발생한 불균형을 복구하는 함수
void rbtree_insert_fixup(rbtree *t,node_t *z) {
  while(z != t->root && z->parent->color == RBTREE_RED) {
    if(z->parent->parent->left == z->parent) {
      node_t *y = z->parent->parent->right;
      if(y->color == RBTREE_RED) {
        // Case 1: 부모와 삼촌이 모두 빨간색인 경우
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      } else {
        // Case 2, 3: 부모는 빨간색이지만 삼촌은 검은색인 경우
        if(z == z->parent->right) {
          // Case 2: z가 부모의 오른쪽 자식인 경우
          z = z->parent;
          left_rotate(t,z);
        }
        // Case 3: z가 부모의 왼쪽 자식인 경우
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        right_rotate(t,z->parent->parent);
      }
    } else {
      // 대칭
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
  // 루트 노드의 색상을 검은색으로 변경
  t->root->color = RBTREE_BLACK;
}

// 왼쪽으로 회전하는 함수
void left_rotate(rbtree *t, node_t *x) {
  node_t *y = x->right; // y는 x의 오른쪽 자식 노드로 설정
  x->right = y->left; // x의 오른쪽 자식을 y의 왼쪽 자식으로 설정

  // 만약 y의 왼쪽 자식이 nil이 아니라면, 그 노드의 부모를 x로 설정
  if (y->left != t->nil) {
    y->left->parent = x;
  }

  // y의 부모를 x의 부모로 설정
  y->parent = x->parent;

  // 만약 x의 부모가 nil이라면, y를 새로운 루트로 설정
  if(x->parent == t->nil)
    t->root = y;

  // x가 x의 부모의 왼쪽 자식이라면, y를 x의 부모의 왼쪽 자식으로 설정
  else if (x == x->parent->left)
    x->parent->left = y;
  
  // y를 x의 부모의 오른쪽 자식으로 설정
  else 
    x->parent->right = y;

  // x를 y의 왼쪽 자식으로 설정
  y->left = x;

  // y를 x의 부모로 설정
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

  // 노드가 nil이 아닌 동안 반복
  while(p != t->nil) {
    // 현재 노드의 키 값과 찾고자 하는 키 값을 비교
    if(p->key == key)
      return p; // 키 값이 일치하는 경우 해당 노드를 반환
    else if(p->key > key) // 찾고자 하는 키 값이 현재 노드의 키 값보다 작은 경우
      p = p->left; // 현재 노드의 왼쪽 자식으로 이동
    else // 찾고자 하는 키 값이 현재 노드의 키 값보다 큰 경우
      p = p-> right; // 현재 노드의 오른쪽 자식으로 이동
  }

  return NULL; // 키 값을 가진 노드가 없는 경우
}

// 4-2. 주어진 노드의 다음 노드를 반환하는 함수 (후계자 찾기)
node_t *rbtree_successor(rbtree *t, node_t *x) {
    // 현재 노드의 왼쪽 자식이 nil이 아닌 동안 반복하여 탐색
    while(x->left != t->nil) {
      x = x->left; // 현재 노드의 왼쪽 자식으로 이동
    }
    return x; // 가장 왼쪽 자식 노드를 반환
}

// 4-3. 트리에서 최소값을 가진 노드를 탐색하여 반환하는 함수
node_t *rbtree_min(const rbtree *t) {
  node_t* x = t->root; // 루트 노드부터 시작

  // 현재 노드의 왼쪽 자식이 nil일때까지
  while(x->left != t->nil) {
    x = x->left; // 현재 노드의 왼쪽 자식으로 이동
  }
  return x; 
}

// 4-4. 트리에서 최대값을 가진 노드를 탐색하여 반환하는 함수
node_t *rbtree_max(const rbtree *t) {
  node_t* x = t->root; // 루트 노드부터 시작

  // 현재 노드의 오른쪽 자식이 nil일때까지
  while(x->right != t->nil) {
    x = x->right; // 현재 노드의 오른쪽 자식으로 이동
  }
  return x;
}

/* 5. 노드 삭제 */
// 노드를 삭제하는 함수
int rbtree_erase(rbtree *t, node_t *z) {
  node_t* y = z; // 삭제할 노드를 가리키는 포인터 y를 z로 초기화
  color_t y_original_color = y->color; // 삭제할 노드의 색상을 저장
  node_t *x; // 삭제된 노드의 자식 노드를 가리킬 포인터 x를 선언
  
  // 삭제할 노드의 왼쪽 자식이 nil인 경우
  if (z->left == t->nil) {
    x = z->right; // 삭제할 노드의 오른쪽 자식을 x로 지정
    rbtree_transplant(t, z, z->right); // 삭제할 노드를 오른쪽 자식으로 교체

  // 삭제할 노드의 오른쪽 자식이 nil인 경우
  } else if (z->right == t->nil) {
    x = z->left; // 삭제할 노드의 왼쪽 자식을 x로 지정
    rbtree_transplant(t, z, z->left); // 삭제할 노드를 왼쪽 자식으로 교체

  // 삭제할 노드의 양쪽 자식이 모두 존재하는 경우
  } else {
    y = rbtree_successor(t, z->right); // 오른쪽 서브트리에서 후계자 노드 찾기
    y_original_color = y->color; // 후계자 노드의 색상을 저장
    x = y->right; // 후계자 노드의 오른쪽 자식을 x로 지정
    
    // 후계자 노드가 삭제할 노드의 바로 다음 노드인 경우
    if (y->parent == z)
      x->parent = y;
    else {
      rbtree_transplant(t, y, y->right); // 후계자 노드를 오른쪽 자식으로 교체
      y->right = z->right; // 삭제할 노드의 오른쪽 서브트리를 후계자 노드의 오른쪽 서브트리로 설정
      y->right->parent = y; // 후계자 노드의 오른쪽 서브트리의 부모를 후계자 노드로 설정
    }
    
    rbtree_transplant(t, z, y); // 삭제할 노드를 후계자 노드로 교체
    y->left = z->left; // 삭제할 노드의 왼쪽 서브트리를 후계자 노드의 왼쪽 서브트리로 설정
    y->left->parent = y; // 후계자 노드의 왼쪽 서브트리의 부모를 후계자 노드로 설정
    y->color = z->color; // 후계자 노드의 색상을 삭제할 노드의 색상으로 설정
  }
  free(z); // 삭제할 노드의 메모리를 해제

  // 삭제된 노드의 색상이 검정일 경우, 불균형을 복구
  if (y_original_color == RBTREE_BLACK){
    rbtree_erase_fixup(t, x);
  }
  return 0; 
}

// 노드 v의 부모를 노드 u의 부모로 교체하는 함수
void rbtree_transplant(rbtree *t, node_t *u, node_t *v) {
  // u의 부모가 nil인 경우, v를 트리의 새로운 루트로 설정
  if (u->parent == t->nil) {
    t->root = v;
  }
  // u가 부모의 왼쪽 자식인 경우, v를 부모의 새로운 왼쪽 자식으로 설정
  else if (u == u->parent->left) {
    u->parent->left = v;
  }
  // u가 부모의 오른쪽 자식인 경우, v를 부모의 새로운 오른쪽 자식으로 설정
  else u->parent->right = v;
  v->parent = u->parent; // v의 부모를 u의 부모로 설정(양방향 연결)
}


// 노드 삭제 후 발생한 불균형을 복구하는 함수
void rbtree_erase_fixup(rbtree *t, node_t *x){
 // x가 루트가 아니고, x의 색상이 검정인 동안 반복합니다.
  while (x != t->root && x->color==RBTREE_BLACK)   
  {
    // x가 부모의 왼쪽 자식인 경우
    if(x == x->parent->left){                       
      node_t *w = x->parent->right; // w는 삼촌
      
      // Case 1: w의 색상이 빨강인 경우
      if(w->color == RBTREE_RED){                   
        w->color = RBTREE_BLACK; // w의 색상을 검정으로 변경
        x->parent->color = RBTREE_RED; // x의 부모의 색상을 빨강으로 변경
        left_rotate(t, x->parent); // x의 부모를 기준으로 왼쪽 회전
        w = x->parent->right; // w를 x의 부모의 새로운 오른쪽 자식으로 설정
      }                                             
      
      // Case 2: 삼촌 w의 왼쪽 자식과 오른쪽 자식이 모두 검정인 경우
      if(w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK){
        w->color = RBTREE_RED; // w의 색상을 빨강으로 변경
        x = x->parent; // x를 x의 부모로 이동
      }else{                                        
        // Case 3: 삼촌 w의 오른쪽 자식의 색상이 검정
        if (w->right->color == RBTREE_BLACK){
          w->left->color = RBTREE_BLACK; // w의 왼쪽 자식의 색상을 검정으로 변경
          w->color = RBTREE_RED; // w의 색상을 빨강으로 변경
          right_rotate(t, w); // w를 기준으로 오른쪽 회전 
          w = x->parent->right; // w를 x의 부모의 새로운 오른쪽 자식으로 설정
        }
        // Case 4: 삼촌 w가 검정, w의 오른쪽 자식의 색상이 빨강
        w->color = x->parent->color; // w의 색상을 x의 부모의 색상으로 변경
        x->parent->color = RBTREE_BLACK; // x의 부모의 색상을 검정으로 변경
        w->right->color = RBTREE_BLACK; // w의 오른쪽 자식의 색상을 검정으로 변경
        left_rotate(t, x->parent); // x의 부모를 기준으로 오른쪽 회전
        x = t->root; // x를 트리의 루트로 설정(종료)
      }
    }else{ // 대칭                                    
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
  // 트리가 비어있지 않다면 중위순회 시작
  if (t->root != t->nil) {
    inorder(t->root, t, arr,0);
  }
  return 0;
}

// 중위 순회하며 트리의 노드들을 배열에 저장하는 함수
int inorder(node_t *x, const rbtree *t, key_t *arr, int i){
  if(x == t->nil) // 현재 노드가 nil인 경우, 종료
    return i;
  i = inorder(x->left ,t,arr,i); // 왼쪽 서브트리를 중위 순회하여 배열에 저장하고, 배열 인덱스를 업데이트
  arr[i++] = x->key; // 현재 노드의 키 값을 배열에 저장하고, 배열 인덱스를 증가
  i = inorder(x->right,t,arr,i); // 오른쪽 서브트리를 중위 순회하여 배열에 저장하고, 배열 인덱스를 업데이트
  return i;
}