#pragma once
#include "Common.h"

// Entity class to make it easy to work with raylib, treat grid cells as objects that are easy to position and draw
class Entity
{
public:
   Entity(float x, float y, float width, float height, Color color) :
      rect(x, y, width, height), col(color){}

   void setPosition(float x, float y)
   {
      rect.x = x;
      rect.y = y;
   }

   virtual void draw();

   int x() const { return static_cast<int>(rect.x) / GRID_SIZE; }
   int y() const { return static_cast<int>(rect.y) / GRID_SIZE; }
   Vector2 pos() const { return {rect.x, rect.y}; } 

protected:
   Rectangle rect;
   Color col;
};

class Obstacle : public Entity
{
public:
   Obstacle(float x, float y, float width, float height, Color color) : Entity(x, y, width, height, color), m_canBeDeleted(false) {}
   void setCanBeDeleted(bool canBeDeleted) { m_canBeDeleted = canBeDeleted; }
   bool getCanBeDeleted() const { return m_canBeDeleted; }

private:
   bool m_canBeDeleted;
};

// Class used for start/end nodes
class Endpoint : public Entity
{
public:
   Endpoint(float x, float y, float width, float height, Color color) : Entity(x, y, width, height, color), m_followMouse(false) {}
   virtual void draw();
   bool update(const std::unordered_map<int, Obstacle>& obstacles);
private:
   void shrink();
   void grow();
   bool m_followMouse;
};

