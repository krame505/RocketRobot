/**
 * \author Lucas Kramer
 * \file   NeuralNetworkRobot.cpp
 * \brief  The representation of a neural network robot within the simulation
 */

 //Can be uncommented for debugging
/* #include <iostream>
   using std::cout;
   using std::endl;
   using std::string;
*/

#include <GL/glu.h>
#include <GL/glut.h>
#include <stdexcept>

#include "Environment.h"
#include "Robot.h"
#include "NeuralNetworkRobot.h"
#include "NeuralNetwork.h"
#include "configuration.h"
using namespace std;

NeuralNetworkRobot::NeuralNetworkRobot(int radius,
                                       Color color,
                                       Color lineColor,
                                       NeuralNetwork network,
                                       int targetId,
                                       string filename,
                                       Environment *env) :
  Robot(NEURAL_NETWORK, radius, color, lineColor, targetId, env),
  filename(filename),
  network(network) {}

NeuralNetworkRobot::NeuralNetworkRobot(int radius,
                                       Location loc,
                                       Color color,
                                       Color lineColor,
                                       NeuralNetwork network,
                                       int targetId,
                                       string filename,
                                       Environment *env) :
  Robot(NEURAL_NETWORK, radius, loc, color, lineColor, targetId, env),
  filename(filename),
  network(network) {}

NeuralNetworkRobot::~NeuralNetworkRobot() {}

float NeuralNetworkRobot::getLeftSpeed(float leftLightSensorVal, float rightLightSensorVal,
                                       float leftRobotSensorVal, float rightRobotSensorVal,
                                       float leftObstacleSensorVal, float rightObstacleSensorVal,
                                       float leftTargetSensorVal, float rightTargetSensorVal) {
  return
    network.compute({
        leftRobotSensorVal, rightRobotSensorVal,
          leftObstacleSensorVal, rightObstacleSensorVal,
          leftTargetSensorVal * GET_FLOAT("TARGET_SENSOR_SCALE"),
          rightTargetSensorVal * GET_FLOAT("TARGET_SENSOR_SCALE")})[0] *
    GET_FLOAT("SPEED_SCALE_FACTOR") *
    GET_FLOAT("NEURAL_NETWORK_SPEED_SCALE");
}

float NeuralNetworkRobot::getRightSpeed(float leftLightSensorVal, float rightLightSensorVal,
                                        float leftRobotSensorVal, float rightRobotSensorVal,
                                        float leftObstacleSensorVal, float rightObstacleSensorVal,
                                        float leftTargetSensorVal, float rightTargetSensorVal) {
  return
    network.compute({
        leftRobotSensorVal, rightRobotSensorVal,
          leftObstacleSensorVal, rightObstacleSensorVal,
          leftTargetSensorVal * GET_FLOAT("TARGET_SENSOR_SCALE"),
          rightTargetSensorVal * GET_FLOAT("TARGET_SENSOR_SCALE")})[1] *
    GET_FLOAT("SPEED_SCALE_FACTOR") *
    GET_FLOAT("NEURAL_NETWORK_SPEED_SCALE");
}
