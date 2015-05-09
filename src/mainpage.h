/**
 * \mainpage Summary
 *
 * \section Overview
 * RocketRobot is a multi-featured robot simulator.  It simulates robots, targets, and
 * obstacles in an environment, with the goal of robots seeking and finding their targets.
 * When a robot finds its target, both disappear from the simulation.  In addtion, stimuli
 * or light sources can be placed in the environment.  There are several types of robots.
 * Simple robots have sensors that only seek the target, and ignore obstacles.  Complex
 * robots have sensors for lights, robots, obstacles, and the target.  Note that obstacle
 * sensors include the wall as an 'obstacle', straight ahead of the sensor.  For both robot
 * types, the wheels speeds are controlled directly by the sensor readings.  Complex robots
 * can have each type of sensor scaled or connected in any pattern, this is controllable
 * from the 'New complex robot settings' tab in the interface.  Neural network robots use a
 * feed-forward neural network for control.  The network takes the target, robot, and
 * obstacle settings as inputs, and outputs the wheel speeds.  The network is loaded from a
 * file, specified in the 'New neural network robot settings' tab in the interface.  
 * \subsection Installation
 * You can execute the 'gorobot' executable directly from the src or bin folder, but it is
 * easier to directly install it.  This allows you to run it as 'rocketrobot <optional
 * filename>' from any folder.  To install on a Linux computer, simply run the install
 * script from the main folder.  This soft-links a script to your ~/bin folder.  This
 * requires that you have a ~/bin folder on your path, if not you can create one and add it
 * to your .bashrc or .cshrc.  To uninstall, you can simply run the uninstall script.  
 * \subsection Interface User Interface
 * The RocketRobot user interface is fairly simple.  The control panel allows you to start,
 * stop, pause, resume, and quit the simulation.  The reset button reverts to the previous
 * time it was started if the simulation was opened from a file, or randomly generates a
 * new simulation if the current simulation is random.  The 'Refresh settings' button
 * reloads the configuration files (described later).  The open/save tab also lets you
 * open or save the current state.  You can also provide a file to open as the command-line
 * argument.  There are several examples simulations for this in the examples folder.  
 *
 * The user inteface is fairly straightforward for adding and removing objects.  There are
 * several control tabs from which the number of obstacles, robots, and lights can be
 * controlled.  Objects are added and removed in a FILO manner from the interface.  Targets
 * are always created with a robot, but not all robots need to have a target.  Decreasing
 * the number of robots through the control panel deletes the target paired with the
 * removed robot.  
 *
 * For more information about an object (location, speed, orientation, radius), you can
 * click on an object.  You can also drag and drop objects to move them, or drag them off
 * the screen to delete them.  The radius off the selected object can be changed with the 
 * up and down arrow keys, the orientation with the left and right keys, and the speed with
 * the + and - keys.  You can also middle-click to paste a copy of the selected object.  
 * Note that copying a robot creates a new robot with the same target (if any.)  You cannot
 * copy targets.  
 * 
 * \section Optimization Neural network optimization
 * A major portion of the project is the neural network-driven robot, and the associated
 * optimization algorithims.  To optimize the neural network robot, a seperate simulation
 * class, OptimizeSimulation, was created.  Optimization employs a genetic algorthim, in
 * which a pool of possible networks is maintained, along with their performance.  The
 * performances are calculated by performing a large number of runs of (consistant) random
 * simulations, and summing the total time for all robots to find all targets.  In
 * addition, several spesfic test cases are run for each network to test some interesting
 * cases (e.g. navigating a simple maze, etc.)  Pairs of networks are randomly selected
 * from the pool (with one network from a better subset of the pool.)  These are combined
 * by taking half the weights from each and merging them into a new network, then making
 * more random tweaks.  This is then tested and added to the pool if it is better than the
 * worst performance in the pool.  The best performance in the pool is the optimal network.   
 * 
 * To run the optimization, run ./optimize from the scripts folder.  The temporary and
 * intermediate files get saved to runtime/neuralnetwork.  This includes the optimization
 * log, pool performances, optimial network, pool networks, and several others.  
 *
 * \section Implementation
 * The various objects are all subclasses of PhysicalObject, which handles basic behaviors
 * and attributes.  This class is virtual, so that spesific bevhaviors for collisions, etc
 * are handled individually, and can optionally call back to the default handlers in
 * PhysicalObject.  Each type of robot is a subclass of Robot, which is also virtual, as it
 * has virtual functions to get new wheel speeds given the sensor readings.  Everything
 * else, including reading from sensors and collision behavior, is handled directly by the
 * robot class.  
 *
 * There is also an environment namespace with which all objects regester, and are
 * automaticly removed when they are destructed.  An iterator over all objects can be
 * requested from the environment, and they can also be accessed by id.  The environment
 * also contains functions for detecting collisions between walls and objects.  
 * A util namespace provides an easy way of adding and removing objects, managing stacks
 * so that objects are added and removed in FILO order.  It also contains functionality
 * to open and save the current state to and from files.  
 *
 * The Simulation class handles the user interface, providing a wrapper for the functions
 * in util.  It also acts as a driver, requesting updates and redraws from all objects.  
 * \subsection Motion Object motion
 * Object movement is handled by PhysicalObject.  It has an updateMembers function which
 * advances all objects by a distance caluclated from the speed, after calling the virtual
 * update function.  The robot class update sets the position and speed from the calculated
 * wheel speeds.  The updateMembers function is called for each object from advance in
 * util, which in turn is called from advance in Simulation which is triggered by a glut
 * timer function.  
 * \subsection Graphics
 * A graphical display of the simulation is rendered by OpenGL.  Most of the graphics
 * functionallity is in the artist namespace, which has simple functions for drawing
 * circles and lines, which are used by more complex functions for different kinds of
 * objects.  There is a virtual function in PhysicalObject to render each object as a
 * circle, and this is overridden by Robot and LightSource.  These are called by simulation
 * similarly to updatePosition.  
 * \subsection Configuration Configuration system
 * Most settings are loaded through the configuration system, see configuration.h
 * for more details.  This allows settings to be stored externally, and avoids
 * needing to recompile often when making small changes.  Configuration values can also be
 * set from the command line.  
 * 
 * \section Licence
 * This project has been developed by Lucas Kramer, Carl Bahn, Himawan Go, and Xi Zhang.  
 * It has been extended by Lucas Kramer to add neural network-controlled robots and made
 * the interface more useable.  
 * Copyright (C) 2015 Lucas Kramer, Carl Bahn, Himawan Go, and Xi Zhang 
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
