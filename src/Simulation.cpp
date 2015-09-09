/**
 * \author Lucas Kramer
 * \editor Xi Zhang, Carl Bahn
 * \file  Simulation.cpp
 * \brief Implementation for the main application class of the robot simulation
 *
 */

#include <unistd.h>
#include <limits.h>
#include <deque>
#include <mutex>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <sys/stat.h>
using namespace std;

#include "PhysicalObject.h"
#include "Robot.h"
#include "SimpleRobot.h"
#include "ComplexRobot.h"
#include "NeuralNetworkRobot.h"
#include "Target.h"
#include "Obstacle.h"
#include "LightSource.h"
#include "Location.h"
#include "configuration.h"
#include "Environment.h"
#include "util.h"
#include "artist.h"
#include "Simulation.h"
using namespace util;

Simulation* Simulation::s_currentApp = NULL;

Simulation::Simulation(int argc, char* argv[]) :
  BaseGfxApp(argc, argv, GET_INT("DISPLAY_WIDTH"), GET_INT("DISPLAY_HEIGHT"),
             50, 50, GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH, true, 851, 50),
  isPaused(false),
  isStarted(GET_BOOL("START_IMMEDIATE")),
  simulationFile(NULL),
  openedFile(false),
  mouseDownLoc(Location(-1, -1)),
  mouseDownId(-1) {

  parseArgs(argc, argv);
  
  setCaption("Robot Simulation");

  // Initialize static variables
  s_currentApp = this;

  // Start graphics
  graphicsBegin(GET_INT("FRAMES_PER_SECOND"), // framerate
                GET_INT("START_DELAY"));      // msec delay before first frame

  //creates a basic UI panel with quit button
  GLUI_Panel *toolPanel = new GLUI_Panel(m_glui, "Control Panel");
  m_glui->add_button_to_panel(toolPanel, "Start", 0, (GLUI_Update_CB)s_start);
  m_glui->add_button_to_panel(toolPanel, "Pause", 0, (GLUI_Update_CB)s_pause);
  m_glui->add_button_to_panel(toolPanel, "Resume", 0, (GLUI_Update_CB)s_resume);
  m_glui->add_button_to_panel(toolPanel, "Reset", 0, (GLUI_Update_CB)s_reset);
  m_glui->add_button_to_panel(toolPanel, "Clear", 0, (GLUI_Update_CB)s_clear);
  m_glui->add_button_to_panel(toolPanel, "New random", 0, (GLUI_Update_CB)s_random);
  m_glui->add_button_to_panel(toolPanel, "Quit", 0, (GLUI_Update_CB)exit);
  m_glui->add_button_to_panel(toolPanel, "Refresh settings", 0, (GLUI_Update_CB)s_refreshConfiguration);
  GLUI_Rollout *simulationFiles = new GLUI_Rollout(m_glui, "Open/save simulation");
  controls.push_back(simulationFiles);
  controlsOpen.push_back(true);
  char *oldSimulationFile = simulationFile;
  simulationFile = new char[PATH_MAX];
  if (oldSimulationFile != NULL)
    strcpy(simulationFile, oldSimulationFile);
  simulationFileBrowser =
    m_glui->add_edittext_to_panel(simulationFiles, "File name");
  simulationFileBrowser->set_text(simulationFile);
  simulationFileBrowser->set_w(150);
  m_glui->add_button_to_panel(simulationFiles, "Open", 0, (GLUI_Update_CB)s_open);
  m_glui->add_button_to_panel(simulationFiles, "Save", 0, (GLUI_Update_CB)s_save);
  GLUI_Rollout *robots = new GLUI_Rollout(m_glui, "Number of robots/targets", true);
  controls.push_back(robots);
  controlsOpen.push_back(true);
  m_glui->add_button_to_panel(robots, "+ With target", 0, (GLUI_Update_CB)s_addRobotTarget);
  m_glui->add_button_to_panel(robots, "+ No target", 0, (GLUI_Update_CB)s_addRobot);
  m_glui->add_button_to_panel(robots, "-", 0, (GLUI_Update_CB)s_removeRobotTarget);
  m_glui->add_button_to_panel(robots, "Remove all", 0, (GLUI_Update_CB)s_removeAllRobotTarget);
  GLUI_Panel *robotTypePanel = new GLUI_Panel(robots, "New robot type");
  GLUI_RadioGroup *robotTypeRG =
    m_glui->add_radiogroup_to_panel(robotTypePanel, &robotType);
  m_glui->add_radiobutton_to_group(robotTypeRG, "Simple");
  m_glui->add_radiobutton_to_group(robotTypeRG, "Complex");
  m_glui->add_radiobutton_to_group(robotTypeRG, "Neural network");
  robotTypeRG->set_int_val(GET_INT("DEFAULT_ROBOT_TYPE"));
  GLUI_Rollout *obstacles = new GLUI_Rollout(m_glui, "Number of obstacles     ", false);
  controls.push_back(obstacles);
  controlsOpen.push_back(false);
  m_glui->add_button_to_panel(obstacles, "+", 0, (GLUI_Update_CB)s_addObstacle);
  m_glui->add_button_to_panel(obstacles, "-", 0, (GLUI_Update_CB)s_removeObstacle);
  m_glui->add_button_to_panel(obstacles, "Remove all", 0, (GLUI_Update_CB)s_removeAllObstacle);
  GLUI_Rollout *lights = new GLUI_Rollout(m_glui, "Number of light sources ", false);
  controls.push_back(lights);
  controlsOpen.push_back(false);
  m_glui->add_button_to_panel(lights, "+ Stationary", 0, (GLUI_Update_CB)s_addStationaryLightSource);
  m_glui->add_button_to_panel(lights, "+ Moving", 0, (GLUI_Update_CB)s_addMovingLightSource);
  m_glui->add_button_to_panel(lights, "-", 0, (GLUI_Update_CB)s_removeLightSource);
  m_glui->add_button_to_panel(lights, "Remove all", 0, (GLUI_Update_CB)s_removeAllLightSource);
  m_glui->add_column(true);
  string statInitText(GET_INT("MAX_STAT_LENGTH"), ' ');
  statsBox = new GLUI_Rollout(m_glui, "Selected object details", true);
  settings.push_back(statsBox);
  settingsOpen.push_back(true);
  radiusText = new GLUI_StaticText(statsBox, statInitText.c_str());
  LocationText = new GLUI_StaticText(statsBox, statInitText.c_str());
  m_glui->add_column_to_panel(statsBox, false);
  orientationText = new GLUI_StaticText(statsBox, statInitText.c_str());
  speedText = new GLUI_StaticText(statsBox, statInitText.c_str());
  string messageInitText(GET_INT("MAX_MESSAGE_LENGTH"), ' ');
  messageBox = new GLUI_Rollout(m_glui, "Messages");
  settings.push_back(messageBox);
  settingsOpen.push_back(true);
  for (int i = 0; i < GET_INT("MAX_MESSAGES"); i++)
    messages.push_back(new GLUI_StaticText(messageBox, messageInitText.c_str()));
  neuralNetworkFile = new char[PATH_MAX];
  strcpy(neuralNetworkFile, GET_STRING("DEFAULT_NEURAL_NETWORK_FILE").c_str());
  /*  neuralNetworkFileBrowser =
      new GLUI_FileBrowser(m_glui, "Neural network specification file");
      neuralNetworkFileBrowser->fbreaddir("../..");*/
  GLUI_Rollout *neuralNetworkRobotSettings = new GLUI_Rollout(m_glui, "New neural network robot settings                                  ", false);
  settings.push_back(neuralNetworkRobotSettings);
  settingsOpen.push_back(false);
  neuralNetworkFileBrowser =
    m_glui->add_edittext_to_panel(neuralNetworkRobotSettings, "Weight file");
  neuralNetworkFileBrowser->set_text(string(neuralNetworkFile));
  neuralNetworkFileBrowser->set_w(330);
  m_glui->add_button_to_panel(neuralNetworkRobotSettings, "Select",
                              -1, (GLUI_Update_CB)s_neuralNetworkFileChanged);
  GLUI_Rollout *complexRobotSettings = new GLUI_Rollout(m_glui, "New complex robot settings                                           ", false);
  settings.push_back(complexRobotSettings);
  settingsOpen.push_back(false);
  GLUI_Panel *lightConnectionPanel =  new GLUI_Panel(complexRobotSettings, "Light sensor connections");
  GLUI_RadioGroup *lightConnections =
    m_glui->add_radiogroup_to_panel(lightConnectionPanel, &lightSensorConnectionPattern);
  m_glui->add_radiobutton_to_group(lightConnections, "Crossed");
  m_glui->add_radiobutton_to_group(lightConnections, "Uncrossed");
  m_glui->add_radiobutton_to_group(lightConnections, "Dissabled");
  lightConnections->set_int_val(GET_INT("ROBOT_INITIAL_LIGHT_SENSOR_CONNECTION_PATTERN"));
  GLUI_Panel *robotConnectionPanel = new GLUI_Panel(complexRobotSettings, "Robot sensor connections");
  GLUI_RadioGroup *robotConnections =
    m_glui->add_radiogroup_to_panel(robotConnectionPanel, &robotSensorConnectionPattern);
  m_glui->add_radiobutton_to_group(robotConnections, "Crossed");
  m_glui->add_radiobutton_to_group(robotConnections, "Uncrossed");
  m_glui->add_radiobutton_to_group(robotConnections, "Dissabled");
  //  m_glui->add_column_to_panel(complexRobotSettings, true);
  robotConnections->set_int_val(GET_INT("ROBOT_INITIAL_ROBOT_SENSOR_CONNECTION_PATTERN"));
  GLUI_Panel *obstacleConnectionPanel = new GLUI_Panel(complexRobotSettings, "Obstacle sensor connections");
  GLUI_RadioGroup *obstacleConnections =
    m_glui->add_radiogroup_to_panel(obstacleConnectionPanel, &obstacleSensorConnectionPattern);
  m_glui->add_radiobutton_to_group(obstacleConnections, "Crossed");
  m_glui->add_radiobutton_to_group(obstacleConnections, "Uncrossed");
  m_glui->add_radiobutton_to_group(obstacleConnections, "Dissabled");
  obstacleConnections->set_int_val(GET_INT("ROBOT_INITIAL_OBSTACLE_SENSOR_CONNECTION_PATTERN"));
  GLUI_Panel *targetConnectionPanel =  new GLUI_Panel(complexRobotSettings, "Target sensor connections");
  GLUI_RadioGroup *targetConnections =
    m_glui->add_radiogroup_to_panel(targetConnectionPanel, &targetSensorConnectionPattern);
  m_glui->add_radiobutton_to_group(targetConnections, "Crossed");
  m_glui->add_radiobutton_to_group(targetConnections, "Uncrossed");
  m_glui->add_radiobutton_to_group(targetConnections, "Dissabled");
  targetConnections->set_int_val(GET_INT("ROBOT_INITIAL_TARGET_SENSOR_CONNECTION_PATTERN"));
  m_glui->add_column_to_panel(complexRobotSettings, true);
  initialSpeed = GET_INT("ROBOT_INITIAL_SPEED");
  GLUI_Spinner *initialSpeed =
    m_glui->add_spinner_to_panel(complexRobotSettings, "Initial speed        ",
                                  GLUI_SPINNER_INT, &this->initialSpeed);
  initialSpeed->set_int_limits(GET_INT("ROBOT_MIN_SPEED"), GET_INT("ROBOT_MAX_SPEED"));
  lightSensorScale = GET_FLOAT("ROBOT_INITIAL_LIGHT_SENSOR_SCALE");
  GLUI_Spinner *lightSensorScale =
    m_glui->add_spinner_to_panel(complexRobotSettings, "Light scale      ",
                                  GLUI_SPINNER_FLOAT, &this->lightSensorScale);
  lightSensorScale->set_float_limits(GET_FLOAT("ROBOT_MIN_SENSOR_SCALE"),
                                     GET_FLOAT("ROBOT_MAX_SENSOR_SCALE"));
  robotSensorScale = GET_FLOAT("ROBOT_INITIAL_ROBOT_SENSOR_SCALE");
  GLUI_Spinner *robotSensorScale =
    m_glui->add_spinner_to_panel(complexRobotSettings, "Robot scale    ",
                                  GLUI_SPINNER_FLOAT, &this->robotSensorScale);
  robotSensorScale->set_float_limits(GET_FLOAT("ROBOT_MIN_SENSOR_SCALE"),
                                     GET_FLOAT("ROBOT_MAX_SENSOR_SCALE"));
  obstacleSensorScale = GET_FLOAT("ROBOT_INITIAL_OBSTACLE_SENSOR_SCALE");
  GLUI_Spinner *obstacleSensorScale =
    m_glui->add_spinner_to_panel(complexRobotSettings, "Obstacle scale",
                                  GLUI_SPINNER_FLOAT, &this->obstacleSensorScale);
  obstacleSensorScale->set_float_limits(GET_FLOAT("ROBOT_MIN_SENSOR_SCALE"),
                                     GET_FLOAT("ROBOT_MAX_SENSOR_SCALE"));
  targetSensorScale = GET_FLOAT("ROBOT_INITIAL_TARGET_SENSOR_SCALE");
  GLUI_Spinner *targetSensorScale =
    m_glui->add_spinner_to_panel(complexRobotSettings, "Target scale    ",
                                 GLUI_SPINNER_FLOAT, &this->targetSensorScale);
  targetSensorScale->set_float_limits(GET_FLOAT("ROBOT_MIN_SENSOR_SCALE"),
                                      GET_FLOAT("ROBOT_MAX_SENSOR_SCALE"));
  
  // Initialize OpenGL
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_BLEND); 
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
  glMatrixMode(GL_PROJECTION); 
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW); 
  glLoadIdentity();
  gluOrtho2D(0, m_width, 0, m_height);

  // Needed to use a subwindow
  int tx, ty, tw, th;
  GLUI_Master.get_viewport_area(&tx, &ty, &tw, &th);
  m_width += m_width - tw + GET_INT("FRAME_SHIFT");
  //  m_height += m_height - tw;
  
  // Show startup message
  showStartup();

  // Create objects if you aren't opening an existing simulation
  if (!openedFile)
    initObjects();
}

Simulation::~Simulation() {
  delete neuralNetworkFile;
  delete simulationFile;
  for (PhysicalObject *o : *Environment::getEnv()) {
    delete o;
  }
}

void Simulation::showStartup() {
  showMessage("Welcome to RocketRobot, an open source robot simulator.");
  showMessage("Click to select an object, and click-and-drag to move it.");
  showMessage("Drag an object off the screen to delete it. The left/right keys");
  showMessage("rotate the selected object, the up/down keys change the");
  showMessage("radius, and the +/- keys change the speed.  The below");
  showMessage("panels contain controls for settings for different types of");
  showMessage("robots. These apply only to new robots; existing ones remain");
  showMessage("unchanged. The control panels allow you to start, stop,");
  showMessage("pause, reset to the last start configuration, clear, open, save,");
  showMessage("and generate new random simulations.  The other panels");
  showMessage("control adding/removing objects in different variations.");
  for (int i = 0; i < GET_INT("MAX_MESSAGES") - 12; i++) {
    showMessage("");
  }
}

void Simulation::parseArgs(int argc, char **argv) {
  for (int i = 1; i < argc; i++) {
    if (!strncmp(argv[i], "-D", 2))
      i += 2;
    else if (!strcmp(argv[i], "--use-config"))
      i++;
    else if (!strcmp(argv[i], "--add-config"))
      i++;
    else if (!openedFile) {
      openedFile = true;
      if (!open(GET_STRING("WORKING_DIR") + "/" + string(argv[i]))) {
        cerr << "Simulation file " << argv[i] << " not found" << endl;
        exit(0);
      }
      else {
        simulationFile = argv[i];
        isStarted = false;
      }
    }
    else {
      cerr << "Invalid command-line argument: " << argv[i] << endl;
      exit(1);
    }
  }
}

void Simulation::initObjects() {
  // Raise an error when there is not enough space
  bool success = false;
  for (int tries = 0;
       tries < GET_INT("PLACE_ALL_RETRIES") && !success;
       tries++) {
    success = true;

    // Add lights first so the gradient is on the "bottom"
    for (int i = 0; i < GET_INT("NUM_LIGHT_SOURCES"); i++) {
      success &= addMovingLightSource();
    }
    for (int i = 0; i < GET_INT("NUM_ROBOTS_TARGETS"); i++) {
      success &= addRobotTarget(robotType,
                                lightSensorConnectionPattern,
                                robotSensorConnectionPattern,
                                obstacleSensorConnectionPattern,
                                targetSensorConnectionPattern,
                                lightSensorScale,
                                robotSensorScale,
                                obstacleSensorScale,
                                targetSensorScale,
                                initialSpeed,
                                string(neuralNetworkFile));
    }
    for (int i = 0; i < GET_INT("NUM_OBSTACLES"); i++) {
      success &= addObstacle();
    }
  }

  if (!success)
    showMessage("Objects do not fit the window.  Please check settings");
  else
    showMessage("Objects initalized successfully");
  showStats();
}

void Simulation::display() {
  util::display();
  updateUI();
}

void Simulation::advance() {
  /* This is called between each frame. */
  // Check if we should exit, if that condition is set
  if (GET_BOOL("EXIT_ON_ALL_ROBOTS_FINISHED") && getNumRobotsTargets() == 0)
    exit(0);

  if (!isPaused && isStarted) {
    util::advance();
  }
  else usleep(10);
  showStats();
}

void Simulation::tryAddRobotTarget() {
  if (!addRobotTarget(robotType,
                      lightSensorConnectionPattern,
                      robotSensorConnectionPattern,
                      obstacleSensorConnectionPattern,
                      targetSensorConnectionPattern,
                      lightSensorScale,
                      robotSensorScale,
                      obstacleSensorScale,
                      targetSensorScale,
                      initialSpeed,
                      string(neuralNetworkFile)))
    showMessage("Robots do not fit the window");
  showStats();
}

void Simulation::tryAddRobot() {
  if (!addRobot(robotType,
                lightSensorConnectionPattern,
                robotSensorConnectionPattern,
                obstacleSensorConnectionPattern,
                targetSensorConnectionPattern,
                lightSensorScale,
                robotSensorScale,
                obstacleSensorScale,
                targetSensorScale,
                initialSpeed,
                string(neuralNetworkFile)))
                showMessage("Robots do not fit the window");
  showStats();
}

void Simulation::tryAddStationaryLightSource() {
  if (!addStationaryLightSource())
    showMessage("Lights do not fit the window");
  showStats();
}

void Simulation::tryAddMovingLightSource() {
  if (!addMovingLightSource())
    showMessage("Lights do not fit the window");
  showStats();
}

void Simulation::tryAddObstacle() {
  if (!addObstacle())
    showMessage("Obstacles do not fit the window");
  showStats();
}

void Simulation::tryRemoveRobotTarget() {
  if (!removeRobotTarget())
    showMessage("There are no robots to remove");
  showStats();
}

void Simulation::tryRemoveLightSource() {
  if (!removeLightSource())
    showMessage("There are no lights to remove");
  showStats();
}

void Simulation::tryRemoveObstacle() {
  if (!removeObstacle())
    showMessage("There are no obstacles to remove");
  showStats();
}

void Simulation::tryOpen() {
  strcpy(simulationFile, simulationFileBrowser->get_text());
  if (!open(string(GET_STRING("WORKING_DIR") + "/" + simulationFile))) {
    showMessage("Simulation file " + string(simulationFile) + " not found");
  }
  else {
    openedFile = true;
    isStarted = false;
    showStats();
  }
}

void Simulation::trySave() {
  strcpy(simulationFile, simulationFileBrowser->get_text());
  if (!save(string(GET_STRING("WORKING_DIR") + "/" + simulationFile))) {
    showMessage("Save failed");
  }
}

void Simulation::start(int) {
  save(GET_STRING("INITIAL_SIMULATION_FILE"));
  isStarted = true;
  isPaused = false;
}

void Simulation::pause(int) {
  isPaused = true;
}

void Simulation::resume(int) {
  isPaused = false;
}

void Simulation::reset(int) {
  isStarted = GET_BOOL("START_IMMEDIATE");
  open(GET_STRING("INITIAL_SIMULATION_FILE"));
}

void Simulation::clear(int) {
  util::reset();
}

void Simulation::random(int) {
  isStarted = GET_BOOL("START_IMMEDIATE");
  util::reset();
  initObjects();
}

void Simulation::gluiControl(int controlID) {
  // nothing here for now
}

void Simulation::leftMouseDown(int x, int y) {
  if (mouseDownId != -1 && getObject(mouseDownId) != NULL)
    getObject(mouseDownId)->setColor(oldColor);

  mouseDownLoc = Location(x, glutGet(GLUT_WINDOW_HEIGHT) - y);
  mouseDownId = Environment::getEnv()->getCollisionId(mouseDownLoc, 0);
  if (mouseDownId != -1) {
    if (GET_BOOL("DEBUG_MESSAGES")) {
      cout << "Clicked on " << mouseDownId << endl;
    }
    mouseDownDeltaX =
      getObject(mouseDownId)->getXPosition() - mouseDownLoc.x;
    mouseDownDeltaY =
      getObject(mouseDownId)->getYPosition() - mouseDownLoc.y;
    oldColor = getObject(mouseDownId)->getColor();
    Color newColor(oldColor.red * 0.8, oldColor.green * 0.8, oldColor.blue * 0.8);
    getObject(mouseDownId)->setColor(newColor);
  }

  showStats();
}

void Simulation::leftMouseUp(int x, int y) {
  if (mouseDownId != -1 && getObject(mouseDownId) != NULL) {
    if (Environment::getEnv()->isTouchingWall(mouseDownId)) {
      delete getObject(mouseDownId);
    }
  }

  showStats();
}

void Simulation::middleMouseDown(int x, int y) {
  if (mouseDownId != -1 && getObject(mouseDownId) != NULL) {
    // Needed because the selected object is highlighted
    Color prevColor = getObject(mouseDownId)->getColor();
    getObject(mouseDownId)->setColor(oldColor);

    if (!copy(mouseDownId, Location(x, glutGet(GLUT_WINDOW_HEIGHT) - y)))
      showMessage("Cannot copy object");

    getObject(mouseDownId)->setColor(prevColor);
  }

  showStats();
}

void Simulation::mouseDragged(int x, int y) {
  PhysicalObject *o;
  if (mouseDownId != -1 && (o = getObject(mouseDownId)) != NULL) {
    Location newLoc = Location(x + mouseDownDeltaX, GET_INT("DISPLAY_HEIGHT") - y + mouseDownDeltaY);
    if (!Environment::getEnv()->isTouchingHitableObject(newLoc, o->getRadius(), mouseDownId)) {
      o->forceSetPosition(newLoc.x, newLoc.y);
    }
  }

  showStats();
}

void Simulation::keyboard(unsigned char key, int x, int y) {
  static int lastCallTime = 0;
  int currentTime = glutGet(GLUT_ELAPSED_TIME);
  int deltaTime = currentTime - lastCallTime;
  lastCallTime = currentTime;

  PhysicalObject *o;
  if (mouseDownId != -1 && (o = getObject(mouseDownId)) != NULL) {
    switch (key) {
    case '+':
      if (deltaTime > 100)
        o->setSpeed(o->getSpeed() + 1);
      else
        o->setSpeed(o->getSpeed() + 3);
      break;
    case '-':
      if (deltaTime > 100) {
        if (o->getSpeed() > 0)
          o->setSpeed(o->getSpeed() - 1);
      }
      else if (o->getSpeed() > 2)
        o->setSpeed(o->getSpeed() - 3);
      else
        o->setSpeed(0);
      break;
    case 127: // Delete
        delete o;
      break;
    default:
      break;
    }
  }

  showStats();
}

void Simulation::keyboardSpecial(int key, int x, int y) {
  static int lastCallTime = 0;
  int currentTime = glutGet(GLUT_ELAPSED_TIME);
  int deltaTime = currentTime - lastCallTime;
  lastCallTime = currentTime;

  PhysicalObject *o;
  if (mouseDownId != -1 && (o = getObject(mouseDownId)) != NULL) {
    switch (key) {
    case GLUT_KEY_LEFT:
      if (deltaTime > 100)
        o->rotate(-1);
      else
        o->rotate(-3);
      break;
    case GLUT_KEY_RIGHT:
      if (deltaTime > 100)
        o->rotate(1);
      else
        o->rotate(3);
      break;
    case GLUT_KEY_UP:
      if (deltaTime > 100)
        o->setRadius(o->getRadius() + 1);
      else
        o->setRadius(o->getRadius() + 3);
      while (Environment::getEnv()->isCollidingWithHitable(o->getId()))
        o->setRadius(o->getRadius() - 1);
      break;
    case GLUT_KEY_DOWN:
      if (deltaTime > 100) {
        if (o->getRadius() > 0)
          o->setRadius(o->getRadius() - 1);
      }
      else if (o->getRadius() > 2)
        o->setRadius(o->getRadius() - 3);
      else
        o->setRadius(0);
      break;
    default:
      break;
    }
  }

  showStats();
}

void Simulation::reshape(int width, int height) {
  BaseGfxApp::reshape(width, height);
  s_currentApp->updateUI();
}

// Static callback functions
void Simulation::s_start(int a) {
  s_currentApp->start(a);
}

void Simulation::s_pause(int a) {
  s_currentApp->pause(a);
}

void Simulation::s_resume(int a) {
  s_currentApp->resume(a);
}

void Simulation::s_reset(int a) {
  s_currentApp->reset(a);
}

void Simulation::s_clear(int a) {
  s_currentApp->clear(a);
}

void Simulation::s_random(int a) {
  s_currentApp->random(a);
}

void Simulation::s_addRobotTarget(int a) {
  s_currentApp->tryAddRobotTarget();
}

void Simulation::s_addRobot(int a) {
  s_currentApp->tryAddRobot();
}

void Simulation::s_addStationaryLightSource(int a) {
  s_currentApp->tryAddStationaryLightSource();
}

void Simulation::s_addMovingLightSource(int a) {
  s_currentApp->tryAddMovingLightSource();
}

void Simulation::s_addObstacle(int a) {
  s_currentApp->tryAddObstacle();
}

void Simulation::s_removeRobotTarget(int a) {
  s_currentApp->tryRemoveRobotTarget();
}

void Simulation::s_removeLightSource(int a) {
  s_currentApp->tryRemoveLightSource();
}

void Simulation::s_removeObstacle(int a) {
  s_currentApp->tryRemoveObstacle();
}

void Simulation::s_removeAllRobotTarget(int a) {
  removeAllRobotTarget();
}

void Simulation::s_removeAllLightSource(int a) {
  removeAllLightSource();
}

void Simulation::s_removeAllObstacle(int a) {
  removeAllObstacle();
}

void Simulation::s_refreshConfiguration(int a) {
  Configuration::refresh();
}

void Simulation::s_neuralNetworkFileChanged(int a) {
  const char *filename = s_currentApp->neuralNetworkFileBrowser->get_text();
  if (!ifstream(string(s_currentApp->neuralNetworkFile))) {
    s_currentApp->showMessage("Neural network file not found");
    cout << string(s_currentApp->neuralNetworkFile) << endl;
  }
  else strcpy(s_currentApp->neuralNetworkFile, filename);
}

void Simulation::s_open(int a) {
  s_currentApp->tryOpen();
}

void Simulation::s_save(int a) {
  s_currentApp->trySave();
}

// Graphics
void Simulation::showMessage(string message) {
  /*
  GLUI *messageBox = GLUI_Master.create_glui("Message", 0, 300, 300);
  messageBox->add_statictext(message.c_str());
  */
  /*
  if (messageBox == NULL)
    messageBox = GLUI_Master.create_glui_subwindow(m_glutWindowHandle, GLUI_SUBWINDOW_BOTTOM);
  */
  messageBox->open();

  static deque<string> message_queue;
  message_queue.push_back(message);
  if ((int)message_queue.size() > GET_INT("MAX_MESSAGES"))
    message_queue.pop_front();

  for (unsigned i = 0; i < message_queue.size(); i++) {
    messages[i]->set_text(message_queue[i].c_str());
    messages[i]->update_size();
  }
}

string ftos(float f) {
  std::ostringstream os;
  os << f;
  return os.str();
}

void Simulation::showStats() {
  if (mouseDownId != -1 && getObject(mouseDownId) != NULL) {
    PhysicalObject *o = getObject(mouseDownId);
    radiusText->set_text((     "Radius:    " + to_string(o->getRadius())).c_str());
    radiusText->update_size();
    LocationText->set_text((   "Location: (" + ftos(o->getXPosition()) + ", " +
                               ftos(o->getYPosition()) + ")").c_str());
    LocationText->update_size();
    orientationText->set_text(("Orientation: " + to_string(o->getOrientation())).c_str());
    orientationText->update_size();
    speedText->set_text((      "Speed:       " + to_string(o->getSpeed())).c_str());
    speedText->update_size();
  }
  else {
    string statInitText(GET_INT("MAX_STAT_LENGTH"), ' ');
    radiusText->set_text(statInitText.c_str());
    LocationText->set_text(statInitText.c_str());
    orientationText->set_text(statInitText.c_str());
    speedText->set_text(statInitText.c_str());
  }
}

void Simulation::updateUI() {
  // Force an update
  if (settingsOpen[0]) {
    settings[0]->close();
    settings[0]->open();
  }
  else {
    settings[0]->open();
    settings[0]->close();
  }
  organizePanels(controls, controlsOpen, GET_INT("MAX_CONTROLS_OPEN"));
  organizePanels(settings, settingsOpen, GET_INT("MAX_SETTINGS_OPEN"));
}

void Simulation::organizePanels(const vector<GLUI_Rollout*> &panels, std::vector<bool> &panelsOpen, int maxOpen) {
  int numOpen = 0;
  for (unsigned i = 0; i < panels.size(); i++) {
    numOpen += panels[i]->is_open;
  }

  for (unsigned i = panels.size() - 1; numOpen > maxOpen && i >= 0; i--) {
    if (panels[i]->is_open && panelsOpen[i]) {
      panels[i]->close();
      numOpen--;
    }
  }

  for (unsigned i = 0; i < panels.size(); i++) {
    panelsOpen[i] = panels[i]->is_open;
  }
}
