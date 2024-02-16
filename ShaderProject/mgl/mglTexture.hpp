////////////////////////////////////////////////////////////////////////////////
//
// Textures (small example for 2D textures)
//
// (c)2022 by Carlos Martinho
//
////////////////////////////////////////////////////////////////////////////////

#ifndef MGL_TEXTURE_HPP
#define MGL_TEXTURE_HPP

#include <iostream>
#include <string>

#include "mglShader.hpp"

namespace mgl {

class Texture;
class Texture2D;
struct TextureInfo;

//////////////////////////////////////////////////////////////////////// TEXTURE

class Texture {
protected:
  GLuint id;

public:
  Texture();
  ~Texture();
  virtual void bind(int unit = 0) = 0;
  virtual void unbind() = 0;
};

/////////////////////////////////////////////////////////////////////// TEXTURES

class Texture2D : public Texture {
public:
  void bind(int unit = 0) override;
  void unbind() override;
  void load(const std::string &filename);
};

class TextureHDRi : public Texture {
public:
  void bind(int unit = 0) override;
  void unbind() override;
  void load(const std::string &filename);
};


////////////////////////////////////////////////////////////////////////////////
} // namespace mgl

#endif /* MGL_TEXTURE_HPP */
