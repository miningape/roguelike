#ifndef WAVE_FUNCTION_COLLAPSE_HPP
#define WAVE_FUNCTION_COLLAPSE_HPP

#include "map_factory.hpp"
#include <set>

template<class T>
class MapGenerationFactory<T>::WaveFunctionCollapse {
	public:
	struct AdjacencyInfo {
		std::set<T> north;
		std::set<T> east;
		std::set<T> south;
		std::set<T> west;
	};

	private:
	enum Direction {
		NORTH, SOUTH, EAST, WEST
	};
	std::vector<std::vector<std::set<T>>> map;
	std::map<T, AdjacencyInfo> definition;

	int maxNumPossibilities;
	int width;
	int height;

	bool collapsable() {
		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				if ( map[x][y].size() > 1 ) {
					return true;
				}
			}
		}

		return false;
	}

	std::pair<int, int> cellWithLeastEntropy() {
		std::vector<std::pair<int, int>> choices[maxNumPossibilities];
		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				choices[map[x][y].size() - 1].push_back({ x, y });
			}
		}

		std::vector<std::pair<int, int>> selection;
		for (int i = 1; i < maxNumPossibilities; i++) {
			if (choices[i].size() > 0) {
				selection = choices[i];
				break;
			}
		}

		if (selection.size() == 0) {
			throw std::runtime_error("Not possible to find next cell because all posibilities have been exhausted");
		}

		auto elem = *select_randomly(selection.begin(), selection.end());
		return elem;
	}

	void collapseAt(int x, int y) {
		auto possibilities = map[x][y];
		auto choice = *select_randomly(possibilities.begin(), possibilities.end());
		map[x][y].clear();
		map[x][y].insert(choice);
	}

	std::set<std::string> getAdjacentPossibilities(T possibility, Direction dir) {
		auto possibilityDefinition = definition.find(possibility);
		if (possibilityDefinition == definition.end()) {
			throw std::runtime_error("WFC Dungeon Gen - Couldn't find definition for: " + possibility);
		}

		switch (dir) {
			case NORTH:
				return possibilityDefinition->second.north;
			case EAST:
				return possibilityDefinition->second.east;
			case SOUTH:
				return possibilityDefinition->second.south;
			case WEST:
				return possibilityDefinition->second.west;
		}
	}

	std::set<std::string> getAllowedPossibilities(std::pair<int, int> pos, Direction dir) {
		auto currentPossibilities = map[pos.first][pos.second];
		std::set<std::string> adjacentPossibilities;

		for ( auto possibility : currentPossibilities ) {
			auto allowedAdjacentPossibilities = getAdjacentPossibilities(possibility, dir);
			if (adjacentPossibilities.size() == 0) {
				adjacentPossibilities = allowedAdjacentPossibilities;
			}

			std::set<std::string> intersection;
			std::set_intersection( 
				allowedAdjacentPossibilities.begin(), allowedAdjacentPossibilities.end(),
				adjacentPossibilities.begin(), adjacentPossibilities.end(),
				std::inserter(intersection, intersection.begin())
			);
			adjacentPossibilities = intersection;
		}

		return adjacentPossibilities;
	}

	std::pair<int, int> directionToVector(Direction dir) {
		switch (dir) {
			case NORTH:
				return { 0, -1 };
			case SOUTH:
				return { 0, 1 };
			case EAST:
				return { 1, 0 };
			case WEST:
				return { -1, 0 };
		}
	}

	std::pair<int, int> nextCellInDir(std::pair<int, int> pos, Direction dir) {
		auto dirVec = directionToVector(dir);
		return { pos.first + dirVec.first, pos.second + dirVec.second };
	}

	std::vector<Direction> validAdjacentDirections(std::pair<int, int> pos) {
		std::vector<Direction> directions;
		if (pos.first > 0) {
			directions.push_back(WEST);
		}

		if (pos.first < (width - 1)) {
			directions.push_back(EAST);
		}

		if (pos.second > 0) {
			directions.push_back(NORTH);
		}

		if (pos.second < (height - 1)) {
			directions.push_back(SOUTH);
		}

		return directions;
	}

	void propegateCollapseAt(int x, int y) {
		std::vector<std::pair<int, int>> stack(1, { x, y });
		
		while (stack.size() > 0) {
			auto pos = stack.back();
			stack.pop_back();

			for (auto dir : validAdjacentDirections(pos)) {
				std::pair<int, int> adjacentCell = nextCellInDir(pos, dir);
				auto adjacentCellCurrentPossibilities = map[adjacentCell.first][adjacentCell.second];

				if (adjacentCellCurrentPossibilities.size() == 1) {
					continue;
				}

				auto adjacentCellAllowedPossibilities = getAllowedPossibilities(pos, dir);
				std::set<std::string> adjacentCellNewPosibilities;
				std::set_intersection(
					adjacentCellCurrentPossibilities.begin(), adjacentCellCurrentPossibilities.end(),
					adjacentCellAllowedPossibilities.begin(), adjacentCellAllowedPossibilities.end(),
					std::inserter(adjacentCellNewPosibilities, adjacentCellNewPosibilities.begin())
				);

				if (adjacentCellNewPosibilities.size() == 0) {
					throw std::runtime_error("Uncollapseable");
				}

				std::set<std::string> diff;
				std::set_difference(
					adjacentCellCurrentPossibilities.begin(), adjacentCellCurrentPossibilities.end(),
					adjacentCellNewPosibilities.begin(), adjacentCellNewPosibilities.end(), 
					std::inserter(diff, diff.begin())
				);

				if (diff.size() > 0) {
					map[adjacentCell.first][adjacentCell.second] = adjacentCellNewPosibilities;
					auto stackIterator = find(stack.begin(), stack.end(), adjacentCell);
					if (stackIterator == stack.end()) {
						stack.push_back(adjacentCell);
					}
				}
			}
		}
	}
	public:
	void print() {
		for (auto col : map) {
			std::cout << "[ ";
			for (auto cell : col) {
				std::cout << "[ ";
				for (auto posi : cell) {
					std::cout << posi;
				}
				std::cout << " ], ";
			}
			std::cout << " ]" << std::endl;
		}
	}

	Map<T> generate(std::map<T, AdjacencyInfo> definition_, int width_, int height_) {
		definition = definition_;
		width = width_;
		height = height_;

		std::set<T> possibilities;
		for (auto entry : definition) {
			possibilities.insert(entry.first);
		}
		maxNumPossibilities = possibilities.size();
		
		map = std::vector<std::vector<std::set<T>>>(width, std::vector<std::set<T>>(height, possibilities));

		while (collapsable()) {
			auto [x, y] = cellWithLeastEntropy();
			collapseAt(x, y);
			propegateCollapseAt(x, y);
		}



		std::vector<std::vector<T>> tMap;
		for ( auto col : map ) {
			std::vector<T> mapCol;
			for ( auto cell : col ) {
				mapCol.push_back(*(cell.begin()));
			}
			tMap.push_back(mapCol);
		}

		Map<T> newMap;
		newMap.internalMap = tMap;
		return newMap;
	}
};

#endif
