#ifndef _RLCOREH_
#define _RLCOREH_

#include <vector>
#include <string>
#include <math.h>
#include <algorithm>
#include "ResourceAllocator.h"

enum AvailabilityLevel { LOW = 0, MEDIUM = 1, HIGH = 2 };

/*
 * Interface for a state.
*/
class ResourceState
{
   
   public:
   std::string districtName;
   std::string resourceType;
   AvailabilityLevel availLevel;
   int importanceBin;
   
   // TODO: Point of impact
   // TODO: injected_current ...

   /*
    * A Simple method to comapare resource states.
   */
   bool compareResourceState(const ResourceState& other) const {
      return districtName  == other.districtName  &&
         resourceType  == other.resourceType  &&
         availLevel    == other.availLevel    &&
         importanceBin == other.importanceBin;
   }

   std::string getName() const {
      return districtName + "_" + resourceType + 
         "_avail" + to_string(availLevel) + 
         "_imp"   + to_string(importanceBin);
   }

   // Build state from a live Resource object
   static ResourceState fromResource(const Resource* res) {
      ResourceState s;
      s.districtName  = res->district;
      s.resourceType  = res->type;
      s.importanceBin = min(res->importance / 3, 2); // bucket into 0,1,2

      if      (res->availability < 0.33) s.availLevel = LOW;
      else if (res->availability < 0.66) s.availLevel = MEDIUM;
      else                               s.availLevel = HIGH;
      return s;
   }
};

enum AllocationLevel { ALLOCATE_NONE = 0, ALLOCATE_LOW = 1, ALLOCATE_MEDIUM = 2, ALLOCATE_FULL = 3 };

/*
 * Interface for an Action.
*/
class ResourceAction
{
   public:
   AllocationLevel level;

   // Maps allocation level → actual allocation units
   double toUnits() const {
      switch(level) {
         case ALLOCATE_NONE:   return 0.0;
         case ALLOCATE_LOW:    return 0.25;
         case ALLOCATE_MEDIUM: return 0.5;
         case ALLOCATE_FULL:   return 1.0;
      }
      return 0.0;
    }

   /*
    * A simple method to compare Actions i.e rs_neuron values //TODO: Also incorporate pfneuron only if applicable.
   */
   bool compareActions(const ResourceAction& other) const {
      return level == other.level;
   }

   string getName() const {
      return "AllocLevel_" + to_string(level);
   }
};

class StateActionPair
{
public:
   ResourceState state;
   ResourceAction action;
   StateActionPair() {}
   StateActionPair(ResourceState state, ResourceAction) {this->state = state; this->action = action; } 
   StateActionPair(ResourceState& state, ResourceAction& action) {this->state = state, this->action = action; }
};

class QTable
{
public:
   StateActionPair state_action_pair;
   double qvalue;
};

// Interface for an agent.
class Agent
{
public:

   virtual ResourceAction getPolicy(const ResourceState& state) = 0;

   virtual bool savePolicy(const std::string filename) {return true;};

   virtual ~Agent() {};

};

#endif
