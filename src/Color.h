#pragma once
/**
 * \author Lucas Kramer
 *
 * \file  Color.h
 * \brief Definintions for representing any hexedecimal color
 *
 */

#include <iostream>

#define GET_COLOR(COLOR_VAR)                            \
  (GET_BOOL("USE_HEX_COLORS")?                          \
   (DEFINED(COLOR_VAR "_HEX")?                          \
    Color(GET_INT(COLOR_VAR "_HEX")) :                  \
    Color(GET_CHAR(COLOR_VAR))) :                       \
   (DEFINED(COLOR_VAR)?                                 \
    Color(GET_CHAR(COLOR_VAR)) :                        \
    Color(GET_INT(COLOR_VAR "_HEX"))))

/**
 * \brief This struct holds the representation of a color.  
 * \details Each field is a float value that shoult be between 0 and 1.  This
 * allows a direct translation from hex colors, with each field being the hex
 * RGB value / 255.  
 */
typedef struct Color {
  float red;
  float green;
  float blue;

  Color() {}
  Color(float r, float g, float b) {
    red = r;
    green = g;
    blue = b;
  }
  
  /**
   * \author Lucas Kramer
   * \brief Creates a RGB color from its char color name
   * \detail The following is the list of allowed colors:
   * <table style="width:100%">
   *  <tr>
   *    <td><b>Color char</b></td>
   *    <td><b>Color name</b></td>    
   *    <td><b>RGB floating-point value</b></td>
   *  </tr>
   *  <tr>
   *    <td>'R'</td>
   *    <td>Red</td>    
   *    <td>(1, 0, 0)</td>
   *  </tr>
   *  <tr>
   *    <td>'O'</td>
   *    <td>Orange</td>  
   *    <td>(1, 0.4, 0)</td>  
   *  </tr>
   *  <tr>
   *    <td>'Y'</td>
   *    <td>Yellow</td>
   *    <td>(1, 1, 0)</td>
   *  </tr>
   *  <tr>
   *    <td>'G'</td>
   *    <td>Green</td>
   *    <td>(0, 1, 0)</td>
   *  </tr>
   *  <tr>
   *    <td>'B'</td>
   *    <td>Blue</td>
   *    <td>(0, 0, 1)</td>
   *  </tr>
   *  <tr>
   *    <td>'V'</td>
   *    <td>Violet</td>
   *    <td>(0.5, 0, 0.5)</td>
   *  </tr>
   *  <tr>
   *    <td>'W'</td>
   *    <td>White</td>
   *    <td>(1, 1, 1)</td>
   *  </tr>
   *  <tr>
   *    <td>default</td>
   *    <td>Black</td>
   *    <td>(0, 0, 0)</td>
   *  </tr>
   * </table>
   */
  Color(char c);
  
  /**
   * \author Carl Bahn
   * \editor Lucas Kramer
   * \brief Creates an RGB color from a hex value 
   * \detail The color is stored as the last 3 bytes, as 
   * 0x\<unused (set to 0)\>\<red\>\<green\>\<blue\><br>
   * Each byte is interpreted as an integer holding an RGB value from 0-255.  
   * These are then converted to floats between 0 and 1 by dividing by 255.  
   * Note that this requires at least 3 byte integers.  
   */
  Color(int c);

  /**
   * \author Lucas Kramer
   * \brief Checks if two colors are similar
   * \param c1 the color to compare
   * \return true if the colors are similar
   */
  bool isSimilar(Color c1);

  friend std::ostream& operator<<(std::ostream& out, Color &c) {
    return out << "(" << c.red << ", " << c.green << ", " << c.blue << ")";
  }

  /**
   * \author Lucas Kramer
   * \brief Checks if two colors are equal
   * \param c1 the color to check
   * \return true if the colors are equal
   */
  bool operator==(Color c1);

  /**
   * \author Lucas Kramer
   * \brief Checks if two colors are not equal
   * \param c1 the color to check
   * \return true if the colors are not equal
   */
  bool operator!=(Color c1);
} Color;
