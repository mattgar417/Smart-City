#include "QLearner.h"

QLearner::QLearner() {}

QLearner::QLearner(float epsilon, float alpha, 
                   float gamma, float tsprate): epsilon(epsilon), alpha(alpha),
               	   gamma(gamma), currentQ(NULL)
{
   // hit = false;  /* assume that robot is not hit just at the start TODO: make this assumption dynamic + realistic */
   // down = false; /* assume that robot is not down just at the start TODO: make this assumption dynamic + realistic */
}

/*
 * This method is needed as object declaration & initialization cannot be done seperately (C++), so the 3 params constructor
 * is of no use if QLearner has to be used a member of a custom class.
*/

void QLearner::init(float epsilon, float alpha, float gamma, float tsprate, unsigned int fallthreshold, double myTime)
{
   this->epsilon       = epsilon;
   this->alpha         = alpha;
   this->gamma         = gamma;
   this->fallthreshold = fallthreshold;
   this->myTime        = myTime;
}

/*
* Returns Q(state,action)   
*/
double QLearner::getQValue(const ResourceState& state, const ResourceAction& action)
{
   // search '<State, Action> Q' and return the 'Q' value for that state, if not found return 0

   // Searching for 'Q' value of an action 'a' in state 's'
   std::vector<QTable>::iterator iter;
   for(iter = Q.begin(); iter != Q.end(); ++iter)
   {
      if(isStateSeen(iter->state_action_pair, state, action)) {
         return iter->qvalue;
      }
   }
   // State-Action does not exist in the Q-Table, so add it
   insertStateActionPair(state, action);
   return 0;
}

/*
 * Called after update(..) to update the new calculated q-value 
*/
bool QLearner::updateQValue(const ResourceState& state, const ResourceAction& action, double qvalue)
{
   std::vector<QTable>::iterator iter;
   for(iter = Q.begin(); iter != Q.end(); ++iter)
   {
      if(isStateSeen(iter->state_action_pair, state, action)) {
         iter->qvalue = qvalue;
         return true;
      }
   }
   return false;
}

/*
 * Returns max_action Q(state,action)        
 * where max is over legal actions
 *
 * Max 'Q' value for an action in a state.
*/
double QLearner::getValue(const ResourceState& state)
{
   std::vector<ResourceAction> actionlist = getTriedActions(state);
   std::vector<ResourceAction>::iterator iter;
   std::vector<double> qvalues;
   for(iter = actionlist.begin() ; iter < actionlist.end() ; ++iter)
   {
      qvalues.push_back(getQValue(state, *iter));
   }

   // Got the 'q-values' for all the actions, now find the max q-val
   std::vector<double>::iterator iterd; //iterator double :D
   double max = -DBL_MAX;
   for(iterd = qvalues.begin() ; iterd < qvalues.end() ; ++iterd)
   {
      if(*iterd > max)
      {
         max = *iterd;
      }
   }
   
   return max;
}

/*
 * What is the best action to take in a state.
*/
ResourceAction QLearner::getPolicy(const ResourceState& state)
{
   std::vector<ResourceAction> actionlist = getTriedActions(state);
   std::vector<ResourceAction>::iterator iter;
   std::vector<double> qvalues;

   for(iter = actionlist.begin() ; iter < actionlist.end() ; ++iter)
   {
      qvalues.push_back(getQValue(state, *iter));
   }

   /*
    * Got the 'q-values' for all the actions, now find the max q-val and then return action for that q-val.
   */

   std::vector<double>::iterator iterd;
   double max = -DBL_MAX;
   int idx = 0;
   int maxidx = 0;
   for(iterd = qvalues.begin() ; iterd < qvalues.end() ; ++iterd)
   {
      if(*iterd > max)
      {
         max = *iterd;
         maxidx = idx;
      }
      idx++;
   }
   
   return actionlist[maxidx];
}

/*
 * Once the agent has learned all the 'good actions' for all 'possible states' then no need to go through, 
 * just use the 'policy' which it had already learned :) 
 * call ResourceAction::isValid() to make sure that you get the correct action;
*/
ResourceAction QLearner::justPolicy(ResourceState& state) {
   std::vector<QTable>::iterator iter;
   for(iter = Policy.begin(); iter != Policy.end(); ++iter)
   {
      if(iter->state_action_pair.state.compareResourceState(state))
         return iter->state_action_pair.action;
   }
   /* If policy for unseen state is requested then just send the random action, the caller should have known not to use 
    * QLearner::justPolicy(..) when correct policy is not discovered for many possible states.
   */
   ResourceAction action = getAction(state);
   LOG("No policy found for state: %s, generating random action: %s\n", 
           state.getName().c_str(), action.getName().c_str());
   return action;
}

/*
 * What action to take in the current state. With
 * probability self.epsilon, we should take a random
 * action and take the best policy action otherwise.
    
 * After you choose an action make sure to
 * inform your parent self.doAction(state,action) 
 * This is done for you, just don't clobber it
       
 * HINT: you might want to use util.flipCoin
 * Where..... (see util.py)
 
 * Greater the value of 'epsilon', greater will be the chances to select a random action.

*/
ResourceAction QLearner::getAction(ResourceState& state)
{
   /*
    * If state is not 'seen' then do a random action with probability '1'.
   */
   float epsilon = this->epsilon;
   if(!isStatePresent(Q, state))
      epsilon = 1.0f;
   ResourceAction action;
   action = getPolicy(state);

   return action;
}

/*
 * The parent class calls this to observe a 
 * state = action => nextState and reward transition.
 * You should do your Q-Value update here
      
 * NOTE: You should never call this function,
 * it will be called on your behalf
*/
void QLearner::update(ResourceState& state, ResourceAction& action, ResourceState& nextstate, int reward)
{
   std::vector<ResourceAction> actionlist = getTriedActions(nextstate);
   std::vector<ResourceAction>::iterator iter;
   std::vector<double> qvalues;
   double sample;
   for(iter = actionlist.begin() ; iter < actionlist.end() ; ++iter)
   {
      qvalues.push_back(getQValue(nextstate, *iter));
   }
   if(qvalues.size() == 0)
      sample = reward;
   else
   {
      // For speed avoiding call to getValue() as half of compuatation is already performed in the above loop...
      double max = -DBL_MAX;
      std::vector<double>::iterator iterd;
      for(iterd = qvalues.begin() ; iterd < qvalues.end() ; ++iterd)
      {
         if(*iterd > max)
         {
            max = *iterd;
         }
      }
      sample = reward + (gamma * max);
   }
   
   double *ptr = NULL;
   double valueupdate = ( (1.0 - alpha) * getQValue(state, action) ) + (alpha * sample);
   // update the 'q-value'
   if(updateQValue(state, action, valueupdate))
      LOG("Updated qvalue for state: %s , action: %s with qvalue: %f.\n", 
          state.getName().c_str(), action.getName().c_str(), valueupdate);
   else
      LOG("'QLearner::update()': Failed to Update qvalue for state: %s , action: %s with qvalue: %f. State-Action pair not found in 'Q-Table'.\n", state.getName().c_str(), action.getName().c_str(), valueupdate);
}

int QLearner::getReward(double allocated, double demand, double cost){
   double unmet = max(0.0, demand - allocated);
   double waste = max(0.0, allocated - demand);

   return -(unmet * 3.0)   // heavy penalty: unmet demand hurts people
         -(waste * 0.5)   // light penalty: waste costs money  
         -(cost  * 0.1);  // small penalty: prefer cheaper allocations
}

bool QLearner::loadPolicy(const std::string filename)
{
   LOG("QLearner::loadPolicy()\n");
   std::ifstream file(filename.c_str());
   if(!file)
      return false;
   std::string line;
   /*
    * ResoirceState Q-value action1,action2... \n
    */
   while(std::getline(file, line))
   {
      std::vector<double> tvalues;
      std::stringstream linestream(line);
      double j;
      while(linestream >> j)
         tvalues.push_back(j);
      QTable qtab;
      int state;
      state = tvalues[0];
      qtab.qvalue = tvalues[1];
      qtab.state_action_pair.state = getResourceState(state);
      Policy.push_back(qtab);
   }
      LOG("Size Policy: %zu\n", Policy.size());
      file.close();
      return true;
}

bool QLearner::savePolicy(const std::string filename)
{
   std::vector<QTable> policyQ = getCurrentPolicy();
   std::vector<QTable>::iterator iter;
   FILE* file= NULL;
   file = fopen(filename.c_str(),"w");
   if(!file)
      return false;
   for(iter = policyQ.begin(); iter != policyQ.end(); ++iter)
   {
      /*
       * ResourceState Q-value action1,action2... \n
       */
      fprintf(file,"%i %f ",iter->state_action_pair.state, iter->qvalue);
      fprintf(file,"\n");
   }
   fclose(file);
   return true;
}

/*
 * Print 'current policy' from the 'Q' table.
*/

void QLearner::printCurrentPolicy()
{
   LOG("QLearner::printCurrentPolicy()\n");
   std::vector<QTable> policyQ = getCurrentPolicy();
   std::vector<QTable>::iterator iter;
   LOG("# of elements in policy : %zu\n", policyQ.size());
   LOG("\nState\n  * Action\n    -> Q-value\n\n");
   int count = 1;
   for(iter = policyQ.begin(); iter != policyQ.end(); ++iter)
   {
      LOG("%i.\n%s\n *  %s\n  ->  %lf\n\n", count, iter->state_action_pair.state.getName().c_str(), iter->state_action_pair.action.getName().c_str(), iter->qvalue);
      count++;
   }
}

/*
 * Print the 'stored' policy.
*/

void QLearner::printPersistentPolicy()
{
   LOG("QLearner::printPersistentPolicy()\n");
   std::vector<QTable>::iterator iter;
   LOG("# of elements in policy : %zu\n", Policy.size());
   LOG("\nState\n  * Action\n    -> Q-value\n\n");
   int count = 1;
   for(iter = Policy.begin(); iter != Policy.end(); ++iter)
   {
      LOG("%i.\n%s\n *  %s\n  ->  %lf\n\n", count, iter->state_action_pair.state.getName().c_str(), iter->state_action_pair.action.getName().c_str(), iter->qvalue);
      count++;
   }
}

bool QLearner::loadQTable(const std::string filename)
{
   LOG("QLearner::loadQTable()\n");
   std::ifstream file(filename.c_str());
   if(!file)
      return false;
   std::string line;
   /*
    * FeetState Q-value action1,action2... \n
    */
   while(std::getline(file, line))
   {
      std::vector<double> tvalues;
      std::stringstream linestream(line);
      double j;
      while(linestream >> j)
         tvalues.push_back(j);
      QTable qtab;
      int state;
      state = tvalues[0];
      qtab.qvalue = tvalues[1];
      qtab.state_action_pair.state = getResourceState(state);
      Q.push_back(qtab);
   }
      LOG("Size QTable: %zu\n", Q.size());
      file.close();
      return true;
}

bool QLearner::saveQTable(const std::string filename)
{
   std::vector<QTable>::iterator iter;
   FILE* file= NULL;
   file = fopen(filename.c_str(),"w");
   if(!file)
      return false;
   for(iter = Q.begin(); iter != Q.end(); ++iter)
   {
      /*
       * FeetState Q-value action1,action2... \n
       */
      fprintf(file,"%i %f ",iter->state_action_pair.state, iter->qvalue);
      fprintf(file,"\n");
   }
   fclose(file);
   return true;
}

/*
 * Print the whole 'Q' table.
*/

void QLearner::printQTable()
{
   LOG("QLearner::printQTable()\n");
   std::vector<QTable>::iterator iter;
   LOG("# of elements in QTable : %zu\n", Q.size());
   LOG("\nState\n  * Action\n    -> Q-value\n\n");
   int count = 1;
   for(iter = Q.begin(); iter != Q.end(); ++iter)
   {
      LOG("%i.\n%s\n *  %s\n  ->  %lf\n\n", count, iter->state_action_pair.state.getName().c_str(), iter->state_action_pair.action.getName().c_str(), iter->qvalue);
      count++;
   }
}

bool QLearner::createPersistence(const std::string qtablePath, const std::string policyPath)
{
   FILE* file= NULL;
   file = fopen(qtablePath.c_str(),"w");
   if(!file)
   {
      ERROR("Error in creating file %s, please check that you have the required permissions...\n", qtablePath.c_str());
      return false;
   }
   fclose(file);
   file = fopen(policyPath.c_str(),"w");
   if(!file)
   {
      ERROR("Error in creating file %s, please check that you have the required permissions...\n", policyPath.c_str());
      return false;
   }
   fclose(file);
   return true;
}

/*
 * Returns true is state already seen by 'Q' datastructure :)
 */
bool QLearner::isStateSeen(const StateActionPair& s_a_pair, const ResourceState& state, const ResourceAction& action) const {
   return s_a_pair.state.compareResourceState(state) &&
         s_a_pair.action.compareActions(action);
}

/*
 * Get all the already 'tried' actions i.e the ones stored in Q-table.
*/
std::vector<ResourceAction> QLearner::getTriedActions(const ResourceState& state) const
{
   std::vector<ResourceAction> actionlist;
   std::vector<QTable>::const_iterator iter;
   for(iter = Q.begin(); iter != Q.end(); ++iter)
   {
      if(iter->state_action_pair.state.compareResourceState(state))
      {
         actionlist.push_back(iter->state_action_pair.action);
      }
   }
   //TODO: @warn: remove this code
   ResourceState rstate = state;
   // LOG("Size [TriedActions]: %zu for State: %s\n", actionlist.size(), State::getName(fstate).c_str());
   return actionlist;
}

/*
 * Get all the legal actions for a state [in our case, all states have same legal actions], first return the actions which were already
 * tried before and for whom we already have a Q-value.
*/
std::vector<ResourceAction> QLearner::getLegalActions(const ResourceState& state) const {
   vector<ResourceAction> actions;

   // Always offer all 4 allocation levels as legal choices
   actions.push_back({ALLOCATE_NONE});
   actions.push_back({ALLOCATE_LOW});
   actions.push_back({ALLOCATE_MEDIUM});
   actions.push_back({ALLOCATE_FULL});

   // Prune impossible actions: can't allocate more than available
   if (state.availLevel == LOW) {
      actions.erase(remove_if(actions.begin(), actions.end(),
         [](const ResourceAction& a){ return a.level == ALLOCATE_FULL; }),
         actions.end());
   }
   return actions;        
}

/*
 * Use for selecting an action with probability 'p'
 * TODO: Use better version of random generation, sth like 'std::uniform_real_distribution'
 * @imp: Candidate for improvement.
*/

bool QLearner::flipCoin(double p)
{
   double r = ((double) rand() / (RAND_MAX));
   
   return r < p;
}

/*
 * Generate random number in between the limit (boundry values included).
 * TODO: Use better version of random generation, sth like 'std::uniform_real_distribution'
 * @imp: Candidate for improvement.
*/

int QLearner::randomLimit(unsigned int min, unsigned int max) const
{
   return (min + (rand() % (int)(max - min + 1)));
}

bool QLearner::insertStateActionPair(const ResourceState& state, const ResourceAction& action)
{
   QTable q;
   q.state_action_pair.state = state;
   q.state_action_pair.action = action;
   q.qvalue = 0.0; // for the new experienced state, 'q-value' is 0
   Q.push_back(q);
   return false;
}

/*
 * This method takes the 8 FSRS of both feet to determine the state of the robot, which would be one state out of possible 
 * 'FeetState', see core.hpp for more details.
*/

ResourceState QLearner::determineState(const Resource* resource) {
    return ResourceState::fromResource(resource);
}

/*
 * Used by print/load/save policy to the identify the unique states.
*/

bool QLearner::isStatePresent(std::vector<QTable>& q, const ResourceState& state) const
{
   std::vector<QTable>::iterator iter;
   for(iter = q.begin(); iter != q.end(); ++iter)
   {
      if(iter->state_action_pair.state.compareResourceState(state))
         return true;
   }
   return false;
}

/*
 * Used by print/load/save policy to the get the index of unique states.
*/

int QLearner::getStateIndex(std::vector<QTable>& q, const ResourceState& state) const
{
   std::vector<QTable>::iterator iter;
   int idx = 0;
   for(iter = q.begin(); iter != q.end(); ++iter)
   {
      if(iter->state_action_pair.state.compareResourceState(state))
         return idx;
      idx++;
   }

   return -1;

}

/*
 * This method is needed by printPolicy() & savePolicy() to get the unique 'policy' for each state.
*/

std::vector<QTable> QLearner::getCurrentPolicy() const
{
   // Get max action for all the states.
   std::vector<QTable>::const_iterator iter;
   std::vector<QTable> policyQ;
   /*
    * Get unique states.
   */
   for(iter = Q.begin(); iter != Q.end(); ++iter)
   {
      if(!(isStatePresent(policyQ, iter->state_action_pair.state)))
         policyQ.push_back(*iter);
      else
      {
         /*
          * If the 'q-value' of that state for another action is '>' than our q-value, then erase the previous action and add this one.
         */
         int idx = getStateIndex(policyQ, iter->state_action_pair.state);
         if(policyQ.at(idx).qvalue < iter->qvalue)
         {
            policyQ.erase(policyQ.begin() + idx);
            policyQ.push_back(*iter);
         }
            
      } 
   }
   return policyQ;
}

ResourceState QLearner::getResourceState(const int idx) const
{
   ResourceState rstate;
   return rstate;
}

QLearner::~QLearner() {}