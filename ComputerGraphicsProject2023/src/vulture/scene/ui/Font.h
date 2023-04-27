#pragma once

#include "vulture/renderer/Renderer.h"

namespace vulture {

class Font
{
public:
	struct CharacterMapping
	{
		int32_t codepoint;
		uint16_t x;
		uint16_t y;
		uint16_t width;
		uint16_t height;
		int16_t xOffset;
		int16_t yOffset;
		int16_t xAdvance;
	};

	struct Kerning
	{
		int32_t first;
		int32_t second;
		int16_t amount;
	};

	static Ref<Font> getDefault()
	{
		if (s_DefaultFont.expired())
		{
			auto font = Ref<Font>(new Font("FiraCode"));
			s_DefaultFont = font;
			return font;
		}
		return s_DefaultFont.lock();
	}
	
	Font(const std::string& name);

	inline const Texture& getTexture() const { return *m_Texture; }

	const CharacterMapping& getCharacterMapping(int32_t code) const;
	const Kerning* getKerning(int32_t code1, int32_t code2) const;
	inline float getCharacterSize() const { return static_cast<float>(m_CharacterSize); }
	inline int32_t getLineHeight() const { return m_LineHeight; }
	inline uint32_t getAtlasWidth() const { return m_TextureWidth; }
	inline uint32_t getAtlasHeight() const { return m_TextureHeight; }
	inline float getTabXAdvance() const { return m_TabXAdvance; }

	friend class UIHandler;
private:
	static WRef<Font> s_DefaultFont;

	Ref<Texture> m_Texture;

	uint32_t m_CharacterSize;
	int32_t m_LineHeight;
	uint32_t m_TextureWidth;
	uint32_t m_TextureHeight;
	std::unordered_map<int32_t, CharacterMapping> m_CharacterSet;
	std::unordered_map<uint64_t, Kerning> m_KerningSet;
	float m_TabXAdvance;

	bool loadFnt(const std::string& path);
};

} // namespace vulture
