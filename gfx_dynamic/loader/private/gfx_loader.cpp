#include <stdexcept>
#include <iostream>

#include <dlfcn.h>

#include "gfx_loader.hpp"

GfxLoader::GfxLoader()
  : _handler(nullptr)
  , _creator(nullptr)
  , _deleter(nullptr)
{}

GfxLoader::~GfxLoader()
{
    if (_handler)
        closeLib();
}

void
GfxLoader::openLib(std::string const &libpath)
{
    std::cout << "Loading Shared Lib : " << libpath << std::endl;
    if (_handler)
        closeLib();
    if (!(_handler = dlopen(libpath.c_str(), RTLD_LAZY))) {
        throw std::runtime_error(dlerror());
    }
    if (!(_creator =
            reinterpret_cast<gfx_creator_t *>(dlsym(_handler, "creator")))) {
        throw std::runtime_error(dlerror());
    }
    if (!(_deleter =
            reinterpret_cast<gfx_deleter_t *>(dlsym(_handler, "deleter")))) {
        throw std::runtime_error(dlerror());
    }
}

void
GfxLoader::closeLib()
{
    if (_handler) {
        if (dlclose(_handler)) {
            throw std::runtime_error(dlerror());
        }
        _creator = nullptr;
        _deleter = nullptr;
        _handler = nullptr;
    }
}

gfx_creator_t *
GfxLoader::getCreator() const
{
    return (_creator);
}

gfx_deleter_t *
GfxLoader::getDeleter() const
{
    return (_deleter);
}