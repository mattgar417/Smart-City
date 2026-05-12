#include "ResourceAllocator.h"
#include "QLearner.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

map<string, double> districtDemand;
QLearner qlearner;

// Constructor for the ResourceAllocator class
ResourceAllocator::ResourceAllocator() {
    // Initializes an empty resources map
}

// Destructor for the ResourceAllocator class
ResourceAllocator::~ResourceAllocator() {
    // Clean up dynamically allocated resources
    for (auto& [type, resourceList] : resources) {
        for (auto* res : resourceList) {
            delete res; // Delete each resource object
        }
    }
}

// Add a new resource type to the map
void ResourceAllocator::addNewResourceType(Resource* resource) {
    resources[resource->type].push_back(resource); // Add the resource to the list for its type
}

// Load resources from a CSV file
void ResourceAllocator::loadResources(const string& filePath) {
    ifstream file(filePath); // Open the CSV file
    if (!file.is_open()) {
        // Throw an exception if the file cannot be opened
        throw runtime_error("Could not open file: " + filePath);
    }

    string line;
    getline(file, line);  // Skip the header line in the CSV file

    // Process each line in the file
    while (getline(file, line)) {
        istringstream ss(line);
        string type, district;
        double cost, availability, additional1 = 0, additional2 = 0;
        int importance;

        // Parse the fields from the line
        getline(ss, type, ','); // Read resource type
        getline(ss, district, ','); // Read district name
        ss >> cost; // Read cost
        ss.ignore(1, ','); // Skip comma
        ss >> importance; // Read importance
        ss.ignore(1, ','); // Skip comma
        ss >> availability; // Read availability
        ss.ignore(1, ','); // Skip comma
        ss >> additional1; // Read additional attribute 1
        ss.ignore(1, ','); // Skip comma
        ss >> additional2; // Read additional attribute 2

        Resource* resource = nullptr;

        // Create a specific resource object based on the type
        if (type == "Electricity") {
            resource = new Electricity(type, district, cost, importance, availability, additional1, additional2);
        } else if (type == "Water") {
            resource = new Water(type, district, cost, importance, availability, additional1);
        } else if (type == "Gas") {
            resource = new Gas(type, district, cost, importance, availability, additional1, additional2);
        } else if (type == "Waste") {
            resource = new Waste(type, district, cost, importance, availability);
        }

        // Add the resource to the map if it was created
        if (resource) {
            addNewResourceType(resource);
        }
    }

    file.close(); // Close the file after processing
}

// Allocate resources to districts based on conditions
void ResourceAllocator::allocateResources() {
    map<string, map<string, double>> districtAllocations; // Map to store allocation results

    for (auto& [type, resourceList] : resources) {

        // Phase 1: Score with linear regression (fast, learned offline)
        computeMLScores(learnedWeights);

        // Phase 2: Sort by ML score as a starting heuristic
        sort(resourceList.begin(), resourceList.end(),
            [](const Resource* a, const Resource* b) {
                return a->mlPriorityScore > b->mlPriorityScore;
            });

        for (auto* resource : resourceList) {
            // Build current state
            ResourceState currentState = qlearner.determineState(resource);

            // Q-learner picks how much to allocate (explore vs exploit)
            ResourceAction action = qlearner.getAction(currentState);
            double allocated = min(action.toUnits(), resource->availability);

            // Apply allocation
            resource->availability -= allocated;
            districtAllocations[resource->district][type] += allocated;

            // Compute reward and update Q-table
            double demand = districtDemand[resource->district]; 
            double reward = qlearner.getReward(allocated, demand, resource->cost);

            ResourceState nextState = qlearner.determineState(resource); // post-allocation
            qlearner.update(currentState, action, nextState, reward);
        }
    }

    // Persist what the agent learned this cycle
    qlearner.saveQTable("qtable.dat");

    // Display allocation results
    if (districtAllocations.empty()) {
        cout << "No resources allocated. The districtAllocations map is empty.\n";
    } else {
        cout << "Allocations in districtAllocations:\n";
        for (const auto& [district, types] : districtAllocations) {
            for (const auto& [type, amount] : types) {
                cout << "District: " << district << ", Resource: " << type << ", Amount: " << amount << "\n";
            }
        }
    }
}

// Display all resources grouped by district
void ResourceAllocator::displayResourcesByDistrict() {
    map<string, vector<Resource*>> resourcesByDistrict; // Map to store resources grouped by district

    // Group resources by district
    for (const auto& [type, resourceList] : resources) {
        for (const auto* resource : resourceList) {
            resourcesByDistrict[resource->district].push_back(const_cast<Resource*>(resource));
        }
    }

    // Display resources for each district
    for (const auto& [district, resourceList] : resourcesByDistrict) {
        cout << "\nDistrict: " << district << "\n";
        for (const auto* resource : resourceList) {
            resource->display(); // Display resource details
        }
    }
}

void ResourceAllocator::loadWeights(const string& filePath) {
    ifstream file(filePath);
    if (!file.is_open()) {
        // Fallback: sensible hand-tuned defaults
        learnedWeights = {1.5,  // importance coefficient
                          1.0,  // availability coefficient
                          0.8,  // 1/cost coefficient
                          1.2}; // utilization rate coefficient
        return;
    }
    double w;
    while (file >> w) learnedWeights.push_back(w);
    file.close();
}

void ResourceAllocator::computeMLScores(const vector<double>& weights) {
    // weights[0..3] = learned coefficients for [importance, availability, 1/cost, utilization_rate]
    for (auto& [type, resourceList] : resources) {
        for (auto* res : resourceList) {
            res->mlPriorityScore =
                weights[0] * res->importance +
                weights[1] * res->availability +
                weights[2] * (1.0 / (res->cost + 1e-9));
                // + weights[3] * res->getUtilizationRate();
        }
    }
}
