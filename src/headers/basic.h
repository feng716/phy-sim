
#include <glm/ext/quaternion_float.hpp>
#include <glm/ext/vector_float3.hpp>
#define DECLARE_COMPONENT(type,name) type name;\
    public:\
        const inline type get##name(){ return name;}\
        void set##name(type input){ name=input;}\
    private:
class transform{

private:
    friend class model;
    DECLARE_COMPONENT(glm::vec3, Position)
    DECLARE_COMPONENT(glm::quat, Rotation)
    DECLARE_COMPONENT(glm::vec3, Scale)
public:
};