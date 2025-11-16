
#ifndef tower_hpp
#define tower_hpp

#include "GObject.hpp"
#include <glm/glm.hpp>
#include "utils.hpp"

typedef glm::vec3 v3;
typedef glm::vec2 v2;

class Tower : public GObject {

    public:
        float radius;
        float height;
};

#endif