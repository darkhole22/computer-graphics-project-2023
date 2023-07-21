# Project Details

This document details how the **Project Requirements** are being achieved in our codebase.

## Load Models and Textures

**Models** and **Textures** are loaded using the [vulture::Model](../ComputerGraphicsProject2023/src/vulture/renderer/Model.h) and [vulture::Texture](../ComputerGraphicsProject2023/src/vulture/renderer/Model.h) classes.

The `vulture::Model` class can load meshes from `.obj` files thanks to [tinyobjloader](https://github.com/tinyobjloader/tinyobjloader). The loading algorithm is also optimized not to reuse vertices.

Loaded models are cached so that they can be easily retrieved and used multiple times without any extra overhead.

When a new `vulture::GameObject` is created, it retrieves the corresponding model by calling `Model::get()`.

The `vulture::Texture` system behaves similarly, offering the functionality to load both 2D textures and Cubemap textures from files, other than algorithmically defining them.

Vulture's GameObjects can specify **albedo**, **roughness**  and **emission** textures ([See Implementation](../ComputerGraphicsProject2023/src/vulture/scene/GameObject.cpp)).

## Shaders and Pipelines

Shaders are loaded from SPIR-V binaries in the [vulture::Shader](../ComputerGraphicsProject2023/src/vulture/renderer/Pipeline.h) class.

Pipelines are configured in the `vulture::Pipeline` constructor, using mostly default values and with the option to customize some advanced settings such as *cull mode* or *depth testing*.

[vulture::Scene](../ComputerGraphicsProject2023/src/vulture/scene/Scene.h) creates, on startup, a **default pipeline** for GameObjects using the default *Phong* shader.

However, different pipelines can be created for different needs: each `vulture::SceneObjectList` manages its own pipeline, creating it based on the required *Shaders* and *Descriptor Set Layouts*.

Moreover, the [vulture::Skybox](../ComputerGraphicsProject2023/src/vulture/scene/Skybox.h) class manages its own *Pipeline*, providing its shaders and explicitly disabling **Depth Testing**.

> Disabling Depth Testing guarantees that the skybox is drawn behind everything else.

The [vulture::UIHandler](../ComputerGraphicsProject2023/src/vulture/scene/ui/UIHandler.h) class also manages its own *Pipelines*, one for rendering **Text** and one for rendering **Images**. These two pipelines use vastly different shaders, but the same settings, enabling **color blending** and specifying a **compare operator**.

> Color Blending allows text to be correctly drawn on top of other fragments, and `VK_COMPARE_OP_ALWAYS` guarantees that UI is always rendered on top of everything else.

## Set Vertex Format and Uniforms

## Record Draw calls in Command Buffer

## Object Transforms (Matrices)

## Navigation (Input)