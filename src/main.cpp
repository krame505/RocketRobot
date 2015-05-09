/**
 * \file  main.cpp
 * \brief The main function to execute the simulation program.
 * \author Lucas Kramer
 * \author Carl Bahn
 * \author Himawan Go
 */

#include <stdlib.h>
#include <time.h>

#include "OptimizeSimulation.h"
#include "Simulation.h"
#include "configuration.h"

#define DEFAULT_CONFIG "../config/default" // Can't use INSTALL_DIR since it needs config to be loaded first...

/** Main function to execute the simulation */
int main(int argc, char* argv[]) {
  srand(time(NULL)); //Seed RNG

  // Initialize the configuration library
  Configuration::initConfig(argc, argv, DEFAULT_CONFIG);
  
  // Run the simulation
  if (GET_BOOL("OPTIMIZE_SIMULATION")) {
    OptimizeSimulation *app = new OptimizeSimulation(argc, argv);
    app->runMainLoop();
  }
  else {
    Simulation *app = new Simulation(argc, argv);
    app->runMainLoop();
  }

  exit(0);
}
