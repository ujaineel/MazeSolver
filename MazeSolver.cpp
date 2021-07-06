// The maze is encoded in the file data5x5.txt

// Compile with: g++ test00.cpp maze.cpp -o test00

#include <iostream>
#include "maze.h"
#include <windows.h>
#include <shlwapi.h>

using namespace std;

int main() {


    Maze mz;

    mz.readFile("data5x5.txt");

    mz.dump();

    cout << "This came here" << endl;

    auto path = mz.solve();
    
    for (auto x : path) {
      std::cout << x << std::endl;
    }

    return 0;
}
