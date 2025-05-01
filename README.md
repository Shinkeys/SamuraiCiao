# Samurai Ciao
This is OpenGL game/engine in development. Main purpose of this project is to learn main render techniques and
to gain experience in developing multifunctional application.

The goal is to create a desired game based on this engine with modern graphics techniques such as Ray Tracing, PBR.
Also to make some intersting graphics effects such as realistic water, destructions.

# Requirements
Originally this engine started it's way on Linux/vscode using OpenGL functionality up to 4.6 version.
You can still build it on Windows using CMake.

# What's already done
[x] Mesh loader<br>
[x] Basic lighting and shadows<br>
[x] Gamma correction<br>
[x] Normal mapping<br>
[x] Skybox<br>
[x] Collision<br>
[x] Particles<br>
[ ] Scene editor: WIP<br>
### TO DO:<br>
[ ] Global Illumination<br>
[ ] PBR<br>
[ ] Scene saving into JSON file<br>
[ ] Cascaded Shadow Mapping<br>
[ ] Prototype of the game level<br>
[ ] Ray Tracing(modern API)<br>

# Building
To build and run the project, first of all clone this repository using --recursive flag:<br>

    git clone https://github.com/Shinkeys/SamuraiCiao --recursive

Example of building and compilation with CMake, Ninja:

    mkdir build
    cmake build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=g++
    cd build
    ninja

Execute project via:

    ./build/SamuraiCiao
  
    
