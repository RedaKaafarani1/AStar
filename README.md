# AStar

A visual A* pathfinding demo built with C++ and [raylib](https://www.raylib.com/) as a small project for fun!

https://github.com/user-attachments/assets/f5472393-4631-4224-aa8a-70317f37a044

## What it does

Watch the A* algorithm search a grid for the shortest path between a start (green) and goal (red) node, avoiding obstacles you place yourself. Explored nodes are colored by their score (blue = low, orange = high), and the final path is animated once found.

## How it works

- The grid is made of cells; you drag the start/goal nodes (using left-click) and right-click to add or remove obstacles.
- Pressing `Space` runs A* using octile distance as the heuristic, with diagonal movement allowed.
- The result is animated: first the searched nodes, then the reconstructed path.
- You cannot move start/goal nodes nor place obstacles during animation

## Controls

| Action | Input |
|---|---|
| Move start/goal | Left-click drag |
| Add/remove obstacle | Right-click drag |
| Run pathfinding | `Space` |

## Build & run

Requires `raylib` installed.

```bash
cd src
make run
```
