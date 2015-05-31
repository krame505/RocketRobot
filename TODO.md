# TODO items, by category
## User interface / graphics
* Fix window to be resizeable, work with full screen
  * Need to figure out how to handle different dimentions with opening simulations
  * Scale everything?  
* Color of selected object gets messed up when reseting

## Optimization simulation
* Optimization processes seem to be dying after a while with no error?

## Object behavior
* Modify collisions behavior?
* Possible bug in wall detection?
* Rectangle obstacles
  * Would requre major changes to object framework, shrink PhysicalObject class, add CircleObject and RectangleObject subclasses
  * All current objects become subclasses of CircleObject
  * Add RectangleObstacle class as subclass of RectangleObject
* More advanced neural network support - maybe feed-back?

## Project/general
* Move configuration stuff to lib, build .a and link with -lconfiguration
* Fix makefile so everything gets rebuilt properly
  * Use automake?
* Fix documentation to include source for .cpp files?
* Online documentation is really slow to load
  * Create gh-pages branch instead of using htmlpreview
