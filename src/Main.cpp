#include "Common.h"
#include "Entity.h"
#include "AStar.h"

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
   Endpoint p{0, 0, GRID_SIZE, GRID_SIZE, GREEN};
   // Goal
   Endpoint g{ GRID_SIZE * (ROWS - 1), GRID_SIZE * (COLS - 1), GRID_SIZE, GRID_SIZE, RED };

   AStar astar {20};
   std::unordered_map<int, Obstacle> obstacles;

   while (!WindowShouldClose())
   {
      BeginDrawing();
      ClearBackground(BLACK);

      // while animating, start/goal and obstacles cannot be updated
      if (astar.isAnimationOver())
      {
         obstacles = astar.updateObstacles(); // adds obstacles with right click
         bool playerPosUpdated = p.update(obstacles); // returns true if player position changed
         bool goalPosUpdated   = g.update(obstacles);
         if (playerPosUpdated || goalPosUpdated) astar.computeAStar(p, g); // computes the path if possible
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
