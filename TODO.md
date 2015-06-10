# TODO items, by category
## User interface / graphics
* Fix window to be resizeable, work with full screen
  * Need to figure out how to handle different dimentions with opening simulations
  * Requires environment mod
  * How to handle shrinking the field
    * Delete off-screen objects?
    * Warn when a simulation is too big to open on the current screen?  
* Color of selected object gets messed up when reseting

## Optimization simulation
* Optimization processes seem to be dying after a while with no error?
  * Fix memory leaks that are causing this
* Occasional hang-ups after finding new optimal weights

## Object behavior
* Modify collisions behavior?
* Possible bug in wall detection?
* Rectangle obstacles
  * Would requre major changes to object framework, shrink PhysicalObject class, add CircleObject and RectangleObject subclasses
  * All current objects become subclasses of CircleObject
  * Add RectangleObstacle class as subclass of RectangleObject
* More advanced neural network support - maybe feed-back?

## Project/general
* Make environment a class to allow multiple simulations in the same process
  * Default instance allows objects to be constructed without a new environment
  * Size passed to constructor
* Fix makefile so everything gets rebuilt properly
  * Use automake?
* Fix documentation to include source for .cpp files?
* Online documentation is really slow to load
  * Create gh-pages branch instead of using htmlpreview
