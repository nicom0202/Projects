# 🔴 N Queens 🔴

## Place Your Queens With Care

🔹Overview🔹
---------------------------------------------------------------------------------
The objective of this project is to solve the n-queens problem, which involves placing n queens on an NxN chessboard such that no two queens attack each other. However, there is a twist in this project. Instead of placing all queens from scratch, the code takes as input the positions of some queens already placed on the chessboard. The task is to place the remaining queens without any attacks. The chessboard is indexed starting from 1, and the queens' positions are given in standard (x, y) coordinates.

🔹Installation🔹
---------------------------------------------------------------------------------
    (1) Make sure your system has a C++ compiler 

    (2) Compile code: 
        g++ -c main.cpp -o main.o

    (3) Link Object Files:
        g++ main.o -o my_program
    
    (4) Run Executable:
        ./my_program 

🔹Usage🔹
---------------------------------------------------------------------------------
Download all files to same directory.

Input file:

Each line of the input file will be of the following form: 

    <int> <int> ... <int>

The first <int> represents the size of the NxN chessboard. The following <int>'s are considered in pairs. So the follow two <int>'s 
represent the coordinate pair to where a queen is to be placed. For example, "11 8 8 4 3" represents a chessboard 11x11 with queens placed
at coordinates (8,8) and (4,3). The output file will determine if that combination of queen placement can produce a solution. 

• If there is no solution to the problem, it prints “No solution” (with a newline at the end) to the output file.

• If there is a solution, it will print the position of each queen as <column> <space> <row> <space> followed by the position of the next queen. The positions will be in increasing order of column, so first column first, second column second, etc. 

Edit the input to add more test cases. Have fun trying new combinations. 