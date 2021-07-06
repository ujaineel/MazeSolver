#include <stdexcept>
#include "maze.h"

Maze::Maze(int ncell) {
    if (ncell < 1) {
        throw std::invalid_argument("Maze::Maze(): ncell must be >= 1");
    }

    _ncell = ncell;
    _maze = new cell_t[_ncell];
}

Maze::~Maze() {
    // IMPLEMENT DESTRUCTOR
}

void Maze::readFile(std::string fileName) {
    int numCells;
    int cell, n1, n2, n3, n4;
    std::ifstream dataFile;

    dataFile.open(fileName);

    if (dataFile.is_open()) {
        dataFile >> numCells;
        this->reset(numCells);

        for (int i = 0; i < numCells; i++) {
            dataFile >> cell >> n1 >> n2 >> n3 >> n4;
            this->insert(cell_t(cell, n1, n2, n3, n4));
        }
    }
}

int Maze::getNcell() const {
    return _ncell;
}

void Maze::reset(int ncell) {
    if (ncell < 1) {
        throw std::invalid_argument("Maze::reset(): ncell must be >= 1");
    }

    if (_maze != nullptr) {
        delete[] _maze;
    }

    _ncell = ncell;
    _maze = new cell_t[_ncell];
}

void Maze::insert(cell_t cell) {
    if (_maze == nullptr) {
        throw std::domain_error("Maze::insert(): attempt to insert into uninitialized Maze object");
    }
    if (cell.cellNum < 0 || cell.cellNum >= _ncell) {
        throw std::invalid_argument("Maze:insert(): invalid cell number");
    }
    _maze[cell.cellNum] = cell;
}

cell_t Maze::retrieve(int nc) const {
    if (_maze == nullptr) {
        throw std::domain_error("Maze::retrieve(): attempt to retrieve from an uninitialized Maze object");
    }
    if (nc < 0 || nc >= _ncell) {
        throw std::invalid_argument("Maze:retrieve(): invalid cell index");
    }

    return _maze[nc];
}

void Maze::dump() const {

    std::cout << "[*] Dumping the maze cells..." << std::endl;

    for (int i = 0; i < _ncell; i++) {
        int nc = _maze[i].cellNum;
        if (nc != i) {
            std::cerr << "Warning: maze cell at index " << i
                << " has cellNum of " << nc << std::endl;
        }
        std::cout << "Cell " << i << " has neighbor cells: ";
        neighbor_t nbs = _maze[i].neighbors;
        int n0, n1, n2, n3;
        if (nbs[0] >= 0) std::cout << nbs[0] << " ";
        if (nbs[1] >= 0) std::cout << nbs[1] << " ";
        if (nbs[2] >= 0) std::cout << nbs[2] << " ";
        if (nbs[3] >= 0) std::cout << nbs[3] << " ";
        std::cout << std::endl;
    }
}

std::vector<int> Maze::solve() const {
    // SOLVE METHOD
    if (this->getNcell() == 0) {
        return {};
    }

    // Initialize path and stack, and 2nd maze for deep copy.
    std::vector<int> path;
    SolveStack stack = SolveStack();
    Maze *deepMaze = new Maze(_ncell);
    for (int i = 0; i < _ncell; i++) {
        cell_t temp = _maze[i];
        deepMaze->insert(cell_t(temp.cellNum, temp.neighbors.at(0), temp.neighbors.at(1), temp.neighbors.at(2), temp.neighbors.at(3)));
    }

    // Initialize further data for maze navigation.
    int nextCellind = 0;    //Hard-coded since maze starts at 0.
    cell_t cell = deepMaze->retrieve(nextCellind);
    neighbor_t cellNeighbors;
    stack.push(src_dest_t(VACANT, nextCellind));
    /*
        Logic :

        Stack the current maze cell on the top. Check its neighbors. 
        Go to the one which is not accessed and is accessible. Add it to top of stack.
        If no neigbors/reach deadend then pop it from stack and backtrack.
    */

    while (!stack.empty() || nextCellind != _ncell - 1) {
        // Get neighbors of a cell.
        cellNeighbors = deepMaze->retrieve(nextCellind).neighbors;
    
        // Check if we have to backtrack.
        if (std::count(cellNeighbors.begin(), cellNeighbors.end(), VACANT) == 3 && std::count(cellNeighbors.begin(), cellNeighbors.end(), nextCellind) == 1) {
            nextCellind = stack.pop().first;
            cellNeighbors = deepMaze->retrieve(nextCellind).neighbors;
            std::replace_if(cellNeighbors.begin(), cellNeighbors.end(), [&](int i) {return i == nextCellind; }, VACANT);
        }

        else {
            // If no backtracking needed.
            for (int i = 0; i < cellNeighbors.size(); i++) {
                if (cellNeighbors.at(i) != VACANT && cellNeighbors.at(i) != nextCellind) {
                    stack.push(src_dest_t(nextCellind, cellNeighbors.at(i)));
                }
            }
            nextCellind = stack.read().second;
        }

        if (std::count(cellNeighbors.begin(), cellNeighbors.end(), VACANT) == 4) {
            return {};
        }
    }
    std::cout << "DOne ! " << std::endl;
    
    while (!stack.empty()) {
        path.push_back(stack.pop().second);
    }

    return path;
}

Maze::SolveStack::SolveStack() {
    _stack = nullptr;
}

Maze::SolveStack::~SolveStack() {
    // DESTRUCTOR
    std::cout << "Deleting Stack." << std::endl;
    entry_t* temp = nullptr;
    while (_stack) {
        temp = _stack->next;
        delete _stack;
        _stack = temp;
    }
    if (temp) {
        delete temp;
        temp = nullptr;
    }
}

bool Maze::SolveStack::empty() const {
    return _stack == nullptr;
}

void Maze::SolveStack::push(src_dest_t src_dest) {
    // PUSH METHOD
    entry_t* newHead = new entry_t(src_dest);
    entry_t* temp = _stack;
    newHead->next = temp;
    _stack = newHead;
}

src_dest_t Maze::SolveStack::pop() {
    // POP METHOD
    if (!_stack) {
        std::cout << "Nothing to pop!" << std::endl;
    }

    src_dest_t *top = new src_dest_t(_stack->src_dest.first, _stack->src_dest.second);

    // Remove top of stack
    entry_t* temp = _stack;
    _stack = _stack->next;
    delete temp;
    temp = nullptr;

    // Return the top of stack
    return *top;
}

src_dest_t Maze::SolveStack::read() const {
    // READ METHOD
    return (_stack != nullptr) ? src_dest_t(_stack->src_dest.first, _stack->src_dest.second) : src_dest_t(NULL, NULL);
}
