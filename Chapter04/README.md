SDL_Log() for writing to console

# Pathfinding

## Graphs
Nodes and edges between some of them. Edges can have direction, and sometimes weight representing a cost

Eg adjacency lists. Each node has a list of adjacent nodes

Either
- store GraphNodes in a Graph
- node stores edges, that store the nodes they point to with the weight

## Breadth first search
Go one step down the graph and compute all possible paths, before going further down the graph

Consider searching in reverse if you need to reconstruct the path after

Guarantees fewest steps, but doesn't account for weights. Also tests paths that move away from the goal

## Heuristics
Information about the problem used to shorten the search. Eg if a path is closer to the solution than the others, run this to completion first

The point of this could be to decide if point A or B are currently closer to the goal

H(x) is the estimated cost from node x to the goal. If a heuristic always results in a lower cost than the true cost, it is admissible. Else, it is inadmissible.

2 heuristic examples:
- Manhattan - no diagonals, sometimes not admissible
h(x) = |start.x-end.x| + |start.y-end.y|
- Euclidean - diagonals, almost always admissible
h(x) = sqrt((start.x-end.x)²+(start.y-end.y)²)

## Greedy best first search
follows up on nodes with the minimum heuristic at each step. This cannot guarantee a minimum path

openSet - nodes still under consideration
closedSet - nodes no longer under consideration

## A* search
f(x) = g(x) + h(x) where g(x) is the path cost

Same as GBFS but:
- computes path cost before adding a node to openSet
- a node can change its parent if it would lower the path cost (number of steps to get to this node)

A* is computationally expensive because it may need to track several paths to resolve ties. You can multiply h(x) by 0.75 to weight g(x) more highly. Works on any layout. JPS+ us more efficient for grids (only)

## Djikstra's algorithm
Calculates the path cost to every node. Then afterwards, if a goal is at one of them, you can go to the closest. Generally more expensive than A*

A character can follow this path if we turn them to face the correct path at each step. Then so long as they don't move more than one node at once, they will follow the path

Many games won't have a grid. You can use:
- path nodes - waypoints for the AI to head to. If there's no obstruction between them, add an edge
- navigation mesh - break the map into polygons, and allow characters to move to different areas. Advantage of this is each character can calculate if a route is accessible. Recast is a tool to generate navigation meshes

## Game tree
Current game state is a node. Game states following each action are the next nodes

## Minimax
Max player - the side you are trying to make win
Min player - the other one

Max player is trying to maximise their score, the min player is trying to reduce the score of the max player

Compute the game tree, then each side follows the path to maximise their score.

You can also work out the best single move, in case you cannot compute a complete tree. Calculate a few moves ahead, and make the move with the best score as far as you can see. Then you need a way to score incomplete game states.

## Alpha-beta pruning
Removes nodes without checking their children if they give a worse score than another inspected, to reduce number of steps needed

Alpha is the best score the max player can get, initially infinity
Beta is the lowest score the max player can get, initially negative infinity
