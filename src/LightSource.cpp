
/**
 * \author Himawan Go
 * \editor Lucas Kramer
 *
 * \file  LightSource.cpp
 * \brief The representation of a light source (stimuli) in the simulation.
 *
 */
#include <stdlib.h>
#include <math.h>

#include "artist.h"
#include "Environment.h"
#include "LightSource.h"
#include "configuration.h"

LightSource::LightSource(int radius,
                         Color color,
                         Environment *env) :
  PhysicalObject(LIGHT, radius, color, false, env) {}

LightSource::LightSource(int maxRadius,
                         int minRadius,
                         Color color,
                         Environment *env) :
  PhysicalObject(LIGHT, maxRadius, minRadius, color, false, env) {}

LightSource::LightSource(int radius,
                         Location loc,
                         Color color,
                         Environment *env) :
  PhysicalObject(LIGHT, radius, loc, color, false, env) {}

LightSource::~LightSource() {}

bool LightSource::handleCollision(int otherId, bool wasHit) {
  if (GET_BOOL("LIGHT_SOURCE_RANDOM_WANDER")) {
    if (otherId == -1 ||
        env->getObject(otherId) == NULL || 
        env->getObject(otherId)->getSpeed() == 0)
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

