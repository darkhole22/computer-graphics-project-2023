#include "Font.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <tuple>
#include <iostream>

namespace vulture {

bool getValue(std::string* pair, int32_t* value);
bool parseInfoLine(std::stringstream& stream, uint32_t* size);
bool parseCommonLine(std::stringstream& stream, int32_t* lineHeight, uint32_t* width, uint32_t* height);
bool parseCharLine(std::stringstream& stream, Font::CharacterMapping* c);
bool parseKerningLine(std::stringstream& stream, Font::Kerning* k);

#define KERNING_MAP_KEY(v1, v2) (static_cast<uint64_t>(v1) << 32 | static_cast<uint64_t>(v2))

constexpr int TAB_MULTIPLIER = 4;

WRef<Font> Font::s_DefaultFont{};

Font::Font(const std::string& name)
{
	std::string texturePath = "res/textures/" + name + ".png";
	std::string fontPath = "res/fonts/" + name + ".fnt";

	m_Texture = makeRef<Texture>(texturePath.c_str());

	if (!loadFnt(fontPath))
	{
		std::cerr << "Unable to load [" << fontPath << "]" << std::endl;
	}

	auto& space = getCharacterMapping(' ');
	if (space.codepoint == ' ')
	{
		m_TabXAdvance = static_cast<float>(space.xAdvance * TAB_MULTIPLIER);
	}
	else
	{
		CharacterMapping c;
		c.codepoint = ' ';
		c.x = 0;
		c.y = 0;
		c.width = 0;
		c.height = 0;
		c.xOffset = 0;
		c.yOffset = 0;
		c.xAdvance = m_CharacterSize;
		m_CharacterSet.insert({ c.codepoint , c });
		m_TabXAdvance = static_cast<float>(m_CharacterSize * TAB_MULTIPLIER);
	}
	auto& tab = getCharacterMapping('\t');
	if (tab.codepoint == '\t')
	{
		m_TabXAdvance = tab.xAdvance;
	}
}

const Font::CharacterMapping& Font::getCharacterMapping(int32_t code) const
{
	auto it = m_CharacterSet.find(code);

	if (it != m_CharacterSet.end())
	{
		return it->second;
	}

	it = m_CharacterSet.find(-1);
	return it->second;
}

const Font::Kerning* Font::getKerning(int32_t code1, int32_t code2) const
{
	auto it = m_KerningSet.find(KERNING_MAP_KEY(code1, code2));
	if (it != m_KerningSet.end())
	{
		return &it->second;
	}
	return nullptr;
}

bool Font::loadFnt(const std::string& path)
{
	std::ifstream file(path);

	if (!file.is_open()) return false;

	CharacterMapping c{};
	c.codepoint = -1;
	m_CharacterSet.insert({ c.codepoint, c });

	std::string lineToken;
	for (std::string line; getline(file, line); )
	{
		std::stringstream lineStream{ line };
		
		lineStream >> lineToken;

		if (lineToken == "kerning")
		{
			Kerning k;
			if (parseKerningLine(lineStream, &k))
			{
				m_KerningSet.insert({ KERNING_MAP_KEY(k.first, k.second) , k });
			} // TODO else Log
		}
		else if (lineToken == "char")
		{
			if (parseCharLine(lineStream, &c))
			{
				m_CharacterSet.insert({ c.codepoint, c });
				if (c.codepoint == '?')
				{
					m_CharacterSet.insert_or_assign(-1, c);
				}
			} // TODO else Log
		}
		else if (lineToken == "info") 
		{
			if (!parseInfoLine(lineStream, &m_CharacterSize)) return false;
		}
		else if (lineToken == "common")
		{
			if (!parseCommonLine(lineStream, &m_LineHeight, &m_TextureWidth, &m_TextureHeight)) return false;
		}
		// else if (lineToken == "page") { }
		else if (lineToken == "chars")
		{
			lineStream >> lineToken;
			int32_t value;
			if (!getValue(&lineToken, &value)) return false;
			
			if (lineToken != "count" || value <= 0) return false;
			
			m_CharacterSet.reserve(value);
		}
		else if (lineToken == "kernings")
		{
			lineStream >> lineToken;
			int32_t value;
			if (!getValue(&lineToken, &value)) return false;

			if (lineToken != "count" || value < 0) return false;

			m_KerningSet.reserve(value);
		}
	}

	return true;
}

bool parseInfoLine(std::stringstream& stream, uint32_t* size)
{
	std::string token;
	size_t quotes = 0;
	while (stream >> token)
	{
		quotes += std::count(token.begin(), token.end(), '\"');
		if (quotes % 2 != 0) continue;
		
		int32_t value;
		if (!getValue(&token, &value)) continue;

		if (token == "size" && value > 0)
		{
			*size = static_cast<uint32_t>(value);
			return true;
		}
	}
	return false;
}

bool parseCommonLine(std::stringstream& stream, int32_t* lineHeight, uint32_t* width, uint32_t* height)
{
	std::string token;
	uint32_t quotes = 0;
	bool lh = false, w = false, h = false;
	while (stream >> token)
	{
		int32_t value;
		if (!getValue(&token, &value)) return false;

		if (token == "lineHeight")
		{
			*lineHeight = value;
			lh = true;
		}
		else if (token == "scaleW" && value > 0)
		{
			*width = static_cast<uint32_t>(value);
			w = true;
		}
		else if (token == "scaleH" && value > 0)
		{
			*height = static_cast<uint32_t>(value);
			h = true;
		}
		if (lh && w && h) return true;
	}
	return false;
}

bool parseCharLine(std::stringstream& stream, Font::CharacterMapping* c)
{
	std::string token;
	int32_t value;

	if (!(stream >> token)) return false;
	if (!getValue(&token, &value)) return false;
	if (token != "id") return false;
	c->codepoint = value;

	if (!(stream >> token)) return false;
	if (!getValue(&token, &value)) return false;
	if (token != "x" || value < 0) return false;
	c->x = value;

	if (!(stream >> token)) return false;
	if (!getValue(&token, &value)) return false;
	if (token != "y" || value < 0) return false;
	c->y = value;

	if (!(stream >> token)) return false;
	if (!getValue(&token, &value)) return false;
	if (token != "width" || value < 0) return false;
	c->width = value;

	if (!(stream >> token)) return false;
	if (!getValue(&token, &value)) return false;
	if (token != "height" || value < 0) return false;
	c->height = value;

	if (!(stream >> token)) return false;
	if (!getValue(&token, &value)) return false;
	if (token != "xoffset") return false;
	c->xOffset = value;

	if (!(stream >> token)) return false;
	if (!getValue(&token, &value)) return false;
	if (token != "yoffset") return false;
	c->yOffset = value;

	if (!(stream >> token)) return false;
	if (!getValue(&token, &value)) return false;
	if (token != "xadvance") return false;
	c->xAdvance = value;

	return true;
}

bool parseKerningLine(std::stringstream& stream, Font::Kerning* k)
{
	std::string token;
	if (!(stream >> token)) return false;

	int32_t value;
	if (!getValue(&token, &value)) return false;
	if (token != "first") return false;
	k->first = value;

	if (!(stream >> token)) return false;
	if (!getValue(&token, &value)) return false;
	if (token != "second") return false;
	k->second = value;

	if (!(stream >> token)) return false;
	if (!getValue(&token, &value)) return false;
	if (token != "amount") return false;
	k->amount = value;

	return true;
}

bool getValue(std::string* pair, int32_t* value)
{
	size_t eqPos = pair->find('=');
	if (eqPos >= pair->length()) return false;
	std::string v = pair->substr(eqPos + 1);
	try
	{
		*value = std::stoi(v);
	}
	catch (const std::invalid_argument& ex)
	{
		std::cerr << ex.what() << std::endl;
		return false;
	}
	*pair = pair->substr(0, eqPos);
	return true;
}

} // namespace vulture
