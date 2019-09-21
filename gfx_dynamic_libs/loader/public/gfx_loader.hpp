#ifndef GFX_LOADER_HPP
#define GFX_LOADER_HPP

#include <string>

#include "IGraphic.hpp"

class GfxLoader
{
  public:
    // Constructors
    GfxLoader();
    virtual ~GfxLoader();
    GfxLoader(GfxLoader const &src) = delete;
    GfxLoader &operator=(GfxLoader const &rhs) = delete;
    GfxLoader(GfxLoader &&src) = delete;
    GfxLoader &operator=(GfxLoader &&rhs) = delete;

    // Class Loading functions
    void openLib(std::string const &libpath);
    void closeLib();
    [[nodiscard]] gfx_creator_t *getCreator() const;
    [[nodiscard]] gfx_deleter_t *getDeleter() const;

  private:
    void *_handler;
    gfx_creator_t *_creator;
    gfx_deleter_t *_deleter;
};

#endif
