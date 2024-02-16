#include <cassert>
#include <sstream>

#include "mglTexture.hpp"
#include "../stb_image.h"

namespace mgl {

//////////////////////////////////////////////////////////////////////// Texture

Texture::Texture() : id(-1) {}

Texture::~Texture() {}

////////////////////////////////////////////////////////////////////// Texture2D

void Texture2D::bind(int unit) {
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(GL_TEXTURE_2D, id);
}

void Texture2D::unbind() { glBindTexture(GL_TEXTURE_2D, 0); }

void Texture2D::load(const std::string &filename) {
  std::cout << "File name: " << filename << std::endl;
  stbi_set_flip_vertically_on_load(true);
  int width, height, channels;
  std::cout << "Loading image file " << filename << "... ";
  unsigned char *image =
      stbi_load(filename.c_str(), &width, &height, &channels, 0);
  // assert(channels == 4);
  if (image == nullptr) {
    std::cout << "error." << std::endl;
    exit(EXIT_FAILURE);
  } else {
    std::cout << "ok. " << std::endl;
  }

  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);

  // Pré OpenGL v3.30 (still compatible with core)

  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
  //                GL_LINEAR_MIPMAP_LINEAR);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  GLenum colorFormat = GL_RGBA;
  if (channels == 3) {
      colorFormat = GL_RGB;
  }
  if (channels == 2) { //FIXME
      colorFormat = GL_RG;
  }
  if (channels == 1) { //FIXME
      colorFormat = GL_R;
  }

  glTexImage2D(GL_TEXTURE_2D, 0, colorFormat, width, height, 0, colorFormat,
               GL_UNSIGNED_BYTE, image);
  // syntax: glTexImage2D(target, level, internalformat, width, height, border,
  // format, type, data)

  glGenerateMipmap(GL_TEXTURE_2D);


  GLfloat max = 0.0f;
  glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, (GLint)max);

  glBindTexture(GL_TEXTURE_2D, 0);

  stbi_image_free(image);
}

////////////////////////////////////////////////////////////////////// TextureHDRi

void TextureHDRi::bind(int unit) {
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(GL_TEXTURE_2D, id);
}

void TextureHDRi::unbind() { glBindTexture(GL_TEXTURE_2D, 0); }

void TextureHDRi::load(const std::string &filename) {
  stbi_set_flip_vertically_on_load(true);
  int width, height, nrComponents;
  float *data = stbi_loadf(filename.c_str(), &width, &height, &nrComponents, 0);
  if (data)
  {
      glGenTextures(1, &id);
      glBindTexture(GL_TEXTURE_2D, id);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      glBindTexture(GL_TEXTURE_2D, 0);

      stbi_image_free(data);

      std::cout << "Loaded HDRi: " + filename << std::endl;

  }
  else
  {
      std::cout << "Failed to load HDR image." << std::endl;
  }
}

////////////////////////////////////////////////////////////////////////////////
} // namespace mgl
