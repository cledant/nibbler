#ifndef GLFWGRAPHIC_HPP
#define GLFWGRAPHIC_HPP

#include "IGraphic.hpp"

class GlfwGraphic : public IGraphic
{
  public:
    GlfwGraphic() = default;
    ~GlfwGraphic() override = default;

    void init() override;
};

#endif