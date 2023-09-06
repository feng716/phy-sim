#include <glm/ext/vector_float3.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#define STATIC_REGISTER_NAME(declaration) declaration;\
        public: static decltype(declaration) get##declaration(){return declaration;}\
                static void change_##declaration(decltype(declaration) origin){declaration=origin;}\
        private:
class sceneTransform{
private:
    static glm::mat4 STATIC_REGISTER_NAME(perspectiveMat)
    static glm::vec3 STATIC_REGISTER_NAME(LookAt);
    static float STATIC_REGISTER_NAME(cursorXPos)
    static float STATIC_REGISTER_NAME(cursorYPos)
    static float STATIC_REGISTER_NAME(windowW)
    static float STATIC_REGISTER_NAME(windowH)
public:
    static void changeCursorPos(float,float);
};