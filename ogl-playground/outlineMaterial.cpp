#include "OutlineMaterial.h"

void OutlineMaterial::bind() {
    shaders->bind();
}

void OutlineMaterial::unbind() {
    shaders->unbind();
}
