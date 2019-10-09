#include "World.hpp"

World::World(WorldParams &&params)
  : _params(params)
  , _gfx_interface(nullptr)
  , _path_gfx_lib()
  , _events()
  , _snake_pos({ glm::uvec2{ 0, 0 },
                 glm::uvec2{ 0, 1 },
                 glm::uvec2{ 0, 2 },
                 glm::uvec2{ 0, 3 },
                 glm::uvec2{ 0, 4 } })
  , _snake_color({ glm::vec3{ 0.0, 1.0, 0.0 },
                   glm::vec3{ 0.0, 0.5, 0.0 },
                   glm::vec3{ 0.0, 0.5, 0.0 },
                   glm::vec3{ 0.0, 0.5, 0.0 },
                   glm::vec3{ 0.0, 0.5, 0.0 } })
  , _snake_size(5)
{}

void
World::run()
{}
