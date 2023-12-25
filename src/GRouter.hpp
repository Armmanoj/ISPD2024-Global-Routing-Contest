#include <vector>
#include<iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <map>
#include <algorithm>
#include <queue>
#include <cmath>

struct Dim3{
    int l;
    int x;
    int y;
};
typedef struct Dim3 Dim3;

class double_grid_3d {
    private: 
        std::vector<std::vector<std::vector<double>>> grid;
    public:
        double_grid_3d();
        double_grid_3d(int L, int X, int Y);
        double& at(std::size_t index1, std::size_t index2, std::size_t index3);
};

class chip {
    private:
        double_grid_3d Capacity;
    public:
        std::vector<double> unit_length_short_costs;
        std::vector<double> hEdgeLengths;
        std::vector<double> vEdgeLengths;
        std::vector<double> layerMinLengths;
        double unit_length_wire_cost; 
        double unit_via_cost;
        Dim3 dimensions;
        int nLayers;
        int xSize;
        int ySize;
        std::vector<int> layerDirections;
        chip(char* capFile);
        double distance(int ll, int xl, int yl, int xh, int yh, int lh);
};

class Net{
    private:
        std ::vector<std ::vector<std ::vector<int>>> net;
    public:
        int numPins;
        std ::string Name;
        Net();
        Net(int N, std ::vector<std ::vector<std ::vector<int>>> accessPoints, std ::string name);
        std ::vector<Dim3> getPin(int j);
};

class Route{
    private:
        // Here store the route information, a different data structure can be used
        std ::vector<std ::vector<int>> route;
        int lineCount;
        int viaCount;
        int wireCount;
        int routeCounter;
        // More variables may be added depending on how RRR is done
    public:
        Route();
        std ::vector<int> getline(int j);
        // The below constructor does an initial routing
        Route(Net net, chip chip);
        void RipUp();
        void reRoute();
        int get_lineCount();
        int get_viaCount();
        int get_wireCount();
        int get_routeCounter();
};

void grFileWrite(std ::map<int, Route>& routes, std ::map<int, Net>& nets, std ::string grFilename);
std ::map<int, Route> initial_route(std ::map<int, Net> nets, chip chip);
std ::map<int, Net> parseNet(std ::string netFilename);
