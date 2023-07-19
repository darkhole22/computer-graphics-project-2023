#pragma once

#include "Buffers.h"
#include "Texture.h"

#include <vector>
#include <unordered_set>
#include <optional>

namespace vulture {

/**
* @class DescriptorSetLayout
*
* @brief Represents a Vulkan descriptor set layout used for describing descriptor sets.
*/
class DescriptorSetLayout
{
public:
	NO_COPY(DescriptorSetLayout)

	DescriptorSetLayout() = default;

	/**
	 * @brief Adds a binding to the descriptor set layout.
	 *
	 * @param type The type of descriptor (e.g., uniform buffer, combined image sampler).
	 * @param target The shader stage that the descriptor is intended for (e.g., vertex shader, fragment shader).
	 * @param count The number of descriptors in the binding (default is 1).
	 */
	void addBinding(VkDescriptorType type, VkShaderStageFlags target, u32 count = 1);

	/**
	 * @brief Creates the descriptor set layout.
	 *
	 * @return True if the descriptor set layout is successfully created; otherwise, false.
	 */
	bool create();

	/**
	 * @brief Gets the Vulkan handle of the descriptor set layout.
	 *
	 * @return The Vulkan handle of the descriptor set layout.
	 */
	inline VkDescriptorSetLayout getHandle() const { return m_Handle; }

	/**
	 * @brief Gets the bindings of the descriptor set layout.
	 *
	 * @return A constant reference to the vector of descriptor set layout bindings.
	 */
	inline const auto& getBindings() const { return m_Bindings; }

	~DescriptorSetLayout();
private:
	VkDescriptorSetLayout m_Handle = VK_NULL_HANDLE;
	std::vector<VkDescriptorSetLayoutBinding> m_Bindings;
};

/**
 * @class Uniform
 *
 * @brief Represents a uniform buffer used for descriptor sets in a rendering pipeline.
 *        Templated class to allow different types of uniform data.
 */
template <class Type>
class Uniform
{
public:
	Uniform() = default;
	Uniform(const Uniform& other) = delete;
	Uniform(Uniform&& other)
	{
		m_Buffers = std::move(other.m_Buffers);
		m_LocalData = other.m_LocalData;

		other.m_LocalData = nullptr;
	}

	Uniform(u32 count)
	{
		m_LocalData = new Type();
		m_Buffers.reserve(count);

		for (u64 i = 0; i < count; i++)
		{
			m_Buffers.emplace_back(sizeof(Type),
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				m_LocalData);
		}
	}

	inline const std::vector<Buffer>* getBuffers() const { return &m_Buffers; }

	inline Type* operator->() noexcept { return m_LocalData; }
	inline const Type* operator->() const noexcept { return m_LocalData; }

	Uniform& operator=(const Uniform& other) = delete;
	Uniform& operator=(Uniform&& other) noexcept {
		if (m_Buffers.size() != other.m_Buffers.size())
		{
			delete m_LocalData;

			m_Buffers = std::move(other.m_Buffers);
			m_LocalData = other.m_LocalData;

			other.m_LocalData = nullptr;
		}
		return *this;
	}

	~Uniform()
	{
		delete m_LocalData;
	}
private:
	std::vector<Buffer> m_Buffers;

	Type* m_LocalData = nullptr;
};

class DescriptorWrite
{
public:
	template <class T>
	inline DescriptorWrite(const Uniform<T>& uniform)
	{
		m_UniformBuffers = uniform.getBuffers();

		m_UniformInfos.reserve(m_UniformBuffers->size());
		for (auto& buffer : (*m_UniformBuffers))
		{
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = buffer.getHandle();
			bufferInfo.offset = 0;
			bufferInfo.range = buffer.getSize();
			m_UniformInfos.push_back(bufferInfo);
		}
	}

	inline DescriptorWrite(const TextureSampler& sampler) :
		m_UniformBuffers(nullptr)
	{
		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = sampler.getLayout();
		imageInfo.imageView = sampler.getView();
		imageInfo.sampler = sampler.getHandle();

		m_TextureInfo = imageInfo;
	}

	/**
	 * @brief Gets the Vulkan write descriptor set structure for the descriptor set update.
	 *
	 * @param descriptorSet The Vulkan handle of the descriptor set to be updated.
	 * @param index The index of the uniform buffer (frame) associated with the descriptor set.
	 * @param binding The binding index of the descriptor set layout to be updated.
	 * @return The Vulkan write descriptor set structure for the descriptor set update.
	 */
	VkWriteDescriptorSet getWriteDescriptorSet(VkDescriptorSet descriptorSet, u32 index, u32 binding) const;

	/**
	 * @brief Maps the uniform buffer or texture sampler associated with the descriptor set.
	 *
	 * @param index The index of the uniform buffer (frame) associated with the descriptor set.
	 */
	void map(u32 index) const;

	~DescriptorWrite() = default;
private:
	std::vector<Buffer> const* m_UniformBuffers;
	std::vector<VkDescriptorBufferInfo> m_UniformInfos;
	std::optional<VkDescriptorImageInfo> m_TextureInfo;
};

/**
 * @class DescriptorPool
 *
 * @brief Represents a Vulkan descriptor pool used for allocating descriptor sets.
 */
class DescriptorPool
{
public:
	NO_COPY(DescriptorPool)

	/**
	* @brief Constructor of a DescriptorPool
	*
	* @param frameCount The number of framebuffers.
	*/
	explicit DescriptorPool(u32 frameCount);

	/**
	 * @brief Gets the number of framebuffers associated with the descriptor pool.
	 *
	 * @return The number of framebuffers.
	 */
	inline u32 getFrameCount() const { return m_FrameCount; }

	/**
	 * @brief Sets the number of framebuffers associated with the descriptor pool.
	 *
	 * @param frameCount The number of framebuffers to set.
	 */
	void setFrameCount(u32 frameCount);

	/**
	 * @brief Gets the Vulkan handle of the descriptor pool.
	 *
	 * @return The Vulkan handle of the descriptor pool.
	 */
	inline VkDescriptorPool getHandle() const { return m_Handle; }

	/**
	 * @brief Reserves space in the descriptor pool for the specified number of descriptors with the given layout.
	 *
	 * @param count The number of descriptors to reserve space for.
	 * @param layout The descriptor set layout to use for the descriptors.
	 */
	void reserveSpace(u32 count, Ref<DescriptorSetLayout> layout);

	/**
	 * @brief Gets a descriptor set from the descriptor pool with the specified layout and descriptor writes.
	 *
	 * @param layout The descriptor set layout for the descriptor set.
	 * @param descriptorWrites The descriptor writes used to update the descriptor set with data.
	 * @return A reference to the created DescriptorSet instance.
	 */
	Ref<DescriptorSet> getDescriptorSet(Ref<DescriptorSetLayout> layout, const std::vector<DescriptorWrite>& descriptorWrites);

	~DescriptorPool();

	/**
	 * @struct DescriptorTypePoolInfo
	 *
	 * @brief Contains information about descriptor types in the descriptor pool.
	 */
	struct DescriptorTypePoolInfo
	{
		u32 size;
		u32 count;
	};

	friend class DescriptorSet;
private:
	VkDescriptorPool m_Handle = VK_NULL_HANDLE;
	u32 m_FrameCount = 0;
	u32 m_Size = 0;
	std::unordered_map<VkDescriptorType, DescriptorTypePoolInfo> m_TypeInfos;
	std::unordered_set<Ref<DescriptorSet>> m_Sets;

	void cleanupDescriptorSet(const std::vector<VkDescriptorSetLayoutBinding>& bindings);
	void cleanup();
	bool recreate();
};

/**
 * @class DescriptorSet
 *
 * @brief Represents a Vulkan descriptor set allocated from a descriptor pool.
 */
class DescriptorSet
{
public:
	NO_COPY(DescriptorSet)

	/**
	 * @brief Gets the descriptor set layout associated with the descriptor set.
	 *
	 * @return A constant reference to the DescriptorSetLayout instance.
	 */
	inline const DescriptorSetLayout& getLayout() const { return *m_Layout; }

	/**
	 * @brief Gets the Vulkan handle of the descriptor set at the specified index.
	 *
	 * @param index The index of the descriptor set in the handles array.
	 * @return The Vulkan handle of the descriptor set at the specified index.
	 */
	inline VkDescriptorSet getHandle(u32 index) const { return m_Handles[index]; }

	/**
	 * @brief Maps the descriptor set for the specified index to update its contents.
	 *
	 * @param index The index of the descriptor set to be mapped.
	 */
	void map(u32 index) const;

	~DescriptorSet();
	friend class DescriptorPool;
private:
	DescriptorSet(DescriptorPool& pool, Ref<DescriptorSetLayout> layout, const std::vector<DescriptorWrite>& descriptorWrites);

	VkResult create();
	void cleanup();
	VkResult recreate();

	std::vector<VkDescriptorSet> m_Handles;
	DescriptorPool* m_Pool;
	Ref<DescriptorSetLayout> m_Layout;
	std::vector<DescriptorWrite> m_DescriptorWrites;
};

} // namespace vulture
