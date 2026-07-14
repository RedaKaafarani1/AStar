#pragma once

#include "raylib.h"
#include <iostream>
#include <algorithm>
#include <chrono>
#include <limits>
#include <queue>
#include <unordered_map>
#include <utility>
#include <vector>
#include <array>
#include <cmath>
#include <string>
#include <string_view>

constexpr static int WIDTH     = 1280;
constexpr static int HEIGHT    = 1280;
constexpr static int FPS       = 60;
constexpr static int GRID_SIZE = 32;
constexpr static int ROWS = WIDTH  / GRID_SIZE;
constexpr static int COLS = HEIGHT / GRID_SIZE;

inline Color AWHITE {255, 255, 255, 100}; 
inline Color AYELLOW { 253, 249, 0, 100}; 

//Point to represent coordinates on grid. Raylib uses float and I want to work with integers easily
struct Point
{
   constexpr bool operator==(const Point& b) const
   {
      return (this->x == b.x) && (this->y == b.y);
   }
   int x;
   int y;
};



using PointVec = std::vector<Point>; 

// Node struct used for AStart calculations
struct Node
{
   Node(Point p) : pos(p), f_score(0) {}
   constexpr bool operator==(const Node& b) const
   {
      return (this->pos.x == b.pos.x) && (this->pos.y == b.pos.y);
   }
   struct Compare
   {
      bool operator()(const Node& a, const Node& b)
      {
         return a.f_score > b.f_score;
      }
   };
   Point pos;
   double f_score;
};

inline std::pair<float, float> getMouseGridPos()
{
   float x = static_cast<float>((GetMouseX() / GRID_SIZE) * GRID_SIZE);
   float y = static_cast<float>((GetMouseY() / GRID_SIZE) * GRID_SIZE);
   return {x, y};
}

inline int gridIndex(float x, float y)
{
   int iX = static_cast<int>(x) / GRID_SIZE;
   int iY = static_cast<int>(y) / GRID_SIZE;
   return iY * COLS + iX;
}
