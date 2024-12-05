#include "ResourceAllocator.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

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

    // Sort and allocate resources for each type
    for (auto& [type, resourceList] : resources) {
        // Sort the resources based on importance, availability, and cost
        for (size_t i = 0; i < resourceList.size(); ++i) {
            for (size_t j = 0; j < resourceList.size() - i - 1; ++j) {
                bool swapCondition =
                        resourceList[j]->importance < resourceList[j + 1]->importance ||
                        (resourceList[j]->importance == resourceList[j + 1]->importance &&
                         resourceList[j]->availability < resourceList[j + 1]->availability) ||
                        (resourceList[j]->importance == resourceList[j + 1]->importance &&
                         resourceList[j]->availability == resourceList[j + 1]->availability &&
                         resourceList[j]->cost > resourceList[j + 1]->cost);

                if (swapCondition) {
                    swap(resourceList[j], resourceList[j + 1]); // Swap resources if conditions are met
                }
            }
        }

        // Allocate resources to districts
        for (auto* resource : resourceList) {
            string district = resource->district; // Get the district name
            double allocation = min(resource->availability, 1.0); // Allocate up to 1 unit
            resource->availability -= allocation; // Deduct allocated amount
            districtAllocations[district][type] += allocation; // Update allocation map
        }
    }

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
