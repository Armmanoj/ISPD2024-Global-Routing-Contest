#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <cassert>
#include <cmath>
#include <set>
#include <sstream>
#include <algorithm>
#include <unordered_map>

class Net {
public:
    std::string name;
    int numPins;
    std::vector<std::vector<std::vector<int>>> accessPoints;

    Net(std::string n, int num, std::vector<std::vector<std::vector<int>>>& access) : name(n), numPins(num), accessPoints(access) {};
    Net() {};
};

void traversal(std::vector<int>& point, std::vector<std::vector<std::vector<int>>>& GR, std::vector<int>& layerDirections) {
    std::vector<std::vector<int>> queue;
    queue.push_back(point);
    int l = point[0], x = point[1], y = point[2];
    GR[l][x][y] = 0;
    int L = GR.size(), X = GR[0].size(), Y = GR[0][0].size();
    while (!queue.empty()) {
        l = queue[0][0];
        x = queue[0][1];
        y = queue[0][2];
        queue.erase(queue.begin());
        int direction = layerDirections[l];
        // east
        if (l > 0 && direction == 0 && x > 0) {
            if (GR[l][x - 1][y] > 0) {
                GR[l][x - 1][y] = 0;
                queue.push_back({l, x - 1, y});
            }
        }
        // west
        if (l > 0 && direction == 0 && x < X - 1) {
            if (GR[l][x + 1][y] > 0) {
                GR[l][x + 1][y] = 0;
                queue.push_back({l, x + 1, y});
            }
        }
        // south
        if (l > 0 && direction == 1 && y > 0) {
            if (GR[l][x][y - 1] > 0) {
                GR[l][x][y - 1] = 0;
                queue.push_back({l, x, y - 1});
            }
        }
        // north
        if (l > 0 && direction == 1 && y < Y - 1) {
            if (GR[l][x][y + 1] > 0) {
                GR[l][x][y + 1] = 0;
                queue.push_back({l, x, y + 1});
            }
        }
        // up
        if (l < L - 1) {
            if (GR[l + 1][x][y] > 0) {
                GR[l + 1][x][y] = 0;
                queue.push_back({l + 1, x, y});
            }
        }
        // down
        if (l > 0) {
            if (GR[l - 1][x][y] > 0) {
                GR[l - 1][x][y] = 0;
                queue.push_back({l - 1, x, y});
            }
        }
    }
}

int arraySum(std::vector<std::vector<std::vector<int>>>& A) {
    int L = A.size(), X = A[0].size(), Y = A[0][0].size();
    // std::cout << "array dimensions: " << L << " " << X << " " << Y << std::endl;
    int cnt = 0;
    for (int l=0; l<L; ++l) {
        for (int x=0; x<X; ++x) {
            for (int y=0; y<Y; ++y) {
                cnt = cnt + A[l][x][y];
            }
        }
    }
    return cnt;
}

void commitVia(std::vector<int>& point, double demand, std::vector<std::vector<std::vector<double>>>& Demand, std::vector<int>& layerDirections, std::vector<double>& hEdgeLengths, std::vector<double>& vEdgeLengths, std::vector<double>& layerMinLengths) {
    int L = Demand.size(), X = Demand[0].size(), Y = Demand[0][0].size();
    int l = point[0], x = point[1], y = point[2];
    assert(l + 1 < L);
    for (int layer = l; layer <= l + 1; ++layer) {
        int direction = layerDirections[layer];
        if (direction == 0) {
            if (x + 1 < X) {
                Demand[layer][x][y] = Demand[layer][x][y] + (layerMinLengths[layer] * demand) / hEdgeLengths[x];
            }
            if ( x > 0) {
                Demand[layer][x-1][y] = Demand[layer][x-1][y] + (layerMinLengths[layer] *  demand) / hEdgeLengths[x-1];
            }

        } else {
            if (y + 1 < Y) {
                Demand[layer][x][y] = Demand[layer][x][y] + (layerMinLengths[layer] *  demand) / vEdgeLengths[y];
            }
            if ( y > 0) {
                Demand[layer][x][y-1] = Demand[layer][x][y-1] + (layerMinLengths[layer] *  demand) / vEdgeLengths[y-1];
            }
        }
    }
}

double logistic(double input, double slope){
    return 1.0 / (1.0 + exp(input * slope));
}

double overflowLossFunc(double input, double slope) {
    if (input >= 0) {
        return 0.5 * (exp(-1 * input * slope));
    } else {
        return 0.5  - input * slope;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " resource_file net_file GR_file" << std::endl;
        return 1;
    }

    std::ifstream resourceFile(argv[1]);
    if (!resourceFile) {
        std::cerr << "Failed to open resource file." << std::endl;
        return 1;
    }

    std::ifstream netFile(argv[2]);
    if (!netFile) {
        std::cerr << "Failed to open net file." << std::endl;
        return 1;
    }

    std::ifstream grFile(argv[3]);
    if (!grFile) {
        std::cerr << "Failed to open GR file." << std::endl;
        return 1;
    }

    // Parse resource file
    // Get layout dimensions
    std::string line;
    std::getline(resourceFile, line);
    std::vector<int> dimensions;
    std::istringstream iss(line);
    for (int value; iss >> value;) {
        dimensions.push_back(value);
    }
    int nLayers = dimensions[0];
    int xSize = dimensions[1];
    int ySize = dimensions[2];

    std::cout << "dimensions: " << nLayers << " " << xSize << " " << ySize << std::endl;

    // Get unit costs
    std::getline(resourceFile, line);
    std::istringstream unitCosts(line);
    double unit_length_wire_cost, unit_via_cost;
    unitCosts >> unit_length_wire_cost >> unit_via_cost;

    std::vector<double> unit_length_short_costs;
    double cost;
    while (unitCosts >> cost) {
        unit_length_short_costs.push_back(cost);
    }
    std::cout << "Got unit costs: " << unit_length_wire_cost << " " << unit_via_cost << " "  << std::endl;

    // Get edge lengths
    std::vector<double> hEdgeLengths;
    std::getline(resourceFile, line);
    std::istringstream hEdgeLine(line);
    while (hEdgeLine >> cost) {
        hEdgeLengths.push_back(cost);
    }

    for (int x=0; x<xSize-1; ++x) {
        std::cout << "hedge " << x << " length " << hEdgeLengths[x] << std::endl;
    }

    std::vector<double> vEdgeLengths;
    std::getline(resourceFile, line);
    std::istringstream vEdgeLine(line);
    while (vEdgeLine >> cost) {
        vEdgeLengths.push_back(cost);
    }
    for (int y=0; y<ySize-1; ++y) {
        std::cout << "vedge " << y << " length " << vEdgeLengths[y] << std::endl;
    }

    std::cout << "Got edge lengths: " << std::endl;

    // Get capacity map
    std::vector<std::vector<std::vector<double>>> Capacity(nLayers, std::vector<std::vector<double>>(xSize, std::vector<double>(ySize)));
    std::vector<int> layerDirections;
    std::vector<double> layerMinLengths;

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
                capacityInfo >> Capacity[l][x][y];
            }
        }
    }
    std::cout << "Got capacity map" << std::endl;

    for (int l=0; l< nLayers; ++l) {
        std::cout << "Layer " << l << " direction " << layerDirections[l] << " minLength " << layerMinLengths[l] << std::endl;
    }

    // Parse net file
    // Get net info
    std::string name;
    int numPins = 0;
    // std::vector<Net> nets;
    std::unordered_map<std::string, Net> nets;
    // int netId = -1;
    std::vector<std::vector<std::vector<int>>> accessPoints;

    while (std::getline(netFile, line)) {
        // if (line.find("net") != std::string::npos || line.find("pin") != std::string::npos) {
        if (line.find("(") == std::string::npos && line.find(")") == std::string::npos && line.length()>1) {
            // name = line.substr(0, line.size() - 1);
            name = line;
            size_t found = name.find('\n');
            if (found != std::string::npos) {
                name.erase(found, 1);
            }
            // netId = std::stoi(line.substr(3));
            numPins = 0;
            // std::cout << name << " " << netId << std::endl;
        } else if (line.find('[') != std::string::npos) {
            std::vector<std::vector<int>> access;
            std::string text = line.substr(1, line.size() - 2); // Remove brackets and trailing comma
            std::string charsToRemove = "(),";
            text.erase(std::remove_if(text.begin(), text.end(), [&charsToRemove](char c) {
                return charsToRemove.find(c) != std::string::npos;
            }), text.end());
            // std::cout << "current line is: " << text << std::endl;
            std::istringstream ss(text);
            int x, y, z;
            while (ss >> x >> y >> z) {
                std::vector<int> point;
                point.push_back(x);
                point.push_back(y);
                point.push_back(z);
                access.push_back(point);
            }
            accessPoints.push_back(access);
            numPins++;
        } else if (line.find(')') != std::string::npos) {
            // if (netId == 3153 || netId == 3151) {
            //     std::cout << name << " " << netId << " " << numPins << std::endl;
            // }
            // nets.push_back(Net(name, numPins, accessPoints));
            Net net(name, numPins, accessPoints);
            nets[name] = net;
            accessPoints.clear();
        }
    }
    std::cout << "Got net info" << std::endl;

    // Parse GR file
    std::vector<std::vector<std::vector<int>> > GR(nLayers, std::vector<std::vector<int>>(xSize, std::vector<int>(ySize)));
    std::set<std::string> checkedNets;
    std::vector<std::vector<std::vector<int>> > viaMap(nLayers, std::vector<std::vector<int>>(xSize, std::vector<int>(ySize)));
    std::vector<std::vector<std::vector<int>> > wireMap(nLayers, std::vector<std::vector<int>>(xSize, std::vector<int>(ySize)));
    for (int l=0; l<nLayers; ++l) {
        for (int x=0; x<xSize; ++x) {
            for (int y=0; y<ySize; ++y) {
                GR[l][x][y] = 0;
                viaMap[l][x][y] = 0;
                wireMap[l][x][y] = 0;
            }
        }
    }

    bool first = true;
    std::vector<int> startPoint(3);
    int uncovered_net_cnt = 0;
    while (std::getline(grFile, line)) {
        // if (line.find("net") != std::string::npos || line.find("pin") != std::string::npos) {
        if (line.find("(") == std::string::npos && line.find(")") == std::string::npos && line.find("Metal") == std::string::npos && line.find("metal") == std::string::npos && line.length()>1) {
            name = line;
            size_t found = name.find('\n');
            if (found != std::string::npos) {
                name.erase(found, 1);
            }
            // currentNetId = std::stoi(line.substr(3));
            first = true;
            // startPoint.clear();
            // std::cout << "Working on net " << currentNetId << std::endl;
        } else if (line.find("Metal") != std::string::npos || line.find("metal") != std::string::npos) {
            int xl, yl, xh, yh, layer;
            std::istringstream ss(line);
            ss >> xl >> yl >> xh >> yh;
            std::string temp;
            ss >> temp;
            layer = std::stoi(temp.substr(5));
            layer--;
            // std::cout << "Got metal " << xl << " " << yl << " " << xh << " " << yh << " " << layer << std::endl;

            int direction = layerDirections[layer];
            // std::cout << "Got layer direction " << direction << std::endl;
            if (direction == 0) { // horizontal
                assert(yl + 1 == yh);
            } else { // vertical
                assert(xl + 1 == xh);
            }
            // std::cout << "verified" << std::endl;

            for (int x = xl; x < xh; ++x) {
                for (int y = yl; y < yh; ++y) {
                    GR[layer][x][y] = 1;
                }
            }
            // std::cout << "Updated GR " << std::endl;

            if (first) {
                startPoint[0] = layer;
                startPoint[1] = xl;
                startPoint[2] = yl;
                first = false;
            }
            // std::cout << "Finish parsing Metal " << std::endl;

            // Rest of the code for processing Metal lines...
        } else if (line.find(')') != std::string::npos) {
            // Check connectivity, traversal, and other operations...
            // std::cout << "Check net " << std::endl;
            // Check connectivity
            Net net = nets[name];
            std::vector<std::vector<std::vector<int>>> accessPoints = net.accessPoints;
            int numPins = net.numPins;
            bool cover = false;
            // std::cout << "numPins " << numPins << std::endl;

            for (int i = 0; i < numPins; ++i) {
                std::vector<std::vector<int>> points = accessPoints[i];
                for (auto point : points) {
                    int l = point[0], x = point[1], y = point[2];
                    // std::cout << "Access point " << l << " " << x << " " << y << std::endl;
                    if (l == 0) {
                        if (GR[l][x][y] == 1 && GR[l + 1][x][y] == 1) {
                            cover = true;
                            break;
                        }
                    } else {
                        if (GR[l][x][y] == 1) {
                            cover = true;
                            break;
                        }
                    }
                }

                if (!cover) {
                    uncovered_net_cnt++;
                    std::cout << "uncovered net " << name << std::endl;
                }
                // assert(cover);
            }

            // Update viaMap, wireMap, and other metrics
            for (int l=0; l<nLayers-1; ++l) {
                for (int x=0; x<xSize; ++x) {
                    for (int y=0; y<ySize; ++y) {
                        if (GR[l][x][y] == 1 && GR[l+1][x][y] == 1) {
                            viaMap[l][x][y]++;
                            // std::cout << "found one via" << std::endl;
                        }
                    }
                }
            }

            for (int l=1; l<nLayers; ++l) {
                int direction = layerDirections[l];
                if (direction == 0) { // horizontal
                    for (int x=0; x<xSize-1; ++x) {
                        for (int y=0; y<ySize; ++y) {
                            if (GR[l][x][y] == 1 && GR[l][x+1][y] == 1) {
                                wireMap[l][x][y]++;
                            }
                        }
                    }
                } else { // vertical
                    for (int x=0; x<xSize; ++x) {
                        for (int y=0; y<ySize-1; ++y) {
                            if (GR[l][x][y] == 1 && GR[l][x][y+1] == 1) {
                                wireMap[l][x][y]++;
                            }
                        }
                    }
                }
            }

            // Perform traversal
            traversal(startPoint, GR, layerDirections);
            checkedNets.insert(name);
            // std::cout << "Checked net " << name << std::endl;
            assert(arraySum(GR)==0);  // Assuming you have a function to sum all elements in the GR array

        }
    }
    std::cout << "Got GR solution" << std::endl;
    std::cout << "num of uncovered nets " << uncovered_net_cnt << std::endl;

    // all nets are visited
    int unchecked_nets_cnt = nets.size() - checkedNets.size();
    std::cout << "num of unchecked nets: " << unchecked_nets_cnt << std::endl;
    // assert (checkedNets.size() == nets.size());

    // calculate numVia and totalWL
    int numVia = arraySum(viaMap);
    double totalWL = 0;
    // Calculate numVia and totalWL
    for (int l = 0; l < nLayers; ++l) {
        int direction = layerDirections[l];
        if (direction == 0) { // horizontal
            for (int x=0; x<xSize-1; ++x) {
                for (int y=0; y<ySize; ++y) {
                    if (wireMap[l][x][y] > 0) {
                        totalWL = totalWL + wireMap[l][x][y] * hEdgeLengths[x];
                    }
                }
            }
        } else { // vertical
            for (int x=0; x<xSize; ++x) {
                for (int y=0; y<ySize-1; ++y) {
                    if (wireMap[l][x][y] > 0) {
                        totalWL = totalWL + wireMap[l][x][y] * vEdgeLengths[y];
                    }
                }
            }
        }
    }
    std::cout << "Finished!" << std::endl;
    std::cout << "numVia: " << numVia << std::endl;
    std::cout << "totalWL: " << totalWL << std::endl;

    // Calculate routing demand
    std::vector<std::vector<std::vector<double>>> Demand(nLayers, std::vector<std::vector<double>>(xSize, std::vector<double>(ySize)));
    for (int l = 0; l < nLayers; ++l) {
        int direction = layerDirections[l];
        if (direction == 0) { // horizontal
            for (int x=0; x<xSize-1; ++x) {
                for (int y=0; y<ySize; ++y) {
                    Demand[l][x][y] =  wireMap[l][x][y];
                }
            }
        } else { // vertical
            for (int x=0; x<xSize; ++x) {
                for (int y=0; y<ySize-1; ++y) {
                    Demand[l][x][y] =  wireMap[l][x][y];
                }
            }
        }
    }
    for (int l = 0; l < nLayers-1; ++l) {
        for (int x=0; x<xSize; ++x) {
            for (int y=0; y<ySize; ++y) {
                std::vector<int> point = {l, x, y};
                double demand = double(viaMap[l][x][y]);
                commitVia(point, demand, Demand, layerDirections, hEdgeLengths, vEdgeLengths, layerMinLengths);
            }
        }
    }

    // overflow
    std::vector<std::vector<std::vector<double>>> Overflow(nLayers, std::vector<std::vector<double>>(xSize, std::vector<double>(ySize)));
    for (int l = 0; l < nLayers; ++l) {
        double min = 100;
        int direction = layerDirections[l];
        if (direction == 0) { // horizontal
            for (int x=0; x<xSize-1; ++x) {
                for (int y=0; y<ySize; ++y) {
                    Overflow[l][x][y] = Capacity[l][x][y] - Demand[l][x][y];
                    if (Overflow[l][x][y] < min) {
                        min = Overflow[l][x][y];
                    }
                }
            }
        } else {
            for (int x=0; x<xSize; ++x) {
                for (int y=0; y<ySize-1; ++y) {
                    Overflow[l][x][y] = Capacity[l][x][y] - Demand[l][x][y];
                    if (Overflow[l][x][y] < min) {
                        min = Overflow[l][x][y];
                    }
                }
            }
        }
        std::cout << "Layer " << l << " worst oveflow " << min << std::endl;
    }


    double overflowCost = 0.0;
    double s = 0.5;
    for (int l = 0; l < nLayers; ++l) {
        int direction = layerDirections[l];
        if (direction == 0) { // horizontal
            for (int x=0; x<xSize-1; ++x) {
                for (int y=0; y<ySize; ++y) {
                    overflowCost = overflowCost + unit_length_short_costs[l] * (overflowLossFunc(Overflow[l][x][y], s) - overflowLossFunc(Capacity[l][x][y], s)) * hEdgeLengths[x];
                }
            }
        } else { // vertical
            for (int x=0; x<xSize; ++x) {
                for (int y=0; y<ySize-1; ++y) {
                    overflowCost = overflowCost + unit_length_short_costs[l] * (overflowLossFunc(Overflow[l][x][y], s) - overflowLossFunc(Capacity[l][x][y], s)) * vEdgeLengths[y];
                }
            }
        }
    }

    double via_cost_scale = 1.0;
    double overflow_cost_scale = 1.0;
    double wireCost = totalWL * unit_length_wire_cost;
    double viaCost = numVia * unit_via_cost * via_cost_scale;
    overflowCost = overflowCost * overflow_cost_scale;
    double totalCost = wireCost + viaCost + overflowCost;

    std::cout << wireCost << " " << viaCost << " " << overflowCost << std::endl;
    std::cout << wireCost / totalCost << " " << viaCost / totalCost << " " << overflowCost / totalCost << std::endl;
    return 0;
}
