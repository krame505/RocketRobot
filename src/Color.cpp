/**
 * \author Carl Bahn
 * \editor Lucas Kramer
 *
 * \file  Color.cpp
 * \brief Utility functions for handling color.  
 *
 */

#include "Color.h"
#include "configuration.h"

#include <iostream>
#include <cstdlib>
#include <math.h>
using namespace std;

Color::Color(char c) {
  Color color;
  switch (c) {
  case 'R':
    color = Color(1, 0, 0);
    break;
  case 'O':
    color = Color(1, 0.4, 0);
    break;
  case 'Y':
    color = Color(1, 1, 0);
    break;
  case 'G':
    color = Color(0, 1, 0);
    break;
  case 'B':
    color = Color(0, 0, 1);
    break;
  case 'V':
    color = Color(0.5, 0, 0.5);
    break;
  case 'W':
    color = Color(1, 1, 1);
    break;
  case '?':
    color = Color((static_cast<float>(rand())) / (static_cast<float>(RAND_MAX)),
                  (static_cast<float>(rand())) / (static_cast<float>(RAND_MAX)),
                  (static_cast<float>(rand())) / (static_cast<float>(RAND_MAX)));
    break;
  default:
    color = Color(1, 0.5, 1);
    break;
  }
  
  red = color.red;
  green = color.green;
  blue = color.blue;
}

/* Color codes shoud be of the form:
 * blue*256*256 + green*256 + red
 * where red,green,blue are between 0 and 255,
 * like in standard html color codes like #E0A366, which are #RRGGBB in hex
 */
Color::Color(int c) {
  blue  = static_cast<float>(c % 256) / 256.0;
  c /= 256;
  green = static_cast<float>(c % 256) / 256.0;
  c /= 256;
  red   = static_cast<float>(c % 256) / 256.0;
}

bool Color::isSimilar(Color c1) {
  return
    fabs(c1.red - red) < GET_FLOAT("COLOR_DELTA") &&
    fabs(c1.green - green) < GET_FLOAT("COLOR_DELTA") &&
    fabs(c1.blue - blue) < GET_FLOAT("COLOR_DELTA");
}

bool Color::operator==(Color c1) {
  return
    c1.red == red &&
    c1.green == green &&
    c1.blue == blue;
}

bool Color::operator!=(Color c1) {
  return
    c1.red != red ||
    c1.green != green ||
    c1.blue != blue;
}
