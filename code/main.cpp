#include <iostream>
#include "ResourceAllocator.h"

using namespace std;

// Function to display the menu
void displayMenu() {
    cout << "\n--- Resource Management System Menu ---\n";
    cout << "1. Add a New Resource\n"; // Option to add a new resource
    cout << "2. Display Resources by Type\n"; // Option to display resources grouped by type
    cout << "3. Allocate Resources\n"; // Option to allocate resources to districts
    cout << "4. Exit\n"; // Option to exit the program
    cout << "Enter your choice: ";
}

int main() {
    try {
        // Initialize the ResourceAllocator
        cout << "Initializing Resource Allocator...\n";
        ResourceAllocator allocator;

        // Load resources from a CSV file
        allocator.loadResources("C:/Users/gmatt/CLionProjects/AlgosProject/resources_data.csv");

        int choice;
        do {
            // Display the menu and capture the user's choice
            displayMenu();
            cin >> choice;

            switch (choice) {
                case 1: { 
                    // Case to add a new resource
                    string type, district;
                    double cost, availability, additional1 = 0, additional2 = 0;
                    int importance;

                    cout << "Enter resource type (Electricity, Water, Gas, Waste): ";
                    cin >> type; // Capture resource type
                    cout << "Enter district name: ";
                    cin >> district; // Capture district name
                    cout << "Enter cost: ";
                    cin >> cost; // Capture cost
                    cout << "Enter importance: ";
                    cin >> importance; // Capture importance (priority)
                    cout << "Enter availability: ";
                    cin >> availability; // Capture availability

                    // Capture additional attributes based on resource type
                    if (type == "Electricity") {
                        cout << "Enter peak demand: ";
                        cin >> additional1;
                        cout << "Enter average demand: ";
                        cin >> additional2;
                    } else if (type == "Water") {
                        cout << "Enter pH level: ";
                        cin >> additional1;
                    } else if (type == "Gas") {
                        cout << "Enter pressure: ";
                        cin >> additional1;
                        cout << "Enter usage: ";
                        cin >> additional2;
                    }

                    Resource* resource = nullptr;

                    // Create specific resource objects based on the type
                    if (type == "Electricity") {
                        resource = new Electricity(type, district, cost, importance, availability, additional1, additional2);
                    } else if (type == "Water") {
                        resource = new Water(type, district, cost, importance, availability, additional1);
                    } else if (type == "Gas") {
                        resource = new Gas(type, district, cost, importance, availability, additional1, additional2);
                    } else if (type == "Waste") {
                        resource = new Waste(type, district, cost, importance, availability);
                    }

                    // Add resource to the allocator if valid
                    if (resource) {
                        allocator.addNewResourceType(resource);
                        cout << "Resource added successfully.\n";
                    } else {
                        cout << "Invalid resource type.\n";
                    }
                    break;
                }
                case 2:
                    // Displays resources by district
                    cout << "\nResource Allocations by District:\n";
                    allocator.displayResourcesByDistrict();
                    break;
                case 3:
                    // Allocates resources and displays them
                    cout << "\nAllocating Resources\n";
                    allocator.allocateResources();
                    break;
                case 4:
                    // Exit case
                    cout << "Exiting the system. Goodbye!\n";
                    break;
                default:
                    // Handle invalid menu options
                    cout << "Invalid choice. Please select a valid option.\n";
            }
        } while (choice != 4); // Loop until the user chooses to exit
    } catch (const exception& e) {
        // Handle runtime errors gracefully
        cerr << "Error: " << e.what() << "\n";
    }

    return 0;
}
