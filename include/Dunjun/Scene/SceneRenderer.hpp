#ifndef DUNJUN_SCENE_SCENERENDERER_HPP
#define DUNJUN_SCENE_SCENERENDERER_HPP

#include <Dunjun/Material.hpp>
#include <Dunjun/Camera.hpp>
#include <Dunjun/ModelAsset.hpp>
#include <Dunjun/Scene/Lighting.hpp>

#include <vector>

namespace Dunjun
{
	class SceneNode;
	class MeshRenderer;
	class SceneRenderer : private NonCopyable
	{
	public:
		struct ModelInstance
		{
			const MeshRenderer* meshRenderer;
			Transform transform;
		};

		SceneRenderer();

		virtual ~SceneRenderer() {}

		void reset();

		void draw(const SceneNode& node, Transform t = Transform());
		void draw(const Mesh* mesh) const;

		void addModelInstance(const MeshRenderer& meshRenderer, Transform transform);

		void addPointLight(const PointLight* light);

		void renderAll();

		const Camera* currentCamera = nullptr;
	private:
		bool setShaders(const ShaderProgram* shaders);
		bool setTexture(const Texture* texture, GLuint position);

		std::vector<ModelInstance> m_modelInstances;
		std::vector<const PointLight*> m_pointLights;

		const Material* m_currentMaterial = nullptr;
		const ShaderProgram* m_currentShaders = nullptr;
		const Texture* m_currentTexture = nullptr;
	};
}


#endif // !DUNJUN_RENDERER_HPP
