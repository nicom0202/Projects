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
    string key;
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

        string printPreOrder();
        string printPreOrder(Node*);

        Node* insert(string);
        Node* insert(Node*,Node*, string);

        int subtree_size_update(Node*);

        Node* right_rotate(Node*);
        Node* left_rotate(Node*);

        int update_height(Node*);
        
        int range_queue(string,string);

        int range3(string, string);
        int range3(Node*,string,string);
        int range_only_low3(Node*,string,int);
        int range_only_high3(Node*,string,int);
};

#endif

