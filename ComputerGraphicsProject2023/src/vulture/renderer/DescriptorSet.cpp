#include "DescriptorSet.h"

#define VU_LOGGER_DISABLE_INFO
#include "vulture/core/Logger.h"
#include "VulkanContext.h"

#define ASSERT_VK_SUCCESS(func, message)   \
	if (func != VK_SUCCESS)                \
	{                                      \
		VUERROR(message);                  \
		return false;                      \
	}

namespace vulture {

extern VulkanContextData vulkanData;

void DescriptorSetLayout::addBinding(VkDescriptorType type, VkShaderStageFlags target, u32 count)
{
	VkDescriptorSetLayoutBinding binding{};
	binding.binding = static_cast<uint32_t>(m_Bindings.size());
	binding.descriptorType = type;
	binding.descriptorCount = count;
	binding.stageFlags = target;
	binding.pImmutableSamplers = nullptr; // Optional
	m_Bindings.push_back(binding);
}

bool DescriptorSetLayout::create()
{
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(m_Bindings.size());
	layoutInfo.pBindings = m_Bindings.data();

	VkResult result = vkCreateDescriptorSetLayout(vulkanData.device, &layoutInfo, vulkanData.allocator, &m_Handle);
	if (result != VK_SUCCESS)
	{
		VUERROR("Failed to create descriptor set layout!");
		return false;
	}
	return true;
}

DescriptorSetLayout::~DescriptorSetLayout()
{
	if (m_Handle != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorSetLayout(vulkanData.device, m_Handle, vulkanData.allocator);
		m_Handle = VK_NULL_HANDLE;
	}
}

VkWriteDescriptorSet DescriptorWrite::getWriteDescriptorSet(VkDescriptorSet descriptorSet, u32 index, u32 binding) const
{
	VkWriteDescriptorSet write{};

	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.dstSet = descriptorSet;
	write.dstBinding = binding;
	write.dstArrayElement = 0;
	if (m_TextureInfo)
	{
		write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		write.descriptorCount = 1;
		write.pImageInfo = &(*m_TextureInfo);
	}
	else
	{
		write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		write.descriptorCount = 1;
		write.pBufferInfo = &m_UniformInfos[index];
	}

	return write;
}

void DescriptorWrite::map(u32 index) const
{
	if (m_UniformBuffers != nullptr)
	{
		(*m_UniformBuffers)[index].map();
	}
}

DescriptorSet::DescriptorSet(DescriptorPool& pool, Ref<DescriptorSetLayout> layout, const std::vector<DescriptorWrite>& descriptorWrites)
	: m_Pool(&pool), m_Layout(layout), m_DescriptorWrites(descriptorWrites)
{
	create();
}

bool DescriptorSet::create()
{
	std::vector<VkDescriptorSetLayout> layouts(m_Pool->getFrameCount(), m_Layout->getHandle());
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = m_Pool->getHandle();
	allocInfo.descriptorSetCount = static_cast<u32>(layouts.size());
	allocInfo.pSetLayouts = layouts.data();

	m_Handles.resize(layouts.size());
	ASSERT_VK_SUCCESS(vkAllocateDescriptorSets(vulkanData.device, &allocInfo, m_Handles.data()),
		"Failed to allocate descriptor sets!");

	for (u64 i = 0; i < m_Handles.size(); i++)
	{
		std::vector<VkWriteDescriptorSet> writes;
		writes.reserve(m_DescriptorWrites.size());

		for (u32 binding = 0; binding < m_DescriptorWrites.size(); binding++)
		{
			writes.push_back(m_DescriptorWrites[binding].getWriteDescriptorSet(m_Handles[i], static_cast<u32>(i), binding));
		}

		vkUpdateDescriptorSets(vulkanData.device, static_cast<u32>(writes.size()), writes.data(), 0, nullptr);
	}

#ifdef VU_LOGGER_INFO_ENABLED
	for (u64 i = 0; i < m_Handles.size(); i++)
	{
		VUINFO("Descriptor set [%p] created from the pool [%p]", m_Handles[i], m_Pool->getHandle());
	}
#endif

	return true;
}

void DescriptorSet::cleanup()
{
	vkFreeDescriptorSets(vulkanData.device, m_Pool->getHandle(), static_cast<u32>(m_Handles.size()), m_Handles.data());
}

void DescriptorSet::recreate()
{
	m_Handles.resize(0);
	create();
}

void DescriptorSet::map(u32 index) const
{
	for (auto& dw : m_DescriptorWrites)
	{
		dw.map(index);
	}
}

DescriptorSet::~DescriptorSet()
{
	m_Pool->cleanupDescriptorSet(m_Layout->getBindings());
	cleanup();
}


DescriptorPool::DescriptorPool(u32 frameCount)
	: m_FrameCount(frameCount)
{
}

void DescriptorPool::setFrameCount(u32 frameCount)
{
	if (m_FrameCount == frameCount)
		return;

	m_FrameCount = frameCount;
	recreate();
}

void DescriptorPool::reserveSpace(u32 count, Ref<DescriptorSetLayout> layout)
{
	m_Size += count;

	auto& layoutBindings = layout->getBindings();
	for (auto& binding : layoutBindings)
	{
		auto it = m_TypeInfos.find(binding.descriptorType);
		if (it != m_TypeInfos.end())
		{
			auto& [size, space] = it->second;
			space += count;
		}
		else
		{
			m_TypeInfos.insert({ binding.descriptorType, {0, count} });
		}
	}

	recreate();
}

Ref<DescriptorSet> DescriptorPool::getDescriptorSet(Ref<DescriptorSetLayout> layout, const std::vector<DescriptorWrite>& descriptorWrites)
{
	bool shouldRecreate = false;
	for (auto it = m_Sets.begin(); it != m_Sets.end();)
	{
		if (it->use_count() <= 1)
		{
			it = m_Sets.erase(it);
		}
		else
			it++;
	}
	if (m_Size <= m_Sets.size())
	{
		shouldRecreate = true;
		m_Size++;
	}

	auto& layoutBindings = layout->getBindings();
	for (auto& binding : layoutBindings)
	{
		auto it = m_TypeInfos.find(binding.descriptorType);
		if (it != m_TypeInfos.end())
		{
			auto& [size, count] = it->second;
			count++;
			if (size < count)
				shouldRecreate = true;
		}
		else
		{
			m_TypeInfos.insert({ binding.descriptorType, {0, 1} });
			shouldRecreate = true;
		}
	}

	if (shouldRecreate)
		recreate();

	Ref<DescriptorSet> descriprtorSet(new DescriptorSet(*this, layout, descriptorWrites));

	m_Sets.insert(descriprtorSet);

	return descriprtorSet;
}

void DescriptorPool::cleanup()
{
	if (m_Handle != VK_NULL_HANDLE)
	{
		vkDeviceWaitIdle(vulkanData.device);
		vkDestroyDescriptorPool(vulkanData.device, m_Handle, vulkanData.allocator);
	}
}

bool DescriptorPool::recreate()
{
	cleanup();
	vkQueueWaitIdle(vulkanData.graphicsQueue);
	std::vector<VkDescriptorPoolSize> poolSizes{};
	poolSizes.reserve(m_TypeInfos.size());

	for (auto& [type, info] : m_TypeInfos)
	{
		info.size = info.count;
		poolSizes.push_back({ type, info.size * m_FrameCount });
	}

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	poolInfo.poolSizeCount = static_cast<u32>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = m_Size * m_FrameCount;

	ASSERT_VK_SUCCESS(vkCreateDescriptorPool(vulkanData.device, &poolInfo, vulkanData.allocator, &m_Handle),
		"Failed to create descriptor pool!");

	for (auto& set : m_Sets)
	{
		if (set.use_count() > 1)
		{
			set->recreate();
		}
	}

	return true;
}

DescriptorPool::~DescriptorPool()
{
	m_Sets.clear();
	cleanup();
}

void DescriptorPool::cleanupDescriptorSet(const std::vector<VkDescriptorSetLayoutBinding>& bindings)
{
	for (auto& binding : bindings)
	{
		auto it = m_TypeInfos.find(binding.descriptorType);
		if (it != m_TypeInfos.end())
		{
			it->second.count--;
		}
	}
}

} // namespace vulture
