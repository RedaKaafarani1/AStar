#include "Common.h"
#include "Entity.h"

class AStar
{
public:
   struct SearchedEntry { Point pos; double f_score; };
   AStar() : m_delay(0), m_message(""), m_lastUpdate(std::chrono::steady_clock::now()), m_currentStep(0), m_currentSearchStep(0), m_minMaxScore({0.0, 0.0}), m_animationOver(true){}
   AStar(float delay) : AStar() { m_delay = delay; }
   void computeAStar(const Endpoint& p, const Endpoint& g);
   void draw();
   const std::unordered_map<int, Obstacle>& updateObstacles();
   std::string_view getMessage() { return m_message; };
   void reconstructPath(PointVec& parent, const Node& a, const Node& b);
   bool isAnimationOver() const { return m_animationOver; }

private:
   PointVec m_path;
   std::unordered_map<int, Obstacle> m_obstacles;
   std::vector<SearchedEntry> m_searchedNodes;
   float m_delay;
   std::string m_message;
   std::chrono::steady_clock::time_point m_lastUpdate;
   size_t m_currentStep;
   size_t m_currentSearchStep;
   std::pair<double, double> m_minMaxScore;
   bool m_animationOver;
};
