#pragma once
/**
 * \author Lucas Kramer
 * \editor Xi Zhang, Carl Bahn
 * \file  Simulation.h
 * \brief Main application class for the robot simulation
 */

#include "Color.h"
#include "Location.h"
#include "configuration.h"
#include "environment.h"
#include "util.h"
#include "BaseGfxApp.h"
#include "PhysicalObject.h"

/** \brief Simulation class, sets up the GUI and the drawing environment. */

class Simulation : public BaseGfxApp {
public:
  enum UIControlType {
    UI_QUIT = 0,
    UI_START = 1,
    UI_PAUSE = 2,
    UI_RESUME = 3,
    UI_RESET = 4,
    UI_ADD_RT = 5,
    UI_REMOVE_RT = 6,
    UI_REMOVE_ALL_RT = 7,
    UI_ADD_O = 8,
    UI_REMOVE_O = 9,
    UI_REMOVE_ALL_O = 10,
    UI_REFRESH_SETTINGS_O = 11
  };

  /**
   * \brief The constructor for the Simulation class
   * \param argc The number of command-line arguments
   * \param argv The command-line arguments
   */
  Simulation(int argc, char* argv[]);
  virtual ~Simulation();

  /**
   * \brief Adds objects according to configuration
   */
  void initObjects();

  /**
   * \brief Adds a robot and target to the simulation, or raises an error on failure
   */
  void tryAddRobotTarget();

  /**
   * \brief Adds a robot to the simulation, or raises an error on failure
   */
  void tryAddRobot();

  /**
   * \brief Adds a stationary light to the simulation, or raises an error on failure
   */
  void tryAddStationaryLightSource();

  /**
   * \brief Adds a moving light to the simulation, or raises an error on failure
   */
  void tryAddMovingLightSource();

  /**
   * \brief Adds a obstacle to the simulation, or raises an error on failure
   */
  void tryAddObstacle();

  /**
   * \brief Removes a robot and target from the simulation, or raises an error on failure
   */
  void tryRemoveRobotTarget();

  /**
   * \brief Removes a light from the simulation, or raises an error on failure
   */
  void tryRemoveLightSource();

  /**
   * \brief Removes a obstacle from the simulation, or raises an error on failure
   */
  void tryRemoveObstacle();

  /**
   * \brief Loads a simulation, or raises an error on failure
   */
  void tryOpen();

  /**
   * \brief Saves a simulation, or raises an error on failure
   */
  void trySave();

  /**
   * \brief Call-back function to render all the objects.   
   * \detail This function is called repeatedly from glut, and iterates through
   * the objects and calls their respective display functions.  
   */
  void display();

  /**
   * \brief Call-back function to update the positions of all objects.  
   * \detail This function is called repeatedly from glut, and iterates through
   * the objects and calls their respective updatePosition functions.  
   */
  void advance();

  /**
   * \brief Call-back function for user input.   
   */
  void gluiControl(int controlID);

  /**
   * \brief Call-back function for mouse click.   
   */
  void leftMouseDown(int x, int y);

  /**
   * \brief Call-back function for mouse click.   
   */
  void leftMouseUp(int x, int y);

  /**
   * \brief Call-back function for mouse click.   
   */
  void middleMouseDown(int x, int y);

  /**
   * \brief Call-back function for mouse drag.   
   */
  void mouseDragged(int x, int y);

  /**
   * \brief Call-back function keyboard press.   
   */
  void keyboard(unsigned char key, int x, int y);

  /**
   * \brief Call-back function for special keyboard press.   
   */
  void keyboardSpecial(int key, int x, int y);

  void start(int);
  void pause(int);
  void resume(int);
  void reinit(int);

  static void s_start(int);
  static void s_pause(int);
  static void s_resume(int);
  static void s_reinit(int);

  static void s_addRobotTarget(int);
  static void s_addRobot(int);
  static void s_addStationaryLightSource(int);
  static void s_addMovingLightSource(int);
  static void s_addObstacle(int);
  static void s_removeRobotTarget(int);
  static void s_removeLightSource(int);
  static void s_removeObstacle(int);
  static void s_removeAllRobotTarget(int);
  static void s_removeAllLightSource(int);
  static void s_removeAllObstacle(int);

  static void s_refreshConfiguration(int);
  static void s_open(int);
  static void s_save(int);

  static void s_neuralNetworkFileChanged(int);

private:
  /**
   * \brief Displays a startup message
   */
  void showStartup();

  /**
   * \brief Parses command-line arguments and sets variables accordingly
   * \param argc the number of arguments
   * \param argv the arguments
   */
  void parseArgs(int argc, char **argv);

  /**
   * \brief Displays a message in the messages window
   * \param message the message to display
   */
  void showMessage(std::string message);

  /**
   * \brief Displays stats about the selected object in the stats window
   */
  void showStats();

  static Simulation* s_currentApp;

  bool isPaused;
  bool isStarted;

  int robotType;

  int lightSensorConnectionPattern;
  int robotSensorConnectionPattern;
  int obstacleSensorConnectionPattern;
  int targetSensorConnectionPattern;
  float lightSensorScale;
  float robotSensorScale;
  float obstacleSensorScale;
  float targetSensorScale;
  int initialSpeed;

  char *neuralNetworkFile;
  char *simulationFile;
  bool openedFile;

  GLUI_EditText *simulationFileBrowser;
  GLUI_EditText *neuralNetworkFileBrowser;
  GLUI_Panel *messageBox;
  std::vector<GLUI_StaticText*> messages;
  GLUI_Panel *statsBox;
  GLUI_StaticText *radiusText;
  GLUI_StaticText *LocationText;
  GLUI_StaticText *orientationText;
  GLUI_StaticText *speedText;

  Location mouseDownLoc;
  int mouseDownDeltaX, mouseDownDeltaY;
  int mouseDownId;
  Color oldColor;
};
