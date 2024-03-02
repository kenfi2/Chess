#ifndef SPRITES_H
#define SPRITES_H

class Image {
public:
	Image(int width, int height) : m_width(width), m_height(height) {
		m_data.resize(width * height * 4, 0);
	}

	void Fill(uint8_t* data) {
		memcpy(m_data.data(), data, m_data.size());
	}

	const std::string& GetName() const { return m_name; }
	void SetName(const std::string& name) { m_name = name; }

	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }

	const uint8_t* Data() const { return m_data.data(); }

	void GaussianBlur();
	void ToGrayScale();
	std::vector<wxPoint> CannyFilter(double lowerThreshold, double higherThreshold) const;

private:
	int m_width = 0;
	int m_height = 0;

	std::vector<uint8_t> m_data;
	std::string m_name;
};

class Texture {
public:
	Texture(GLuint textureId, const std::string& name) : m_id(textureId), m_name(name) {}
	~Texture();

	GLuint GetId() const { return m_id; }
	const std::string& GetName() const { return m_name; }

	void Bind();
	void DrawPoints(const wxRect2DDouble& rect);
	void Fill(const Image& image);

	bool IsInside(const wxPoint& point, const wxRect2DDouble& rect);

private:
	GLuint m_id;
	int m_width = 0;
	int m_height = 0;

	std::string m_name;
	std::vector<bool> m_area;
};

class Sprites {
public:
	Sprites() = default;
	~Sprites();

	Texture* Load(const std::string& file);
	Texture* GetTexture(const std::string& name);

private:
	std::vector<Texture*> m_textures;
};

extern Sprites g_sprites;

#endif
