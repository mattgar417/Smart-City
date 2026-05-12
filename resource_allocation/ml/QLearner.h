#ifndef _QLEARNER_
#define _QLEARNER_

#include "core.h"
#include "log.h"
#include <float.h>
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <sstream>

/*
 * Agent that uses Q-learning with ...
*/
class QLearner: public Agent
{
private:

   std::vector<QTable> Q;

   std::vector<QTable> Policy;
   
   float epsilon; /* exploration prob */
   float alpha; /* learning rate */
   float gamma; /* discount rate */

   double myTime; //TODO: temperary variable, time at which collision occurs
   int fallcount;
   unsigned int fallthreshold;

   double *currentQ; // need to update 'Q-values' :)

   bool isStateSeen(const StateActionPair& s_a_pair, const ResourceState& state, const ResourceAction& action) const;

   std::vector<ResourceAction> getTriedActions(const ResourceState& state) const;

   std::vector<ResourceAction> getLegalActions(const ResourceState& state) const ;

   bool flipCoin (double p);

   int randomLimit(unsigned int min, unsigned int max) const;

   bool insertStateActionPair(const ResourceState& state, const ResourceAction& action);

   bool isStatePresent(std::vector<QTable>& q, const ResourceState& state) const;

   int getStateIndex(std::vector<QTable>& q, const ResourceState& state) const;

   std::vector<QTable> getCurrentPolicy() const;

   ResourceAction getBaseActionTSP() const;

   ResourceAction getAllActionTSP() const;  

   ResourceState getResourceState(const int idx) const;

public:

   QLearner();

   QLearner(float epsilon, float alpha, float gamma, float tsprate);

   void init(float epsilon, float alpha, float gamma, float tsprate, unsigned int fallthreshold, double myTime);

   int getReward(double allocated, double demand, double cost);

   double getQValue(const ResourceState& state, const ResourceAction& action);

   bool updateQValue(const ResourceState& state, const ResourceAction& action, double qvalue);

   double getValue(const ResourceState& state);

   ResourceAction getAction(ResourceState& state);

   void doAction(ResourceAction& action);

   virtual ResourceAction getPolicy(const ResourceState& state);

   ResourceAction justPolicy(ResourceState& state);

   ResourceState determineState(const Resource* resource);

   void update(ResourceState& state, ResourceAction& action, ResourceState& nextstate, int reward);

   virtual ~QLearner();

   virtual bool savePolicy(const std::string filename);

   bool loadPolicy(const std::string filename);
   
   void printCurrentPolicy();

   void printPersistentPolicy();

   bool loadQTable(const std::string filename);

   bool saveQTable(const std::string filename);

   void printQTable();

   bool createPersistence(const std::string qtablePath, const std::string policyPath);

   bool detectPerturbation(double myTime);
 
   bool getHit();

   bool getFall();
   
};

#endif