#include "Common.h"
#include "Entity.h"
#include "AStar.h"
#include <unordered_map>

void drawGrid()
{
   for (int i = 0; i < ROWS; i++)
   {
      DrawLine(i * GRID_SIZE, 0, i * GRID_SIZE, HEIGHT, AWHITE);
      DrawLine(0, i * GRID_SIZE, WIDTH, i * GRID_SIZE, AWHITE);
   }
}

int main()
{
   SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

   InitWindow(WIDTH, HEIGHT, "AStar");
   SetTargetFPS(FPS);

   Entity p{0, 0, GRID_SIZE, GRID_SIZE, GREEN, Entity::Type::Start};
   Entity g{ GRID_SIZE * (ROWS - 1), GRID_SIZE * (COLS - 1), GRID_SIZE, GRID_SIZE, RED, Entity::Type::Goal};
   AStar astar {20};
   std::unordered_map<int, Entity> obstacles;

   while (!WindowShouldClose())
   {
      BeginDrawing();
      ClearBackground(BLACK);

      obstacles = astar.updateObstacles(); // adds obstacles with right click
      bool playerPosUpdated = p.update(obstacles); // returns true if player position changed
      bool goalPosUpdated   = g.update(obstacles);
      if (astar.isAnimationOver())
      {
         if (playerPosUpdated || goalPosUpdated) astar.computeAStar(p, g); // computes the path if possible
      }
      drawGrid();
      astar.draw();
      g.draw();
      p.draw();
      EndDrawing();
   }

   return 0;
}
