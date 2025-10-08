#include <SFML/Graphics.hpp>
#include <chrono>
#include <iostream>
#include <omp.h>
#include <queue>
#include <random>
#include <string>
#include <thread>
#include <vector>

#define NUMTHREADS 8
// TODO: 3 functions for the game of life:
// sequential, threaded, OpenMP
// RNG for setting grid element to alive or dead -> enum for each grid value
// continuous generation up until ESC key or window close
// draw graphics and output processing time

std::vector<std::vector<int>> lifeSEQ(const std::vector<std::vector<int>> &board, int x, int y)
{
  // check how the cell SHOULD change, update afterwards to maintain "simultaneous" behavior
  // temporary board
  std::vector<std::vector<int>> tBoard(x, std::vector<int>(y));
  for (int i = 0; i < board.size(); i++)
  {
    for (int j = 0; j < board[i].size(); j++)
    {
      int countAlive = 0;
      int rows = board.size();
      int cols = board[i].size();

      // iterate through the 8 possible neighbor offsets
      for (int di = -1; di <= 1; ++di)
      {
        for (int dj = -1; dj <= 1; ++dj)
        {
          if (di == 0 && dj == 0)
            continue; // skip the cell itself

          int ni = i + di;
          int nj = j + dj;

          // boundary check
          if (ni >= 0 && ni < rows && nj >= 0 && nj < cols)
          {
            if (board[ni][nj] == 1)
            {
              countAlive++;
            }
          }
        }
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

// run the computation: check cells around the current cell
int mtHelper(const std::vector<std::vector<int>> &board, int i, int j)
{
  int countAlive = 0;

  // iterate through the 8 possible neighbor offsets
  for (int di = -1; di <= 1; ++di)
  {
    for (int dj = -1; dj <= 1; ++dj)
    {
      if (di == 0 && dj == 0)
        continue; // skip the cell itself

      int ni = i + di;
      int nj = j + dj;

      // boundary check
      if (ni >= 0 && ni < board.size() && nj >= 0 && nj < board[0].size())
      {
        if (board[ni][nj] == 1)
        {
          countAlive++;
        }
      }
    }
  }
  return countAlive;
}

// initialize the threads and divvy up the work
std::vector<std::vector<int>> lifeMT(const std::vector<std::vector<int>> &board, int x, int y)
{
  // temporary board
  std::vector<std::vector<int>> tBoard(x, std::vector<int>(y));

  // set up threading
  std::vector<std::thread> threads;
  threads.reserve(NUMTHREADS);

  // lambda for thread spinning
  auto worker = [&](int start, int end)
  {
    for (int i = start; i < end; ++i)
    {
      for (int j = 0; j < y; ++j)
      {
        int countAlive = mtHelper(board, i, j);
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
  };

  for (int t = 0; t < NUMTHREADS; t++)
  {
    int start = (x * t) / NUMTHREADS;
    int end = (x * (t + 1)) / NUMTHREADS;

    threads.emplace_back(worker, start, end);
  }

  for (auto &th : threads)
  {
    th.join();
  }

  return tBoard;
}

void lifeOMP(const std::vector<std::vector<int>> &board, int x, int y) {
#pragma omp parallel for
  {
  }
}

// accomodate CLA
// note the cases for threading selection
int main(int argc, char *argv[])
{
  int threads = 2;
  int cellSize = 5, width = 800, height = 600;
  int t = 1; // 0 -> SEQ, 1 -> THRD, 2 -> OMP
  std::vector<int> gens;

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

  int xCells = width / cellSize;
  int yCells = height / cellSize;

  std::vector<std::vector<int>> board(xCells, std::vector<int>(yCells));

  // initialize randomness of the board
  std::mt19937 rng{std::random_device{}()}; // ~true randomness
  for (int i = 0; i < xCells; i++)
  {
    for (int j = 0; j < yCells; j++)
    {
      board[i][j] = rng() % 2; // mod 2 always [0,1], 0 = dead, 1 = alive
      // std::cout << board[i][j];
    }
    // std::cout << "\n";
  }

  // create variable sized window for the game
  sf::RenderWindow window(sf::VideoMode(width, height), "Game of Life");
  sf::RectangleShape rect(sf::Vector2f(width, height));
  sf::Clock clk;
  clk.restart();
  rect.setFillColor(sf::Color::Black);
  while (window.isOpen())
  {
    // escape conditions
    sf::Event event;
    while (window.pollEvent(event))
    {
      switch (event.type)
      {
        case sf::Event::Closed:
          window.close();
          break;
        case sf::Event::KeyPressed:
          if (event.key.code == sf::Keyboard::Escape)
          {
            window.close();
            break;
          }
      }
    }

    // clk.getElapsedTime().asSeconds() >= 0.1
    if (true)
    {
      // switch case to determine which game type to run
      if (t == 0)
      {
        board = lifeSEQ(board, xCells, yCells);
      }
      else if (t == 1)
      {
        board = lifeMT(board, xCells, yCells);
      }
      else if (t == 2)
      {
        board = lifeOMP(board, xCells, yCells);
      }

      clk.restart();
    }

    // draw the tiles
    window.clear();
    window.draw(rect);
    for (int i = 0; i < xCells; i++)
    {
      for (int j = 0; j < yCells; j++)
      {
        sf::RectangleShape cell(sf::Vector2f(cellSize, cellSize));
        cell.setPosition((cellSize * i), (cellSize * j));
        cell.setFillColor(board[i][j] == 1 ? sf::Color::White : sf::Color::Black);
        window.draw(cell);
      }
    }
    window.display();
  }
  return 0;
}