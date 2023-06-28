#include "AVL.hpp"
#include <algorithm>
#include <iostream>
#include <stack>

using namespace std;

AVL :: AVL(){
  root = NULL;
}

Node* AVL::insert(int num){
  return insert(root,NULL,num);
}

Node* AVL::insert(Node* sub_root, Node* sub_root_parent, int num){
  //BASE CASE: when subroot is equal to NULL then crease new Node dynamically
  if(sub_root == NULL){
    Node* new_node = new Node();
    new_node->parent = sub_root_parent;
    new_node->key = num;

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

  //case 1: num is greater than sub_root->key
  if(num > sub_root->key)
    insert(sub_root->right, sub_root, num);
  //case 2: num is less than sub_root->key
  else if(num < sub_root->key)
    insert(sub_root->left, sub_root, num);
  //case 3: they are equal (want to exit b/c no multiple keys in AVL)
  else
    return sub_root;

  //BALANCING FOR AVL
  int l_child_height = 0;
  int r_child_height = 0;
  if(sub_root->left != NULL)
    l_child_height = sub_root->left->height;
  if(sub_root->left != NULL)
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
  if(balance > 1 && num < sub_root->left->key){
    return right_rotate(sub_root);
  }
  //(2) LEFT-RIGHT CASE --> 1 rotate left, then 1 rotate right
  if(balance > 1 && num > sub_root->left->key){
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
  if(balance < -1 && num > sub_root->right->key){
    return left_rotate(sub_root);
  }

  //(4) RIGHT-LEFT CASE --> 1 rotate right, then 1 rotate left
  if(balance < -1 && num < sub_root->right->key){
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
  string output = to_string(start->key);
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





int AVL::range_queue(int low, int high){
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
/*
AVL :: AVL(){
  root = NULL;
}

Node* AVL::insert(int num){
    return insert(root, NULL, num);
}

Node* AVL::insert(Node* sub_root, Node* sub_root_parent, int num){
    //BASE CASE: when subroot is equal to NULL then create new Node dynamically
    if(sub_root == NULL){
        Node* new_node = new Node();
        new_node->parent = sub_root_parent;
        new_node->key = num;
        
        //check if new_node is parent's left or right child then assign parent's pointer
        if(sub_root_parent != NULL){//this is a protection for first insertion of tree
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
    //case 1: num is greater than sub_root->key
    if(num > sub_root->key)
        insert(sub_root->right, sub_root, num);
    //case 2: num is less than sub_root->key
    else if(num < sub_root->key)
        insert(sub_root->left, sub_root, num);
    //case 3: they are equal (want to exit bc no multiple keys in AVL
    else
        return sub_root;
    
    
    //BALANCING FOR AVL
    //this portion of the insert only happens when traversing back up the recursion
    //this part checks for balance factor through height (REMEMBER-BOTTOM UP)
    
    int l_child_height = 0;
    int r_child_height = 0;
    if(sub_root->left != NULL)
        l_child_height = sub_root->left->height;
    if(sub_root->right != NULL)
        r_child_height = sub_root->right->height;
    
    
    //checking balance factor
    int balance = l_child_height - r_child_height;
    
    //if balance = {1, 0, 1} then no need for rotations, return sub_root node
    if(balance == -1 || balance == 0 || balance == 1){
        //need to update subtree and heihgt size in case going back up recusion and theres no rotation
        sub_root->height = update_height(sub_root);
        sub_root->subtree_size = subtree_size_update(sub_root);
        return sub_root;
    }
    
    //here is where the four cases of rotation occur: LL, LR, RR and RL
    //(1) LEFT-LEFT CASE  --> 1 rotate right
    if (balance > 1 && num < sub_root->left->key)
        return right_rotate(sub_root);
    
    //(2) LEFT-RIGHT CASE  --> 1 rotate left, then 1 rotate right
    if (balance > 1 && num > sub_root->left->key){
      
      //       z                               z                           x
      //      / \                            /   \                        /  \
      //     T4  Left Rotate (y)        x    T4  Right Rotate(z)    y      z
      //    / \      - - - - - - - - ->    /  \      - - - - - - - ->  / \    / \
      //  T1   x                          y    T3                    T1  T2 T3  T4
      //      / \                        / \
      //    T2   T3                    T1   T2
      
        sub_root->left = left_rotate(sub_root->left);
        return right_rotate(sub_root);
    }
    
    //(3) RIGHT-RIGHT CASE  --> 1 rotate left
    if (balance < -1 && num > sub_root->right->key)
        return left_rotate(sub_root);
    
    //(4) RIGHT-LEFT CASE  --> 1 rotate right, then 1 rotate left
    if (balance < -1 && num < sub_root->right->key){
    
    //     z                            z                            x
    //    / \                          / \                          /  \
    //  T1   y   Right Rotate (y)    T1   x      Left Rotate(z)   z      y
    //      / \  - - - - - - - - ->     /  \   - - - - - - - ->  / \    / \
    //     x   T4                      T2   y                  T1  T2  T3  T4
    //    / \                              /  \
    //  T2   T3                           T3   T4
     
        sub_root->right = right_rotate(sub_root->right);
        return left_rotate(sub_root);
    }
    //this is a safety net in case it somehow makes it past everyting
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
    
    //return the sum of left and right subtree sizes
    return l_child_size + r_child_size;
}



















// Prints tree Preorder. Calls the recursive function from the root
// Input: None
// Output: string that has all elements of the tree pre order
string AVL :: printPreOrder()
{
    return printPreOrder(root);
}
// Prints rooted subtree tree preorder, by making recursive calls
// Input: None
// Output: string that has all elements of the rooted tree preorder
string AVL :: printPreOrder(Node* start)
{
    if(start == NULL) // base case
        return ""; // return empty string
    string leftpart = printPreOrder(start->left);
    string rightpart = printPreOrder(start->right);
    string output = to_string(start->key);
    if(leftpart.length() != 0) // left part is empty
        output = output + " " + leftpart; // append left part
    if(rightpart.length() != 0) // right part in empty
        output = output + " " + rightpart; // append right part
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
    

//     ~~~~~LEFT-LEFT CASE~~~~~
//     T1, T2, T3 and T4 are subtrees.
//              z                                      y
//             / \                                   /   \
//            y   T4      Right Rotate (z)          x      z
//           / \          - - - - - - - - ->      /  \    /  \
//          x   T3                               T1  T2  T3  T4
//         / \
//       T1   T2
     
    if(z == NULL)
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
    
    //now update heights and subtree size
    z->height = update_height(z);
    y->height = update_height(y);
    
    //now update subtree size
    z->subtree_size = subtree_size_update(z);//z needs to be updated first
    y->subtree_size = subtree_size_update(y);//y needs to be updated second
    
    //now return new sub_root
    return y;
}


Node* AVL::left_rotate(Node* z){
    
//     ~~~~~RIGHT-RIGHT CASE~~~~~
//     z                                y
//    /  \                            /   \
//   T1   y     Left Rotate(z)       z      x
//       /  \   - - - - - - - ->    / \    / \
//      T2   x                     T1  T2 T3  T4
//          / \
//        T3  T4
     
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
    
    //now update heights and subtree size
    z->height = update_height(z);
    y->height = update_height(y);
    
    //now update subtree size
    z->subtree_size = subtree_size_update(z);//z needs to be updated first
    y->subtree_size = subtree_size_update(y);//y needs to be updated second
    
    //now return new sub_root
    return y;
}



























int AVL::range(int low, int high){
    return range(root, low, high);
}

int AVL::range(Node* start, int low, int high){
    //base cases
    //NEED A BASE CASE WHERE THE ROOT HAS NO CHILDREN AND IT'S BETWEEN THE RANGE
    
    if(start == NULL)
        return 0;
    if(start->key < low)
        return range(start->right, low, high);
    if(start->key > high)
        return range(start->left, low, high);
    return range_only_low(start->left, low) + range_only_high(start->right, high); //the plus includes the start itself
    
}











int AVL::range_only_low(Node* start, int low){
    //base case
    if(start == NULL)
        return 0;
    
    if(start->key >= low){
        range_only_low(start->right, low);
    }
        
    //NEED SAFE GAURDS IN CASE THERE ARE NO CHILDREN AT START
    int start_right_subtree_size = 0;
    if(start->right != NULL)
        start_right_subtree_size = start->right->subtree_size + 1;
    
    
    return range_only_low(start->left, low) + start_right_subtree_size + 1;

}

int AVL::range_only_high(Node* start, int high){
    if(start == NULL)
        return 0;
    
    if(start->key <= high){
        range_only_high(start->left, high);
    }
        
    //NEED SAFE GAURDS IN CASE THERE ARE NO CHILDREN AT START
    int start_left_subtree_size = 0;
    if(start->left != NULL)
        start_left_subtree_size = start->left->subtree_size + 1; 
    return range_only_high(start->right, high) + start_left_subtree_size + 1;
    
}



































int AVL::getCount(int low, int high){
    return getCount(root, low, high);
}


int AVL::getCount(Node* start, int low, int high)
{
    // Base case
    if (!start) return 0;
 
    // Special Optional case for improving efficiency
    if (start->key == high && start->key == low)
        return 1;
 
    // If current node is in range, then include it in count and
    // recur for left and right children of it
    if (start->key <= high && start->key >= low)
         return 1 + getCount(start->left, low, high) +
                    getCount(start->right, low, high);
 
    // If current node is smaller than low, then recur for right
    // child
    else if (start->key < low)
         return getCount(start->right, low, high);
 
    // Else recur for left child
    else return getCount(start->left, low, high);
}



int AVL::range_queue(int low, int high){
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

*/
