#include "GVertice.hpp"

GVertice::GVertice(v3 coords, v3 colors, v2 texture) {
    this->coords = coords;
    this->colors = colors;
    this->texture = texture;
    this->normal = v3(0.5f, 0.5, 0.5f);
}

GVertice::GVertice(v3 coords, v3 colors, v2 texture, v3 normal) {
    this->coords = coords;
    this->colors = colors;
    this->texture = texture;
    this->normal = normal;
}