# Brickview

![brickviewAppPreview](res/BrickviewPreview.png)

## Purpose

This program is a standalone application available on _Windows_. Lego parts can be viewed in a 3D space with the help of the LDraw resources.

## How to run the app

Make sure to clone the repository using the following command

``` bash
# SSH
git clone --recursive git@github.com:Malkrich/Brickview.git
# HTTPS
git clone --recursive https://github.com/Malkrich/Brickview.git
```

This project uses [Premake](https://premake.github.io/) for the build system, and can be compiled / run on _Visual Studio Community_. When the project is cloned, the `GenerateProject.bat` file can be executed to generate the solution file.

## Control

The camera can be controlled with the mouse and the keyboard. The controls are similar than the one on _Blender_:

| Button                    | Description                                               |
|---------------------------|-----------------------------------------------------------|
| Mouse middle button       | Rotate the camera around the center (0, 0, 0)             |
| Mouse scroll              | Move the camera closer or farer from the center (0, 0, 0) |
| Maj + Mouse middle button | Translate the camera in relation with the orientation     |

## Resources

As the data folder containing the Lego parts is big, it is not available on this repository (work in progress...). For now on, the example of one lego part can be viewed on the screen.
The entire resource folder is available on the [LDraw](https://library.ldraw.org/updates?latest) website.

## References

The arhcitecture of the software as well as well as the code style is highly inspired from the youtube channel [The Cherno](https://www.youtube.com/@TheCherno). Especially the [Game Engine Series](https://youtube.com/playlist?list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT&si=-5jZiF8NPV4YkNbc) for the Input, Event system, Layers etc... However the renderer and the LDraw file loader has been made from scratch.

## Next steps

Here is a list of the things to improve / add:

- Fix the __normal orientation__ of the lego parts.
- Add __render settings__ on the GUI so the user can see the normal orientation etc...
- Make a __better renderer__, with multiple lights / HDRI.
- Add an __Entity Component System__ (ECS).
- Add an editor to __assemble lego parts__.
- __Serialize__ and load the scenes.
- Make the __documentation__ of the program.