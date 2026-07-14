#include "Common.h"
#include "Entity.h"
#include "AStar.h"

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

   Player p{0, 0, GRID_SIZE, GRID_SIZE, GREEN};
   Goal g{ GRID_SIZE * (ROWS - 1), GRID_SIZE * (COLS - 1), GRID_SIZE, GRID_SIZE, RED};
   AStar astar {20};

   while (!WindowShouldClose())
   {
      BeginDrawing();
      ClearBackground(BLACK);

      if (astar.isAnimationOver())
      {
         bool playerPosUpdated = p.update(); // returns true if player position changed
         bool goalPosUpdated   = g.update();
         if (playerPosUpdated || goalPosUpdated) astar.computeAStar(p, g); // computes the path if possible
      }

      drawGrid();
      astar.updateObstacles(); // adds obstacles with right click
      astar.draw();
      g.draw();
      p.draw();
      EndDrawing();
   }

   return 0;
}
