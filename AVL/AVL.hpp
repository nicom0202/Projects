//
//  AVL.hpp
//  AVL
//
//  Created by Nico Martinez on 11/3/22.
//

#ifndef AVL_hpp
#define AVL_hpp

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class Node{
  public:
    int height;
    int subtree_size;
    int key;
    Node* parent;
    Node* left;
    Node* right;

    Node(){
      left = right = parent = NULL;
    }
    Node(int num){
      key = num;
      left = right = parent = NULL;
    }
};

class AVL{
    private:
        Node* root;
    
    public:
        AVL();
        Node* find(int);
        int range(int, int);
        int range(Node*, int, int);
        int range_only_low(Node*,int);
        int range_only_high(Node*,int);
        string printPreOrder();
        string printPreOrder(Node*);
        Node* insert(int);
        Node* insert(Node*,Node*, int);
        int subtree_size_update(Node*);
        Node* right_rotate(Node*);
        Node* left_rotate(Node*);
        int update_height(Node*);
        
        int range_queue(int,int);
        
        int getCount(int,int);
        int getCount(Node*,int,int);

    
    
};

#endif
