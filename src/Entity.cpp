#include "Entity.h"
#include "Common.h"

void Entity::shrink()
{
   int newSize = GRID_SIZE * 0.9;
   if (rect.width > newSize)
   {
      rect.width  -= (GRID_SIZE - newSize) / 2;
      rect.height -= (GRID_SIZE - newSize) / 2;
   }
}

void Entity::grow()
{
   if (rect.width < GRID_SIZE)
   {
      rect.width  += (GRID_SIZE - rect.width) / 2;
      rect.height += (GRID_SIZE - rect.height) / 2;
   }
}

void Entity::draw()
{
   if (m_followMouse)
      shrink();
   else
      grow();
   DrawRectangleRec(rect, col);
}

std::pair<float, float> getAvailablePosition(const std::unordered_map<int, Entity>& obstacles)
{
   auto [mX, mY] = getMouseGridPos();
   int newIdx = gridIndex(mX, mY); 
   if (obstacles.find(newIdx) == obstacles.end())
   {
      if (!isOutsideGrid(mX, mY))
         return {mX, mY};
      else return {0.0, 0.0}; // snap to 0,0 if we try to place outside grid (currently happens when in full screen mode, where we can place outside grid)
   }
   else 
   {
      // We keep testing while we have grid available, starting with directions that are at most 1 grid away from obstacle
      // and up to the number of cols.
      for (int i = 1; i < COLS; i++)
      {
         for (auto [dx, dy] : DIAGDIRS)
         {
            dx *= i;
            dy *= i;
            int newIdx = gridIndex(mX, mY, {dx, dy});
            if (obstacles.find(newIdx) == obstacles.end())
            {
               // is new position still inside grid ?
               float newX = mX + dx * GRID_SIZE;
               float newY = mY + dy * GRID_SIZE;
               if (!isOutsideGrid(newX, newY))
                  return {newX, newY};
            }
         }
      }
   }
   return {0.0, 0.0};
}

bool Entity::update(const std::unordered_map<int, Entity>& obstacles)
{
   // check if we are moving
   if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
   {
      Vector2 mPos = GetMousePosition();
      Vector2 pPos = pos();
      if (mPos.x >= pPos.x && mPos.x <= pPos.x + GRID_SIZE &&
          mPos.y >= pPos.y && mPos.y <= pPos.y + GRID_SIZE)
         m_followMouse = true;
   }

   //release player
   if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && m_followMouse)
   {
      auto newPos = getAvailablePosition(obstacles); 
      setPosition(newPos.first, newPos.second);
      m_followMouse = false;
      return true;
   }

   //update position to snap to grid
   if (m_followMouse)
   {
      Vector2 mPos = GetMousePosition();
      rect.x = mPos.x - GRID_SIZE * 1.0f / 2;
      rect.y = mPos.y - GRID_SIZE * 1.0f / 2;
   }
   return false;
}
