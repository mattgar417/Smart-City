#ifndef RESOURCEALLOCATOR_H
#define RESOURCEALLOCATOR_H

#include <iostream>
#include <vector>
#include <map>
#include <string>

using namespace std;

class Resource {
public:
    string type;
    string district;
    double cost;
    int importance;
    double availability;

    Resource(string type, string district, double cost, int importance, double availability)
            : type(type), district(district), cost(cost), importance(importance), availability(availability) {}

    virtual void display() const = 0;
};

class Electricity : public Resource {
public:
    double peakDemand;
    double averageDemand;

    Electricity(string type, string district, double cost, int importance, double availability,
                double peakDemand, double averageDemand)
            : Resource(type, district, cost, importance, availability), peakDemand(peakDemand), averageDemand(averageDemand) {}

    void display() const override {
        cout << type << " - " << district << ": Cost: " << cost << ", Importance: " << importance
             << ", Availability: " << availability << ", Peak Demand: " << peakDemand
             << ", Average Demand: " << averageDemand << endl;
    }
};

class Water : public Resource {
public:
    double pHLevel;

    Water(string type, string district, double cost, int importance, double availability, double pHLevel)
            : Resource(type, district, cost, importance, availability), pHLevel(pHLevel) {}

    void display() const override {
        cout << type << " - " << district << ": Cost: " << cost << ", Importance: " << importance
             << ", Availability: " << availability << ", pH Level: " << pHLevel << endl;
    }
};

class Gas : public Resource {
public:
    double pressure;
    double usage;

    Gas(string type, string district, double cost, int importance, double availability, double pressure, double usage)
            : Resource(type, district, cost, importance, availability), pressure(pressure), usage(usage) {}

    void display() const override {
        cout << type << " - " << district << ": Cost: " << cost << ", Importance: " << importance
             << ", Availability: " << availability << ", Pressure: " << pressure << ", Usage: " << usage << endl;
    }
};

class Waste : public Resource {
public:
    Waste(string type, string district, double cost, int importance, double availability)
            : Resource(type, district, cost, importance, availability) {}

    void display() const override {
        cout << type << " - " << district << ": Cost: " << cost << ", Importance: " << importance
             << ", Availability: " << availability << endl;
    }
};

class ResourceAllocator {
private:
    map<string, vector<Resource*>> resources;

public:
    ResourceAllocator();
    ~ResourceAllocator();
    void addNewResourceType(Resource* resource);
    void loadResources(const string& filePath);
    void allocateResources();
    void displayAllocation();
    void displayResourcesByDistrict();
};

#endif

