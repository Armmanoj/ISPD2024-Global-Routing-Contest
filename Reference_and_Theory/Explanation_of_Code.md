# Global Routing flowchart  
  
## Step1: Parsing  
 Parse the .cap file. Make a grid that stores all the required information (demand, capacity, edgelength, etc) at each grid cell, and multiple variables to store the dimensions, layer directions, layer_min_lengths etc.  
 Parsing the net will be done in the next step along with pattern routing, where each net is parsed and temporarily pattern routed.

## Step2: Pattern routing  
 For a 2 -pin net, find the pair of access points with least distance between them, and connect them by the least cost L shape connection out of the 2 possibilities. If the costs are equal, make the lower (n coordinate system) connection.
 For a multi-pin net, calculate the centroid of the net, find the access points on each pin which is closest to the centroid. Then use Kruscal's algorithm to find the edges that have to be connected in a graph whose vertices are those access points, to form a minimum spanning tree. Order the edges of this spanning tree in increasing order of rectilinear edge length (ie. L shape length, which are the weights for this graph's edges).
 Then in the same order, do all the L shape connections. To choose which L connection to make, first compare the cost of both, remembering that overlapping edges are desired here, as that reduces the wire length, if the costs are equal, choose the one closer to the centroid, if that too is equal, choose the lower L. This construction is guaranteed to be within 1.5 of the Steiner minimum tree of the net.    
   
The distance between 2 connections is totalWL * unit_length_wire_cost+numVia * unit_via_cost * via_cost_scale; and via cost scale is 1 according to evaluator.cpp. The cost = dist+(delta in) the overflow cost or an approximation to it from evaluator.cpp, summed over all new vias introduced and all wires drawn in making 1 connection.  
 While doing this, make a dictionary of all nets as well as their initial routing solutions.  

## Step3: Decide which connections(2 access points, like in step 2) to rip-up  
  Order all 2 end connections in order of their net cost/(euclidean distance) and rip up in the same order till we reach a net that has the previous ratio less than say 1.5, else till half the nets are done.  
## Step4:  Rip-Up-And-Reroute (RRR)  
 Rip_up the net using BFS.    
 Use A* and heuristic optimized maze-route to reroute those promising connections, to minimize the cost as defined in evaluator.cpp.  

 ## Step5: Graphical/ visual representation/ code analysis  
 Here we must visualize the route of the chip. We must also graphically show how RRR improves the cost of the route. Then we must also profile our algorithms for memory usage delays and time various parts of our code.    

  
 Go back to step 3 and repeat till some condition, either time runs out or the cost has decreased by less than some percentage after 3 rounds.

 
## Functions and classes involved

function pattern_route(net, cost grid){
// This function is an implementation detail
If net size=2:
Do L shape routing.
Else:
First number all the nets. Then find the minimum spanning tree. Then make L shape connections.  Check for if any horizontal or vertical lines overlap. Then do simulated annealing to find the version with maximum overlap.  Return a Route object. This can be an external constructor.
}

Function RRRorder{
Make ordered array of nets to reroute.
Such will always be needed, even if RRR is done in parallel, and if this array is made using ML.
It will also decide when to stop the routing.
Also, after each RRR, the cost will be stored in an array.
}

Function ParseNet{
   Parses the .net file and creates all the Net and Route objects, as well as the cost grid.

Arrange all nets in a hash table or unordered_map, with integer keys, denoting the initial order of parsing.
}

Class Net-
. Stores the number of pins in the net( as an attribute, less likely as a function on the pin data)
. Store the bounding box
. Efficiently access all access points, pins, as this data is const,  storing them continuously, with integers in between telling the size if the next pin is one method.
. Initialized using external constructor in 1st parsing round.

Class Route-
. Store the routing connections data, for now in a format similar to in.PR_output, but for efficient access and traversal.
. Method to Check connectedness,
. Method to check whether all pins are covered.
. A reroute function, to be called later.
. A rip-up function, also to be called later.
. Store the wire length, via count, cost due to overflowing.

// note that distance function is now placed inside the chip class.
Class Demand
. Stores the number of wires passing right/north and the number of via passing up from each gCell. It stores this in some abstract data structure optimized for fast and efficient memory access.
. Has 3 functions that calculates net wirelength, viacost and overflow cost.
. Has function which calculates the overflow cost from demand, capacity, viacount.
. Has function which updates net cost of a step in a path (which is usefull in maze route or A*), which is inputted, due to adding a new via or unit gCell wire. 


### Simplest successful routing:  
 First find the lowest metal layer above metal1 that has horizontal tracks and that which has vertical tracks.
 To connect 2 pins, first find the 2 gCells corresponding to the 1st access point of each pin. Then make vias(1x1 routing lines/rectangles) that connect each gCell to the projection of the gCells on the 1st horizontal layer for the 1st gCell, and 1st vertical layer for the second gCell. Then from these projected points, make 2 lines that have the same x,y coordinate at their end points (like L shape). Then make vias to connect those 2 ends. In a k-pin net, connect k-1 pairs of 2 as before.  