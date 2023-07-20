#include <iostream>
#include "AVL.hpp"
#include <stdexcept>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char** argv){
  
  ifstream input;
  ofstream output;


  input.open("input");
  output.open("output");

  AVL myAVL;
  string line;

  //parse the input FILE
  if(input.is_open()){
    while(getline(input, line)){
      if(line.front() == 'i'){
        //do insert here
        std::size_t found = line.find_first_of(" ");
        line.erase(line.begin(), line.begin()+found+1); //now line only contains the number
        myAVL.insert(line);
        line.erase();
        continue;
      }
      if(line.front() == 'r'){
        //do range function here
        std::size_t found = line.find_first_of(" ");
        line.erase(line.begin(), line.begin()+found+1); //now line only contains the two numbers

        //find lower range
        found = line.find_first_of(" ");
        string low = line.substr(0,found);

        //now delete lower range
        line.erase(line.begin(), line.begin()+found+1);

        //now line only contains the last number
        string high = line;

        //now call range
        output << myAVL.range3(low, high) << endl;
      }
    }
  }
  else{
    cout << "<INPUT FILE> does not exist" << endl;
  }

  output.close();
    
  return 0;
    
}

