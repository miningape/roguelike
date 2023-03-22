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
		inject_singleton<MapFactory<std::string>>( std::vector({
			MapFactory<std::string>::Factories::WaveFunctionCollapse
		}) );

		std::map<std::string, MapFactory<std::string>::WaveFunctionCollapse::AdjacencyInfo> adjacency = {
				{"wall_east_west", {
					.north = { "floor_basic" },
					.east = { "wall_east_west", "wall_east_south", "wall_north_east", "wall_west_south", "wall_north_west" },
					.south = { "floor_basic" },
					.west = { "wall_east_west", "wall_east_south", "wall_north_east", "wall_west_south", "wall_north_west"  }
				}},
				{"wall_north_south", {
					.north = { "wall_north_south", "wall_east_south", "wall_north_west", "wall_north_east", "wall_west_south" },
					.east = { "floor_basic" },
					.south = { "wall_north_south", "wall_east_south", "wall_north_west", "wall_north_east", "wall_east_south" },
					.west = { "floor_basic"}
				}},
				{"wall_east_south", {
					.north = { "floor_basic" },
					.east = { "wall_east_west", "wall_west_south", "wall_north_west" },
					.south = { "wall_north_south", "wall_north_east", "wall_north_west" },
					.west = { "floor_basic" }
				}},
				{"wall_west_south", {
					.north = { "floor_basic" },
					.east = { "floor_basic" },
					.south = { "wall_north_south", "wall_north_east", "wall_north_west" },
					.west = { "wall_east_west", "wall_east_south", "wall_north_east" },
				}},
				{"wall_north_east", {
					.north = { "wall_north_south", "wall_east_south", "wall_west_south" },
					.east = { "wall_east_west", "wall_north_west", "wall_west_south" },
					.south = { "floor_basic" },
					.west = { "floor_basic" },
				}},
				{"wall_north_west", {
					.north = { "wall_north_south", "wall_west_south", "wall_east_south" },
					.east = { "floor_basic" },
					.south = { "floor_basic" },
					.west = { "wall_east_west", "wall_east_south", "wall_north_east" }
				}},
				{"floor_basic", {
					// .filename = "floor_tiles",
					// .position = {0, 0},
					.north = {"floor_basic", "wall_east_west", "wall_north_south", "wall_east_south", "wall_west_south", "wall_north_east", "wall_north_west"},
					.east = {"floor_basic", "wall_east_west", "wall_north_south", "wall_east_south", "wall_west_south", "wall_north_east", "wall_north_west"},
					.south = {"floor_basic", "wall_east_west", "wall_north_south", "wall_east_south", "wall_west_south", "wall_north_east", "wall_north_west"},
					.west = {"floor_basic", "wall_east_west", "wall_north_south", "wall_east_south", "wall_west_south", "wall_north_east", "wall_north_west"},
				}}
			};

		auto mapDefinition = get<MapFactory<std::string>::WaveFunctionCollapse>()->generate(
			adjacency,
			8, 8
		);

		mapDefinition.map<entt::entity>([this]( std::string tile, int x, int y ){
			auto registry = get<entt::registry>();
			auto entity = registry->create();

			if (tile == "floor_basic") {
				registry->emplace<RenderableComponent>(entity, "floor_tiles", std::pair<int, int>({ 0, 0 }), x, y);
			} else if (tile == "wall_east_west") {
				registry->emplace<RenderableComponent>(entity, "wall_tiles", std::pair<int, int>({ 2, 1 }), x, y);
			} else if (tile == "wall_north_south") {
				registry->emplace<RenderableComponent>(entity, "wall_tiles", std::pair<int, int>({ 1, 2 }), x, y);
			} else if (tile == "wall_east_south") {
				registry->emplace<RenderableComponent>(entity, "wall_tiles", std::pair<int, int>({ 1, 1 }), x, y);
			} else if (tile == "wall_west_south") {
				registry->emplace<RenderableComponent>(entity, "wall_tiles", std::pair<int, int>({ 3, 1 }), x, y);
			} else if (tile == "wall_north_east") {
				registry->emplace<RenderableComponent>(entity, "wall_tiles", std::pair<int, int>({ 1, 3 }), x, y);
			} else if (tile == "wall_north_west") {
				registry->emplace<RenderableComponent>(entity, "wall_tiles", std::pair<int, int>({ 3, 3 }), x, y);
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