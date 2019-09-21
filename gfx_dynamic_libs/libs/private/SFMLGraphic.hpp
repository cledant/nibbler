#ifndef SFMLGRAPHIC_HPP
#define SFMLGRAPHIC_HPP

#include "IGraphic.hpp"

class SFMLGraphic : public IGraphic
{
  public:
    SFMLGraphic() = default;
    ~SFMLGraphic() override = default;

    void init() override;
};

#endif
