# 🔴 AVL 🔴

## Fast Data Structure for Text Insertions and Range Queries

🔹Overview🔹
---------------------------------------------------------------------------------
This project involves designing a fast data structure in C/C++ to perform insertions and range queries on text. The input file contains a long list of words to insert (i), interspersed with range queries (r). The goal is to efficiently find the number of words lexicographically between two given strings.

🔹Installation🔹
---------------------------------------------------------------------------------
    (1) Make sure your system has a C++ compiler 

    (2) Compile code: 
        g++ -c main.cpp -o main.o
        g++ -c AVL.cpp -o AVL.o

    (3) Link Object Files:
        g++ main.o AVL.o -o my_program
    
    (4) Run Executable:
        ./my_program input

🔹Useage🔹
---------------------------------------------------------------------------------
Input file:

Each line of the input file will be of the following two forms: 

    i <STRING>
    or
    r <STRING1> <STRING2>

The first line above means insert the string into your data structure. If the string is already present, do not insert again. Each word should only appear once in the data structure.

The second line above means: count the number of strings (currently stored) that are lexicographically between STRING1 and STRING2. In other words, we want the number of all strings STR such that STRING1 ≤ STR ≤ STRING2, where comparison is lexicographic. This number, also called the range size, should be printed in the output file, in a separate line. You can assume that STRING1 < STRING2.