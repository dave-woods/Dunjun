#ifndef DUNJUN_SCENE_SCENENODE_HPP
#define DUNJUN_SCENE_SCENENODE_HPP

#include <memory>
#include <string>
#include <vector>

#include <Dunjun/Common.hpp>
#include <Dunjun/ReadOnly.hpp>
#include <Dunjun/Transform.hpp>

namespace Dunjun
{
	class SceneNode
	{
	public:
		using UPtr = std::unique_ptr<SceneNode>;

		SceneNode();

		virtual ~SceneNode() {}

		SceneNode& attachChild(UPtr child);
		UPtr detachChild(const SceneNode& node);

		SceneNode* findChildByName(const std::string& name) const;

		std::string name;

		Transform getGlobalTransform() const;
		Transform transform;

		void onStart();
		void update(f32 dt);
		virtual void draw(Transform t = Transform());

		ReadOnly<SceneNode*, SceneNode> parent;
	protected:
		virtual void onStartCurrent();
		void onStartChildren();

		virtual void updateCurrent(f32 dt);
		void updateChildren(f32 dt);

		virtual void drawCurrent(Transform t);
		void drawChildren(Transform t);


		std::vector<UPtr> m_children;
	};

}


#endif // !DUNJUN_SCENE_SCENENODE_HPP
