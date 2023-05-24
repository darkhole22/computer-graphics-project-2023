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

bool checkDescriptorSetCreation(VkResult result)
{
	switch (result)
	{
	case VK_ERROR_FRAGMENTED_POOL:
	case VK_ERROR_OUT_OF_POOL_MEMORY:
	{
		throw std::exception();
	}
	case VK_SUCCESS:
	break;
	default:
	{
		VUERROR("Failed to reallocate Descriptor Set!");
		return false;
	}
	}
	return true;
}

DescriptorSet::DescriptorSet(DescriptorPool& pool, Ref<DescriptorSetLayout> layout, const std::vector<DescriptorWrite>& descriptorWrites)
	: m_Pool(&pool), m_Layout(layout), m_DescriptorWrites(descriptorWrites)
{
	auto result = create();
	checkDescriptorSetCreation(result);
}

VkResult DescriptorSet::create()
{
	std::vector<VkDescriptorSetLayout> layouts(m_Pool->getFrameCount(), m_Layout->getHandle());
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = m_Pool->getHandle();
	allocInfo.descriptorSetCount = static_cast<u32>(layouts.size());
	allocInfo.pSetLayouts = layouts.data();

	m_Handles.resize(layouts.size());
	auto result = vkAllocateDescriptorSets(vulkanData.device, &allocInfo, m_Handles.data());

	if (result != VK_SUCCESS)
	{
		// VUERROR("Failed to allocate descriptor sets!");
		return result;
	};

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

	return VK_SUCCESS;
}

void DescriptorSet::cleanup()
{
	vkFreeDescriptorSets(vulkanData.device, m_Pool->getHandle(), static_cast<u32>(m_Handles.size()), m_Handles.data());
}

VkResult DescriptorSet::recreate()
{
	m_Handles.resize(0);
	return create();
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
{}

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

	Ref<DescriptorSet> descriprtorSet;

	do
	{
		try
		{
			descriprtorSet = Ref<DescriptorSet>(new DescriptorSet(*this, layout, descriptorWrites));
			break;
		}
		catch (const std::exception&)
		{
			recreate();
		}
	} while (true);

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
	do
	{
		try
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
					if (!checkDescriptorSetCreation(set->recreate()))
					{
						return false;
					}
				}
			}

			return true;
		}
		catch (std::exception&)
		{
		}
	} while (true);
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
