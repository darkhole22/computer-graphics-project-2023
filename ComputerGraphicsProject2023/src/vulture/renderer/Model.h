#pragma once

#include <functional>

#include <tiny_obj_loader.h>

#include "Buffers.h"

namespace vulture {

/**
* @brief Structure representing a 3D vertex with position, normal, and texture coordinates.
*/
struct Vertex
{
	glm::vec3 pos = { 0, 0, 0 };
	glm::vec3 norm = { 0, 0 , 0 };
	glm::vec2 texCoord = { 0, 0 };
};

/**
 * @brief Class representing a 3D model, composed of vertices and indices.
 *
 * This class also provides static methods to load meshes from obj files or to construct models algorithmically.
 */
class Model
{
public:
	/**
	 * @brief Static function to retrieve a model by name, optionally applying a loading transform to its vertices.
	 *
	 * @param name The name of the model to retrieve.
	 * @param loadTransform An optional transformation matrix to apply to the model's vertices.
	 * @return A reference to the loaded model.
	 */
	static Ref<Model> get(const String& name, const glm::mat4& loadTransform = glm::mat4(1));

	/**
	 * @brief Static function to create a plane model with the specified horizontal and vertical counts.
	 *
	 * @param hCount The number of horizontal segments.
	 * @param vCount The number of vertical segments.
	 * @return A reference to the plane model.
	 */
	static Ref<Model> getPlane(u32 hCount = 1, u32 vCount = 1);

	/**
	 * @brief Gets the vertex buffer associated with the model.
	 *
	 * @return A constant reference to the vertex buffer.
	 */
	inline const Buffer& getVertexBuffer() const { return m_VertexBuffer; }

	/**
	 * @brief Gets the index buffer associated with the model.
	 *
	 * @return A constant reference to the index buffer.
	 */
	inline const Buffer& getIndexBuffer() const { return m_IndexBuffer; }

	/**
	 * @brief Gets the number of indices in the model.
	 *
	 * @return The number of indices in the model.
	 */
	inline u32 getIndexCount() const { return m_IndexCount; }

	friend class Renderer;
private:
	/**
	* @brief Constructor for the `Model` class.
	*
	* @param vertices A vector containing the vertices of the model.
	* @param indices A vector containing the indices of the model.
	*/
	Model(std::vector<Vertex> vertices, std::vector<u32> indices);

	Buffer m_VertexBuffer;
	Buffer m_IndexBuffer;
	u32 m_IndexCount = 0;

	/**
	 * @brief Static function to initialize resources and prepare for loading models.
	 *
	 * @return True if initialization is successful; otherwise, false.
	 */
	static bool init();

	/**
	 * @brief Static function to clean up and release resources.
	 */
	static void cleanup();

	static std::unordered_map<String, WRef<Model>> s_Models;
	static Ref<Model> s_Default;

	static void makeDefaultModel();
};

} // namespace vulture
