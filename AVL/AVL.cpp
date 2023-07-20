#include "AVL.hpp"
#include <algorithm>
#include <iostream>
#include <stack>

using namespace std;


AVL :: AVL(){
  root = NULL;
}

Node* AVL::insert(string word){
  return insert(root,NULL,word);
}

Node* AVL::insert(Node* sub_root, Node* sub_root_parent, string word){
  //BASE CASE: when subroot is equal to NULL then crease new Node dynamically
  if(sub_root == NULL){
    Node* new_node = new Node();
    new_node->parent = sub_root_parent;
    new_node->key = word;

    //check if new_node is parent's left or right child then assign parent's pointer
    if(sub_root_parent != NULL){//this is a protection for first insertion
      if(sub_root_parent->left == NULL && sub_root_parent->key > new_node->key)
        sub_root_parent->left = new_node;
      if(sub_root_parent->right == NULL && sub_root_parent->key < new_node->key)
        sub_root_parent->right = new_node;
      
      sub_root_parent->subtree_size = subtree_size_update(sub_root_parent);
    }

    //update height and subtree size for new_node
    new_node->height = 1;
    new_node->subtree_size = 0;
    if(root == NULL)
      root = new_node;
    return new_node;
  }

  //NORMAL INSERTION FOR BST
  //now do regular insertion for BST by checking if key is greater than or less than sub_root

  //case 1: word is greater than sub_root->key
  if(word > sub_root->key)
    insert(sub_root->right, sub_root, word);
  //case 2: word is less than sub_root->key
  else if(word < sub_root->key)
    insert(sub_root->left, sub_root, word);
  //case 3: they are equal (want to exit b/c no multiple keys in AVL)
  else
    return sub_root;

  //BALANCING FOR AVL
  int l_child_height = 0;
  int r_child_height = 0;
  if(sub_root->left != NULL)
    l_child_height = sub_root->left->height;
  if(sub_root->right != NULL)
    r_child_height = sub_root->right->height;

  //checking balance factor
  int balance = l_child_height - r_child_height;

  //if balance = {-1, 0, 1} then no need for rotations, return sub_root Node
  if(balance == -1 || balance == 0 || balance == 1){
    //need to update subtree and height size in case going back up recusion and theres no rotations
    sub_root->height = update_height(sub_root);
    sub_root->subtree_size = subtree_size_update(sub_root);
    return sub_root;
  }

  //here is where the four cases of rotation occur: LL, LR, RR, and regular
  //(1) LEFT-LEFT CASE --> 1 rotate right
  if(balance > 1 && word < sub_root->left->key){
    return right_rotate(sub_root);
  }
  //(2) LEFT-RIGHT CASE --> 1 rotate left, then 1 rotate right
  if(balance > 1 && word > sub_root->left->key){
    /*
             z                               z                           x
            / \                            /   \                        /  \
           y   T4  Left Rotate (y)        x    T4  Right Rotate(z)    y      z
          / \      - - - - - - - - ->    /  \      - - - - - - - ->  / \    / \
        T1   x                          y    T3                    T1  T2 T3  T4
            / \                        / \
          T2   T3                    T1   T2
    */
    sub_root->left = left_rotate(sub_root->left);
    return right_rotate(sub_root);
  }

  //(3) RIGHT-RIGHT CASE --> 1 rotate left
  if(balance < -1 && word > sub_root->right->key){
    return left_rotate(sub_root);
  }

  //(4) RIGHT-LEFT CASE --> 1 rotate right, then 1 rotate left
  if(balance < -1 && word < sub_root->right->key){
    /*
         z                            z                            x
        / \                          / \                          /  \
      T1   y   Right Rotate (y)    T1   x      Left Rotate(z)   z      y
          / \  - - - - - - - - ->     /  \   - - - - - - - ->  / \    / \
         x   T4                      T2   y                  T1  T2  T3  T4
        / \                              /  \
      T2   T3                           T3   T4
    */
    sub_root->right = right_rotate(sub_root->right);
    return left_rotate(sub_root);
  }

  //this is a saftey net in case it somehow makes it past everything
  return sub_root;
}







int AVL::subtree_size_update(Node* sub_root){
  //check if children exist
  bool l_child = false;
  bool r_child = false;

  //if exist then set to true
  if(sub_root->left != NULL)
    l_child = true;
  if(sub_root->right != NULL)
    r_child = true;

  //set children subtree size to zero
  int l_child_size = 0;
  int r_child_size = 0;

  //if they exist then set children subtree sizes
  if(l_child){
    if(sub_root->left->subtree_size != 0)
      l_child_size = sub_root->left->subtree_size + 1; //add one because subtree size for child doesn't include child itself
    else
      l_child_size = 1;
  }
  if(r_child){
    if(sub_root->right->subtree_size != 0)
      r_child_size = sub_root->right->subtree_size + 1; //add one because subtree size for child doesn't include child itself
    else
      r_child_size = 1;
  }

  //return the sum of left and right subtreee sizes
  return l_child_size + r_child_size;
}






string AVL::printPreOrder(){
  return printPreOrder(root);
}

string AVL::printPreOrder(Node* start){
  if(start == NULL)
    return "";
  string leftpart = printPreOrder(start->left);
  string rightpart = printPreOrder(start->right);
  string output = start->key;
  if(leftpart.length() != 0)//left part is empty
    output = output + " " + leftpart;
  if(rightpart.length() != 0)//right part is empty
    output = output + " " + rightpart;
  return output;
}








int AVL::update_height(Node* sub_root){
  int l_child_height = 0;
  int r_child_height = 0;

  if(sub_root->left != NULL)
    l_child_height = sub_root->left->height;

  if(sub_root->right != NULL)
    r_child_height = sub_root->right->height;

  return max(l_child_height, r_child_height)+1;
}










Node* AVL::right_rotate(Node* z){
  /*
     ~~~~~LEFT-LEFT CASE~~~~~
     T1, T2, T3 and T4 are subtrees.
              z                                      y
             / \                                   /   \
            y   T4      Right Rotate (z)          x      z
           / \          - - - - - - - - ->      /  \    /  \
          x   T3                               T1  T2  T3  T4
         / \
       T1   T2
  */
  if(z== NULL)
    return z;

  //make pointer to z's left child
  Node* y = z->left;
  Node* T3 = z->left->right;

  z->left = T3;
  if(T3 != NULL)
    T3->parent = z;
  y->right = z;

  if(z->parent != NULL){
    if(z->parent->key > y->key){
      z->parent->left = y;
      y->parent = z->parent;
      z->parent = y;
    }
    if(z->parent->key < y->key){
      z->parent->right = y;
      y->parent = z->parent;
      z->parent = y;
    }
  }

  if(z == root){
    root = y;
    y->parent = NULL;
    z->parent = y;
  }

  //now update heights and subtree sizes
  z->height = update_height(z);
  y->height = update_height(y);

  //now update subtree sizes
  z->subtree_size = subtree_size_update(z);
  y->subtree_size = subtree_size_update(y);

  return y;
}










Node* AVL::left_rotate(Node* z){
  /*
     ~~~~~RIGHT-RIGHT CASE~~~~~
     z                                y
    /  \                            /   \
   T1   y     Left Rotate(z)       z      x
       /  \   - - - - - - - ->    / \    / \
      T2   x                     T1  T2 T3  T4
          / \
        T3  T4
  */
  if(z == NULL)
    return z;

  //make pointer to z's right child
  Node* y = z->right;
  Node* T3 = z->right->left;

  z->right = T3;
  if(T3 != NULL)
    T3->parent = z;
  y->left = z;

  if(z->parent != NULL){
    if(z->parent->key > y->key){
      z->parent->left = y;
      y->parent = z->parent;
      z->parent = y;
    }
    if(z->parent->key < y->key){
      z->parent->right = y;
      y->parent = z->parent;
      z->parent = y;
    }
  }

  if(z == root){
    root = y;
    y->parent = NULL;
    z->parent = y;
  }

  //now update heights and subtree heights
  z->height = update_height(z);
  y->height = update_height(y);

  z->subtree_size = subtree_size_update(z);
  y->subtree_size = subtree_size_update(y);

  return y;
}





int AVL::range_queue(string low, string high){
  int total_nodes = 0;
  if(root == NULL)
    return total_nodes;
  stack<Node*> q;
  Node* start = root;
  q.push(start);
  while(!q.empty()){
    Node* current = q.top();
    q.pop();
    if(current->key <= high && current->key >= low){
      total_nodes += 1;
    }
    if(current->left != NULL && current->key > low){
      q.push(current->left);
    }
    if(current->right != NULL && current->key < high){
      q.push(current->right);
    }
  }
  return total_nodes;
}










int AVL::range3(string low, string high){
    return range3(root, low, high);
}


int AVL::range3(Node* start, string low, string high){
    //base case
    if(start == NULL)
        return 0;
    
    while(start != NULL){
        if(start->key > high){ //changed >= to >
            start = start->left;
            if(start == NULL)
                return 0;
            continue;
        }
        if(start->key < low){ //changed <= to <
            start = start->right;
            if(start == NULL)
                return 0;
            continue;
        }
        if(start->key >= low && start->key <= high) //changed > to >= and changed < to <=
            break;
    }
    
    
    int low_total = 0;
    if(start->key != low)
        low_total = range_only_low3(start->left, low, low_total);
    
    int high_total = 0;
    if(start->key != high)
        high_total = range_only_high3(start->right, high, high_total);
    
    int total = low_total + high_total; //removed + 1
    
    if(start->key >= low || start->key <= high)
        total += 1;
    
    return total;
}


int AVL::range_only_low3(Node* start, string low, int total){
    //base case
    if(start == NULL) //hits leaf
        return 0;
    
    if(start->key < low && start->right != NULL) 
        total = range_only_low3(start->right, low, total);
    
    if(start->key < low) 
        return total;
    
    if(start->key >= low){//} && start->right != NULL){
        int right_subtree_size = 0;
        if(start->right != NULL)
            right_subtree_size = start->right->subtree_size+1;
        total = total + range_only_low3(start->left, low, total) + right_subtree_size; //right subtree and right child
        total += 1; //itself
    }

        
    return total;
}



int AVL::range_only_high3(Node* start, string high, int total){
    //base case
    if(start == NULL) //hits leaf
        return 0;
    
    if(start->key > high && start->left != NULL) //changed > to >=
        total = range_only_high3(start->left, high, total);
    
    if(start->key > high)
        return total;
    
    if(start->key <= high){//} && start->left != NULL){
        int left_subtree_size = 0;
        if(start->left != NULL)
            left_subtree_size = start->left->subtree_size + 1;
        total = total + range_only_high3(start->right, high, total) + left_subtree_size; //right subtree and right child
        total += 1; //itself
    }
    
    
    return total;
}

