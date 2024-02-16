#pragma once
#include "Material.h"

class OutlineMaterial : public Material {
public:
    OutlineMaterial() {}
    ~OutlineMaterial() {}
    void bind() override;
    void unbind() override;
};

