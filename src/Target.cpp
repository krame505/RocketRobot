
/**
 * \author Himawan Go
 * \editor Lucas Kramer
 * \file  Target.cpp
 * \brief The representation of a target in the simulation.
 *
 */

#include <stdlib.h>
#include <math.h>

#include "Environment.h"
#include "Target.h"
#include "configuration.h"

Target::Target(int radius,
               Color color,
               int targetNum,
               Environment *env) :
  PhysicalObject((ObjectType)targetNum, radius, color, false, env) {}

Target::Target(int maxRadius,
	       int minRadius,
	       Color color,
               int targetNum,
               Environment *env) :
  PhysicalObject((ObjectType)targetNum, maxRadius, minRadius, color, false, env) {}

Target::Target(int radius,
               Location loc,
               Color color,
               int targetNum,
               Environment *env) :
  PhysicalObject((ObjectType)targetNum, radius, loc, color, false, env) {}

Target::~Target() {}

bool Target::handleCollision(int, bool wasHit) {
  if (GET_BOOL("TARGET_RANDOM_WANDER")) {
    setOrientation(rand() % 360);
  }
  else {
    reorient(GET_BOOL("OPPOSITE_ANGLES") && wasHit?
             GET_INT("REORIENT_ANGLE") :
             -GET_INT("REORIENT_ANGLE"), GET_INT("REORIENT_DISTANCE"));
  }

  return false;
}
