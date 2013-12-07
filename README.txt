glGA Geometric Application framework for modern Shader-Based OpenGL
@Copyright George Papagiannakis, 2012-2013, University of Crete & FORTH




This is a simple openGL Geometric Applications framework for shader-based computer graphics development for Windows and MacOSX-Linux platforms and modern GPUs.

Dependencies on all required third-party, open-source, freely available, multi-platform libraries are included inside the framework folders.


Windows7: Visual C++ 2010 Express
-----------------------------------
In the folder _platforms/VisualStudio/glGA run the glGA.sln file. Then Build/Run each example. In a second separate window you can see the console output messages.
Note that the code assumes OpenGL 3.2 and GLSL 1.5 but you can easily modify the header file and shaders for OpenGL 2.1 and GLSL 1.2 (commented currently)
Also don't forget to register your Visual Studio 2010 Express (free)


MacOSX Lion: XCODE5
----------------------
In the folder _platforms/MacOSX/glGA run the glGA.xcodeproj file. 
You will need to modify the Build-Settings::SearchPaths as well as Build-Phases:: Link Binary with Libraries to the paths of your system inside glGA, before Build/Run
Note that the code assumes OpenGL 3.2 and GLSL 1.5
in folder: _thirdPartyLibs/glew-1.9.0
make
sudo make install

in folder: _thirdPartyLibs/glfw-2.7.6/
make cocoa
sudo make cocoa-install
sudo make cocoa-dist-install

in folder: _thirdPartyLibs/AntTweakBar/src
make
sudo make install

Linux:
---------------------
For the linux platform you can also install (as super user) the following third-party libs as packages (e.g. Ubuntu 12.10, 3.5.0-17-generic)

1) apt-get update

2) apt-get install build-essential

3) apt-get install libglfw-dev

4) apt-get install libglew-dev

5) apt-get install libglm-dev

Then you can build/run all the examples using the provided Makefiles inside the folders of each example (BasicWindow etc.).