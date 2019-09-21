#ifndef NIBBLER_IGRAPHIC_HPP
#define NIBBLER_IGRAPHIC_HPP

class IGraphic
{
  public:
    IGraphic() = default;
    virtual ~IGraphic() = default;

    virtual void init() = 0;
};

typedef IGraphic *gfx_creator_t();
typedef void gfx_deleter_t(IGraphic *);

#endif
