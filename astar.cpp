#include "raylib.h"
#include <algorithm>
#include <limits>
#include <queue>
#include <unordered_map>
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

// Entity class to make it easy to work with raylib, treat grid cells as objects that are easy to position and draw
class Entity
{
public:
   Entity() = default;
   Entity(float x, float y, float width, float height, Color color) :
      rect(x, y, width, height), col(color) {}

   void setPosition(float x, float y)
   {
      rect.x = x;
      rect.y = y;
   }

   void draw() const 
   {
      DrawRectangleRec(rect, col);
   }

   int x() const { return static_cast<int>(rect.x) / GRID_SIZE; }
   int y() const { return static_cast<int>(rect.y) / GRID_SIZE; }
private:
   Rectangle rect;
   Color col;
};

// Player, for now nothing special wrt base class
class Player : public Entity
{
public:
   Player(float x, float y, float width, float height, Color color) :
      Entity(x, y, width, height, color) {}
};

// Goal, for now nothing special wrt base class
class Goal : public Entity
{
public:
   Goal(float x, float y, float width, float height, Color color) :
      Entity(x, y, width, height, color) {}
};

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

using pointVec = std::vector<Point>; 

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

std::pair<float, float> getMouseGridPos()
{
   float x = static_cast<float>((GetMouseX() / GRID_SIZE) * GRID_SIZE);
   float y = static_cast<float>((GetMouseY() / GRID_SIZE) * GRID_SIZE);
   return {x, y};
}

int gridIndex(float x, float y)
{
   int iX = static_cast<int>(x) / GRID_SIZE;
   int iY = static_cast<int>(y) / GRID_SIZE;
   return iY * COLS + iX;
}

class AStar
{
public:
   // Diagonal movement directions
   constexpr static std::array<std::array<int, 2>, 8> dirs = {{
      {{-1, 0}},
      {{1, 0}},
      {{0, -1}},
      {{0, 1}},
      {{-1, 1}},
      {{-1, -1}},
      {{1, 1}},
      {{1, -1}},
   }};

   void computeAStar(Player& p, Goal& g);
   void drawPath()
   {
      for (const auto& c : m_path)
         DrawRectangleRec({c.x * 1.0f * GRID_SIZE, c.y * 1.0f * GRID_SIZE, GRID_SIZE, GRID_SIZE}, WHITE);
   }
   void updateObstacles()
   {
      if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
      {
         auto [mx, my] = getMouseGridPos();
         int idx = gridIndex(mx, my);
         if (m_obstacles.find(idx) == m_obstacles.end())
            m_obstacles.insert({gridIndex(mx, my), {mx, my, GRID_SIZE, GRID_SIZE, BLUE}});
      }
   }
   void drawObstacles()
   {
      for (const auto [index, obstacle] : m_obstacles)
         obstacle.draw();
   }
   std::string_view getMessage() { return m_message; };
private:
   pointVec m_path;
   std::string m_message;
   std::unordered_map<int, Entity> m_obstacles;
};

void drawGrid()
{
   for (int i = 0; i < ROWS; i++)
   {
      DrawLine(i * GRID_SIZE, 0, i * GRID_SIZE, HEIGHT, WHITE);
      DrawLine(0, i * GRID_SIZE, WIDTH, i * GRID_SIZE, WHITE);
   }
}

bool updatePlayer(Player& p)
{
   if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
   {
      auto [newX, newY] = getMouseGridPos();
      p.setPosition(newX, newY);
      return true;
   }
   return false;
}

double octileDist(const Node& a, const Node& b)
{
   int dx = std::abs(a.pos.x - b.pos.x);
   int dy = std::abs(a.pos.y - b.pos.y);

   return std::sqrt(2.0) * std::min(dx, dy) + (std::max(dx, dy) - std::min(dx, dy));
}

bool isOutsideGrid(const Node& node)
{
   Point pos = node.pos;
   return pos.x > ROWS - 1 || pos.x < 0 or pos.y > COLS - 1 || pos.y < 0;
}

std::vector<Point> reconstructPath(std::vector<Point>& parent, const Node& a, const Node& b)
{
   std::vector<Point> path;
   Point curr = b.pos;
   while (curr != a.pos)
   {
      path.push_back(curr);
      curr = parent[curr.y * COLS + curr.x];
   }
   path.push_back(a.pos);
   std::reverse(path.begin(), path.end());
   return path;
}

void AStar::computeAStar(Player& p, Goal& g)
{
   Node a{{p.x(), p.y()}};
   Node b{{g.x(), g.y()}};

   std::priority_queue<Node, std::vector<Node>, Node::Compare> open_set;
   std::vector<bool> closed_set(ROWS * COLS, false);
   std::vector<int> g_score(ROWS * COLS, std::numeric_limits<int>::max());
   std::vector<Point> parent(ROWS * COLS, {-1, -1});
   std::vector<bool> walkable(ROWS * COLS, true);

   for (const auto [index, obstacle] : m_obstacles)
      walkable[index] = false;

   g_score[a.pos.y * COLS + a.pos.x] = 0;
   a.f_score = octileDist(a, b);
   open_set.push(a);

   m_path.clear();
   m_message.clear();

   while (!open_set.empty())
   {
      Node curr = open_set.top();
      open_set.pop();

      if (curr.f_score != g_score[curr.pos.y * COLS + curr.pos.x] + octileDist(curr, b))
         continue;

      if (curr == b)
      {
         m_message = "Path found!";
         m_path = reconstructPath(parent, a, b);
         return;
      }

      closed_set[curr.pos.y * COLS + curr.pos.x] = true;
      
      for (auto [dx, dy] : dirs)
      {
         Node neigh{{curr.pos.x + dx, curr.pos.y + dy}};

         if (isOutsideGrid(neigh) || !walkable[neigh.pos.y * COLS + neigh.pos.x])
            continue;

         if (closed_set[neigh.pos.y * COLS + neigh.pos.x])
            continue;

         bool isDiagonal = (dx != 0 && dy != 0);
         int move_score = g_score[curr.pos.y * COLS + curr.pos.x] + (isDiagonal ? std::sqrt(2.0) : 1);
         if (move_score < g_score[neigh.pos.y * COLS + neigh.pos.x])
         {
            parent[neigh.pos.y * COLS + neigh.pos.x] = curr.pos;
            g_score[neigh.pos.y * COLS + neigh.pos.x] = move_score;
            neigh.f_score = g_score[neigh.pos.y * COLS + neigh.pos.x] + octileDist(neigh, b);
            open_set.push(neigh);
         }
      }
   }
   m_message = "Path not found!";
}

int main()
{
   SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

   InitWindow(WIDTH, HEIGHT, "AStar");
   SetTargetFPS(FPS);

   Player p{0, 0, GRID_SIZE, GRID_SIZE, GREEN};
   Goal g{ GRID_SIZE * (ROWS - 1), GRID_SIZE * (COLS - 1), GRID_SIZE, GRID_SIZE, RED};
   AStar astar;

   while (!WindowShouldClose())
   {
      BeginDrawing();
      if (updatePlayer(p)) // returns true if player position changed
         astar.computeAStar(p, g); // computes the path if possible

      drawGrid();
      astar.updateObstacles(); // adds obstacles with right click
      astar.drawPath();
      astar.drawObstacles();
      g.draw();
      p.draw();
      ClearBackground(BLACK);
      EndDrawing();
   }

   return 0;
}
