#include <ctime>
#include <iostream>
#include <vector>
enum WallType { open, closed };

struct MazeCell {
    WallType walls[4] { closed, closed, closed, closed };
    bool visited{ false };
};

constexpr int MAZE_DIM = 9;

class Maze {
    MazeCell cells[MAZE_DIM][MAZE_DIM];

public:
    void Randomize(int i, int j) {
        if (i < 0 || j < 0 || i >= MAZE_DIM || j >= MAZE_DIM)
            return;
        MazeCell& cell = cells[i][j];
        if (cell.visited)
            return; // don't repeat;
        cell.visited = true;
        MazeCell* adjacents[4] = { i > 0 ? &cells[i - 1][j] : nullptr, j > 0 ? &cells[i][j - 1] : nullptr, j < MAZE_DIM - 1 ? &cells[i][j + 1] : nullptr, i < MAZE_DIM - 1 ? &cells[i + 1][j] : nullptr };
        std::vector<int> candidates;
        for (int x = 0; x < 4; x++) {
            if (adjacents[x] != nullptr && !adjacents[x]->visited) 
                candidates.push_back(x);
        }
        if (candidates.empty()) return; // no more - all adjacents already visited
        int rnd = std::rand() % candidates.size();
        int dir = candidates[rnd];
        cell.walls[dir] = open;

        switch (dir) {
            case 0: if (i > 0) { cells[i - 1][j].walls[3] = open; Randomize(i - 1, j); }break;
            case 1: if (j > 0) { cells[i][j - 1].walls[2] = open; Randomize(i, j - 1); } break;
            case 2: if (j < MAZE_DIM - 1) { cells[i][j + 1].walls[1] = open; Randomize(i, j + 1); } break;
            case 3: if (i < MAZE_DIM - 1) { cells[i + 1][j].walls[0] = open; Randomize(i + 1, j); } break;
        }

        // check if there's adjacent not visited
        for (int x = 0; x < 4; x++) {
            if (adjacents[x] != nullptr && !adjacents[x]->visited) {
                cell.walls[x] = open;
                switch (x) {
                    case 0: if (i > 0) { cells[i - 1][j].walls[3] = open; Randomize(i - 1, j); }break;
                    case 1: if (j > 0) { cells[i][j - 1].walls[2] = open; Randomize(i, j - 1); } break;
                    case 2: if (j < MAZE_DIM - 1) { cells[i][j + 1].walls[1] = open; Randomize(i, j + 1); } break;
                    case 3: if (i < MAZE_DIM - 1) { cells[i + 1][j].walls[0] = open; Randomize(i + 1, j); } break;
                }
            }
        }

    }

    void Print() {
        //for (int i = 0; i < MAZE_DIM; i++) {
        //    for (int j = 0; j < MAZE_DIM; j++) {
        //        std::cout << i << "," << j;
        //        MazeCell& c = cells[i][j];
        //        if (c.walls[0] == open) std::cout << "o,";
        //        else std::cout << "x,";

        //        if (c.walls[1] == open) std::cout << "o,";
        //        else std::cout << "x,";

        //        if (c.walls[2] == open) std::cout << "o,";
        //        else std::cout << "x,";
        //        if (c.walls[3] == open) std::cout << "o";
        //        else std::cout << "x";
        //        std::cout << std::endl;
        //    }
        //}
        for (int i = 0; i < MAZE_DIM; i++) {
            // print first line
            for (int j = 0; j < MAZE_DIM; j++) {
                std::cout << "+";
                if (cells[i][j].walls[0] == closed)
                    std::cout << "---";
                else
                    std::cout << "   ";
            }
            std::cout << "+" << std::endl;

            // print 2nd line

            std::cout << "|";
            for (int j = 0; j < MAZE_DIM; j++) {
                if (cells[i][j].walls[2] == closed)
                    std::cout << "   |";
                else
                    std::cout << "    ";
            }
            std::cout << std::endl;
        }

        for (int j = 0; j < MAZE_DIM; j++) {
            std::cout << "+";
            if (cells[MAZE_DIM - 1][j].walls[3] == closed)
                std::cout << "---";
            else
                std::cout << "   ";
        }
        std::cout << "+" << std::endl;
    }
};

int main(int argc, char* argv[]) {
    std::srand(std::time(nullptr));
    int i = 0, j = 0;
    if (argc > 1)
        i = std::atoi(argv[1]);
    if (argc > 2)
        j = std::atoi(argv[2]);
    Maze m;
    m.Randomize(i, j);
    m.Print();
}
