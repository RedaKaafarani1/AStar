#include "raylib.h"
#include <algorithm>
#include <limits>
#include <queue>
#include <vector>
#include <array>

constexpr static int WIDTH     = 1280;
constexpr static int HEIGHT    = 1280;
constexpr static int FPS       = 60;
constexpr static int GRID_SIZE = 32;
constexpr static int ROWS = WIDTH  / GRID_SIZE;
constexpr static int COLS = HEIGHT / GRID_SIZE;

constexpr std::array<std::array<int, 2>, 4> dirs = {{
{{-1,0}},
{{1,0}},
{{0,-1}},
{{0,1}}
}};

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

   void draw() 
   {
      DrawRectangleRec(rect, col);
   }

   int x() const { return static_cast<int>(rect.x) / GRID_SIZE; }
   int y() const { return static_cast<int>(rect.y) / GRID_SIZE; }
private:
   Rectangle rect;
   Color col;
};

class Player : public Entity
{
public:
   Player(float x, float y, float width, float height, Color color) :
      Entity(x, y, width, height, color) {}
};

class Goal : public Entity
{
public:
   Goal(float x, float y, float width, float height, Color color) :
      Entity(x, y, width, height, color) {}
};

struct Point
{
   constexpr bool operator==(const Point& b) const
   {
      return (this->x == b.x) && (this->y == b.y);
   }
   int x;
   int y;
};

struct Node
{
   Node(Point p) : pos(p), f_score(0) {}
   constexpr bool operator==(const Node& b) const
   {
      return (this->pos.x == b.pos.x) && (this->pos.y == b.pos.y);
   }
   Point pos;
   int f_score;
};

struct Compare
{
   bool operator()(const Node& a, const Node& b)
   {
      return a.f_score > b.f_score;
   }
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
      float newX = static_cast<float>((GetMouseX() / GRID_SIZE) * GRID_SIZE);
      float newY = static_cast<float>((GetMouseY() / GRID_SIZE) * GRID_SIZE);
      p.setPosition(newX, newY);
      return true;
   }
   return false;
}

int manhattanDist(const Node& a, const Node& b)
{
   return std::abs(a.pos.x - b.pos.x) + std::abs(a.pos.y - b.pos.y);
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

std::vector<Point> computeAStar(Player& p, Goal& g)
{
   Node a{{p.x(), p.y()}};
   Node b{{g.x(), g.y()}};

   std::priority_queue<Node, std::vector<Node>, Compare> open_set;
   std::vector<bool> closed_set(ROWS * COLS, false);
   std::vector<int> g_score(ROWS * COLS, std::numeric_limits<int>::max());
   std::vector<Point> parent(ROWS * COLS, {-1, -1});

   g_score[a.pos.y * COLS + a.pos.x] = 0;
   a.f_score = manhattanDist(a, b);
   open_set.push(a);

   while (!open_set.empty())
   {
      Node curr = open_set.top();
      open_set.pop();

      if (curr.f_score != g_score[curr.pos.y * COLS + curr.pos.x] + manhattanDist(curr, b))
         continue;

      if (curr == b)
         return reconstructPath(parent, a, b);

      closed_set[curr.pos.y * COLS + curr.pos.x] = true;
      
      for (auto [dx, dy] : dirs)
      {
         Node neigh{{curr.pos.x + dx, curr.pos.y + dy}};

         if (isOutsideGrid(neigh))
            continue;

         if (closed_set[neigh.pos.y * COLS + neigh.pos.x])
            continue;

         int move_score = g_score[curr.pos.y * COLS + curr.pos.x] + 1;
         if (move_score < g_score[neigh.pos.y * COLS + neigh.pos.x])
         {
            parent[neigh.pos.y * COLS + neigh.pos.x] = curr.pos;
            g_score[neigh.pos.y * COLS + neigh.pos.x] = move_score;
            neigh.f_score = g_score[neigh.pos.y * COLS + neigh.pos.x] + manhattanDist(neigh, b);
            open_set.push(neigh);
         }
      }
   }
   return {};
}

int main()
{
   SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

   InitWindow(WIDTH, HEIGHT, "AStar");
   SetTargetFPS(FPS);

   Player p{0, 0, GRID_SIZE, GRID_SIZE, GREEN};
   Goal g{ 10 * GRID_SIZE, 20 * GRID_SIZE, GRID_SIZE, GRID_SIZE, RED};
   auto res = computeAStar(p, g);

   while (!WindowShouldClose())
   {
      BeginDrawing();
      drawGrid();
      
      bool recompute = updatePlayer(p);
      if (recompute) 
      {
         res = computeAStar(p, g);
         recompute = false;
      }
      for (const auto& c : res)
      {
         DrawRectangleRec({c.x * 1.0f * GRID_SIZE, c.y * 1.0f * GRID_SIZE, GRID_SIZE, GRID_SIZE}, WHITE);
      }

      g.draw();
      p.draw();
      ClearBackground(BLACK);
      EndDrawing();
   }

   return 0;
}
