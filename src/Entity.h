#pragma once
#include "Common.h"

// Entity class to make it easy to work with raylib, treat grid cells as objects that are easy to position and draw
class Entity
{
public:
   Entity() : m_followMouse(false) {} 
   Entity(float x, float y, float width, float height, Color color) :
      rect(x, y, width, height), col(color) {}

   void setPosition(float x, float y)
   {
      rect.x = x;
      rect.y = y;
   }

   void draw() const { DrawRectangleRec(rect, col); }

   int x() const { return static_cast<int>(rect.x) / GRID_SIZE; }
   int y() const { return static_cast<int>(rect.y) / GRID_SIZE; }
   Vector2 pos() const { return {rect.x, rect.y}; } 

   bool update();

private:
   Rectangle rect;
   Color col;
   bool m_followMouse;
};

// Player, for now nothing special wrt base class
class Player : public Entity
{
public:
   Player(float x, float y, float width, float height, Color color) :
      Entity(x, y, width, height, color) {}
};

// Goal, for now nothing special wrt base class
class Goal : public Entity
{
public:
   Goal(float x, float y, float width, float height, Color color) :
      Entity(x, y, width, height, color) {}
};
