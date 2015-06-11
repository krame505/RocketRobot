#pragma once
/**
 * \author Lucas Kramer
 * \file  NeuralNetworkRobot.h
 * \brief The representation of a neural network robot
 */

#include "PhysicalObject.h"
#include "Robot.h"
#include "Sensor.h"
#include "NeuralNetwork.h"
#include "configuration.h"

/** \brief A robot controlled by a neural network with inputs from the sensors */
class NeuralNetworkRobot : public Robot {
public:
  NeuralNetworkRobot(int radius,
                     Color color,
                     Color lineColor,
                     NeuralNetwork network,
                     int targetId = -1,
                     std::string filename = GET_STRING("DEFAULT_NEURAL_NETWORK_FILE"),
                     Environment *env = Environment::getEnv());

  NeuralNetworkRobot(int radius,
                     Location loc,
                     Color color,
                     Color lineColor,
                     NeuralNetwork network,
                     int targetId = -1,
                     std::string filename = GET_STRING("DEFAULT_NEURAL_NETWORK_FILE"),
                     Environment *env = Environment::getEnv());

  ~NeuralNetworkRobot();

  float getLeftSpeed(float leftLightSensorVal, float rightLightSensorVal,
                     float leftRobotSensorVal, float rightRobotSensorVal,
                     float leftObstacleSensorVal, float rightObstacleSensorVal,
                     float leftTargetSensorVal, float rightTargetSensorVal);
  float getRightSpeed(float leftLightSensorVal, float rightLightSensorVal,
                      float leftRobotSensorVal, float rightRobotSensorVal,
                      float leftObstacleSensorVal, float rightObstacleSensorVal,
                      float leftTargetSensorVal, float rightTargetSensorVal);

  const std::string filename;

private:
  NeuralNetwork network;
};

