#include "Common.h"
#include "Entity.h"
#include "AStar.h"
#include <raylib.h>

void drawGrid()
{
   // ROWS = COLS, so I can do it this way for now
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

   //Start
   Endpoint p{0, 0, GRID_SIZE, GRID_SIZE, RGREEN};
   // Goal
   Endpoint g{ GRID_SIZE * (ROWS - 1), GRID_SIZE * (COLS - 1), GRID_SIZE, GRID_SIZE, RRED };

   AStar astar {20};
   std::unordered_map<int, Obstacle> obstacles;

   while (!WindowShouldClose())
   {
      BeginDrawing();
      ClearBackground(BLACK);

      // while animating, start/goal and obstacles cannot be updated
      if (astar.isAnimationOver())
      {
         obstacles = astar.updateObstacles();
         bool update = p.update(obstacles) || g.update(obstacles);
         if (update)
            astar.reset();
         if (IsKeyPressed(KEY_SPACE))
            astar.computeAStar(p, g);
      }

      //draw everything, order matters
      drawGrid();
      astar.draw();
      g.draw();
      p.draw();
      EndDrawing();
   }

   return 0;
}
