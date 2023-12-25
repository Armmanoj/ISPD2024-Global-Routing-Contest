#include "GRouter.hpp"
/*
class double_grid_3d {
    private: 
        std::vector<std::vector<std::vector<double>>> grid;
    public:
        double_grid_3d();
        double_grid_3d(int L, int X, int Y);
        double& at(std::size_t index1, std::size_t index2, std::size_t index3);
};
*/
double_grid_3d::double_grid_3d() {}

        // 3 input constructor
double_grid_3d::double_grid_3d(int L, int X, int Y)
{
    grid.resize(L);
    for (int l = 0; l < L; ++l) {
        grid[l].resize(X);
        for (int x = 0; x < X; ++x) {
            grid[l][x].resize(Y);
        }
    }
}
double& double_grid_3d::at(std::size_t index1, std::size_t index2, std::size_t index3)
{
    return grid[index1][index2][index3];
}



/*
class chip {
    private:
        Dim3 dimensions;
        int nLayers;
        int xSize;
        int ySize;
        std::vector<double> unit_length_short_costs;
        std::vector<double> hEdgeLengths;
        std::vector<double> vEdgeLengths;
        std::vector<int> layerDirections;
        std::vector<double> layerMinLengths;
        double unit_length_wire_cost; 
        double unit_via_cost;
        double_grid_3d Capacity;
    public:
        chip(char* capFile);
        double distance(int ll, int xl, int yl, int xh, int yh, int lh);
};
*/
chip::chip(char* capFile)
{
    // Argument: a string containing the name of .cap file, ideally argv[1] from int main in cmd line
    // Opening the Resource file
    std::ifstream resourceFile(capFile);
    if (!resourceFile) {
        std::cerr << "Failed to open resource file." << std::endl;
        return;
    }
    // Parsing the layout dimensions
    std::string line;
    std::getline(resourceFile, line);
    std::vector<int> d;
    std::istringstream iss(line);
    for (int value; iss >> value;) {
        d.push_back(value);
    }
    nLayers = d[0];
    xSize = d[1];
    ySize = d[2];
    dimensions = {d[0],d[1],d[2]};
    std::cout << "dimensions: " << nLayers << " " << xSize << " " << ySize << std::endl;

    // Get unit costs
    std::getline(resourceFile, line);
    std::istringstream unitCosts(line);
    unitCosts >> unit_length_wire_cost >> unit_via_cost;
    double cost;
    while (unitCosts >> cost) {
        unit_length_short_costs.push_back(cost);
    }
    std::cout << "Got unit costs: " << unit_length_wire_cost << " " << unit_via_cost << " "  << std::endl;

    // Get edge lengths
    std::getline(resourceFile, line);
    std::istringstream hEdgeLine(line);
    while (hEdgeLine >> cost) {
        hEdgeLengths.push_back(cost);
    }

    for (int x=0; x<xSize-1; ++x) {
        std::cout << "hedge " << x << " length " << hEdgeLengths[x] << std::endl;
    }

    std::getline(resourceFile, line);
    std::istringstream vEdgeLine(line);
    while (vEdgeLine >> cost) {
        vEdgeLengths.push_back(cost);
    }
    for (int y=0; y<ySize-1; ++y) {
        std::cout << "vedge " << y << " length " << vEdgeLengths[y] << std::endl;
    }

    std::cout << "Got edge lengths: " << std::endl;

    // Next step will be slow as huge amounts of memory gets allocated
    // Get capacity map
    double_grid_3d Capacity(nLayers, xSize, ySize);
    for (int l = 0; l < nLayers; ++l) {
        std::getline(resourceFile, line);
        std::istringstream layerInfo(line);
        std::string layer_name;
        layerInfo >> layer_name;
        int direction;
        layerInfo >> direction;
        layerDirections.push_back(direction);
        double min_length;
        layerInfo >> min_length;
        layerMinLengths.push_back(min_length);

        for (int y = 0; y < ySize; ++y) {
            std::getline(resourceFile, line);
            std::istringstream capacityInfo(line);
            for (int x = 0; x < xSize; ++x) {
                capacityInfo  >> Capacity.at(l,x,y);
            }
        }
    }
    std::cout << "Got capacity map" << std::endl;

    for (int l=0; l< nLayers; ++l) {
        std::cout << "Layer " << l << " direction " << layerDirections[l] << " minLength " << layerMinLengths[l] << std::endl;
    }
    resourceFile.close();
}
double chip::distance(int ll, int xl, int yl, int xh, int yh, int lh)
{
    double dist=0;
    for (xl; xl<xh; xl++){
        dist+=hEdgeLengths[xl]*unit_length_wire_cost;
    }
    for (yl; yl<yh; yl++) {
        dist+=vEdgeLengths[yl]*unit_length_wire_cost;
    }  
    dist+=unit_via_cost*(double)(ll-lh);
    return dist;
}
