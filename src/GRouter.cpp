#include "GRouter.hpp"

/*
std ::vector<int> RRRorder(int NetCount)
{

}
*/

std ::map<int, Net> parseNet(std ::string netFilename)
{   
    // Below is a temporary variable to store the lines read
    std ::string line;
    // Create the output map
    std::map<int, Net> nets;
    // Parse net file
    // Get net info
    std::string name;
    std ::fstream netFile(netFilename);
    int numPins = 0;
    std::vector<std::vector<std::vector<int>>> accessPoints;
    // iterator counts how many nets have been read
    int i=0;
    while (std::getline(netFile, line)) {
        // If the line is just the name of a new net
        if (line.find("(") == std::string::npos && line.find(")") == std::string::npos && line.length()>1) {
            // name = line.substr(0, line.size() - 1);
            name = line;
            size_t found = name.find('\n');
            if (found != std::string::npos) {
                name.erase(found, 1);
            }
            numPins = 0;
        // ie. if the line contains a new pin
        } else if (line.find('[') != std::string::npos) {
            std::vector<std::vector<int>> access;
            std::string text = line.substr(1, line.size() - 2); // Remove brackets and trailing comma
            std::string charsToRemove = "(),";
            text.erase(std::remove_if(text.begin(), text.end(), [&charsToRemove](char c) {
            return charsToRemove.find(c) != std::string::npos;
            }), text.end());
            std::istringstream ss(text);
            int x, y, z;
            int cellCount=0;
            while (ss >> x >> y >> z) {
                std::vector<int> point;
                point.push_back(x);
                point.push_back(y);
                point.push_back(z);
                access.push_back(point);
                std::cout << access[cellCount][0] << " " << access[cellCount][1] << " " << access[cellCount][2] << std::endl;
                ++cellCount;
            }
            accessPoints.push_back(access);
            numPins++;
        } else if (line.find(')') != std::string::npos) {
            // Member of Net class is initialized
            std::cout << name << std::endl;

            Net newNet(numPins, accessPoints, name);
            nets.insert({i,newNet});
            i++;
            accessPoints.clear();
        }
    }
    netFile.close();
    return nets;
}

std ::map<int, Route> initial_route(std ::map<int, Net> nets, chip chip)
{
    // Create the output map
    std ::map<int, Route> routes;
    int s=0;
    int size=nets.size();
    for (int i=0; i<size; i++) {
        Route newRoute(nets[i], chip);
        routes.insert({i, newRoute});
        s+=routes[i].get_lineCount();
    }
    std::cout << "Initial route Size is: " << s << "Lines" << std::endl;
    return routes;
}

void grFileWrite(std ::map<int, Route>& routes, std ::map<int, Net>& nets, std ::string grFilename)
{
    std ::vector<int> routeLine;
    std::ofstream GRfile(grFilename, std::ios::out);
    int size=routes.size();
    int i;
    int j;
    int k;
    int routeCount;
    for (i=0; i<size; ++i) {
        GRfile << nets[i].Name << std::endl;
        GRfile << '(' << std::endl;
        routeCount = routes[i].get_lineCount();
        // Now the routing data is added
        for (j=0; j<routeCount; ++j) {
            routeLine = routes[i].getline(j);
            for (k=0; k<4; ++k) {
                GRfile << routeLine[k] << ' ';
            }
            GRfile << "metal" << routeLine[4]+1 << std::endl;
        }
        GRfile << ')' << std::endl;
    }
    GRfile.close();
}


int main(int argc, char** argv)
{
    // ensuring the correct number of arguments are passed, as a resource file, a net file, and a GR file
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " resource_file net_file GR_file" << std::endl;
        return 1;
    }

    std::ifstream resourceFile(argv[1]);
    if (!resourceFile) {
        std::cerr << "Failed to open resource file." << std::endl;
        return 1;
    }
    resourceFile.close();

    std::ifstream netFile(argv[2]);
    std ::string netFilename(argv[2]);
    if (!netFile) {
        std::cerr << "Failed to open net file." << std::endl;
        return 1;
    }
    netFile.close();

    std ::string grFilename(argv[3]);

    // First the chip is initialized from the resource file
    chip chip(argv[1]);

    // Parse net list file, get nets and routes, and do pattern routing
    std::map<int, Net> nets = parseNet(netFilename);
    std::cout << "got net info" << std::endl;
    std ::map<int, Route> routes = initial_route(nets, chip);
    std::cout << "Did initial routing" << std::endl;

    // Insert code for RRR

    // store the routing solution into .PR_output file
    grFileWrite(routes, nets, grFilename);
    return 0;
}

//run Chess.cap Chess.net Chess.PR_output
