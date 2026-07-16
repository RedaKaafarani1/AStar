#pragma once
#include "Common.h"

// Entity class to make it easy to work with raylib, treat grid cells as objects that are easy to position and draw
class Entity
{
public:
   enum class Type { Start, Goal, Obstacle };
   Entity() : m_followMouse(false) {} 
   Entity(float x, float y, float width, float height, Color color, Type type) :
      rect(x, y, width, height), col(color), m_followMouse(false), m_type(type) {}

   void setPosition(float x, float y)
   {
      rect.x = x;
      rect.y = y;
   }

   void draw();

   int x() const { return static_cast<int>(rect.x) / GRID_SIZE; }
   int y() const { return static_cast<int>(rect.y) / GRID_SIZE; }
   Vector2 pos() const { return {rect.x, rect.y}; } 

   bool update(const std::unordered_map<int, Entity>& obstacles);
   void shrink();
   void grow();

private:
   Rectangle rect;
   Color col;
   bool m_followMouse;
   Type m_type;
};
