/*
Author: Prachit Amin
Class: ECE4122
Last Date Modified: 10/11/2025

Description:
Entire Game of Life file using sequential, threaded, and OMP processes.
*/
#include <SFML/Graphics.hpp>
#include <chrono>
#include <iostream>
#include <omp.h>
#include <queue>
#include <random>
#include <string>
#include <thread>
#include <vector>

std::vector<std::vector<int>> lifeSEQ(const std::vector<std::vector<int>> &board)
{
  // check how the cell SHOULD change, update afterwards to maintain "simultaneous" behavior
  // temporary board
  int rows = board.size();
  int cols = board[0].size();
  std::vector<std::vector<int>> tBoard(rows, std::vector<int>(cols));

  for (int i = 0; i < board.size(); i++)
  {
    for (int j = 0; j < board[i].size(); j++)
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
std::vector<std::vector<int>> lifeMT(const std::vector<std::vector<int>> &board, int NUMTHREADS)
{
  // temporary board
  int rows = board.size();
  int cols = board[0].size();
  std::vector<std::vector<int>> tBoard(rows, std::vector<int>(cols));

  // set up threading
  std::vector<std::thread> threads;
  threads.reserve(NUMTHREADS);

  // lambda for thread spinning
  auto worker = [&](int start, int end)
  {
    for (int i = start; i < end; ++i)
    {
      // local refs reduce pointer chasing
      const auto &r0 = (i > 0) ? board[i - 1] : board[i];
      const auto &r1 = board[i];
      const auto &r2 = (i < rows - 1) ? board[i + 1] : board[i];
      auto &out = tBoard[i];

      for (int j = 0; j < cols; ++j)
      {
        // do neighbor sum inline (usually faster than a helper call)
        const int jm1 = (j > 0) ? j - 1 : j;
        const int jp1 = (j < cols - 1) ? j + 1 : j;
        int s = r0[jm1] + r0[j] + r0[jp1] + r1[jm1] + r1[jp1] + r2[jm1] + r2[j] + r2[jp1];

        const int alive = r1[j];
        // branch-free rule
        out[j] = (s == 3) | (alive & (s == 2));
      }
    }
  };

  // run the threads
  for (int t = 0; t < NUMTHREADS; t++)
  {
    int start = (rows * t) / NUMTHREADS;
    int end = (rows * (t + 1)) / NUMTHREADS;

    threads.emplace_back(worker, start, end);
  }

  for (auto &th : threads)
  {
    th.join();
  }

  return tBoard;
}

std::vector<std::vector<int>> lifeOMP(const std::vector<std::vector<int>> &board,
                                      int NUMTHREADS)
{
  const int rows = static_cast<int>(board.size());
  const int cols = static_cast<int>(board[0].size());
  std::vector<std::vector<int>> tBoard(rows, std::vector<int>(cols, 0));

  omp_set_num_threads(NUMTHREADS);
  // run omp pragmas
  #pragma omp parallel for collapse(2) schedule(static)
  for (int i = 0; i < rows; ++i)
  {
    for (int j = 0; j < cols; ++j)
    {
      int countAlive = 0;

      for (int di = -1; di <= 1; ++di)
      {
        for (int dj = -1; dj <= 1; ++dj)
        {
          if (di == 0 && dj == 0)
            continue;

          const int ni = i + di;
          const int nj = j + dj;

          if (ni >= 0 && ni < rows && nj >= 0 && nj < cols)
          {
            countAlive += (board[ni][nj] == 1);
          }
        }
      }

      if ((board[i][j] == 1 && (countAlive == 2 || countAlive == 3)) ||
          (board[i][j] == 0 && countAlive == 3))
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
  int threads = 8;
  int cellSize = 5, width = 800, height = 600;
  int t = 1; // 0 -> SEQ, 1 -> THRD, 2 -> OMP

  for (int i = 0; i < argc; i++)
  {
    std::string arg = argv[i];
    if (arg == "-n")
    {
      try
      {
        threads = std::stoi(argv[++i]);
      }
      catch (const std::invalid_argument &e)
      {
        std::cerr << "Invalid argument " << arg << "\n";
      }
    }
    else if (arg == "-c")
    {
      try
      {
        cellSize = std::stoi(argv[++i]);
      }
      catch (const std::invalid_argument &e)
      {
        std::cerr << "Invalid argument " << arg << "\n";
      }
    }
    else if (arg == "-x")
    {
      try
      {
        width = std::stoi(argv[++i]);
      }
      catch (const std::invalid_argument &e)
      {
        std::cerr << "Invalid argument " << arg << "\n";
      }
    }
    else if (arg == "-y")
    {
      try
      {
        height = std::stoi(argv[++i]);
      }
      catch (const std::invalid_argument &e)
      {
        std::cerr << "Invalid argument " << arg << "\n";
      }
    }
    else if (arg == "-t")
    {
      if ((std::string)(argv[i + 1]) == "SEQ")
      {
        t = 0;
        i++;
      }
      else if ((std::string)argv[i + 1] == "THRD")
      {
        t = 1;
        i++;
      }
      else if ((std::string)argv[i + 1] == "OMP")
      {
        t = 2;
        i++;
      }
      else
      {
        std::cerr << "Invalid -t option: " << arg << "\n";
        return 0;
      }
    }
    else if (i != 0 && arg != " ")
    {
      std::cerr << "Invalid argument " << arg << "\n";
      return 0;
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
    }
  }

  // create variable sized window for the game
  sf::RenderWindow window(sf::VideoMode(width, height), "Game of Life");
  sf::RectangleShape rect(sf::Vector2f(width, height));
  sf::Clock clk;
  clk.restart();
  rect.setFillColor(sf::Color::Black);

  // generation counter for timing metrics
  int gen = 0;
  std::chrono::microseconds acc = std::chrono::microseconds::zero();

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

    // clk.getElapsedTime().asSeconds() >= 0.1 -- to slow down clock speed of game, this might interfere with timing
    if (true)
    {
      // switch case to determine which game type to run
      gen++;
      if (t == 0)
      {
        auto s = std::chrono::steady_clock::now();
        board = lifeSEQ(board);
        auto e = std::chrono::steady_clock::now();
        acc += std::chrono::duration_cast<std::chrono::microseconds>(e - s);
        if (++gen % 100 == 0)
        {
          std::cout << "100 generations took " << acc.count() << " microseconds with single thread.\n";
          acc = std::chrono::microseconds::zero();
        }
      }
      else if (t == 1)
      {
        auto s = std::chrono::steady_clock::now();
        board = lifeMT(board, threads);
        auto e = std::chrono::steady_clock::now();
        acc += std::chrono::duration_cast<std::chrono::microseconds>(e - s);
        if (++gen % 100 == 0)
        {
          std::cout << "100 generations took " << acc.count() << " microseconds with " << threads << " std::threads.\n";
          acc = std::chrono::microseconds::zero();
        }
      }
      else if (t == 2)
      {
        auto s = std::chrono::steady_clock::now();
        board = lifeOMP(board, threads);
        auto e = std::chrono::steady_clock::now();
        acc += std::chrono::duration_cast<std::chrono::microseconds>(e - s);
        if (++gen % 100 == 0)
        {
          std::cout << "100 generations took " << acc.count() << " microseconds with " << threads << " OMP threads.\n";
          acc = std::chrono::microseconds::zero();
        }
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