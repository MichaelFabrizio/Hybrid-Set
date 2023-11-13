#include <glad/glad.h>
#include <iostream>
#include <cmath>

struct QuadColorAlpha
{
  constexpr static float increment_divisor = 1000.0f;
  constexpr static float Index_R_End = 0.172f;
  constexpr static float Index_G_End = 0.518f;
  constexpr static float Index_B_End = 0.647f;

  constexpr static float LeftVal_R_End = 0.157f;
  constexpr static float LeftVal_G_End = 0.337f;
  constexpr static float LeftVal_B_End = 0.157f;


  public:
    QuadColorAlpha() : VAO(0), VBO(0), EBO(0), ID(0), hidden(false) {}

  QuadColorAlpha(float x, float y, float width, float height, float R, float G, float B, float A) :
  vertices  { x,          y,          0.0f, R, G, B, A,     //Bottom left
              x + width,  y,          0.0f, R, G, B, A,   //Bottom right
              x + width,  y + height, 0.0f, R, G, B, A,   //Top right
              x,          y + height, 0.0f, R, G, B, A },  //Top left

  indices { 0, 1, 3, 1, 2, 3 } // (BL, BR, TL) triangle, (BR, TR, TL) Triangle
  {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 28, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 28, (void*)12);
    glEnableVertexAttribArray(1);

    // std::cout << "VAO: " << VAO << '\n';

  }
    ~QuadColorAlpha() 
  {
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
  }
  void UpdateLocationData(float x, float y)
  {
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    float xy[2] = { x, y };
    
    for (std::size_t i = 0; i < 3; i++) {
      glBufferSubData(GL_ARRAY_BUFFER, (i*7*4), 8, xy);
    }

  }
  
  void UpdateID(std::size_t id) { ID = id; }

  void UpdateColorData(std::size_t ID, bool Indexed, float R, float G, float B)
  {
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    float color[3];
    
    if (Indexed) { 
      // auto blueshift = B + ID * increment;
      // auto magnitude = std::sqrt(std::pow(R, 2) + std::pow(G, 2) + std::pow(B, 2));
      color[0] = R + ((Index_R_End - R) / 1024.0f) * ID;
      color[1] = G + ((Index_G_End - G) / 1024.0f) * ID;
      color[2] = B + ((Index_B_End - B) / 1024.0f) * ID;
    }
    else {
      color[0] = R + ((LeftVal_R_End - R) / 1024.0f) * ID;
      color[1] = G + ((LeftVal_G_End - G) / 1024.0f) * ID;
      color[2] = B + ((LeftVal_B_End - B) / 1024.0f) * ID;
    }
    
    for (std::size_t i = 0; i < 4; i++) {
      glBufferSubData(GL_ARRAY_BUFFER, (i*7*4) + 12, 12, color);
    }
  }

    unsigned int VBO, VAO, EBO;
    std::size_t ID;
    bool hidden;
  private:
    float vertices[28]; //4 vertices * (xyz coords + RGB color coords)
    unsigned int indices[6];
};
