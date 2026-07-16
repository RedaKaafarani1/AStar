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
