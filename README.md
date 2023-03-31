# OpenGL TPs Template

This template will help you follow the lessons from [this OpenGL class](https://julesfouchy.github.io/Learn--OpenGL/).

**It works on all platforms (Windows, Linux, MacOS).**

- [Setting up](#setting-up)
  - [Creating a repository](#creating-a-repository)
  - [Downloading your repository](#downloading-your-repository)
  - [Compiling](#compiling)
- [Coding](#coding)
  - [Folder structure](#folder-structure)
  - [Switching exercise](#switching-exercise)
  - [Adding an exercise folder](#adding-an-exercise-folder)

## Setting up

### Creating a repository

First, create your own repository based on this one. If you are using GitHub you can use this repository as a template:

![image](https://user-images.githubusercontent.com/45451201/217287736-20058d69-f0ed-40ff-b4e5-cfd852e2ba82.png)

Otherwise simply create a repository on your own and copy-paste all the files in this repo to your new repo.

### Downloading your repository

Run this command inside the directory where you want to put this project:

```
git clone your_repo_url
```

### Compiling

If that is not already done, [install a compiler](https://julesfouchy.github.io/Learn--Clean-Code-With-Cpp/lessons/install-a-compiler/).

If that is not already done, [setup your IDE for C++ development](https://julesfouchy.github.io/Learn--Clean-Code-With-Cpp/lessons/ide/).

Once that is done, open the project folder in your IDE: it will detect the CMakeLists.txt file automatically and you can just run the project:
![image](https://user-images.githubusercontent.com/45451201/217290500-bd09bc81-861f-4da9-b3c6-fef5e28a83f6.png)

You should see this:

![initial-window](https://user-images.githubusercontent.com/45451201/220456307-94cfa377-b153-4af5-8dfa-e08b075e02b9.png)

## Coding

### Folder structure

All your files for the current exercise will go in the folder with the same name (e.g. `TP1_exo1_triangle_blanc`). All the files that you want to reuse in all the exercises will go in `src-common`. All of your assets (textures, 3D models, etc.) will go in the `assets` folder.

### Switching exercise

To switch between the different exercises, simply change the current CMake target:
![Change CMake Target](https://user-images.githubusercontent.com/45451201/219449536-0ade5391-0f64-404d-958d-067667594686.png)

### Adding an exercise folder

If you want to add a new folder to code in it, simply copy-paste one of the existing exercise folders, rename it however you want, and add it to the bottom of the `CMakeLists.txt`:
```cmake
add_exercise(TP1_exo1_triangle_blanc)
add_exercise(TP1_exo2)
add_exercise(TP2_exo1)
add_exercise(TP2_exo2)
add_exercise(TP2_exo3)
# ...
```
