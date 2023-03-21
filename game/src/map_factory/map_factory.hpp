#ifndef MAP_FACTORY_HPP
#define MAP_FACTORY_HPP

#include "../game.hpp"
#include <vector>
#include <iostream>

template<class T>
class Map {
	public:
	std::vector<std::vector<T>> internalMap;

	template<class R>
	Map<R> map(std::function<R (T, int, int)> callback) {
		std::vector<std::vector<R>> colBuffer;
		for ( int x = 0; x < internalMap.size(); x++ ) {
			std::vector<R> rowBuffer;
			for ( int y = 0; y < internalMap[x].size(); y++ ) {
				rowBuffer.push_back( callback(internalMap[x][y], x, y) );
			}
			colBuffer.push_back(rowBuffer);
		}

		Map<R> newMap;
		newMap.internalMap = colBuffer;
		return newMap;
	}

	template<class R>
	std::vector<R> flatMap(std::function<R (T)> callback) {
		std::vector<R> buffer;
		for ( auto col : internalMap ) {
			for ( auto cell : col ) {
				buffer.push_back( callback(cell) );
			}
		}
		return buffer;
	}
};

template<class T>
class MapGenerationFactory {
	public:
	class Factories {
		public:
		enum ALLOWED {
			WaveFunctionCollapse
		};
	};

	MapGenerationFactory(std::vector<typename Factories::ALLOWED> injections, Engine* engine) {
		for (auto injection : injections) {
			switch (injection) {
				case Factories::WaveFunctionCollapse:
					engine->singleton<WaveFunctionCollapse>();
					break;
			}
		}
	}

	class WaveFunctionCollapse;
};

#endif