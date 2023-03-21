#include "src/game.hpp"
#include "src/spritesheet.hpp"
#include "src/util.hpp"
#include "src/map_factory/wave_function_collapse.hpp"
#include <exception>
#include <iostream>

#include <entt.hpp>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

struct RenderableComponent {
	std::string sprite;
	std::pair<int, int> position;
	int x;
	int y;
};

class Roguelike : public Engine {
	public:
	using Engine::Engine;

	private:
	void constructor() override {
		inject<SpriteSheet>(
			"wall_tiles", 
			"Minifantasy_Dungeon_Assets/Tileset/Minifantasy_DungeonWallTiles.png",
			7, 14
		);
		inject<SpriteSheet>(
			"floor_tiles",
			"Minifantasy_Dungeon_Assets/Tileset/Minifantasy_DungeonFloorTiles.png",
			7, 2
		);
		singleton<entt::registry>();
		inject_singleton<MapGenerationFactory<std::string>>( std::vector({
			MapGenerationFactory<std::string>::Factories::WaveFunctionCollapse
		}) );

		std::map<std::string, MapGenerationFactory<std::string>::WaveFunctionCollapse::AdjacencyInfo> adjacency = {
				{"wall_basic", {
					// .filename = "wall_tiles",
					// .position = {1, 1},
					.north = {"wall_basic"},
					.east = {"floor_basic"},
					.south = {"wall_basic"},
					.west = {"floor_basic"},

				}},
				{"floor_basic", {
					// .filename = "floor_tiles",
					// .position = {0, 0},
					.north = {"wall_basic"},
					.east = {"wall_basic"},
					.south = {"wall_basic"},
					.west = {"wall_basic"},
				}}
			};

		auto mapDefinition = get<MapGenerationFactory<std::string>::WaveFunctionCollapse>()->generate(
			adjacency,
			7, 7
		);

		mapDefinition.map<entt::entity>([this]( std::string tile, int x, int y ){
			auto registry = get<entt::registry>();
			auto entity = registry->create();

			if (tile == "floor_basic") {
				registry->emplace<RenderableComponent>(entity, "floor_tiles", std::pair<int, int>({ 0, 0 }), x, y);
			} else {
				registry->emplace<RenderableComponent>(entity, "wall_tiles", std::pair<int, int>({ 2, 1 }), x, y);
			}

			return entity;
		});
	}

	public:
	void onEvent(SDL_Event e) override {
		if (e.type == SDL_QUIT) {
			stop();
		}
	}

	void main(float dt) override {
		clear();

		SDL_Rect dest{ 0, 0, 64, 64};

		auto registry = get<entt::registry>();
		auto view = registry->view<RenderableComponent>();

		for (auto [_, renderable] : view.each()) {
			auto spritesheet = get<SpriteSheet>(renderable.sprite);

			dest.x = renderable.x * dest.w;
			dest.y = renderable.y * dest.h;

			render(spritesheet, renderable.position.first, renderable.position.second, &dest);
		}

		draw();
	}
};

int main( int argc, char* args[] ) {
	try {
		Roguelike* game = new Roguelike("Name", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		auto hint = SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
		
		game->start();
		delete game;
	} catch (std::exception& e) {
		printf("Encountered error: %s. %s", e.what(), SDL_GetError());
		return 1;
	}

	return 0;
}