
#include <iostream>
#include "AVL.hpp"
#include <stdexcept>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <string>

using namespace std;

int main(int argc, char** argv){
  

  ifstream input;
  ofstream output;

  input.open("simple-intput.txt");
  output.open("test-output.txt");

  AVL myAVL;
  string line;

  //parse the input FILE
  if(input.is_open()){
    while(getline(input, line)){
      if(line.front() == 'i'){
        //do insert here
        std::size_t found = line.find_first_of(" ");
        line.erase(line.begin(), line.begin()+found+1); //now line only contains the number
        int num = stoi(line);
        myAVL.insert(num);
        line.erase();
        continue;
      }
      if(line.front() == 'r'){
        //do range function here
        std::size_t found = line.find_first_of(" ");
        line.erase(line.begin(), line.begin()+found+1); //now line only contains the two numbers

        //find lower range
        found = line.find_first_of(" ");
        string s_low = line.substr(0,found);
        int low = stoi(s_low);

        //now delete lower range
        line.erase(line.begin(), line.begin()+found+1);

        //now line only contains the last number
        int high = stoi(line);

        //now call range
        output << myAVL.range_queue(low, high) << endl;
        
      }
    }
  }
  else{
    cout << "<INPUT FILE> does not exist" << endl;
  }


  input.close();
  output.close();
}
