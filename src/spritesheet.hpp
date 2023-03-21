#ifndef SPRITE_SHEET_HPP
#define SPRITE_SHEET_HPP

#include <string>
#include <stdio.h>
#include <stdexcept>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "game.hpp"



class SpriteSheet : public Renderable {
    SDL_Texture* texture = NULL;
    int width;
    int height;
    int rows;
    int columns;

    public:
    /**
     * @brief Construct a new Sprite Sheet object
     * 
     * BEWARE: 
     * rows_ must divide the height of the file exactly or else you will experience "pixel drift"
     * columns_ must divide the width of the file exactly or else you will experience "pixel drift"
     * 
     * @param filename_ 
     * @param rows_ 
     * @param columns_ 
     */
    SpriteSheet(std::string filename, int columns_, int rows_, Engine* engine): 
        rows(rows_), columns(columns_) 
        {
            SDL_Surface* surface = IMG_Load(filename.c_str());

            if (surface == NULL) {
                std::string s("Could not load surface for: "); s += filename;
                throw std::runtime_error(s);
            }

            width = surface->w / columns;
            height = surface->h / rows;

            texture = SDL_CreateTextureFromSurface(engine->getRenderer(), surface);
            
            SDL_FreeSurface(surface);
            surface = NULL;

            if (texture == NULL) {
                std::string s("Could not load texture for: "); s += filename;
                throw std::runtime_error(s);
            }
        }


    void render(Engine* engine, int x, int y, SDL_Rect* location) {
        if (x < 0 || x > columns) {
            throw std::runtime_error("");
        }

        if (y < 0 || y > rows) {
            throw std::runtime_error("");
        }

        SDL_Rect source { x * width, y * height, width, height };
        SDL_RenderCopy(engine->getRenderer(), texture, &source, location);
    }
};

#endif
