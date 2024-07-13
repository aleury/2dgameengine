#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>

const int FPS = 60;
const int MILLISECONDS_PER_FRAME = 1000 / FPS;

class Game {
    private:
        bool isRunning;
        int millisecondsPreviousFrame = 0;
        SDL_Window* window;
        SDL_Renderer* renderer;

    public:
        Game();
        ~Game();
        int windowWidth;
        int windowHeight;
        void Initialize(bool fullscreen);
        void Run();
        void ProcessInput();
        void Setup();
        void Update();
        void Render();
        void Destroy();
};

#endif
