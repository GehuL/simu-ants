#include "tiles.h"
#include "types.h"
#include <stack>
#include <queue>
#include <functional>
#include <utility>

using namespace simu;

std::vector<Vec2i> Grid::findPath(Vec2i start, Vec2i dest)
{
    using element = std::pair<int, Vec2i>;
    
    std::priority_queue<element, std::vector<element>, std::greater<element>> edges;
    std::unordered_map<Vec2i, Vec2i, VecHasher<int>> paths;
    std::unordered_map<Vec2i, int, VecHasher<int>> reached;
    const std::array<Vec2i, 4> directions = {Vec2i(0, 1), Vec2i(1, 0), Vec2i(-1, 0), Vec2i(0, -1)};
    
    edges.push(element(0, start));
    reached[start] = 0;
    paths[start] = Vec2i(0, 0);

    Vec2i current;

    while(!edges.empty())
    {
        current = edges.top().second;
        edges.pop();

        if(current == dest)
            break;

        // setTile(FOOD, current.x, current.y);

        for(const auto& dir : directions)
        {
            const Vec2i newEdge = current + dir;
            Tile tile = getTile<true>(newEdge);

            if(tile.flags.solid)
                continue;

            int cost = reached[current] + 1;

            if(reached.find(newEdge) == reached.end() || cost < reached[newEdge])
            {
                paths[newEdge] = current;
                reached[newEdge] = cost;
                int totalCost = cost + newEdge.distance(dest);
                edges.push(element(totalCost, newEdge));
            }
        }
    }

    std::vector<Vec2i> path;
    while(current != start)
    {
        Vec2i parent = paths[current];
        current = parent;
        path.push_back(parent);
    }

    return path;
}


int Grid::pathDistance(Vec2i start, Vec2i dest)
{
    return findPath(start, dest).size();
}