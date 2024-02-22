#include <GL/glew.h>

#include "main.h"

#include "sprites.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void Image::GaussianBlur()
{
    static std::vector<std::vector<double>> kernel = { {2.0, 4.0, 5.0, 4.0, 2.0},
                                               {4.0, 9.0, 12.0, 9.0, 4.0},
                                               {5.0, 12.0, 15.0, 12.0, 5.0},
                                               {4.0, 9.0, 12.0, 9.0, 4.0},
                                               {2.0, 4.0, 5.0, 4.0, 2.0} };
    static double kernelConst = (1.0 / 159.0);

    const int sizeDepth = 4;
    const int bytesPerLine = m_width * sizeDepth;
    std::vector<uint8_t> pixelsBlur(m_width * m_height * sizeDepth);
    for(int j = 0; j < m_height; ++j) {
        for(int i = 0; i < m_width; ++i) {
            for(int k = 0; k < sizeDepth; k++) {
                double sum = 0;
                double sumKernel = 0;
                for(int y = -2; y <= 2; y++) {
                    for(int x = -2; x <= 2; x++) {
                        if((i + x) >= 0 && (i + x) < m_width && (j + y) >= 0 && (j + y) < m_height) {
                            double channel = (double)m_data[((j + y) * bytesPerLine) + (i + x) * sizeDepth + k];
                            sum += channel * kernelConst * kernel[x + 2][y + 2];
                            sumKernel += kernelConst * kernel[x + 2][y + 2];
                        }
                    }
                }
                pixelsBlur[(j * bytesPerLine) + i * sizeDepth + k] = (int)(sum / sumKernel);
            }
        }
    }

    m_data = pixelsBlur;
}

void Image::ToGrayScale()
{
    for(int i = 0; i < m_width * m_height; ++i) {
        double r = (double)m_data[i * 4 + 0];
        double g = (double)m_data[i * 4 + 1];
        double b = (double)m_data[i * 4 + 2];

        r *= 0.299;
        g *= 0.587;
        b *= 0.144;

        uint8_t gray = floor(r + g + b + 0.5);

        m_data[i * 4 + 0] = gray;
        m_data[i * 4 + 1] = gray;
        m_data[i * 4 + 2] = gray;
    }
}

std::vector<wxPoint> Image::CannyFilter(double lowerThreshold, double higherThreshold) const
{
    std::vector<uint8_t> copyData = m_data;
    for(int i = 0; i < m_width * m_height; ++i) {
        if(copyData[i * 4 + 3] == 0)
            continue;

        copyData[i * 4 + 0] = 255;
        copyData[i * 4 + 1] = 255;
        copyData[i * 4 + 2] = 255;
    }

    std::vector<int> canny(m_width * m_height);

    int gx[3][3] = { {-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1} };
    int gy[3][3] = { {-1, -2, -1}, {0, 0, 0}, {1, 2, 1} };

    std::vector<double> G(m_width * m_height);
    std::vector<int> theta(m_width * m_height);
    double largestG = 0;

    const int sizeDepth = 4;
    const int bytesPerLine = m_width * sizeDepth;
    // perform canny edge detection on everything but the edges
    for(int j = 1; j < m_height - 1; j++) {
        for(int i = 1; i < m_width - 1; i++) {
            // find gx and gy for each pixel
            double gxValue = 0;
            double gyValue = 0;
            for(int x = -1; x <= 1; x++) {
                for(int y = -1; y <= 1; y++) {
                    gxValue += (gx[1 - x][1 - y] * (double)(copyData[((j + y) * bytesPerLine) + (x + i) * sizeDepth]));
                    gyValue += (gy[1 - x][1 - y] * (double)(copyData[((j + y) * bytesPerLine) + (x + i) * sizeDepth]));
                }
            }

            // calculate G and theta
            G[j * m_width + i] = std::sqrt(std::pow(gxValue, 2) + std::pow(gyValue, 2));
            double atanResult = atan2(gyValue, gxValue) * 180.0 / 3.14159265;
            theta[j * m_width + i] = (int)(180.0 + atanResult);

            if(G[j * m_width + i] > largestG) { largestG = G[j * m_width + i]; }

            // setting the edges
            if(i == 1) {
                G[j * m_width + i - 1] = G[j * m_width + i];
                theta[j * m_width + i - 1] = theta[j * m_width + i];
            }
            else if(j == 1) {
                G[(i - 1) * m_height + j] = G[j * m_width + i];
                theta[(i - 1) * m_height + j] = theta[j * m_width + i];
            }
            else if(i == m_width - 1) {
                G[j * m_width + i + 1] = G[j * m_width + i];
                theta[j * m_width + i + 1] = theta[j * m_width + i];
            }
            else if(j == m_height - 1) {
                G[(i + 1) * m_height + j] = G[j * m_width + i];
                theta[(i + 1) * m_height + j] = theta[j * m_width + i];
            }

            // setting the corners
            if(i == 1 && j == 1) {
                G[(i - 1) * m_height + j - 1] = G[j * m_width + i];
                theta[(i - 1) * m_height + j - 1] = theta[j * m_width + i];
            }
            else if(i == 1 && j == m_height - 1) {
                G[(i - 1) * m_height + j + 1] = G[j * m_width + i];
                theta[(i - 1) * m_height + j + 1] = theta[j * m_width + i];
            }
            else if(i == m_width - 1 && j == 1) {
                G[(i + 1) * m_height + j - 1] = G[j * m_width + i];
                theta[(i + 1) * m_height + j - 1] = theta[j * m_width + i];
            }
            else if(i == m_width - 1 && j == m_height - 1) {
                G[(i + 1) * m_height + j + 1] = G[j * m_width + i];
                theta[(i + 1) * m_height + j + 1] = theta[j * m_width + i];
            }

            // round to the nearest 45 degrees
            theta[j * m_width + i] = round(theta[j * m_width + i] / 45) * 45;
        }
    }

    // non-maximum suppression
    for(int j = 1; j < m_height - 1; j++) {
        for(int i = 1; i < m_width - 1; i++) {
            if(theta[j * m_width + i] == 0 || theta[j * m_width + i] == 180) {
                if(G[j * m_width + i] < G[j * m_width + i - 1] || G[j * m_width + i] < G[j * m_width + i + 1]) {
                    G[j * m_width + i] = 0;
                }
            }
            else if(theta[j * m_width + i] == 45 || theta[j * m_width + i] == 225) {
                if(G[j * m_width + i] < G[(i + 1) * m_height + j + 1] || G[j * m_width + i] < G[(i - 1) * m_height + j - 1]) {
                    G[j * m_width + i] = 0;
                }
            }
            else if(theta[j * m_width + i] == 90 || theta[j * m_width + i] == 270) {
                if(G[j * m_width + i] < G[(i + 1) * m_height + j] || G[j * m_width + i] < G[(i - 1) * m_height + j]) {
                    G[j * m_width + i] = 0;
                }
            }
            else {
                if(G[j * m_width + i] < G[(i + 1) * m_height + j - 1] || G[j * m_width + i] < G[(i - 1) * m_height + j + 1]) {
                    G[j * m_width + i] = 0;
                }
            }

            canny[j * m_width + i] = (int)(G[j * m_width + i] * (255.0 / largestG));
        }
    }

    // double threshold
    bool changes;
    do {
        changes = false;
        for(int i = 1; i < m_width - 1; i++) {
            for(int j = 1; j < m_height - 1; j++) {
                if(G[j * m_width + i] < (lowerThreshold * largestG)) {
                    G[j * m_width + i] = 0;
                }
                else if(G[j * m_width + i] >= (higherThreshold * largestG)) {
                    continue;
                }
                else if(G[j * m_width + i] < (higherThreshold * largestG)) {
                    int tempG = G[j * m_width + i];
                    G[j * m_width + i] = 0;
                    for(int x = -1; x <= 1; x++) {
                        bool breakNestedLoop = false;
                        for(int y = -1; y <= 1; y++) {
                            if(x == 0 && y == 0) { continue; }
                            if(G[(i + x) * m_height + (j + y)] >= (higherThreshold * largestG)) {
                                G[j * m_width + i] = (higherThreshold * largestG);
                                changes = true;
                                breakNestedLoop = true;
                                break;
                            }
                        }
                        if(breakNestedLoop) { break; }
                    }
                }
                canny[j * m_width + i] = (int)(G[j * m_width + i] * (255.0 / largestG));
            }
        }
    } while(changes);

    std::vector<wxPoint> borderPoints;
    for(int j = 0; j < m_height; j++) {
        for(int i = 0; i < m_width; i++) {
            uint8_t v = (uint8_t)canny[j * m_width + i];
            if(v != 0)
                borderPoints.push_back(wxPoint(i, j));
        }
    }
    return borderPoints;
}

Sprites g_sprites;

Sprites::~Sprites()
{
    for(Texture* texture : m_textures)
        delete texture;
}

Texture* Sprites::Load(const std::string& file)
{
    static const char* SPRITES_DATA = "images/\0";
    std::string dataFile = SPRITES_DATA + file;

    int width, height, channels;
    uint8_t* data = stbi_load(dataFile.c_str(), &width, &height, &channels, 4);

    Image image(width, height);
    image.Fill(data);
    image.SetName(file);

    stbi_image_free(data);

    GLuint textureId = 0;
    glGenTextures(1, &textureId);

    Texture* texture = new Texture(textureId, dataFile);
    texture->Fill(image);

    m_textures.push_back(texture);
    return texture;
}

Texture* Sprites::GetTexture(const std::string& name)
{
    for(Texture* texture : m_textures) {
        if(texture->GetName() == name)
            return texture;
    }
    return nullptr;
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_id);
}

void Texture::Bind()
{
    glBindTexture(GL_TEXTURE_2D, m_id);
}

void Texture::DrawPoints(const wxRect2DDouble& rect)
{
    /*glColor4ub(uint8_t(255), uint8_t(255), uint8_t(255), uint8_t(0xAA));
    glBegin(GL_POINTS);
    for(int i = 0; i < m_area.size(); ++i) {
        if(!m_area[i])
            continue;

        double x = (double)(i % m_width) * (double)rect.m_width / (double)m_width;
        double y = (double)(i / m_width) * (double)rect.m_height / (double)m_height;

        glVertex2d((double)rect.m_x + x, (double)rect.m_y + y);
    }
    glEnd();*/
}

void Texture::Fill(const Image& image)
{
    m_width = image.GetWidth();
    m_height = image.GetHeight();

    const uint8_t* const data = image.Data();

    glBindTexture(GL_TEXTURE_2D, m_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // GL_CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // GL_CLAMP_TO_EDGE

    m_area.resize(m_width * m_height, false);
    
    for(int y = 0; y < m_height; ++y) {
        for(int x = 0; x < m_width; ++x) {
            uint8_t alpha = data[y * m_width * 4 + x * 4 + 3];
            if(alpha != 0)
                m_area[y * m_width + x] = true;
        }
    }
}

bool Texture::IsInside(const wxPoint& point, const wxRect2DDouble& rect)
{
    wxPoint mousePoint(point.x, point.y );
    mousePoint.x = (int)((float)mousePoint.x * (float)m_width / rect.m_width);
    mousePoint.y = (int)((float)mousePoint.y * (float)m_height / rect.m_height);

    return m_area[mousePoint.y * m_width + mousePoint.x];
}