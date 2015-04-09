#ifndef DUNJUN_SCENE_MESHRENDERER_HPP
#define DUNJUN_SCENE_MESHRENDERER_HPP

#include <Dunjun/Scene/NodeComponent.hpp>
#include <Dunjun/Scene/SceneRenderer.hpp>
#include <Dunjun/Material.hpp>
#include <Dunjun/Mesh.hpp>
#include <Dunjun/ModelAsset.hpp>

namespace Dunjun
{
	class MeshRenderer : public NodeComponent
	{
	public:
		explicit MeshRenderer(const Mesh* mesh, const Material* material)
			: mesh(mesh)
			, material(material)
		{
			
		}
		explicit MeshRenderer(const ModelAsset& asset)
			: mesh(asset.mesh)
			, material(asset.material)
		{

		}

		virtual void draw(SceneRenderer& renderer, Transform t) const override
		{
			if (!material || !mesh)
				return;

			renderer.addModelInstance(*this, t);
		}
	
		const Material* material;
		const Mesh* mesh;
	};
}

#endif // !DUNJUN_SCENE_MESHRENDERER_HPP
