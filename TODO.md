# TODO items, by category
## User interface / graphics
* Fix window to be resizeable, work with full screen
  * Need to figure out how to handle different dimentions with opening simulations
  * Scale everything?  

## Optimization simulation


## Object behavior
* Modify collisions behavior?
* Possible bug in wall detection?
* Straight walls
  * Would requre major changes to object framework, shrink PhysicalObject class, add CircleObject and RectangleObject subclasses
  * All current objects become subclasses of CircleObject
  * Add RectangleObstacle class as subclass of RectangleObject
* More advanced neural network support - maybe feed-back?

## Project/general
* Fix makefile so everything gets rebuilt properly
  * Use automake?  
* Fix documentation to include source for .cpp files
* Online documentation is really slow to load
  * Create gh-pages branch instead of using htmlpreview
