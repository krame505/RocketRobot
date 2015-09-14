# TODO items, by category
## User interface / graphics
* Warn the user when opening a simulation that doesn't fit on the screen
* Color of selected object gets messed up when reseting

## Optimization simulation
* Optimization processes seem to be dying after a while with no error?
  * Fix memory leaks that are causing this
* Occasional hang-ups after finding new optimal weights

## Object behavior
* Modify collisions behavior?
* Rectangle obstacles
  * Would requre major changes to object framework, shrink PhysicalObject class, add CircleObject and RectangleObject subclasses
  * All current objects become subclasses of CircleObject
  * Add RectangleObstacle class as subclass of RectangleObject
* More advanced neural network support - maybe feed-back?
* More types of interesting robots

## Project/general
* Fix makefile so everything gets rebuilt properly
  * Use automake?
* Fix documentation to include source for .cpp files?
* Online documentation is really slow to load
  * Create gh-pages branch instead of using htmlpreview
