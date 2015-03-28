#ifndef DUNJUN_LEVEL_HPP
#define DUNJUN_LEVEL_HPP

#include <Dunjun/Common.hpp>
#include <Dunjun/ModelAsset.hpp>

#include <vector>

namespace Dunjun
{
class Level
{
public:
	enum class TileSurfaceFace : u32
	{
		Right = 0,
		Left = 1,
		Up = 2,
		Down = 3,
		Front = 4,
		Back = 5,
	};

	struct TileId
	{
		i16 x = 0;
		i16 y = 0;

		TileId()
		{}

		TileId(i16 x, i16 y)
			: x(x)
			, y(y)
		{}

		TileId(i16 xy[2])
			: x(xy[0])
			, y(xy[1])
		{}

		inline bool operator==(const TileId& id)
		{
			return (x == id.x) && (y == id.y);
		}
		inline bool operator!=(const TileId& id)
		{
			return !operator==(id);
		}
	};

	using RandomTileSet = std::vector < TileId > ;

	Level();
	virtual ~Level();

	void generate();
	
	std::vector<std::vector<TileId>> mapGrid;
	ReadOnly<u16, Level> mapWidth = 16;
	ReadOnly<u16, Level> mapHeight = 16;
	ReadOnly<u16, Level> mapDepth = 3;


	void addTileSurface(const Vector3& position, TileSurfaceFace face, const TileId& tilePos);
	void addTileSurface(const Vector3& position, TileSurfaceFace face, const RandomTileSet& randomTilePosSet);

	const Material* material;
	Mesh* mesh;
	Transform transform;

private:

	Mesh::Data m_meshData;
};
}

#endif // !DUNJUN_LEVEL_HPP
