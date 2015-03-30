#ifndef DUNJUN_SCENE_SCENENODE_HPP
#define DUNJUN_SCENE_SCENENODE_HPP

#include <Dunjun/Common.hpp>
#include <Dunjun/ReadOnly.hpp>
#include <Dunjun/Transform.hpp>
#include <Dunjun/NonCopyable.hpp>
#include <Dunjun/Renderer.hpp>

#include <Dunjun/Scene/NodeComponent.hpp>

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <typeindex>

namespace Dunjun
{
	class SceneNode : private NonCopyable
	{
	public:
		using UPtr = std::unique_ptr<SceneNode>;
		using GroupedComponentMap = std::map < std::type_index, std::vector<NodeComponent*> > ;

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
		virtual void draw(Renderer& renderer, Transform t = Transform());

		ReadOnly<SceneNode*, SceneNode> parent;
	
	protected:
		virtual void onStartCurrent();
		void onStartChildren();

		virtual void updateCurrent(f32 dt);
		void updateChildren(f32 dt);

		virtual void drawCurrent(Renderer& renderer, Transform t);
		void drawChildren(Renderer& renderer, Transform t);

		std::vector<UPtr> m_children;
		GroupedComponentMap m_groupedComponents;
	
	public:
		SceneNode* addComponent(NodeComponent* component);

		template <class Derived, class... Args>
		inline SceneNode* addComponent(Args&&... args)
		{
			return addComponent(new Derived(args...));
		}

		inline void removeAllComponents()
		{
			for (auto& group : m_groupedComponents)
				group.second.clear();
			m_groupedComponents.clear();
		}

		template <class ComponentType>
		std::vector<NodeComponent*>* getComponents()
		{
			if (!std::is_base_of<NodeComponent, ComponentType>::value) //vale?
				return nullptr;

			if (m_groupedComponents.size() == 0)
				return nullptr;

			const std::type_index id(typeid(ComponentType));
			for (auto& group : m_groupedComponents)
			{
				if (group.first == id)
					return &m_groupedComponents[id];
			}
			return nullptr;
		}

		template <class ComponentType>
		NodeComponent* getComponent()
		{
			auto c = getComponents<ComponentType>();
			if (c)
				return std::static_pointer_cast<ComponentType>(c->at(0));
			return nullptr;
		}
	};
}


#endif // !DUNJUN_SCENE_SCENENODE_HPP
