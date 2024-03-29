#ifndef NIBBLER_IGRAPHIC_HPP
#define NIBBLER_IGRAPHIC_HPP

#include <array>
#include <string>

#include <cstdint>

#include "glm/glm.hpp"

#include "IGraphicConstants.hpp"
#include "IGraphicTypes.hpp"

class IGraphic
{
  public:
    IGraphic() = default;
    virtual ~IGraphic() = default;
    IGraphic(IGraphic const &src) = delete;
    IGraphic &operator=(IGraphic const &rhs) = delete;
    IGraphic(IGraphic &&src) = delete;
    IGraphic &operator=(IGraphic &&rhs) = delete;

    virtual void init(std::string const &home,
                      int32_t w_square,
                      int32_t h_square) = 0;
    virtual void terminate() = 0;
    virtual void createWindow(std::string &&name) = 0;
    virtual void deleteWindow() = 0;
    virtual uint8_t shouldClose() = 0;
    virtual void triggerClose() = 0;
    virtual void getEvents(
      std::array<uint8_t, IGraphicConstants::NB_EVENT> &events) = 0;
    virtual void drawBoard() = 0;
    virtual void drawSnake(
      std::array<glm::vec2, IGraphicConstants::MAX_SNAKE_SIZE> const &pos,
      std::array<glm::vec3, IGraphicConstants::MAX_SNAKE_SIZE> const &color,
      uint32_t size) = 0;
    virtual void drawSnake(
      std::array<glm::ivec2, IGraphicConstants::MAX_SNAKE_SIZE> const &pos,
      std::array<glm::vec3, IGraphicConstants::MAX_SNAKE_SIZE> const &color,
      uint32_t size) = 0;
    virtual void drawText(std::string const &str,
                          glm::vec3 const &color,
                          glm::vec2 const &pos,
                          float scale) = 0;
    virtual void render() = 0;
    virtual void clear() = 0;
    virtual void toggleFullscreen() = 0;

    typedef IGraphic *creator_t();
    typedef void deleter_t(IGraphic *);
};

#endif
