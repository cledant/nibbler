World::World(WorldParams &&params)
  : _params(params)
  , _gfx_interface(nullptr)
  , _path_gfx_lib()
  , _events()
{}

void
World::run()
{
    _init();
    while (!gfx_interface.shouldClose()) {
        while (_should_be_updated())
        {
            gfx_interface.getEvents(buffer);
            _get_events();
        }
        _move_snake();
        gfx_interface.clear();
        gfx_interface.drawBoard();
        gfx_interface.drawSnake(apple_pos, apple_color, apple_size);
        gfx_interface.drawSnake(snake_pos, snake_color, snake_size);
        gfx_interface.render();
    }
}