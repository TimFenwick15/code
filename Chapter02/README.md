This is a hybrid class/component model.

Actors inherit from the game object, and have components (such as a SpriteComponent).

Inheritance allows components to get data from parents/eachother without expensive querying which a pure component model would need.

In this way we have a shallow hierachy, and components to avoid the issues scaling monolithic hierarchies (adding functionality which all subclasses inherit).
Always avoid inheriting from two classes.

Some other models do away with the GameObject. eg keep an Actor database

Sprite is a graphic in a 2D game. Often an image file. Using PNG here but a larger game wouldn't use this because hardware cannot draw them directly. PC/Xbox use DXT files.

A painters algorithm draws the background first, then objects in the foreground.

Some objects draw and update. Static objects (such as a background) draw and don't update.
A camera is an eg that updates but doesn't draw.

C++ issues:
incomplete class - hadn't included class header
invalid default parameter redeclaration - had put the constructor parameter defaults in header and source
