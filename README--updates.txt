Crash Loyal is a clone of a famous moble app game. This project is not to be
used as a comercial product, simply as a teaching tool for 4150 Game AI.

To build and run the project, open a terminal into this root directory and run
the following command:

g++.exe *.cpp -I./include/SDL2 -L./lib -w -lmingw32 -lSDL2main -lSDL2
-lSDL2_image -o crashloyal

This will generate an executable called 'crashloyal' (windows may make it an
exe, linux or mac may not). The executable will initialize the game state
world, a screen as well as begin scanning for use input.

For more details on the graphics/ application library used please check out
the SDL documentation: https://wiki.libsdl.org/FrontPage

For some concrete examples on how to use SDL, please check out Lazy Foo:
http://lazyfoo.net/tutorials/SDL/index.php

--------------UPDATES------------------------

Collision logic implemented in: Game.h/.cpp and Mob.h/.cpp

In game, added a getMobs function that compiled a list of the mobs owned by each player

Also added a getBuildings function with similar functionality

In Mob, implemented checkCollision and processCollision functions.

checkCollision checked the distance between the current object instance and each mob in the game, returned a list of vectors of objects that the current object collided with

processCollision handled the collisions by running a while loop with condition while collision is true, try to avoid obstacles by moving across the x axis, y axis didn't really change as that affected speed of the object

checkRiverCollision is a Boolean function that returns true if the current object is on the river. There may be some overlap with the river as character can stand on the edge of the river, but they are not allowed to go completely in the river

processRiverCollision handles a river collision and moves the object back as long as it is still on the river

CheckBuildingCollision is a Boolean function that returns true if there is a collision with a building

ProcessBuildingCollision handles the building collisions by bouncing the player back

