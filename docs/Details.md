# Project Details

This file details how the **Project Requirements** explained in the *Project Rules* file are being achieved in our codebase.

## Load Models and Textures

**Models** and **Textures** are loaded using the [vulture::Model](../ComputerGraphicsProject2023/src/vulture/renderer/Model.h) and [vulture::Texture](../ComputerGraphicsProject2023/src/vulture/renderer/Model.h) classes.

The `vulture::Model` class can load meshes from `.obj` files thanks to [tinyobjloader](https://github.com/tinyobjloader/tinyobjloader). The loading algorithm is also optimized not to reuse vertices.

Loaded models are cached so that they can be easily retrieved and used multiple times without any extra overhead.

When a new `vulture::GameObject` is created, it retrieves the corresponding model by calling `Model::get()`.

The `vulture::Texture` system behaves similarly, offering the functionality to load both 2D textures and Cubemap textures from files, other than algorithmically defining them.

Vulture's `GameObject`s can specify **albedo**, **roughness**  and **emission** textures ([See Implementation](../ComputerGraphicsProject2023/src/vulture/scene/GameObject.cpp)).

## Load Shaders

## Set Vertex Format and Uniforms

## Create Pipeline

## Record Draw calls in Command Buffer

## Object Transforms (Matrices)

## Navigation (Input)