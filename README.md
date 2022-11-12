# Galaxy-Gravity-Exlaunch

This mod is a rewrite of the old Galaxy-Gravity mod using 
Exlaunch as a basis, as it allows for more fluiditiy.

## Features

Included in this repository is a `romfs` folder, which can be 
used to convert Cascade Kingdom into a WIP version of Good Egg Galaxy from
Super Mario Galaxy. This also includes its own gravity types, which are the following:

- Point Gravity
- Cube Gravity
- Segment Gravity
- Plane Gravity
- Cone Gravity
- Disk Gravity
- Torus Gravity

Each type has been implemented to mimic Super Mario Galaxy as closely
as possible, including things like area parameters. 

If there is no `GravityArea` present, or the actor is not inside it, they will
experience a gravitational pull that is the normal of the triangle of collision
they are standing on.

Not all actors support gravity by default, and so a patch has been added to 
give them such functionality, though most actors do not function as expected.


# Notes

This mod only works on v1.0 of the game, and requires building yourself using cmake.

# Credits

- Slycer - made the GEG port
- [CraftyBoss](https://github.com/CraftyBoss) - helped create the `SuperSpinDriver` actor and custom camera 
- [exlaunch](https://github.com/shadowninja108/exlaunch/)
- [exlaunch-cmake](https://github.com/EngineLessCC/exlaunch-cmake/)
- [BDSP](https://github.com/Martmists-GH/BDSP)
- [Sanae](https://github.com/Sanae6)
- [Atmosphère](https://github.com/Atmosphere-NX/Atmosphere)
- [oss-rtld](https://github.com/Thog/oss-rtld)
