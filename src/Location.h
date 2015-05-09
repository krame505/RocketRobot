#pragma once

/** \brief an xy position on the screen, approximately in pixels
 * x is horizontal from the left and y is vertical from the bottom

*/
typedef struct Location {
  Location() {}
  Location(float x, float y) {
    this->x = x;
    this->y = y;
  }
  
  bool operator==(Location other) {
    return x == other.x && y == other.y;
  }

  float x;
  float y;
} Location;
