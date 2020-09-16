#ifndef DYN_LIB_LOADER_HPP
#define DYN_LIB_LOADER_HPP

#include <string>
#include <stdexcept>
#include <iostream>

#include <dlfcn.h>

template<class T>
class DynLibLoader
{
  public:
    // Constructors
    DynLibLoader();
    virtual ~DynLibLoader();
    DynLibLoader(DynLibLoader const &src) = delete;
    DynLibLoader &operator=(DynLibLoader const &rhs) = delete;
    DynLibLoader(DynLibLoader &&src) = delete;
    DynLibLoader &operator=(DynLibLoader &&rhs) = delete;

    // Class Loading functions
    void openLib(std::string const &libpath);
    void closeLib();
    [[nodiscard]] typename T::creator_t *getCreator() const;
    [[nodiscard]] typename T::deleter_t *getDeleter() const;

  private:
    void *_handler;
    typename T::creator_t *_creator;
    typename T::deleter_t *_deleter;
};

template<class T>
DynLibLoader<T>::DynLibLoader()
  : _handler(nullptr)
  , _creator(nullptr)
  , _deleter(nullptr)
{}

template<class T>
DynLibLoader<T>::~DynLibLoader()
{
    if (_handler) {
        closeLib();
    }
}

template<class T>
void
DynLibLoader<T>::openLib(std::string const &libpath)
{
    std::cout << "Loading Shared Lib : " << libpath << std::endl;
    if (_handler) {
        closeLib();
    }
    if (!(_handler = dlopen(libpath.c_str(), RTLD_NOW))) {
        std::string error("Nibbler: Dynamic library error: ");
        error += dlerror();
        throw std::runtime_error(error);
    }
    if (!(_creator = reinterpret_cast<typename T::creator_t *>(
            dlsym(_handler, "creator")))) {
        std::string error("Nibbler: Dynamic library error: ");
        error += dlerror();
        throw std::runtime_error(error);
    }
    if (!(_deleter = reinterpret_cast<typename T::deleter_t *>(
            dlsym(_handler, "deleter")))) {
        std::string error("Nibbler: Dynamic library error: ");
        error += dlerror();
        throw std::runtime_error(error);
    }
}

template<class T>
void
DynLibLoader<T>::closeLib()
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

template<class T>
typename T::creator_t *
DynLibLoader<T>::getCreator() const
{
    return (_creator);
}

template<class T>
typename T::deleter_t *
DynLibLoader<T>::getDeleter() const
{
    return (_deleter);
}

#endif
