#ifndef GAME_HPP
#define GAME_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <stdio.h>
#include <stdexcept>
#include <string>
#include <vector>
#include <optional>
#include <map>
#include <memory>
#include <typeindex>
#include <optional>

const std::string LONE_INJECTION = "*";

class Engine;

class Renderable {
    public:
    virtual void render(Engine* engine, int x, int y, SDL_Rect* location) = 0;
};

class Engine {
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
	std::map<std::type_index, std::map<std::string, std::shared_ptr<void>>> instances;
	std::map<std::type_index, std::shared_ptr<void>> singletons;
	bool running = true;

	virtual void constructor(){
		printf("BASE");
	};
	virtual void onEvent(SDL_Event e) = 0;
	virtual void main(float dt) = 0;
	virtual void destructor(){};

	void coreLoop() {
		while (this->running) {
			SDL_Event e;
			while (SDL_PollEvent(&e)) {
				this->onEvent(e);
			}

			this->main(0);
		}
	}

	public:
	Engine(const char* title, int width, int height, Uint32 flags) {
		if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
			throw std::runtime_error("Failed to initialise SDL");
		}

		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		window = SDL_CreateWindow(
			title, 
			SDL_WINDOWPOS_UNDEFINED, 
			SDL_WINDOWPOS_UNDEFINED, 
			width,
			height,
			flags
		);

		if (window == NULL) {
			throw std::runtime_error("Failed to initialise window");
		}

		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

		if (renderer == NULL) {
			throw std::runtime_error("Failed to initialise renderer");
		}

		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(renderer);

		int imgFlags = IMG_INIT_PNG;
		if (!(IMG_Init(imgFlags) & imgFlags)) {
			throw std::runtime_error("Failed to initialise PNG loader");
		}
	}

	~Engine() {
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		renderer = NULL;
		window = NULL;

		IMG_Quit();
		SDL_Quit();
	}

	SDL_Renderer* getRenderer() {
		return renderer;
	}

	void start() {
		constructor();
		coreLoop();
		destructor();
	}

	// protected:
	void stop() {
		this->running = false;
	}

	void draw() {
		SDL_RenderPresent(renderer);
	}

	void clear() {
		SDL_RenderClear(renderer);
	}

	template<class T, typename... Args>
	std::shared_ptr<T> inject(std::string name, Args&&... args);
	void render(std::shared_ptr<Renderable> renderable, int x, int y, SDL_Rect* destination);	
	
	template<class T, typename... Args>
	std::shared_ptr<T> inject_singleton(Args&&... args) {
		static_assert(std::is_constructible_v<T, Args..., Engine*>, "Cannot construct type with given args");
		auto resourcePtr = std::make_shared<T>(std::forward<Args>(args)..., this);
		singletons[typeid(T)] = resourcePtr;
		return resourcePtr;
	}

	template<class T, typename... Args>
	std::shared_ptr<T> singleton(Args&&... args) {
		static_assert(std::is_constructible_v<T, Args...>, "Cannot construct type with given args");
		auto resourcePtr = std::make_shared<T>(std::forward<Args>(args)...);
		singletons[typeid(T)] = resourcePtr;
		return resourcePtr;
	}

	// template<class T>
	// void import() {
	// 	static_assert(std::is_constructible_v<T>, "Cannot construct type with given args");
	// 	auto resourcePtr = std::make_shared<T>();
	// 	singletons[typeid(T)] = resourcePtr;
	// }

	template<class T>
	std::shared_ptr<T> get() {
		auto instance = singletons.find(typeid(T));
		if (instance == singletons.end()) {
			throw std::runtime_error("Could not get instance for type" );
		}

		auto typedInstance = std::static_pointer_cast<T>(instance->second);
		if (!typedInstance) {
			throw std::runtime_error("Bad injection types for: ");
		}

		return typedInstance;
	}

	template<class T>
	std::shared_ptr<T> get(std::string name) {
		auto nameToTMap = instances.find(typeid(T));
		if (nameToTMap == instances.end()) {
			throw std::runtime_error("Could not get type map for: " + name );
		}

		auto mapInstance = nameToTMap->second;
		auto instance = mapInstance.find(name);
		if (instance == mapInstance.end()) {
			throw std::runtime_error("Could not get instance of " + name);
		}

		auto typedInstance = std::static_pointer_cast<T>(instance->second);
		if (!typedInstance) {
			throw std::runtime_error("Bad injection types for: " + name);
		}

		return typedInstance;
	}
};

#include "spritesheet.hpp"

template<class T, typename... Args>
std::shared_ptr<T> Engine::inject(std::string name, Args&&... args) {
	static_assert(std::is_constructible_v<T, Args..., Engine*>, "Cannot construct type with given args");
	auto resourcePtr = std::make_shared<T>(std::forward<Args>(args)..., this);
	instances[typeid(T)][name] = resourcePtr;
	return resourcePtr;
}

void Engine::render(std::shared_ptr<Renderable> renderable, int x, int y, SDL_Rect* destination) {
	renderable->render(this, x, y, destination);
}

#endif