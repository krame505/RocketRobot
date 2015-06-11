/**
 * \author Lucas Kramer
 * \file  Simulation.cpp
 * \brief Implementation for the main application class of the robot simulation
 *
 */

#include <unistd.h>
#include <stdlib.h> 
#include <signal.h>
#include <algorithm>
#include <vector>
#include <mutex>
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <iostream>
#include <climits>
#include <csignal>
#include <sys/stat.h>
#include <boost/interprocess/sync/named_upgradable_mutex.hpp>
using namespace std;
using namespace boost::interprocess;

#include "Robot.h"
#include "SimpleRobot.h"
#include "ComplexRobot.h"
#include "NeuralNetworkRobot.h"
#include "NeuralNetwork.h"
#include "Target.h"
#include "Obstacle.h"
#include "LightSource.h"
#include "Location.h"
#include "configuration.h"
#include "Environment.h"
#include "util.h"
#include "OptimizeSimulation.h"
using namespace util;

OptimizeSimulation *OptimizeSimulation::s_currentInstance;

OptimizeSimulation::OptimizeSimulation(int argc, char* argv[]) :
  lock(open_or_create, GET_STRING("NEURAL_NETWORK_LOCK_NAME").c_str()) {
  s_currentInstance = this;
  //sigemptyset(&sigint);
  //sigaddset(&sigint, SIGINT);
  signal(SIGINT, stopHandler);
  lock.lock_sharable();
  reload();
  lock.unlock_sharable();
  startedEmpty = false;
  if (pool.size() == 0) {
    startedEmpty = true;
    pool.push_back(new NeuralNetwork(GET_STRING("OPTIMAL_NEURAL_NETWORK_FILE")));
    poolPerformance.push_back(getPerformance(*pool.back()));
  }
}

OptimizeSimulation::~OptimizeSimulation() {
  for (PhysicalObject *o : *Environment::getEnv()) {
    delete o;
  }
  for (NeuralNetwork *net : pool) {
    delete net;
  }
}

void OptimizeSimulation::runMainLoop() {
  int lastUpdateTime = getFileTimestamp(GET_STRING("BEST_PERFORMANCE_FILE"));
  bool poolChanged = startedEmpty;

  while (!stopRequest) {
    int oldOptimalPerformance;
    if (poolPerformance.size() > 0)
      oldOptimalPerformance = poolPerformance.front();
    else
      oldOptimalPerformance = -1;

    if (((float)rand()) / ((float)RAND_MAX) < GET_FLOAT("COMBINE_FREQUENCY")) {
      int netId1 = rand() % (pool.size() > (unsigned)GET_INT("SUB_POOL_SIZE")?
                             GET_INT("SUB_POOL_SIZE") : pool.size());
      int netId2 = rand() % pool.size();
      if (GET_BOOL("OPTIMIZE_VERBOSE"))
        cout << "Combining network " << netId1 << " with " << netId2 << endl;
      NeuralNetwork *newNetwork =
        new NeuralNetwork(pool[netId1]->
                          combine(*pool[netId2]).
                          mutate(GET_INT("COMBINE_NUM_CONNECTIONS_MUTATED"),
                                 GET_FLOAT("COMBINE_MUTATION_AMOUNT")));
      newNetwork->write(GET_STRING("TEMP_NEURAL_NETWORK_FILE"));
      int newPerformance = getPerformance(*newNetwork);
    
      lock.lock_upgradable();
      int newUpdateTime = getFileTimestamp(GET_STRING("BEST_PERFORMANCE_FILE"));
      if (lastUpdateTime != newUpdateTime) {
        if (GET_BOOL("OPTIMIZE_VERBOSE"))
          cout << "Refreshing..." << endl;
        lastUpdateTime = newUpdateTime;
        reload();
      }

      if (find(poolPerformance.begin(), poolPerformance.end(), newPerformance) == poolPerformance.end()) {
        int oldOptimalPerformance;
        if (poolPerformance.size() > 0)
          oldOptimalPerformance = poolPerformance.front();
        else
          oldOptimalPerformance = -1;
        pool.push_back(newNetwork);
        poolPerformance.push_back(newPerformance);
        unsigned i;
        for (i = pool.size() - 1; i > 0; i--) {
          if (poolPerformance[i] < poolPerformance[i - 1]) {
            NeuralNetwork *temp = pool[i];
            pool[i] = pool[i - 1];
            pool[i - 1] = temp;
            int tempP = poolPerformance[i];
            poolPerformance[i] = poolPerformance[i - 1];
            poolPerformance[i - 1] = tempP;
            poolChanged = true;
          }
          else break;
        }

        if (newPerformance < oldOptimalPerformance || oldOptimalPerformance == -1)
          cout << "\033[1;31mFound optimal network with performance " << newPerformance << "\033[0m" << endl;
        else if (i != pool.size() - 1 && GET_BOOL("POOL_FOUND_VERBOSE"))
          cout << "Found pool network with performance " << newPerformance << endl;
      }
    }
    else {
      int netId = rand() % pool.size();
      if (GET_BOOL("OPTIMIZE_VERBOSE"))
        cout << "Mutating network " << netId << endl;
      NeuralNetwork *newNetwork =
        new NeuralNetwork(pool[netId]->
                          mutate(GET_INT("NUM_CONNECTIONS_MUTATED"),
                                 GET_FLOAT("MUTATION_AMOUNT")));
      newNetwork->write(GET_STRING("TEMP_NEURAL_NETWORK_FILE"));
      int oldPerformance = poolPerformance[netId];
      int newPerformance = getPerformance(*newNetwork);
    
      lock.lock_upgradable();
      // This may cause duplicate performances, but they will have different structures
      // because it is unlikley that one network is mutated into a copy of another
      if (newPerformance < oldPerformance) {
        int newUpdateTime = getFileTimestamp(GET_STRING("BEST_PERFORMANCE_FILE"));
        if (lastUpdateTime != newUpdateTime) {
          if (GET_BOOL("OPTIMIZE_VERBOSE"))
            cout << "Refreshing..." << endl;
          lastUpdateTime = newUpdateTime;
          reload();
          
          vector<int>::iterator loc = find(poolPerformance.begin(), poolPerformance.end(), oldPerformance);
          if (loc != poolPerformance.end()) {
            netId = loc - poolPerformance.begin();
            if (pool[netId] != NULL) {
              delete pool[netId];
            }
            pool[netId] = newNetwork;
            poolPerformance[netId] = newPerformance;
            poolChanged = true;
          }
          else {
            netId = poolPerformance.size();
            pool.push_back(newNetwork);
            poolPerformance.push_back(newPerformance);
          }
        }
        else {
          if (pool[netId] != NULL) {
            delete pool[netId];
          }
          pool[netId] = newNetwork;
          poolPerformance[netId] = newPerformance;
          poolChanged = true;
        }

        for (; netId > 0; netId--) {
          if (poolPerformance[netId] < poolPerformance[netId - 1]) {
            NeuralNetwork *temp = pool[netId];
            pool[netId] = pool[netId - 1];
            pool[netId - 1] = temp;
            int tempP = poolPerformance[netId];
            poolPerformance[netId] = poolPerformance[netId - 1];
            poolPerformance[netId - 1] = tempP;
            poolChanged = true;
          }
          else break;
        }

        if (newPerformance < oldOptimalPerformance || oldOptimalPerformance == -1)
          cout << "\033[1;31mFound optimal network by improvment with performance " << newPerformance << "\033[0m" << endl;
        else if ((unsigned)netId != pool.size() - 1 && GET_BOOL("POOL_FOUND_VERBOSE"))
          cout << "Improved pool network with performance " << oldPerformance << " to " << newPerformance << endl;
      }
    }

    if (pool.size() > (unsigned)GET_INT("MAX_POOL_SIZE")) {
      delete pool.back();
      pool.pop_back();
      poolPerformance.pop_back();
    }
    
    if (GET_BOOL("PRINT_POOL")) {
      for (unsigned i = 0; i < pool.size(); i++) {
        cout << poolPerformance[i] << endl;
      }
    }

    if (poolChanged) {
      poolChanged = false;
      //sigprocmask(SIG_BLOCK, &sigint, NULL);
      lock.unlock_upgradable_and_lock();

      bool clearPerformances =
        pool.size() == (unsigned)GET_INT("MAX_POOL_SIZE") &&
        poolPerformance.back() - poolPerformance.front() < GET_INT("MIN_DIVERSITY");
      if (clearPerformances)
        cout << "Found low performance diversity, performing bottleneck" << endl;
      ofstream performanceOut(GET_STRING("BEST_PERFORMANCE_FILE"));
      for (unsigned i = 0; i < pool.size(); i++) {
        pool[i]->write(getPoolFile(i));
        if (!clearPerformances || i == 0)
          performanceOut << poolPerformance[i] << endl;
      }
      performanceOut.close();
      pool.front()->write(GET_STRING("OPTIMAL_NEURAL_NETWORK_FILE"));

      lastUpdateTime = getFileTimestamp(GET_STRING("BEST_PERFORMANCE_FILE"));

      //sigprocmask(SIG_UNBLOCK, &sigint, NULL);
      lock.unlock();
    }
    else 
      lock.unlock_upgradable();
  }
}

void OptimizeSimulation::stop() {
  stopRequest = true;
}

int OptimizeSimulation::getFileTimestamp(string filename) {
  struct stat buf;
  stat(filename.c_str(), &buf);
  return buf.st_mtime;
}

string OptimizeSimulation::getPoolFile(int i) {
  return GET_STRING("POOL_NEURAL_NETWORK_BASE_FILE").append(to_string(i));
}

void OptimizeSimulation::reload() {
  ifstream performanceIn(GET_STRING("BEST_PERFORMANCE_FILE"));
  if (performanceIn.is_open()) {
    poolPerformance.clear();
    int performance;
    for (int i = 0; i < GET_INT("MAX_POOL_SIZE") && performanceIn >> performance; i++) {
      poolPerformance.push_back(performance);
      if (GET_BOOL("OPTIMIZE_VERBOSE"))
        cout << "Read performance " << i << ": " << performance << endl;
    }
  }
  else {
    cerr << "Could not find performance file " << GET_STRING("BEST_PERFORMANCE_FILE") << endl;
    exit(1);
  }
  performanceIn.close();
  
  for (unsigned i = 0; i < pool.size(); i++) {
    delete pool[i];
  }
  pool.clear();
  for (unsigned i = 0; i < poolPerformance.size(); i++) {
    pool.push_back(new NeuralNetwork(getPoolFile(i)));
  }
}

int OptimizeSimulation::getPerformance(const NeuralNetwork &network) {
  int result = 0;
  int newSeed = rand();
  srand(123456); // Seed RNG with constant seed so trials are repeatable
  result += getPerformanceRandomRepeated(network);
  result += getPerformanceMaze(network);
  result += getPerformanceObstacles(network);
  srand(newSeed); // Re-seed RNG
  if (GET_BOOL("OPTIMIZE_VERBOSE"))
    cout << "Found performance " << result << endl;
  return result;
}

int OptimizeSimulation::getPerformanceRandomRepeated(const NeuralNetwork &network) {
  int result = 0;
  for (int i = 0; i < GET_INT("NUM_OPTIMIZE_TRIALS"); i++) {
    reset();
    for (int j = 0; j < GET_INT("NUM_OPTIMIZE_ROBOTS_TARGETS"); j++)
      addNeuralNetworkRobotTarget(network);
    for (int j = 0; j < GET_INT("NUM_OPTIMIZE_OBSTACLES"); j++)
      addObstacle();
    while (getNumRobotsTargets() > 0 && result < GET_INT("STEP_LIMIT")) {
      advance();
      result++;
    }
  }
  return result;
}

int OptimizeSimulation::getPerformanceMaze(const NeuralNetwork &network) {
  int result = 0;
  open("../runtime/neuralnetwork/setups/maze.rsim");
  Robot *r = new NeuralNetworkRobot(GET_INT("ROBOT_RADIUS"),
                                    Location(649, 93),
                                    GET_COLOR("ROBOT_COLOR"),
                                    Color(1, 0, 0),
                                    network,
                                    11); // Target id from file
  getObject(11)->setSpeed(GET_INT("OPTIMIZE_MAZE_TARGET_SPEED"));
  while (getNumRobotsTargets() > 0 && result < GET_INT("STEP_LIMIT")) {
    advance();
    result++;
  }

  // Only need to manually delete when it didn't find the target
  // Otherwise it was deleted automaticly
  if (result == GET_INT("STEP_LIMIT")) {
    delete r;
  }

  return result;
}


int OptimizeSimulation::getPerformanceObstacles(const NeuralNetwork &network) {
  int result = 0;
  open("../runtime/neuralnetwork/setups/obstacles1.rsim");
  for (int i = 0; i < GET_INT("NUM_OPTIMIZE_OBSTACLES_TRIALS") / 2; i++) {
    for (int j = 0; j < GET_INT("NUM_OPTIMIZE_OBSTACLES_ROBOTS_TARGETS"); j++)
      addNeuralNetworkRobotTarget(network);
    while (getNumRobotsTargets() > 0 && result < GET_INT("STEP_LIMIT")) {
      advance();
      result++;
    }
  }
  open("../runtime/neuralnetwork/setups/obstacles2.rsim");
  for (int i = 0; i < GET_INT("NUM_OPTIMIZE_OBSTACLES_TRIALS") / 2; i++) {
    for (int j = 0; j < GET_INT("NUM_OPTIMIZE_OBSTACLES_ROBOTS_TARGETS"); j++)
      addNeuralNetworkRobotTarget(network);
    while (getNumRobotsTargets() > 0 && result < GET_INT("STEP_LIMIT")) {
      advance();
      result++;
    }
  }
  return result;
}

void OptimizeSimulation::stopHandler(int) {
  s_currentInstance->stop();
}
