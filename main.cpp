#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include <string>
#include <vector>
// TODO: 3 functions for the game of life:
// sequential, threaded, OpenMP
// RNG for setting grid element to alive or dead -> enum for each grid value
// continuous generation up until ESC key or window close
// draw graphics and output processing time

std::vector<std::vector<int>> lifeSEQ(std::vector<std::vector<int>> board, int x, int y)
{
    // check how the cell SHOULD change, update afterwards to maintain "simultaneous" behavior
    // temporary board
    std::vector<std::vector<int>> tBoard(x, std::vector<int>(y));
    for (int i = 0; i < board.size(); i++)
    {
        for (int j = 0; j < board[i].size(); j++)
        {
            int countAlive = 0;
            if (i > 0 && j > 0 && board[i - 1][j - 1] == 1)
            {
                countAlive++; // top left
            }
            if (i > 0 && board[i - 1][j] == 1)
            {
                countAlive++; // top
            }
            if (i > 0 && j < board[i].size() - 1 && board[i - 1][j + 1] == 1)
            {
                countAlive++; // top right
            }
            if (j > 0 && board[i][j - 1] == 1)
            {
                countAlive++; // left
            }
            if (j < board[i].size() - 1 && board[i][j + 1] == 1)
            {
                countAlive++; // right
            }
            if (i < board.size() - 1 && j > 0 && board[i + 1][j - 1] == 1)
            {
                countAlive++; // bottom left
            }
            if (i < board.size() - 1 && board[i + 1][j] == 1)
            {
                countAlive++; // bottom
            }
            if (i < board.size() - 1 && j < board[i].size() - 1 && board[i + 1][j + 1] == 1)
            {
                countAlive++; // bottom right
            }

            // adjust the temp board according to the rules
            if (board[i][j] == 1 && (countAlive == 2 || countAlive == 3))
            { // if the cell is alive and has 2/3 neighbors alive
                tBoard[i][j] = 1;
            }
            else if (board[i][j] == 0 && countAlive == 3)
            {
                tBoard[i][j] = 1;
            }
            else
            {
                tBoard[i][j] = 0;
            }
        }
    }
    return tBoard;
}

// accomodate CLA
// note the cases for threading selection
int main(int argc, char *argv[])
{
    int threads = 2;
    int cellSize = 5, width = 800, height = 600;
    int t = 1; // 0 -> SEQ, 1 -> THRD, 2 -> OMP

    // TODO: Revise the flag parsing? use a library?
    // TODO: Add error checking
    for (int i = 0; i < argc; i++)
    {
        std::string arg = argv[i];
        // auto nextArg = (i + 1 != argc) ? argv[i + 1] : "";
        auto nextArg = argv[i + 1];
        if (arg == "-n")
        {
            threads = atoi(nextArg);
        }
        else if (arg == "-c")
        {
            cellSize = atoi(nextArg);
        }
        else if (arg == "-x")
        {
            width = atoi(nextArg);
        }
        else if (arg == "-y")
        {
            height = atoi(nextArg);
        }
        else if (arg == "-t")
        {
            std::cout << nextArg << "\n";
            if ((std::string)(nextArg) == "SEQ")
            {
                t = 0;
            }
            else if ((std::string)nextArg == "THRD")
            {
                t = 1;
            }
            else if ((std::string)nextArg == "OMP")
            {
                t = 2;
            }
        }
    }
    std::cout << threads << " " << cellSize << " "
              << width << " "
              << height << " "
              << t << " "
              << "\n";

    // int xCells = width / cellSize;
    // int yCells = height / cellSize;

    int xCells = 5;
    int yCells = 5;
    // std::vector<std::vector<int>> board; // initialize board only to the size required
    std::vector<std::vector<int>> board(xCells, std::vector<int>(yCells));
    std::vector<std::vector<int>> tBoard(xCells, std::vector<int>(yCells));

    // initialize randomness of the board
    std::mt19937 rng{std::random_device{}()}; // ~true randomness
    for (int i = 0; i < xCells; i++)
    {
        for (int j = 0; j < yCells; j++)
        {
            board[i][j] = rng() % 2; // mod 2 always [0,1], 0 = dead, 1 = alive
            std::cout << board[i][j] << " ";
        }
        std::cout << "\n";
    }

    // TODO: add SFML context for the game play
    // should have set clock timing and esc key condition
    switch (t)
    {
    case 0:
        board = lifeSEQ(board, xCells, yCells);
    }
    std::cout << "\n";

    for (int i = 0; i < xCells; i++)
    {
        for (int j = 0; j < yCells; j++)
        {
            std::cout << board[i][j] << " ";
        }
        std::cout << "\n";
    }

    return 0;
}