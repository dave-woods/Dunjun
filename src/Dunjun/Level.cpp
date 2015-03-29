#include <Dunjun/Level.hpp>

#include <random>

namespace Dunjun
{
	Level::Level()
	{}

	Level::~Level()
	{
		delete mesh;
	}

	void Level::generate()
	{
		if (!mesh)
			mesh = new Mesh();

		mapGrid = std::vector<std::vector<TileId>>(mapWidth, std::vector<TileId>(mapHeight));

		//std::random_device rd;
		//std::mt19937 mt(rd());
		std::mt19937 mt(1); // fixed seed
		std::uniform_int_distribution<int> dist(0, 100);

		TileId lightWoodTile = { 0, 11 };
		TileId darkWoodTile = { 0, 10 };
		TileId blankTile = { -1, -1 };
		RandomTileSet stoneTiles;
		for (int i = 1; i < 3; i++)
			stoneTiles.emplace_back(i, 15);
		RandomTileSet darkRockTiles;
		for (int i = 0; i < 4; i++)
			darkRockTiles.emplace_back(i, 0);

		for (int i = 0; i < mapWidth; i++)
		{
			for (int j = 0; j < mapHeight; j++)
			{
				if (dist(mt) > 5)
					mapGrid[i][j] = lightWoodTile;
				else
					mapGrid[i][j] = blankTile;
			}
		}

		for (int i = 0; i < mapWidth; i++)
		{
			for (int j = 0; j < mapHeight; j++)
			{
				if (mapGrid[i][j] != blankTile)
				{
					addTileSurface(Vector3(i, 0, j), TileSurfaceFace::Up, mapGrid[i][j]);
				}
				else
				{
					addTileSurface(Vector3(i, mapDepth, j), TileSurfaceFace::Up, stoneTiles);
				}

				for (int k = 0; k < mapDepth; k++)
				{
					if (mapGrid[i][j] == blankTile)
					{
						if (i > 0)
							if (mapGrid[i - 1][j] != blankTile)
								addTileSurface(Vector3(i, k, j), TileSurfaceFace::Left, stoneTiles);
						if (i < mapWidth - 1)
							if (mapGrid[i + 1][j] != blankTile)
								addTileSurface(Vector3(i + 1, k, j), TileSurfaceFace::Right, stoneTiles);
						if (j > 0)
							if (mapGrid[i][j - 1] != blankTile)
								addTileSurface(Vector3(i, k, j), TileSurfaceFace::Back, stoneTiles);
						if (j < mapHeight - 1)
							if (mapGrid[i][j + 1] != blankTile)
								addTileSurface(Vector3(i, k, j + 1), TileSurfaceFace::Front, stoneTiles);
					}
					else
					{
						if (i == 0)
							addTileSurface(Vector3(i, k, j), TileSurfaceFace::Right, stoneTiles);
						if (i == mapWidth - 1)
							addTileSurface(Vector3(i + 1, k, j), TileSurfaceFace::Left, stoneTiles);
						if (j == 0)
							addTileSurface(Vector3(i, k, j), TileSurfaceFace::Front, stoneTiles);
						if (j == mapHeight - 1)
							addTileSurface(Vector3(i, k, j + 1), TileSurfaceFace::Back, stoneTiles);
					}
				}
			}
		}

		mesh->addData(m_meshData);
	}

	void Level::addTileSurface(const Vector3& position, TileSurfaceFace face, const Level::TileId& tilePos)
	{
		if (!mesh)
			mesh = new Mesh();

		const f32 tileWidth = 1.0f / 16.0f;
		const f32 tileHeight = 1.0f / 16.0f;
		usize index = m_meshData.vertices.size();

		// Right-Left
		if ((u32)face / 2 == 0)
		{
			m_meshData.vertices.append({ position.x + 0.0f, position.y + 0.0f, position.z + 1.0f }, { (tilePos.x + 1) * tileWidth, (tilePos.y + 0) * tileHeight });
			m_meshData.vertices.append({ position.x + 0.0f, position.y + 0.0f, position.z + 0.0f }, { (tilePos.x + 0) * tileWidth, (tilePos.y + 0) * tileHeight });
			m_meshData.vertices.append({ position.x + 0.0f, position.y + 1.0f, position.z + 0.0f }, { (tilePos.x + 0) * tileWidth, (tilePos.y + 1) * tileHeight });
			m_meshData.vertices.append({ position.x + 0.0f, position.y + 1.0f, position.z + 1.0f }, { (tilePos.x + 1) * tileWidth, (tilePos.y + 1) * tileHeight });
		}
		// Up-Down
		else if ((u32)face / 2 == 1)
		{
			m_meshData.vertices.append({ position.x + 0.0f, position.y + 0.0f, position.z + 0.0f }, { (tilePos.x + 0) * tileWidth, (tilePos.y + 1) * tileHeight });
			m_meshData.vertices.append({ position.x + 0.0f, position.y + 0.0f, position.z + 1.0f }, { (tilePos.x + 0) * tileWidth, (tilePos.y + 0) * tileHeight });
			m_meshData.vertices.append({ position.x + 1.0f, position.y + 0.0f, position.z + 1.0f }, { (tilePos.x + 1) * tileWidth, (tilePos.y + 0) * tileHeight });
			m_meshData.vertices.append({ position.x + 1.0f, position.y + 0.0f, position.z + 0.0f }, { (tilePos.x + 1) * tileWidth, (tilePos.y + 1) * tileHeight });
		}
		// Front-Back
		else if ((u32)face / 2 == 2)
		{
			m_meshData.vertices.append({ position.x + 0.0f, position.y + 0.0f, position.z + 0.0f }, { (tilePos.x + 1) * tileWidth, (tilePos.y + 0) * tileHeight });
			m_meshData.vertices.append({ position.x + 1.0f, position.y + 0.0f, position.z + 0.0f }, { (tilePos.x + 0) * tileWidth, (tilePos.y + 0) * tileHeight });
			m_meshData.vertices.append({ position.x + 1.0f, position.y + 1.0f, position.z + 0.0f }, { (tilePos.x + 0) * tileWidth, (tilePos.y + 1) * tileHeight });
			m_meshData.vertices.append({ position.x + 0.0f, position.y + 1.0f, position.z + 0.0f }, { (tilePos.x + 1) * tileWidth, (tilePos.y + 1) * tileHeight });
		}

		if ((u32)face % 2 == 0)
			m_meshData.addFace(index, 0, 1, 2).addFace(index, 2, 3, 0);
		else
			m_meshData.addFace(index, 0, 3, 2).addFace(index, 2, 1, 0);
	}

	void Level::addTileSurface(const Vector3& position, TileSurfaceFace face, const Level::RandomTileSet& randomTilePosSet)
	{
		usize length = randomTilePosSet.size();
		if (length == 0)
			return;
		if (length == 1)
		{
			addTileSurface(position, face, randomTilePosSet[0]);
			return;
		}
		TileId tilePos = randomTilePosSet[rand() % length];
		addTileSurface(position, face, tilePos);
	}
}