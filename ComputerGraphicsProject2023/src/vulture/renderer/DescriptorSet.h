#pragma once

#include "Buffers.h"
#include "Texture.h"

#include <vector>
#include <unordered_set>
#include <optional>

namespace vulture {

class DescriptorSetLayout
{
public:
	NO_COPY(DescriptorSetLayout)

	DescriptorSetLayout() = default;

	void addBinding(VkDescriptorType type, VkShaderStageFlags target, u32 count = 1);
	bool create();

	inline VkDescriptorSetLayout getHandle() const { return m_Handle; }
	inline const auto& getBindings() const { return m_Bindings; }

	~DescriptorSetLayout();
private:
	VkDescriptorSetLayout m_Handle = VK_NULL_HANDLE;
	std::vector<VkDescriptorSetLayoutBinding> m_Bindings;
};

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

	VkWriteDescriptorSet getWriteDescriptorSet(VkDescriptorSet descriptorSet, u32 index, u32 binding) const;

	void map(u32 index) const;

	~DescriptorWrite() = default;
private:
	std::vector<Buffer> const* m_UniformBuffers;
	std::vector<VkDescriptorBufferInfo> m_UniformInfos;
	std::optional<VkDescriptorImageInfo> m_TextureInfo;
};

/*
*/
class DescriptorPool
{
public:
	NO_COPY(DescriptorPool)

	/*
	* @brief Constructor of a DescriptorPool
	*
	* @param device - The logical device on witch the pool will be allocated.
	* @param frameCount - The number of framebuffers.
	*/
	explicit DescriptorPool(u32 frameCount);

	inline u32 getFrameCount() const { return m_FrameCount; }
	void setFrameCount(u32 frameCount);

	inline VkDescriptorPool getHandle() const { return m_Handle; }

	void reserveSpace(u32 count, const DescriptorSetLayout& layout);

	Ref<DescriptorSet> getDescriptorSet(const DescriptorSetLayout& layout, const std::vector<DescriptorWrite>& descriptorWrites);

	~DescriptorPool();

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

class DescriptorSet
{
public:
	NO_COPY(DescriptorSet)

	inline const DescriptorSetLayout& getLayout() const { return *m_Layout; }

	inline VkDescriptorSet getHandle(u32 index) const { return m_Handles[index]; }

	void map(u32 index) const;

	~DescriptorSet();
	friend class DescriptorPool;
private:
	DescriptorSet(DescriptorPool& pool, const DescriptorSetLayout& layout, const std::vector<DescriptorWrite>& descriptorWrites);

	bool create();
	void cleanup();
	void recreate();

	std::vector<VkDescriptorSet> m_Handles;
	DescriptorPool* m_Pool;
	DescriptorSetLayout const* m_Layout;
	std::vector<DescriptorWrite> m_DescriptorWrites;
};

} // namespace vulture
