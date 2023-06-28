#include <iostream>
#include <stack>
#include <list>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>
using namespace std;

/*
//Creating a class Pair that holds two vectors<int>, this will get pushed onto the stack.
//First will represent the final answer, whereas second is the initial placements of the queens.
class Pair{
  public:
  vector<int> first;
  vector<int> second;
};
*/
//declarations
vector<int> nQueens(vector<int>);
bool isLegal(vector<int>&);


int main(int argc, char** argv){
  /*
  vector<int> v(8,-1);
  v[3] = 3;
  v[5] = 0;
  v = nQueens(v);
  
  cout << "hello" << endl;
*/
 
  ifstream input; //creating input stream
  ofstream output; //creating output stream

  string line = ""; //string that will hold the data from each line of the input file
  bool pass = true;

  input.open("simple-input.txt"); //opening input file
  output.open("test-output.txt"); //opeing output file

  if(input.is_open()){ //checking if input file opened

    while(getline(input, line)){ //getting each line of input file until at end of file
      
      if(line == "")
        continue;

      //this will find the first whitespace, everything before this whitespace is the integer that represents
      //the NxN chessboard
      std::size_t found = line.find_first_of(" ");
      string s = line.substr(0,found);
      int v_size = stoi(s);
      
      //------------------------------------------------------------------------------------------------
      //forgot to add this edge case for input file
      //------------------------------------------------------------------------------------------------

      //checking if negative
      if(v_size < 0){
        output << "No solution" << endl;
        continue;
      }
      
      //now erasing the first integer from the input file (aka the size of the NxN chessbaord)
      line.erase(line.begin(), line.begin()+found+1); //i use found+1 because it'll include the whitespace in deletion
      
      //creating a vector with size N(first int from string line) and initializing it all to -1
      vector<int> v(v_size, -1);

      //this is the string parsing, repeat until line.size() == 0
      while(line.size() != 0){
        pass = true;

        //because the size is deleted from the input string, each pair of int's represent (column, row) that a queen
        //will be initially placed in

        //this finds the first int in the pair (aka column)
        std::size_t found_col = line.find_first_of(" ");
        string s_col = line.substr(0,found);
        int col = stoi(s_col);
        //------------------------------------------------------------------------------------------------
        //forgot to add this edge case for input file
        //------------------------------------------------------------------------------------------------
        if(col < 0){
          pass = false;
          break;
        }
        line.erase(line.begin(),line.begin()+found_col+1); //now erase from string

        //this finds the second int in the pair (aka row)
        std::size_t found_row = line.find_first_of(" ");
        string s_row = line.substr(0, found_row);
        int row = stoi(s_row);
        //------------------------------------------------------------------------------------------------
        //forgot to add this edge case for input file
        //------------------------------------------------------------------------------------------------
        if(col < 0){
          pass = false;
          break;
        }
        line.erase(line.begin(), line.begin()+found_row+1); //now erase from string

        //insert to vector
        //REMINDER: subtract by one because vector indexing starts from 0. AND remember that the index of the vector
        //          represents the column a queen is placed at. All queens will have a different column.
        //          Ex. v[0] = 2 --> this says queen1 will be placed on column 1 and row 3 on NxN chessboard


      
        //MAKE A TEST THE SEES IF THEY'RE TRYING TO PLACE A QUEEN THAT ISN'T WITH THE NxN CHESSBOARD
        if(col > v.size() || row > v.size() || col <= 0 || row <= 0){
          //output << "No solution" << '\n';
          pass = false;
          break;
        }
      
      

        //NEED TO MAKE CHECK IF PUTTING A QUEEN ON THE SAME COLUMN BECAUSE nQueens() ASSUMES EVERY QUEEN IS ON A UNIQUE COLUMN
        //nQueens will check if a queen is on the same row BUT not the same column
        if(v[col-1] != -1){
          output << "No solution" << '\n';
          pass = false;
          break;
        }
       
        

        v[col-1] = row-1;

        //because there won't be any int's greater than two digits, break the while-loop string parsing when the
        //string reaches size 1 or 2. This means that the last number is numerically from 0-9 or 10-99
        if(line.size() == 1 || line.size() == 2 || line.size() == 0){
          break; //want to break because there's no space behind the last number so the last erase never erased
        }
      }

      //call nQueens() --> returns A SINGLE POSSIBLE solution to place n queens on a n x n chessboard
      if(pass)
        v = nQueens(v);

      //because there's no solution for a chessboard size <= 3 skip to no solution
      if(v.size() >= 4 && pass){
        //print results to output file: REMEMBER EVERYTHING IS OFF BY 1 so add 1 back to everything
        for(unsigned int i=0; i<v.size(); i++){
          output << i+1 << ' ' << v[i]+1 << ' ';
        }
        output << '\n';
        line = "";
      }
      else{
        output<< "No solution" << '\n';
      }
    }
    input.close(); //closing input file
    output.close(); //closing output file
  }
  else{
    cout << "Input file does not exist." << endl; //print to console if input file does not exist
  }

  return 0;
}


//nQueens(): this preforms the classic problem of placing N queens on a NxN chessboard without having any nQueens
//           attack each other EXCEPT implemented using a stack rather than recursion for the backtracking. It also
//           allows for initial placement of N queens before finding a possible solution

//Input: takes in a vector<int> of size N that will be initialized with -1, UNLESS there are initial placements of
//       queens on the board already. The index of the vector represents the columns(each queen will have a designated
//       column aka queen0 is in index 0, queen1 is in index 1, ... , queenN is in index N).

//Ouptut: returns a vector that will hold a solution to the possible NxN chessboard with initial queen placements.
//        if an empty vector is returned then there is no solution
vector<int> nQueens(vector<int> v){

  //checking if the chessboard is size 3 or lower: no solution for chessboard 3 or lower
  if(v.size() <= 3){
    return v;
  }

  //variables
  pair<vector<int>, vector<int>> q_pair; //creating a pair of vecotr<int>: first holds final solution, second holds initial placement
  stack<pair<vector<int>, vector<int>>> q_stack; //creating a stack that will hold the pairs (aka where the backtacking happens)
  vector<int> results; //creating a vector<int> that will hold the final answer and be returned

  //inititalize
  q_pair.second = v; //second is going to hold the initial placement aka the input vector
  q_stack.push(q_pair); //pushing to stack

  //while loop
  while(!q_stack.empty()){//break if stack is empty

    //get top and pop
    pair<vector<int>, vector<int>> current = q_stack.top(); //creating a current pair that holds the top of stack
    q_stack.pop(); //popping the top of the stack

    //base case (1) --> found solution
    if(current.first.size() == v.size()){ //if first is equal to the size of the input vector THEN this is the FIRST solution found in the stack (NOT ALL SOLUTIONS)
      results = current.first; //have results equal to current.first
      break; //break out of the while loop
    }

    //base case (2) --> reached an initial placement of a queen that cannot be changed
    if(current.second.front() != -1){ //if the front of the vector is not -1
      current.first.push_back(current.second.front()); //push_back to first because cannot change positions
      current.second.erase(current.second.begin()); //now erase from second

      if(isLegal(current.first)){ //check if it is legal with isLegal
        q_stack.push(current); //if it does pass then push to stack and continue
      }
        continue; //continue to next
    }

    //for loop
    for(unsigned int i=0; i <v.size(); i++){ //this for loop tests all possiblilites of the ith queen placement
      pair<vector<int>, vector<int>> temp_current; //create a temperarry pair that'll hold multiple "recursive children" on recursion tree
      temp_current.first = current.first; //set it equal to current
      temp_current.second = current.second;//set it equal to current

      if(temp_current.second.size() > 0){ //checking that second(the initial placement of queens) isn't empty.
        temp_current.second.erase(temp_current.second.begin()); //erasing the front of second
        temp_current.first.push_back(i);//now inserting onto the back of first with i (i is in range from 0 to v.size());
      }

      //if first is 0 or 1 then theres no need to check if is legal because there's only 1 or no queen on board therefore no attacking other queens is possible
      if(temp_current.first.size() <= 1){
        q_stack.push(temp_current); //push to stack
        continue; //continue to next for loop iteration
      }

      //check if the placement onto first is legal
      if(isLegal(temp_current.first)){
        q_stack.push(temp_current); //if it is legal then push temp_current onto stack
      }
      else{
        continue;
      }
    }
  }
  return results; //return results
}


//isLegal(): checks for newest queen is legal on chessboard, assumes all other queens before are legal
//Input: a vector<int> with queen placements
//Output: a bool that says the newest queen placement is either legal (true) or illegal (false);
bool isLegal(vector<int> &v){
  //newst queen on chessboard
  int value = v.back();

  //testing horizontally(rows)
  for(std::size_t i=0; i<v.size()-1; ++i){
    if(v[i] == value){
      return false;
    }
  }

  //testing diagonally
  int offset = 1; //remember: the trick with
  for(int i=v.size()-2; i>=0; --i){
    //positive/upward diagonal test
    if(value == v[i] + offset){
      return false;
    }
    //negative/downward diagonal test
    if(value == v[i] - offset){
      return false;
    }
    ++offset;
  }
  return true;
}
