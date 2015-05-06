# SDLBase
SDL2 experiments
By Paul Hindt, 4/22/2015

This is my work-in-progress 2D game engine developed with SDL2.
The solution file and code was created in Visual Studio 2013 Community Edition. With some tweaking it will compile in VS2012.
I have not yet tried to compile with GNU C++ but it does build in XCode with Clang.

Current Functionality:
- Movable player entity
- Player can "shoot" a projectile aimed with the mouse
- Scrolling tilemap background

Goals:
- Generic entity system to allow creation, destruction, and state tracking of entities (players, enemies, projectiles, etc)
- Read/write tile map data to/from disk.
- Expand vector math library
- Add sprite sheet animation from my Cute2D experiment
- Sound effects and music playback
