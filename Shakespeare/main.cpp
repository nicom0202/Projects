#include <iostream>
#include "bard.h"
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <string>

using namespace std;

int main(int argc, char** argv){
  /*
  if(argc < 3)
    throw std::invalid_argument("Usage: ./hello <INPUT FILE> <OUTPUT FILE>");
  ifstream input;
  ofstream output;
  input.open(argv[1]);
  output.open(argv[2]);
  */
  ifstream input;
  ofstream output;
  
  input.open("input.txt");
  output.open("output.txt");
  
  
  LinkedList myList;
  int length;
  int occurance;
  //do string parse for input file here
  //take in the first int as length, and the second int as occurance from the input file
  //pass to find function and return the word to the output file
  //put all of this ^^^ in a while loop till @ end of file then exit file
  //close files

  if(input.is_open()){
    while(input >> length >> occurance){
      output << myList.find(length, occurance) << endl;
    }
    input.close();
    output.close();
  }
  else{
    cout << "ERROR: INPUT FILE DOES NOT EXIST." << endl;
  }

}

LinkedList::LinkedList(){
  populateLinkedList();
}

void LinkedList::populateLinkedList(){
  string fileWord;
  fstream inFile("shakespeare.txt");
  if(inFile.is_open()){
    while(inFile >> fileWord){
      unsigned int arrIndex = fileWord.length() - 5;
      if(arrLinkedList[arrIndex] == NULL){
        arrLinkedList[arrIndex] = new Node;
        arrLinkedList[arrIndex]->word = fileWord;
        arrLinkedList[arrIndex]->occurance = 1;
      }
      else{
        Node* cur = arrLinkedList[arrIndex];
        while(cur != NULL){
          if(cur->word == fileWord){
            cur->occurance += 1;
            break;
          }
          cur = cur->next;
        }
        if(cur == NULL){
          cur = arrLinkedList[arrIndex];
          Node* newNode = new Node;
          newNode->word = fileWord;
          newNode->occurance = 1;

          arrLinkedList[arrIndex] = newNode;
          newNode->next = cur;
        }
      }
    }
    inFile.close();
    sort();
  }
  else{
    cout << "ERROR: FILE DOES NOT EXIST" << endl;
  }
}

void LinkedList::sort(){
  for(unsigned int i=0; i<getArrSize(); i++){
    sortList(arrLinkedList[i]);
  }
}

void LinkedList::sortList(Node* head_ptr){
  Node* i, *j;
  for(i=head_ptr; i!=NULL;i=i->next){
    for(j=i->next; j!=NULL; j=j->next){
      if(j->occurance > i->occurance){
        switchData(i, j);
      }
      if(j->occurance == i->occurance){
        if(j->word < i->word){
          switchData(i, j);
        }
      }
    }
  }
}

void LinkedList::switchData(Node* &i, Node* &j){
  Node* temp = new Node;
  temp->word = i->word;
  temp->occurance = i->occurance;
  temp->word = i->word;

  i->word = j->word;
  i->occurance = j->occurance;

  j->word = temp->word;
  j->occurance = temp->occurance;

  delete temp;
}


string LinkedList::find(int length, int occurance){
  string word = "-";
  unsigned int arrIndex = length - 5;
  //check if length is less than 5(smallest word is 5 characters)
  //or greater than 39 characters (biggest word is 39 characters)
  //or if occurance is less than 0 (remember that occurance 0 means the ~MOST~ occurant word)
  if(arrIndex < 0 || arrIndex > getArrSize() - 1 || occurance < 0)
    return word;

  //if length is within the appropriate range:
  //(1) go to arrLinkedList[length - 5]
  //(2) make a cur pointer, iterate down and decrement occurance
  //    (2a) If occurance equals 0, return cur->word
  //    (2b) If cur == NULL return "-"

  Node* cur = arrLinkedList[arrIndex];
  while(cur != NULL){
    if(occurance == 0){
      return cur->word;
    }
    cur = cur->next;
    occurance -= 1;
  }

  //now check
  return word;
}

unsigned int LinkedList::getArrSize(){
  return arrSize;
}
