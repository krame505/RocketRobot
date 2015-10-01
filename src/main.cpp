/**
 * \file  main.cpp
 * \brief The main function to execute the simulation program.
 * \author Lucas Kramer
 * \author Carl Bahn
 * \author Himawan Go
 */

#include <stdlib.h>
#include <time.h>
#include <sys/prctl.h>

#include "OptimizeSimulation.h"
#include "Simulation.h"
#include "configuration.h"

#define DEFAULT_CONFIG "../config/default" // Can't use INSTALL_DIR since it needs config to be loaded first...

/** Main function to execute the simulation */
int main(int argc, char* argv[]) {
  // Seed RNG
  srand(time(NULL));
  
  // Change process name so hopefully people don't kill this...
  prctl(PR_SET_NAME, (long int)"plzDontKillMe");

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
