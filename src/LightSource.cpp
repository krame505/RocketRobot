
/**
 * \author Himawan Go
 * \editor Lucas Kramer
 *
 * \file  LightSource.cpp
 * \brief The representation of a target in the simulation.
 *
 */
#include <stdlib.h>
#include <math.h>

#include "artist.h"
#include "environment.h"
#include "LightSource.h"
#include "configuration.h"
using namespace environment;

LightSource::LightSource(int radius,
                         Color color) :
  PhysicalObject(LIGHT, radius, color, false) {}

LightSource::LightSource(int maxRadius,
                         int minRadius,
                         Color color) :
  PhysicalObject(LIGHT, maxRadius, minRadius, color, false) {}

LightSource::LightSource(int radius,
                         Location loc,
                         Color color) :
  PhysicalObject(LIGHT, radius, loc, color, false) {}

LightSource::~LightSource() {}

bool LightSource::handleCollision(int otherId, bool wasHit) {
  if (GET_BOOL("LIGHT_SOURCE_RANDOM_WANDER")) {
    if (otherId == -1 ||
        getObject(otherId) == NULL || 
        getObject(otherId)->getSpeed() == 0)
      setOrientation(rand() % 360);
  }
  else if (getSpeed() != 0) {
    reorient(GET_BOOL("OPPOSITE_ANGLES") && wasHit?
             GET_INT("REORIENT_ANGLE") :
             -GET_INT("REORIENT_ANGLE"), GET_INT("REORIENT_DISTANCE"));
  }

  return false;
}

// Graphics
void LightSource::display() {
  artist::drawLight(getLocation(), getRadius(), getColor());
}

