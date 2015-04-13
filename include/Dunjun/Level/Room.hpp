#ifndef DUNJUN_LEVEL_ROOM_HPP
#define DUNJUN_LEVEL_ROOM_HPP

#include <Dunjun/Scene/SceneNode.hpp>
#include <Dunjun/Mesh.hpp>
#include <Dunjun/Material.hpp>

#include <Dunjun/Math/Random.hpp>

namespace Dunjun
{
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
		s16 x = 0;
		s16 y = 0;

		TileId()
		{}

		TileId(s16 x, s16 y)
			: x(x)
			, y(y)
		{}

		TileId(s16 xy[2])
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

	using RandomTileSet = std::vector < TileId >;

	class Room : public SceneNode
	{
	public:
		using UPtr = std::unique_ptr<Room>;

		struct Size
		{
			Size(u16 x, u16 y, s32 z)
				: x(x)
				, y(y)
				, z(z)
			{}

			u16 x = 0;
			u16 y = 0;
			s32 z = 0;
		};

		explicit Room(Random& random, const Room::Size& size);
		
		virtual ~Room() override;

		const Room::Size size;
		Material* material;
		
		void generate(bool northDoor, bool eastDoor, bool southDoor, bool westDoor);
		
	protected:
		Mesh* m_mesh;
		Random& m_random;
		Mesh::Data m_meshData;
		bool m_generated;

		void addTileSurface(const Vector3& position, TileSurfaceFace face, const TileId& tilePos);
		void addTileSurface(const Vector3& position, TileSurfaceFace face, const RandomTileSet& randomTilePosSet);
	};

}

#endif // !DUNJUN_LEVEL_ROOM_HPP
