//Filename: LinkedList.h

//Header file for the class LinkedList that represents a linked list

// Nicolas Martinez, Oct 2022


#ifndef BARD_H
#define BARD_H

#include <string>
#include <iostream>
using namespace std;
//node to hold data

struct Node{
  string word;
  unsigned int occurance;
  Node* next;
};

class LinkedList
{
  private:
    //want to use dynamic array bc we don't know the size
    //or want to run through entire list and find biggest word, record biggestSize and make an array with biggestSize-5
    unsigned int arrSize = 34;
    Node* arrLinkedList[34] = {NULL};

  public:
    LinkedList();
    void populateLinkedList();
    void sort();
    string find(int, int);
    unsigned int getArrSize();
    void sortList(Node*);
  void switchData(Node*&, Node*&);
};

#endif
