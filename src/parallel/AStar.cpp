#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <climits>

struct Node
{
    int x, y; // Coordinates of the node
    int g;    // Cost from the start node to current node
    int h;    // Heuristic: estimated cost from current node to the goal
    int f;    // Total cost: g + h

    bool operator>(const Node &other) const
    {
        return f > other.f;
    }
};

// Function to check if a given cell is valid (inside the maze and not blocked)
bool isValid(int x, int y, int m, int n, const std::vector<std::vector<bool>> &maze)
{
    return x >= 0 && y >= 0 && x < m && y < n && !maze[x][y];
}

// Function to calculate the heuristic (Manhattan distance)
int heuristic(int x, int y, const std::pair<int, int> &end)
{
    return std::abs(x - end.first) + std::abs(y - end.second);
}

std::vector<std::pair<int, int>> AStar(std::vector<std::vector<bool>> &maze, int m, int n,
                                       std::pair<int, int> start, std::pair<int, int> end)
{
    std::vector<std::pair<int, int>> path;
    // int m = maze.size();
    // int n = maze[0].size();

    // Directions: up, down, left, right
    int dx[] = {-1, 1, 0, 0};
    int dy[] = {0, 0, -1, 1};

    // Priority queue to store nodes based on their total cost
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;

    // Initialize the start node
    Node startNode = {start.first, start.second, 0, heuristic(start.first, start.second, end), 0};
    pq.push(startNode);

    // Matrix to store the cost to reach each cell
    std::vector<std::vector<int>> cost(m, std::vector<int>(n, INT_MAX));
    cost[start.first][start.second] = 0;

    // Matrix to store the parent of each cell
    std::vector<std::vector<std::pair<int, int>>> parent(m, std::vector<std::pair<int, int>>(n, {-1, -1}));

    while (!pq.empty())
    {
        Node current = pq.top();
        pq.pop();

        // Check if the current node is the goal
        if (current.x == end.first && current.y == end.second)
        {
            // Reconstruct the path from the goal to the start
            while (current.x != -1 && current.y != -1)
            {
                path.push_back({current.x, current.y});
                current = {parent[current.x][current.y].first, parent[current.x][current.y].second, 0, 0, 0};
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        // Explore neighbors
#pragma omp parallel for
        for (int i = 0; i < 4; ++i)
        {
            int newX = current.x + dx[i];
            int newY = current.y + dy[i];

            // Check if the neighbor is valid
            if (isValid(newX, newY, m, n, maze))
            {
                int newG = current.g + 1;

                // Check if the new path to the neighbor is shorter
                if (newG < cost[newX][newY])
                {
                    cost[newX][newY] = newG;
                    int newH = heuristic(newX, newY, end);
                    int newF = newG + newH;
                    pq.push({newX, newY, newG, newH, newF});
                    parent[newX][newY] = {current.x, current.y};
                }
            }
        }
    }

    // If the goal is not reachable, return an empty path
    return path;
}
