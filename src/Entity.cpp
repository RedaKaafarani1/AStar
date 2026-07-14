#include "Entity.h"

bool Entity::update()
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
      m_followMouse = false;
      auto [newX, newY] = getMouseGridPos();
      setPosition(newX, newY);
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
