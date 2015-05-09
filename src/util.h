#pragma once

/**
 * \author Lucas Kramer
 * \file  util.h
 * \brief Helper functions for the simulation
 */

#include "NeuralNetwork.h"

/** \brief util namespace, contains helper functions to add and remove robots */
namespace util {
  /**
   * \brief Removes all objects and resets to the initial state
   */
  void reset();

  /**
   * \brief Function to render all the objects.   
   * \detail This function is called repeatedly from the simulation, and iterates through
   * the objects and calls their respective display functions.  
   */
  void display();

  /**
   * \brief Function to update the positions of all objects.  
   * \detail This function is called repeatedly from the simulation, and iterates through
   * the objects and calls their respective updatePosition functions.  
   */
  void advance();

  /**
   * \brief Gets a new, unused color
   * \return The new color
   */
  Color newColor();

  /**
   * \brief Gets the number of robots/target pairs
   * \return The number of robots/target pairs
   */
  int getNumRobotsTargets();

  /**
   * \brief Gets the number of lights
   * \return The number of lights
   */
  int getNumLights();

  /**
   * \brief Gets the number of obstacles
   * \return The number of obstacles
   */
  int getNumObstacles();

  /**
   * \brief Adds a robot and paired target to the simulation
   * \param robotType The type of robot to add.  0 is SimpleRobot, 
   * 1 is ComplexRobot, and 2 is NeuralNetworkRobot
   * \param lightSensorConnectionPattern Settings for ComplexRobot light sensor
   * \param robotSensorConnectionPattern Settings for ComplexRobot robot sensor
   * \param obstacleSensorConnectionPattern Settings for ComplexRobot obstacle sensor
   * \param targetSensorConnectionPattern Settings for ComplexRobot target sensor
   * \param lightSensorScale Settings for ComplexRobot light sensor
   * \param robotSensorScale Settings for ComplexRobot robot sensor
   * \param obstacleSensorScale Settings for ComplexRobot obstacle sensor
   * \param targetSensorScale Settings for ComplexRobot target sensor
   * \param initalSpeed Default speed for ComplexRobot
   * \param neuralNetworkFile File in which neural network is stored
   * \return true if successful
   */
  bool addRobotTarget(int robotType,
                      int lightSensorConnectionPattern,
                      int robotSensorConnectionPattern,
                      int obstacleSensorConnectionPattern,
                      int targetSensorConnectionPattern,
                      float lightSensorScale,
                      float robotSensorScale,
                      float obstacleSensorScale,
                      float targetSensorScale,
                      int initialSpeed,
                      std::string neuralNetworkFile);

  /**
   * \brief Adds a robot to the simulation
   * \param robotType The type of robot to add.  0 is SimpleRobot, 
   * 1 is ComplexRobot, and 2 is NeuralNetworkRobot
   * \param lightSensorConnectionPattern Settings for ComplexRobot light sensor
   * \param robotSensorConnectionPattern Settings for ComplexRobot robot sensor
   * \param obstacleSensorConnectionPattern Settings for ComplexRobot obstacle sensor
   * \param targetSensorConnectionPattern Settings for ComplexRobot target sensor
   * \param lightSensorScale Settings for ComplexRobot light sensor
   * \param robotSensorScale Settings for ComplexRobot robot sensor
   * \param obstacleSensorScale Settings for ComplexRobot obstacle sensor
   * \param targetSensorScale Settings for ComplexRobot target sensor
   * \param initalSpeed Default speed for ComplexRobot
   * \param neuralNetworkFile File in which neural network is stored
   * \return true if successful
   */
  bool addRobot(int robotType,
                int lightSensorConnectionPattern,
                int robotSensorConnectionPattern,
                int obstacleSensorConnectionPattern,
                int targetSensorConnectionPattern,
                float lightSensorScale,
                float robotSensorScale,
                float obstacleSensorScale,
                float targetSensorScale,
                int initialSpeed,
                std::string neuralNetworkFile);

  /**
   * \brief Adds a neural network robot and a paired target to the simulation
   * \param network the network to control the robot
   * \return true if successful
   */
  bool addNeuralNetworkRobotTarget(const NeuralNetwork &network);

  /**
   * \brief Adds a light to the simulation
   * \return true if successful
   */
  bool addStationaryLightSource();

  /**
   * \brief Adds a light to the simulation
   * \return true if successful
   */
  bool addMovingLightSource();

  /**
   * \brief Adds a obstacle to the simulation
   * \return true if successful
   */
  bool addObstacle();

  /**
   * \brief Copies an object to a new Location
   * \param id the id of the object to copy
   * \param loc the destination
   * \return true if successful
   */
  bool copy(int id, Location loc);

  /**
   * \brief Removes a robot from the simulation
   * \return true if successful
   */
  bool removeRobotTarget();

  /**
   * \brief Removes a light from the simulation
   * \return true if successful
   */
  bool removeLightSource();

  /**
   * \brief Removes a obstacle from the simulation
   * \return true if successful
   */
  bool removeObstacle();

  /**
   * \brief Removes all robots
   */
  void removeAllRobotTarget();

  /**
   * \brief Removes all lights
   */
  void removeAllLightSource();

  /**
   * \brief Removes all obstacles
   */
  void removeAllObstacle();

  /**
   * \brief Loads a simulation from a file
   */
  bool open(std::string filename);

  /**
   * \brief Saves the current state to a file
   */
  bool save(std::string filename);
}
