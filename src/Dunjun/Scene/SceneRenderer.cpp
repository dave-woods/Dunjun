#include <Dunjun/Scene/SceneRenderer.hpp>
#include <Dunjun/Scene/SceneNode.hpp>
#include <Dunjun/Scene/MeshRenderer.hpp>

#include <string>

#include <algorithm>
#include <iterator>

namespace Dunjun
{
	SceneRenderer::SceneRenderer()
	{}

	void SceneRenderer::reset()
	{
		if (m_currentShaders)
			m_currentShaders->stopUsing();
		m_currentShaders = nullptr;

		m_currentTexture = nullptr;
		Texture::bind(nullptr, 0);

		m_currentMaterial = nullptr;

		currentCamera = nullptr;

		m_modelInstances.clear();
		m_pointLights.clear();
	}

	void SceneRenderer::draw(const SceneNode& node, Transform t)
	{
		node.draw(*this, t);
	}

	void SceneRenderer::draw(const Mesh* mesh) const
	{
		if (mesh)
			mesh->draw();
	}

	void SceneRenderer::addModelInstance(const MeshRenderer& meshRenderer, Transform transform)
	{
		if (meshRenderer.parent->visible)
			m_modelInstances.push_back({ &meshRenderer, transform });
	}

	void SceneRenderer::addPointLight(const PointLight* light)
	{
		m_pointLights.push_back(light);
	}

	void SceneRenderer::renderAll()
	{
		std::sort(std::begin(m_modelInstances), std::end(m_modelInstances),
		[] (const ModelInstance& a, const ModelInstance& b) -> bool
		{
			const auto& matA = a.meshRenderer->material;
			const auto& matB = b.meshRenderer->material;
			if (matA.shaders == matB.shaders)
				return matA.diffuseMap < matB.diffuseMap;
			else
				return matA.shaders < matB.shaders;
		});
		
		for (const auto& inst : m_modelInstances)
		{
			if (setShaders(inst.meshRenderer->material.shaders))
			{
				m_currentShaders->setUniform("u_camera", currentCamera->getMatrix());
				m_currentShaders->setUniform("u_cameraPosition", currentCamera->transform.position);
				m_currentShaders->setUniform("u_tex", (u32)0);

				m_currentShaders->setUniform("u_light.position", m_pointLights[0]->position); // assumes there is one light
				m_currentShaders->setUniform("u_light.intensities", m_pointLights[0]->intensities);
				m_currentShaders->setUniform("u_light.ambience", m_pointLights[0]->ambience);
			
				m_currentShaders->setUniform("u_light.attenuation.constant", m_pointLights[0]->attenuation.constant);
				m_currentShaders->setUniform("u_light.attenuation.linear", m_pointLights[0]->attenuation.linear);
				m_currentShaders->setUniform("u_light.attenuation.quadratic", m_pointLights[0]->attenuation.quadratic);
			}
			setTexture(inst.meshRenderer->material.diffuseMap, 0);
			m_currentShaders->setUniform("u_transform", inst.transform);
			draw(inst.meshRenderer->mesh);
		}
	}

	bool SceneRenderer::setShaders(const ShaderProgram* shaders)
	{
		if (shaders != m_currentShaders)
		{
			if (m_currentShaders)
				m_currentShaders->stopUsing();
			m_currentShaders = shaders;
			m_currentShaders->use();
			return true;
		}
		return false;
	}

	bool SceneRenderer::setTexture(const Texture* texture, GLuint position)
	{
		if (texture != m_currentTexture)
		{
			m_currentTexture = texture;

			Texture::bind(m_currentTexture, position);
			return true;
		}
		return false;
	}
}