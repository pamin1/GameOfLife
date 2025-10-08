#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
// TODO: 3 functions for the game of life:
// sequential, threaded, OpenMP
// RNG for setting grid element to alive or dead -> enum for each grid value
// continuous generation up until ESC key or window close
// draw graphics and output processing time

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
    return 0;
}