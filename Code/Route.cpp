#include "GRouter.hpp"

/*
class Net{
    private:
        std ::vector<std ::vector<std ::vector<int>>> net;
    public:
        int numPins;
        std:: string name;
        Net();
        Net(int N, std ::vector<std ::vector<std ::vector<int>>> accessPoints, std ::string name);
        std ::vector<Dim3> getPin(int j);
};
*/

Net::Net() {}
Net::Net(int N, std ::vector<std ::vector<std ::vector<int>>> accessPoints, std ::string name)
{
    numPins = N;
    net = accessPoints;
    Name = name;
}
std ::vector<Dim3> Net::getPin(int j)
{
    std::vector<std::vector<int>> pinv = net[j];
    std::vector<Dim3> pin;
    Dim3 accpoint;
    for (int i = 0; i < pinv.size(); i++)
    {
        accpoint = {pinv[i][0],pinv[i][1],pinv[i][2]};
        pin.push_back(accpoint);
    }
    return pin;
}

/*
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
};
*/
Route::Route() {}
Route::Route(Net net, chip chip): lineCount(0), viaCount(0), wireCount(0), routeCounter(1)
{
    std::vector<int> line;
    int lv=1;
    int i = chip.layerDirections[lv];
    int lh=1;
    int j = chip.layerDirections[lh];
    while (i != 1){
        lv++;
        i=chip.layerDirections[lv];
    }
    while (j != 0){
        lh++;
        j=chip.layerDirections[lh];
    }
    std::cout << lv << lh << std::endl;
    for (int k=0; k<net.numPins-1; k++){
        std::cout << k << std::endl;
        Dim3 p0=net.getPin(k)[0];
        Dim3 p1=net.getPin(k+1)[0];
        // Correctly connects line on lh layer
        line = {std::min(p0.x,p1.x),p0.y,std::max(p0.x+1,p1.x+1),p0.y+1,lh};
        route.push_back(line);
        // Correctly connects line on lv layer
        line = {p1.x,std::min(p0.y,p1.y),p1.x+1,std::max(p0.y+1,p1.y+1),lv};
        route.push_back(line);
        for (i=std::min(lv,lh)+1;i<std::max(lv,lh);i++){
            line = {p1.x,p0.y,p1.x+1,p0.y+1,i};
            route.push_back(line);
        }
        for (i=std::min(lh,p0.l);i<std::max(lh,p0.l);i++){
            line = {p0.x,p0.y,p0.x+1,p0.y+1,i};
            route.push_back(line);
        }
        for (i=std::min(lv,p1.l);i<std::max(lv,p1.l);i++){
            line = {p1.x,p1.y,p1.x+1,p1.y+1,i};
            route.push_back(line);
        }
    }  
    lineCount=route.size();
}
std ::vector<int> Route::getline(int j)
{
    if (j<lineCount){
        return route[j];
    }
    else{
        // An empty vector is returned, which can be checked using v.empty()
        std::vector<int> v;
        return v;
    }
}
void Route::RipUp()
{
    viaCount=0;
    wireCount=0;
    lineCount=0;
    // The next step may cause slowdown due to too much memory allocations/ deallocations
    route.clear();
}
void Route::reRoute()
{
    ++routeCounter;
}

int Route::get_lineCount()
{
    return lineCount;
}
int Route::get_viaCount()
{
    return viaCount;
}
int Route::get_wireCount()
{
    return wireCount;
}
int Route::get_routeCounter()
{
    return routeCounter;
}

/*class Demand{
    private:
    public:
        Demand() {}
        double overflowLossFunc(double input, double slope) {
            /*Here, input = capacity - demand, slope = 0.5*/
            /*if (input >= 0) {
                return 0.5 * (exp(-1 * input * slope));
            } else {
                return 0.5  - input * slope;
            }
        }*/

/*
        void commitVia(int l, int x, int y, chip chip, double viaCount) {
            int L = chip.nLayers, X = chip.xSize, Y = chip.ySize;
            assert(l + 1 < L);
            for (int layer = l; layer <= l + 1; ++layer) {
                int direction = chip.layerDirections[layer];
                if (direction == 0) {
                    if (x + 1 < X) {
                        Demand.at(layer,x,y) = Demand.at(layer,x,y) + (chip.layerMinLengths[layer] * viaCount) / chip.hEdgeLengths[x];
                    }
                    if ( x > 0) {
                        Demand.at(layer,x-1,y) = Demand.at(layer,x-1,y) + (chip.layerMinLengths[layer] *  viaCount) / chip.hEdgeLengths[x-1];
                    }

                } else {
                    if (y + 1 < Y) {
                        Demand.at(layer,x,y) = Demand.at(layer,x,y) + (chip.layerMinLengths[layer] *  viaCount) / chip.vEdgeLengths[y];
                    }
                    if ( y > 0) {
                        Demand.at(layer,x,y-1) = Demand.at(layer,x,y-1) + (chip.layerMinLengths[layer] *  viaCount) / chip.vEdgeLengths[y-1];
                    }
                }
            }
}
*/