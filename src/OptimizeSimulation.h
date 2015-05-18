#pragma once
/**
 * \author Lucas Kramer
 * \file  OptimizeSimulation.h
 * \brief Robot simultaion for optimizing the NeuralNetworkRobot
 */

#include <vector>
#include <boost/interprocess/sync/named_upgradable_mutex.hpp>

#include "Color.h"
#include "Location.h"
#include "configuration.h"
#include "environment.h"
#include "util.h"
#include "PhysicalObject.h"
#include "NeuralNetwork.h"

/** \brief OptimizeSimulation class, sets up environments and robots. */
class OptimizeSimulation {
public:

  /**
   * \brief The constructor for the Simulation class
   * \param argc The number of command-line arguments
   * \param argv The command-line arguments
   */
  OptimizeSimulation(int argc, char* argv[]);
  virtual ~OptimizeSimulation();

  void runMainLoop();
  void stop();

  int getPerformance(const NeuralNetwork &network);
  int getPerformanceRandomRepeated(const NeuralNetwork &network);
  int getPerformanceMaze(const NeuralNetwork &network);
  int getPerformanceObstacles(const NeuralNetwork &network);

private:
  static OptimizeSimulation *s_currentInstance;

  boost::interprocess::named_upgradable_mutex lock;

  bool stopRequest = false;
  static void stopHandler(int);

  std::vector<NeuralNetwork*> pool;
  std::vector<int> poolPerformance;

  int getFileTimestamp(std::string filename);
  std::string getPoolFile(int i);
  void reload();

  sigset_t sigint;
};
