# Project Details

This document details how the **Project Requirements** are being achieved in our codebase.

## Load Models and Textures

**Models** and **Textures** are loaded using the [vulture::Model](../ComputerGraphicsProject2023/src/vulture/renderer/Model.h) and [vulture::Texture](../ComputerGraphicsProject2023/src/vulture/renderer/Model.h) classes.

The `vulture::Model` class can load meshes from `.obj` files thanks to [tinyobjloader](https://github.com/tinyobjloader/tinyobjloader). The loading algorithm is also optimized not to reuse vertices.

Loaded models are cached so that they can be easily retrieved and used multiple times without any extra overhead.

When a new `vulture::GameObject` is created, it retrieves the corresponding model by calling `Model::get()`.

The `vulture::Texture` system behaves similarly, offering the functionality to load both 2D textures and Cubemap textures from files, other than algorithmically defining them.

Vulture's GameObjects can specify **albedo**, **roughness**  and **emission** textures ([See Implementation](../ComputerGraphicsProject2023/src/vulture/scene/GameObject.cpp)), whereas the **Skybox** uses a *Cubemap* texture.

## Shaders and Pipelines

Shaders are loaded from SPIR-V binaries in the [vulture::Shader](../ComputerGraphicsProject2023/src/vulture/renderer/Pipeline.h) class.

Pipelines are configured in the `vulture::Pipeline` constructor, using mostly default values and with the option to customize some advanced settings such as *cull mode* or *depth testing*.

[vulture::Scene](../ComputerGraphicsProject2023/src/vulture/scene/Scene.h) creates, on startup, a **default pipeline** for GameObjects using the default *Phong* shader.

However, different pipelines can be created for different needs: each `vulture::SceneObjectList` manages its own pipeline, creating it based on the required *Shaders*, *Descriptor Set Layouts* and *Pipeline Features*.

For example, [Trees](../ComputerGraphicsProject2023/src/game/terrain/Tree.h) use the default pipeline to render the trunks, but a custom one to render leaves, since they require to disable **culling**.

Moreover, the [vulture::Skybox](../ComputerGraphicsProject2023/src/vulture/scene/Skybox.h) class manages its own *Pipeline*, providing its shaders and explicitly disabling **Depth Testing**.

> Disabling Depth Testing guarantees that the skybox is drawn behind everything else, provided that it's rendered first.

The [vulture::UIHandler](../ComputerGraphicsProject2023/src/vulture/scene/ui/UIHandler.h) class also manages its own *Pipelines*, one for rendering **Text** and one for rendering **Images**. These two pipelines use vastly different shaders, but the same settings, enabling **color blending** and specifying a **compare operator**.

> Color Blending allows text to be correctly drawn on top of other fragments, and `VK_COMPARE_OP_ALWAYS` guarantees that UI is always rendered on top of everything else.

The [Terrain](../ComputerGraphicsProject2023/src/game/terrain/Terrain.h) also manages its own *Pipeline*, using the *Oren-Nayar* shader.

## Set Vertex Layouts and Uniforms

The default **Vertex Layout** used for GameObjects is defined in the [vulture::Renderer](../ComputerGraphicsProject2023/src/vulture/renderer/Renderer.h) class.

The **Skybox** and the **UIHandler** both specify their own vertex layouts.

[GameObjects](../ComputerGraphicsProject2023/src/vulture/scene/GameObject.h) defines the following **Uniform Buffers**:
- Three *TextureSamplers* for the *albedo*, *roughness* and *emission* textures.
- A `ModelBufferObject` storing the *World Matrix* of the object
- An `ObjectBufferObject` storing additional details, such as the *Emission Strength* of the object.
  - ~~We couldn't come up with a better name~~

The [Terrain](../ComputerGraphicsProject2023/src/game/terrain/Terrain.h) defines the following **Uniform Buffers**:
- Four *TextureSamplers* for the terrain textures *(water, sand, grass, rock)*
- A *TextureSampler* used for the **Terrain Heightmap**
- A `TerrainBufferObject` used to store heightmap levels 

The **Skybox** uses the following **Uniform Buffers**:
- A `SkyboxBufferObject` used to store the *Projection* and *View Matrices*
- A `samplerCube` to store the Cubemap Texture

The **User Interfaces** uses the following **Uniform Buffers**:
- A *TextureSampler* used to store the image textures and the font atlas.
- A `TextVertexBufferObject` both for *images* and *text*, to store *Position*, *Scale* and *Aspect Ratio*.
- A `TextFragmentBufferObject`, only for *text*, to store info such as color or border.

Moreover, some uniform buffers are shared among multiple shaders:
- A [CameraBufferObject](../ComputerGraphicsProject2023/src/vulture/scene/Camera.h) that stores view and projection matrices.
- A [WorldBufferObject](../ComputerGraphicsProject2023/src/vulture/scene/World.h) storing details about Scene Light

## Record Draw calls in Command Buffer

Commands Recording is handled by the [FrameContext](../ComputerGraphicsProject2023/src/vulture/renderer/FrameContext.h) class. It provides methods to bind necessary parameters and draw models. When destroyed, a`FrameContext` will cause the *SwapChain* to submit the current frame.

A `FrameContext` is managed by the `Scene` class, during its `Scene::render` method ([Go to Implementation](../ComputerGraphicsProject2023/src/vulture/scene/Scene.cpp)).






## Object Transforms (Matrices)