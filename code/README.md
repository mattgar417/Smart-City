# Smart City Resource Management System

## Description
This program manages resources (Electricity, Water, Gas, and Waste) for different districts. It allows users to:
- Add new resources.
- Display resources grouped by district.
- Allocate resources based on predefined conditions such as importance, availability, and cost.

## Features
1. Add a new resource with details such as type, district, cost, importance, and availability.
2. Display resources grouped by their district.
3. Allocate resources automatically based on importance and availability.

## How to Run
1. Compile the program using any C++ compiler that supports C++11 or later.
2. Ensure the `resources_data.csv` file is correctly formatted and placed in the specified directory.
3. Run the executable, and follow the menu prompts to interact with the system.

## Classes
### `Resource`
Abstract base class for all resource types.

### Derived Classes
- `Electricity`: Represents electricity resources.
- `Water`: Represents water resources.
- `Gas`: Represents gas resources.
- `Waste`: Represents waste management resources.

### `ResourceAllocator`
Manages resource loading, addition, allocation, and display operations.

## Files
- **`main.cpp`**: Contains the menu-driven interface.
- **`ResourceAllocator.h`**: Header file for the `ResourceAllocator` and `Resource` classes.
- **`ResourceAllocator.c`**: Source file for the `ResourceAllocator` and `Resource` classes.
- **`ResourceAllocator.h`**: Header file for the `addEdge` and `displayGraph` functions.
- **`ResourceAllocator.c`**: Source file for the `addEdge` and `displayGraph` functions.

## Authors
- **Matthew Leonardo Garcia and Rostyslav Rozhok**
