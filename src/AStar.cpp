#include "AStar.h"
#include "Common.h"

double octileDist(const Node& a, const Node& b)
{
   int dx = std::abs(a.pos.x - b.pos.x);
   int dy = std::abs(a.pos.y - b.pos.y);

   return std::sqrt(2.0) * std::min(dx, dy) + (std::max(dx, dy) - std::min(dx, dy));
}

void AStar::draw()
{
   auto now = std::chrono::steady_clock::now();
   auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastUpdate).count();

   // Path animation, starts when searched nodes animation ends
   if (elapsed > m_delay && (m_currentStep + 1 < m_path.size()) && m_currentSearchStep == m_searchedNodes.size())
   {
      m_currentStep++;
      m_lastUpdate = std::chrono::steady_clock::now();
   }

   // Searched nodes animation
   if (!m_searchedNodes.empty())
   {
      if (elapsed > m_delay && m_currentSearchStep < m_searchedNodes.size())
      {
         m_currentSearchStep++;
         m_lastUpdate = std::chrono::steady_clock::now();
      }

      double minF = m_minMaxScore.first;
      double maxF = m_minMaxScore.second;
      for (size_t i = 0; i < m_currentSearchStep; i++)
      {
         // Color depends on each nodes score
         auto& e = m_searchedNodes[i];
         float t = static_cast<float>((e.f_score - minF) / (maxF - minF + 0.0001));
         Color c = ColorLerp(BLUE, ORANGE, t);
         DrawRectangle(e.pos.x * GRID_SIZE, e.pos.y * GRID_SIZE, GRID_SIZE, GRID_SIZE, Fade(c, 0.6f));
      }
   }

   for (size_t i = 0; i < m_currentStep; i++)
   {
      const auto& s = m_path[i];
      const auto& e = m_path[i + 1];
      // Draw line from center of first node to center of the next one
      Vector2 sPos = {s.x * GRID_SIZE * 1.0f + GRID_SIZE * 1.0f / 2, s.y * GRID_SIZE * 1.0f + GRID_SIZE * 1.0f / 2};
      Vector2 ePos = {e.x * GRID_SIZE * 1.0f + GRID_SIZE * 1.0f / 2, e.y * GRID_SIZE * 1.0f + GRID_SIZE * 1.0f / 2};
      DrawLineEx(sPos, ePos, 2.0f, WHITE);
   }

   // Either we finished animating path or we did not find one, in which case m_currentStep + 1 will be > m_path.size() since it will be 0
   if ((m_currentStep + 1 >= m_path.size()))
      m_animationOver = true;

   for (auto [index, obstacle] : m_obstacles)
      obstacle.draw();

   DrawText(m_message.c_str(), WIDTH / 2 - 150, 0, 30, RED);
}

void AStar::reset()
{
   m_path.clear();
   m_searchedNodes.clear();
   m_message.clear();
   m_currentStep = 0;
   m_currentSearchStep = 0;
   m_minMaxScore = {0.0, 0.0};
   m_animationOver = true;
}

const std::unordered_map<int, Obstacle>& AStar::updateObstacles()
{
   if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
   {
      auto [mx, my] = getMouseGridPos();
      int idx = gridIndex(mx, my);

      // Do not process this cell if already processed during this drag (drag valid until right mouse release below)
      if (!m_processedThisDrag.contains(idx))
      {
         auto it = m_obstacles.find(idx);
         if (it != m_obstacles.end())
            m_obstacles.erase(it);
         else if (!isOutsideGrid(mx, my))
         {
         // do not place outside grid (which can happen when in full screen mode)
            m_obstacles.insert({idx, {mx, my, GRID_SIZE, GRID_SIZE, RBLUE}});
         }
         m_processedThisDrag.insert(idx);
      }
   }

   // Finish drag, reset processed nodes
   if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT))
      m_processedThisDrag.clear();

   return m_obstacles;
}

void AStar::reconstructPath(PointVec& parent, const Node& a, const Node& b)
{
   Point curr = b.pos;
   while (curr != a.pos)
   {
      m_path.push_back(curr);
      curr = parent[curr.y * COLS + curr.x];
   }
   m_path.push_back(a.pos);
   std::reverse(m_path.begin(), m_path.end());
}

void AStar::computeAStar(const Endpoint& p, const Endpoint& g)
{
   Node a{{p.x(), p.y()}};
   Node b{{g.x(), g.y()}};

   std::priority_queue<Node, std::vector<Node>, Node::Compare> open_set;
   std::vector<bool> closed_set(ROWS * COLS, false);
   std::vector<double> g_score(ROWS * COLS, std::numeric_limits<int>::max());
   PointVec parent(ROWS * COLS, {-1, -1});
   std::vector<bool> walkable(ROWS * COLS, true);
   std::vector<bool> alreadyAdded(ROWS * COLS, false);
   std::vector<size_t> searchedNodeIndex(ROWS * COLS, 0);

   m_path.clear();
   m_searchedNodes.clear();
   m_message.clear();
   m_currentStep = 0;
   m_currentSearchStep = 0;
   m_minMaxScore = {0.0, 0.0};
   m_animationOver = false;

   for (const auto &[index, obstacle] : m_obstacles)
      walkable[index] = false;

   g_score[a.pos.y * COLS + a.pos.x] = 0.0;
   a.f_score = octileDist(a, b);
   open_set.push(a);

   while (!open_set.empty())
   {
      Node curr = open_set.top();
      open_set.pop();

      int currIdx = curr.pos.y * COLS + curr.pos.x;

      if (curr.f_score != g_score[currIdx] + octileDist(curr, b))
         continue;

      if (curr == b)
      {
         m_message = "Path found! Cost: " + std::to_string(g_score[currIdx]);
         reconstructPath(parent, a, b);
         auto [minIt, maxIt] = std::minmax_element(
          m_searchedNodes.begin(), m_searchedNodes.end(),
          [](const SearchedEntry& a, const SearchedEntry& b) { return a.f_score < b.f_score; }
         );
         m_minMaxScore = std::make_pair(minIt->f_score, maxIt->f_score);
         return;
      }

      closed_set[currIdx] = true;
      
      for (auto [dx, dy] : DIAGDIRS)
      {
         Node neigh{{curr.pos.x + dx, curr.pos.y + dy}};

         int neighIdx = neigh.pos.y * COLS + neigh.pos.x;

         if (isOutsideGrid(neigh) || !walkable[neighIdx])
            continue;

         if (closed_set[neighIdx])
            continue;

         bool isDiagonal = (dx != 0 && dy != 0);
         double move_score = g_score[currIdx] + (isDiagonal ? std::sqrt(2.0) : 1);
         if (move_score < g_score[neighIdx])
         {
            parent[neighIdx] = curr.pos;
            g_score[neighIdx] = move_score;
            neigh.f_score = g_score[neighIdx] + octileDist(neigh, b);
            if (!alreadyAdded[neighIdx])
            {
               searchedNodeIndex[neighIdx] = m_searchedNodes.size();
               m_searchedNodes.push_back({neigh.pos, neigh.f_score});
               alreadyAdded[neighIdx] = true;
            }
            else
            {
               m_searchedNodes[searchedNodeIndex[neighIdx]].f_score = neigh.f_score;
            }
            open_set.push(neigh);
         }
      }
   }
   m_message = "Path not found!";
}
