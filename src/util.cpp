/**
 * \author Lucas Kramer
 * \file  util.cpp
 * \brief Implmentation of functions for the simulation
 */

#include <unistd.h>
#include <stack>
#include <queue>
#include <mutex>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glui.h>
#include <assert.h>
using namespace std;

#include "Robot.h"
#include "SimpleRobot.h"
#include "ComplexRobot.h"
#include "NeuralNetwork.h"
#include "NeuralNetworkRobot.h"
#include "Target.h"
#include "Obstacle.h"
#include "LightSource.h"
#include "Location.h"
#include "configuration.h"
#include "Environment.h"
#include "artist.h"
#include "util.h"
using namespace util;

// Object addition/removal
static mutex objectsMutex;

static stack<int> robots;
static stack<int> targets;
static stack<int> lights;
static stack<int> obstacles;

// Colors
static int colorNum = 0;

PhysicalObject* util::getObject(int id) {
  return Environment::getEnv()->getObject(id);
}

void util::reset() {
  colorNum = 0;
/*removeAllRobotTarget();
  removeAllLightSource();
  removeAllObstacle();*/
  while (!robots.empty())
    robots.pop();
  while (!targets.empty())
    targets.pop();
  while (!lights.empty())
    lights.pop();
  while (!obstacles.empty())
    obstacles.pop();
  Environment::getEnv()->clear();
}

void util::display() {
  glutSetWindow(1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // drawing commands go here
  objectsMutex.lock();

  // Draw background first
  artist::drawBackground();
  
  // Draw non-hitable objects first so they show up underneath everything...
  for (PhysicalObject *o : *Environment::getEnv()) {
    if (!o->isHitable)
      o->display();
  }

  // ... THEN draw everything else
  for (PhysicalObject *o : *Environment::getEnv()) {
    if (o->isHitable)
      o->display();
  }
  objectsMutex.unlock();

  // debugging messages
  int err;
  if ((err = glGetError()) != GL_NO_ERROR) {
    cerr << "GL is in an error state" << endl;
    cerr << "(GL error code " << err << ")\n";
    assert(0);
  }

  glutSwapBuffers();
}

void util::advance() {
  objectsMutex.lock();
  for (PhysicalObject *o : *Environment::getEnv()) {
    if (o->updatePosition())
      break;
  }
  objectsMutex.unlock();
}

Color util::newColor() {
  int colorNum = 0;
  Color result;
  bool foundColor = false;
  while (!foundColor) {
    result = Color(GET_STRING("TARGET_COLORS")[colorNum]);
    foundColor = true;
    for (PhysicalObject *o : *Environment::getEnv()) {
      if (o->getColor().isSimilar(result) ||
          (o->objectType == ROBOT &&
           ((Robot*)o)->getLineColor().isSimilar(result))) {
        foundColor = false;
        break;
      }
    }
    if ((unsigned)(colorNum + 1) < GET_STRING("TARGET_COLORS").size()) {
      colorNum++; //Increment to next color unless there are no more
    }
  }
  return result;
}


// TODO - relies on robots/targets being the only things that disappear on their own
int util::getNumRobotsTargets() {
  return Environment::getEnv()->getNumObjects() - (getNumLights() + getNumObstacles());
}

int util::getNumLights() {
  return lights.size();
}

int util::getNumObstacles() {
  return obstacles.size();
}

bool util::addRobotTarget(int robotType,
                          int lightSensorConnectionPattern,
                          int robotSensorConnectionPattern,
                          int obstacleSensorConnectionPattern,
                          int targetSensorConnectionPattern,
                          float lightSensorScale,
                          float robotSensorScale,
                          float obstacleSensorScale,
                          float targetSensorScale,
                          int initialSpeed,
                          string neuralNetworkFile) {
  Color targetColor = newColor();
  
  Robot *r = NULL;
  Target *t = NULL;
  try {
    objectsMutex.lock();
    t = new Target(GET_INT("TARGET_RADIUS"),
                   targetColor,
                   LAST + targets.size() + 1);
    switch (robotType) {
    case 0:
      r = new SimpleRobot(GET_INT("ROBOT_RADIUS"),
                          GET_COLOR("ROBOT_COLOR"),
                          targetColor,
                          t->getId());
      break;
    case 1:
      r = new ComplexRobot(GET_INT("ROBOT_RADIUS"),
                           GET_COLOR("ROBOT_COLOR"),
                           targetColor,
                           lightSensorConnectionPattern != 2,
                           robotSensorConnectionPattern != 2,
                           obstacleSensorConnectionPattern != 2,
                           targetSensorConnectionPattern != 2,
                           lightSensorConnectionPattern == 0,
                           robotSensorConnectionPattern == 0,
                           obstacleSensorConnectionPattern == 0,
                           targetSensorConnectionPattern == 0,
                           lightSensorScale,
                           robotSensorScale,
                           obstacleSensorScale,
                           targetSensorScale,
                           initialSpeed,
                           t->getId());
      break;
    case 2:
      r = new NeuralNetworkRobot(GET_INT("ROBOT_RADIUS"),
                                 GET_COLOR("ROBOT_COLOR"),
                                 targetColor,
                                 NeuralNetwork::load(neuralNetworkFile),
                                 t->getId());
      break;
    default:
      throw new invalid_argument("Invalid robot type");
    }
    objectsMutex.unlock();
    robots.push(r->getId());
    targets.push(t->getId());
  }
  catch (const NoOpenLocationException *e) {
    if (t != NULL)
      delete t;
    objectsMutex.unlock();
    return false;
  }
  
  if (GET_BOOL("DEBUG_MESSAGES")) {
    cout << "Added robot " << r->getId() <<
      " at (" << r->getXPosition() << ", " << r->getYPosition() << ")" << endl;
  }

  r->setSpeed(GET_INT("ROBOT_INITIAL_SPEED"));
  t->setSpeed(GET_INT("TARGET_SPEED"));
  return true;
}

bool util::addRobot(int robotType,
                    int lightSensorConnectionPattern,
                    int robotSensorConnectionPattern,
                    int obstacleSensorConnectionPattern,
                    int targetSensorConnectionPattern,
                    float lightSensorScale,
                    float robotSensorScale,
                    float obstacleSensorScale,
                    float targetSensorScale,
                    int initialSpeed,
                    string neuralNetworkFile) {
  Color color = newColor();
  Robot *r = NULL;
  try {
    objectsMutex.lock();
    switch (robotType) {
    case 0:
      r = new SimpleRobot(GET_INT("ROBOT_RADIUS"),
                          GET_COLOR("ROBOT_COLOR"),
                          color,
                          -1);
      break;
    case 1:
      r = new ComplexRobot(GET_INT("ROBOT_RADIUS"),
                           GET_COLOR("ROBOT_COLOR"),
                           color,
                           lightSensorConnectionPattern != 2,
                           robotSensorConnectionPattern != 2,
                           obstacleSensorConnectionPattern != 2,
                           targetSensorConnectionPattern != 2,
                           lightSensorConnectionPattern == 0,
                           robotSensorConnectionPattern == 0,
                           obstacleSensorConnectionPattern == 0,
                           targetSensorConnectionPattern == 0,
                           lightSensorScale,
                           robotSensorScale,
                           obstacleSensorScale,
                           targetSensorScale,
                           initialSpeed,
                           -1);
      break;
    case 2:
      r = new NeuralNetworkRobot(GET_INT("ROBOT_RADIUS"),
                                 GET_COLOR("ROBOT_COLOR"),
                                 color,
                                 NeuralNetwork::load(neuralNetworkFile),
                                 -1,
                                 neuralNetworkFile);
      break;
    default:
      throw new invalid_argument("Invalid robot type");
    }
    objectsMutex.unlock();
    robots.push(r->getId());
    targets.push(-1);
  }
  catch (const NoOpenLocationException *e) {
    objectsMutex.unlock();
    return false;
  }

  if (GET_BOOL("DEBUG_MESSAGES")) {
    cout << "Added robot " << r->getId() <<
      " at (" << r->getXPosition() << ", " << r->getYPosition() << ")" << endl;
  }

  r->setSpeed(GET_INT("ROBOT_INITIAL_SPEED"));
  return true;
}

bool util::addNeuralNetworkRobotTarget(const NeuralNetwork &network) {
  Color targetColor(GET_STRING("TARGET_COLORS")[colorNum]);
  if ((unsigned)(colorNum + 1) < GET_STRING("TARGET_COLORS").size()) {
    colorNum++; //Increment to next color unless there are no more
  }
  
  Robot *r = NULL;
  Target *t = NULL;
  try {
    objectsMutex.lock();
    t = new Target(GET_INT("TARGET_RADIUS"),
                   targetColor,
                   LAST + targets.size() + 1);
    r = new NeuralNetworkRobot(GET_INT("ROBOT_RADIUS"),
                               GET_COLOR("ROBOT_COLOR"),
                               targetColor,
                               network,
                               t->getId());
    objectsMutex.unlock();
    robots.push(r->getId());
    targets.push(t->getId());
  }
  catch (const NoOpenLocationException *e) {
    if (t != NULL)
      delete t;
    objectsMutex.unlock();
    return false;
  }
  
  if (GET_BOOL("DEBUG_MESSAGES")) {
    cout << "Added robot " << r->getId() <<
      " at (" << r->getXPosition() << ", " << r->getYPosition() << ")" << endl;
  }

  r->setSpeed(GET_INT("ROBOT_INITIAL_SPEED"));
  t->setSpeed(GET_INT("TARGET_SPEED"));
  return true;
}

bool util::addStationaryLightSource() {
  LightSource *l;
  try {
    objectsMutex.lock();
    l = new LightSource(GET_INT("LIGHT_SOURCE_RADIUS"),
                        GET_COLOR("LIGHT_SOURCE_COLOR"));
    objectsMutex.unlock();
    lights.push(l->getId());
  }
  catch (const NoOpenLocationException *e) {
    objectsMutex.unlock();
    return false;
  }

  l->setSpeed(0);
  if (GET_BOOL("DEBUG_MESSAGES")) {
    cout << "Added light source " << l->getId() <<
      " at (" << l->getXPosition() << ", " << l->getYPosition() << ")" << endl;
  }

  return true;
}

bool util::addMovingLightSource() {
  LightSource *l;
  try {
    objectsMutex.lock();
    l = new LightSource(GET_INT("LIGHT_SOURCE_RADIUS"),
                        GET_COLOR("LIGHT_SOURCE_COLOR"));
    objectsMutex.unlock();
    lights.push(l->getId());
  }
  catch (const NoOpenLocationException *e) {
    objectsMutex.unlock();
    return false;
  }

  l->setSpeed(GET_INT("LIGHT_SOURCE_SPEED"));
  if (GET_BOOL("DEBUG_MESSAGES")) {
    cout << "Added light source " << l->getId() <<
      " at (" << l->getXPosition() << ", " << l->getYPosition() << ")" << endl;
  }

  return true;
}

bool util::addObstacle() {
  Obstacle *o;
  try {
    objectsMutex.lock();
    o = new Obstacle(GET_INT("MAX_OBSTACLE_RADIUS"),
          GET_INT("MIN_OBSTACLE_RADIUS"), 
          GET_COLOR("OBSTACLE_COLOR"));
    objectsMutex.unlock();
    obstacles.push(o->getId());
  }
  catch (const NoOpenLocationException *e) {
    objectsMutex.unlock();
    return false;
  }

  if (GET_BOOL("DEBUG_MESSAGES")) {
    cout << "Added obstacle " << o->getId() <<
      " at (" << o->getXPosition() << ", " << o->getYPosition() << ")" << endl;
  }

  return true;
}

bool util::copy(int id, Location loc) {
  if (id != -1 && getObject(id) != NULL) {
    PhysicalObject *obj = getObject(id);
    try {
      objectsMutex.lock();
      if (Environment::getEnv()->isCollidingWithHitable(loc, obj->getRadius())) {
        objectsMutex.unlock();
        return false;
      }
      
      Robot *rIn = NULL;
      ComplexRobot *crIn = NULL;
      NeuralNetworkRobot *nnrIn = NULL;
      Robot *r = NULL;
      Obstacle *o = NULL;
      LightSource *l = NULL;
      switch (obj->objectType) {
      case ROBOT:
        rIn = (Robot*)obj;
        switch (rIn->robotType) {
        case SIMPLE:
          r = new SimpleRobot(obj->getRadius(), loc,
                              obj->getColor(), rIn->getLineColor(),
                              rIn->getTarget());
          break;
        case COMPLEX:
          crIn = (ComplexRobot*)rIn;
          r = new ComplexRobot(obj->getRadius(), loc,
                               obj->getColor(), rIn->getLineColor(),
                               crIn->enableLightSensors,
                               crIn->enableRobotSensors,
                               crIn->enableObstacleSensors,
                               crIn->enableTargetSensors,
                               crIn->lightSensorsCrossed,
                               crIn->robotSensorsCrossed,
                               crIn->obstacleSensorsCrossed,
                               crIn->targetSensorsCrossed,
                               crIn->lightSensorScale,
                               crIn->robotSensorScale,
                               crIn->obstacleSensorScale,
                               crIn->targetSensorScale,
                               crIn->defaultSpeed,
                               rIn->getTarget());
          break;
        case NEURAL_NETWORK:
          nnrIn = (NeuralNetworkRobot*)rIn;
          r = new NeuralNetworkRobot(obj->getRadius(), loc,
                                     obj->getColor(), rIn->getLineColor(),
                                     NeuralNetwork::load(nnrIn->filename),
                                     rIn->getTarget());
          break;
        default:
          objectsMutex.unlock();
          return false;
        }
        r->setOrientation(obj->getOrientation());
        robots.push(r->getId());
        targets.push(-1);

        if (GET_BOOL("DEBUG_MESSAGES")) {
          cout << "Added robot " << r->getId() <<
            " at (" << r->getXPosition() << ", " << r->getYPosition() << ")" << endl;
        }
        break;
      case OBSTACLE:
        o = new Obstacle(obj->getRadius(), loc, obj->getColor());
        o->setOrientation(obj->getOrientation());
        obstacles.push(o->getId());

        if (GET_BOOL("DEBUG_MESSAGES")) {
          cout << "Added obstacle " << o->getId() <<
            " at (" << o->getXPosition() << ", " << o->getYPosition() << ")" << endl;
        }
        break;
      case LIGHT:
        l = new LightSource(obj->getRadius(), loc, obj->getColor());
        l->setOrientation(obj->getOrientation());
        lights.push(l->getId());

        if (GET_BOOL("DEBUG_MESSAGES")) {
          cout << "Added light source " << l->getId() <<
            " at (" << l->getXPosition() << ", " << l->getYPosition() << ")" << endl;
        }
        break;
      default:
        objectsMutex.unlock();
        return false;
      }
      objectsMutex.unlock();
    }
    catch (const NoOpenLocationException *e) {
      objectsMutex.unlock();
      return false;
    }
  }

  return true;
}

bool util::removeRobotTarget() {
  bool success = false;
  while (!robots.empty() && !success) {
    objectsMutex.lock();
    if (getObject(robots.top()) != NULL) {
      if (GET_BOOL("DEBUG_MESSAGES")) {
        cout << "Removed robot " << robots.top() << endl;
        if (targets.top() != -1)
          cout << "Removed target " << targets.top() << endl;
      }
      success = true;
    }
    delete getObject(robots.top()); // Destructor calls removeObject
    if (targets.top() != -1)
      delete getObject(targets.top());
    objectsMutex.unlock();
    robots.pop();
    targets.pop();
  }

  return success;
}

bool util::removeLightSource() {
  bool success = false;
  while (!lights.empty() && !success) {
    objectsMutex.lock();
    if (getObject(lights.top()) != NULL) {
      if (GET_BOOL("DEBUG_MESSAGES")) {
        cout << "Removed light source " << lights.top() << endl;
      }
      success = true;
    }
    delete getObject(lights.top()); // Destructor calls removeObject
    objectsMutex.unlock();
    lights.pop();
  }

  return success;
}

bool util::removeObstacle() {
  bool success = false;
  while (!obstacles.empty() && !success) {
    if (getObject(obstacles.top()) != NULL) {
      objectsMutex.lock();
      if (GET_BOOL("DEBUG_MESSAGES")) {
        cout << "Removed obstacle " << obstacles.top() << endl;
      }
      success = true;
    }
    delete getObject(obstacles.top()); // Destructor calls removeObject
    objectsMutex.unlock();
    obstacles.pop();
  }

  return success;
}

void util::removeAllRobotTarget() {
  while (!robots.empty()) {
    if (GET_BOOL("DEBUG_MESSAGES") &&
        getObject(robots.top()) != NULL) {
      cout << "Removed robot " << robots.top() << endl;
      if (targets.top() != -1)
        cout << "Removed target " << targets.top() << endl;
    }
    objectsMutex.lock();
    delete getObject(robots.top()); // Destructor calls removeObject
    if (targets.top() != -1)
      delete getObject(targets.top());
    objectsMutex.unlock();
    robots.pop();
    targets.pop();
  }
}

void util::removeAllLightSource() {
  while (!lights.empty()) {
    if (GET_BOOL("DEBUG_MESSAGES") &&
        getObject(lights.top()) != NULL) {
      cout << "Removed light source " << lights.top() << endl;
    }
    objectsMutex.lock();
    //removeObject(lights.top());
    delete getObject(lights.top()); // Destructor calls removeObject
    objectsMutex.unlock();
    lights.pop();
  }
}

void util::removeAllObstacle() {
  while (!obstacles.empty()) {
    if (GET_BOOL("DEBUG_MESSAGES") &&
        getObject(obstacles.top()) != NULL) {
      cout << "Removed obstacle " << obstacles.top() << endl;
    }
    objectsMutex.lock();
    //removeObject(obstacles.top());
    delete getObject(obstacles.top()); // Destructor calls removeObject
    objectsMutex.unlock();
    obstacles.pop();
  }
}


bool util::open(string filename) {
  ifstream in(filename);
  if (!in.is_open()) {
    return false;
  }
  reset();

  string line;
  string token;
  getline(in, line);
  istringstream parse(line);
  getline(parse, token, ',');
  Environment::getEnv()->setWidth(stoi(token));
  getline(parse, token, ',');
  Environment::getEnv()->setHeight(stoi(token));
  
  while (getline(in, line)) {
    istringstream parse(line);
    queue<string> tokens;
    while (getline(parse, token, ',')) {
      tokens.push(token);
    }
    int type = stoi(tokens.front());
    tokens.pop();
    int radius = stoi(tokens.front());
    tokens.pop();
    int xPos = stoi(tokens.front());
    tokens.pop();
    int yPos = stoi(tokens.front());
    tokens.pop();
    float red = stof(tokens.front());
    tokens.pop();
    float green = stof(tokens.front());
    tokens.pop();
    float blue = stof(tokens.front());
    tokens.pop();
    int orientation = stoi(tokens.front());
    tokens.pop();
    int speed = stoi(tokens.front());
    tokens.pop();

    int robotType, targetId;
    float lineRed, lineGreen, lineBlue;
    bool enableLightSensors, enableRobotSensors, enableObstacleSensors, enableTargetSensors;
    bool lightSensorsCrossed, robotSensorsCrossed, obstacleSensorsCrossed, targetSensorsCrossed;
    float lightSensorScale, robotSensorScale, obstacleSensorScale, targetSensorScale;
    int defaultSpeed;
    string networkFilename;

    LightSource *l;
    Obstacle *o;
    Robot *r;
    Target *t;
    switch ((ObjectType)type) {
    case LIGHT:
      l = new LightSource(radius, Location(xPos, yPos), Color(red, green, blue));
      lights.push(l->getId());
      l->setOrientation(orientation);
      l->setSpeed(speed);
      break;
    case OBSTACLE:
      o = new Obstacle(radius, Location(xPos, yPos), Color(red, green, blue));
      obstacles.push(o->getId());
      o->setOrientation(orientation);
      o->setSpeed(speed);
      break;
    case ROBOT:
      robotType = stoi(tokens.front());
      tokens.pop();
      targetId = stoi(tokens.front());
      tokens.pop();
      lineRed = stof(tokens.front());
      tokens.pop();
      lineGreen = stof(tokens.front());
      tokens.pop();
      lineBlue = stof(tokens.front());
      tokens.pop();
      switch ((RobotType)robotType) {
      case SIMPLE:
        r = new SimpleRobot(radius, Location(xPos, yPos),
                            Color(red, green, blue), Color(lineRed, lineGreen, lineBlue),
                            targetId);
        break;
      case COMPLEX:
        enableLightSensors = stoi(tokens.front());
        tokens.pop();
        enableRobotSensors = stoi(tokens.front());
        tokens.pop();
        enableObstacleSensors = stoi(tokens.front());
        tokens.pop();
        enableTargetSensors = stoi(tokens.front());
        tokens.pop();
        lightSensorsCrossed = stoi(tokens.front());
        tokens.pop();
        robotSensorsCrossed = stoi(tokens.front());
        tokens.pop();
        obstacleSensorsCrossed = stoi(tokens.front());
        tokens.pop();
        targetSensorsCrossed = stoi(tokens.front());
        tokens.pop();
        lightSensorScale = stof(tokens.front());
        tokens.pop();
        robotSensorScale = stof(tokens.front());
        tokens.pop();
        obstacleSensorScale = stof(tokens.front());
        tokens.pop();
        targetSensorScale = stof(tokens.front());
        tokens.pop();
        defaultSpeed = stoi(tokens.front());
        tokens.pop();
        r = new ComplexRobot(radius, Location(xPos, yPos),
                             Color(red, green, blue), Color(lineRed, lineGreen, lineBlue),
                             enableLightSensors,
                             enableRobotSensors,
                             enableObstacleSensors,
                             enableTargetSensors,
                             lightSensorsCrossed,
                             robotSensorsCrossed,
                             obstacleSensorsCrossed,
                             targetSensorsCrossed,
                             lightSensorScale,
                             robotSensorScale,
                             obstacleSensorScale,
                             targetSensorScale,
                             defaultSpeed,
                             targetId);
        colorNum++;
        break;
      case NEURAL_NETWORK:
        networkFilename = tokens.front();
        tokens.pop();
        r = new NeuralNetworkRobot(radius, Location(xPos, yPos),
                                   Color(red, green, blue), Color(lineRed, lineGreen, lineBlue),
                                   NeuralNetwork::load(networkFilename),
                                   targetId);
        break;
      default:
        r = NULL;
        break;
      }
      robots.push(r->getId());
      if (r->getTarget() == -1)
        targets.push(-1);
      r->setOrientation(orientation);
      r->setSpeed(speed);
      break;
    case TARGET:
    default:
      t = new Target(radius, Location(xPos, yPos), Color(red, green, blue), type);
      targets.push(t->getId());
      t->setOrientation(orientation);
      t->setSpeed(speed);
      break;
    }
  }
  return true;
}

bool util::save(string filename) {
  unordered_map<int, int> ids;
  int objectNum = 0;

  ofstream out(filename);
  if (!out.is_open()) {
    return false;
  }
  
  out <<
    Environment::getEnv()->getWidth() << "," <<
    Environment::getEnv()->getHeight() << endl;

  for (PhysicalObject *o : *Environment::getEnv()) {
    out <<
      (int)o->objectType << "," <<
      o->getRadius() << "," <<
      o->getXPosition() << "," <<
      o->getYPosition() << "," <<
      o->getColor().red << "," <<
      o->getColor().green << "," <<
      o->getColor().blue << "," <<
      o->getOrientation() << "," << 
      o->getSpeed();
    if (o->objectType == ROBOT) {
      Robot *r = (Robot*)o;
      out << "," <<
        (int)r->robotType << "," <<
        ids[r->getTarget()] << "," <<
        r->getLineColor().red << "," <<
        r->getLineColor().green << "," <<
        r->getLineColor().blue;
      ComplexRobot *cr;
      NeuralNetworkRobot *nnr;
      switch (r->robotType) {
      case SIMPLE:
        break;
      case COMPLEX:
        cr = (ComplexRobot*)r;
        out << "," <<
          cr->enableLightSensors << "," <<
          cr->enableRobotSensors << "," <<
          cr->enableObstacleSensors << "," <<
          cr->enableTargetSensors << "," <<
          cr->lightSensorsCrossed << "," <<
          cr->robotSensorsCrossed << "," <<
          cr->obstacleSensorsCrossed << "," <<
          cr->targetSensorsCrossed << "," <<
          cr->lightSensorScale << "," <<
          cr->robotSensorScale << "," <<
          cr->obstacleSensorScale << "," <<
          cr->targetSensorScale << "," <<
          cr->defaultSpeed;
        break;
      case NEURAL_NETWORK:
        nnr = (NeuralNetworkRobot*)r;
        out << "," <<
          nnr->filename;
        break;
      }
    }
    ids[o->getId()] = objectNum;
    objectNum++;
    out << endl;
  }
  out.close();
  return true;
}
