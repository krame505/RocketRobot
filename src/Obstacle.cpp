/**
 * \author Himawan Go
 * \editor Lucas Kramer
 * \file  Obstacle.cpp
 * \brief The representation of an obstacle in the simulation.
 */

#include <stdlib.h>
#include <math.h>

#include "environment.h"
#include "Obstacle.h"
#include "configuration.h"
#include "artist.h"

Obstacle::Obstacle(int radius,
                   Color color) :
  PhysicalObject(OBSTACLE, radius, color) {}

Obstacle::Obstacle(int maxRadius,
       int minRadius,
       Color color) :
  PhysicalObject(OBSTACLE, maxRadius, minRadius, color) {}

Obstacle::Obstacle(int radius,
                   Location loc,
                   Color color) :
  PhysicalObject(OBSTACLE, radius, loc, color) {}

Obstacle::~Obstacle() {}

bool Obstacle::handleCollision(int, bool wasHit) {
  if (getSpeed() != 0) {
    reorient(GET_BOOL("OPPOSITE_ANGLES") && wasHit?
             GET_INT("REORIENT_ANGLE") :
             -GET_INT("REORIENT_ANGLE"), GET_INT("REORIENT_DISTANCE"));
  }
  return false;
}
