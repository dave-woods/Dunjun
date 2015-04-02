#ifndef DUNJUN_SCENE_SCENENODE_HPP
#define DUNJUN_SCENE_SCENENODE_HPP

#include <Dunjun/Common.hpp>
#include <Dunjun/ReadOnly.hpp>
#include <Dunjun/Transform.hpp>
#include <Dunjun/NonCopyable.hpp>
#include <Dunjun/Renderer.hpp>
#include <Dunjun/Drawable.hpp>

#include <Dunjun/Scene/NodeComponent.hpp>

#include <array>
#include <bitset>
#include <memory>
#include <string>
#include <vector>
#include <deque>
#include <map>
#include <typeindex>

namespace Dunjun
{
	using ComponentId = usize;
	
	namespace Impl
	{
		inline ComponentId getUniqueComponentId()
		{
			LOCAL_INTERNAL ComponentId lastId = 0;
			return lastId++;
		}
	}

	template <class ComponentType>
	inline ComponentId getComponentTypeId()
	{
		static_assert(std::is_base_of<NodeComponent, ComponentType>::value,
			"ComponentType must inherit from NodeComponent.");

		LOCAL_INTERNAL ComponentId typeId = Impl::getUniqueComponentId();
		return typeId;
	}

	class SceneNode : public Drawable, private NonCopyable
	{
	public:
		using UPtr = std::unique_ptr<SceneNode>;
		using GroupedComponentMap = std::map < std::type_index, std::vector<NodeComponent::UPtr>>;

		GLOBAL const usize MaxComponents = 32;
		using ComponentBitset = std::bitset<MaxComponents>;
		using ComponentArray = std::array<NodeComponent*, MaxComponents>;

		SceneNode();

		virtual ~SceneNode() {}

		SceneNode& attachChild(UPtr child);
		UPtr detachChild(const SceneNode& node);

		SceneNode* findChildById(const usize id) const;
		SceneNode* findChildByName(const std::string& name) const;

		Transform getGlobalTransform() const;

		void onStart();
		void update(f32 dt);

		const usize id;
		std::string name;
		Transform transform;
		ReadOnly<SceneNode*, SceneNode> parent;
		bool visible = true;
	
		template <class ComponentType, class... Args>
		ComponentType& addComponent(Args&&... args)
		{
			assert(!hasComponent<ComponentType>());

			ComponentType* component(new ComponentType(std::forward<Args>(args)...));
			component->parent = this;
			std::unique_ptr<NodeComponent> uPtr(component);
			m_components.push_back(std::move(uPtr));

			m_componentArray[getComponentTypeId<ComponentType>()] = component;

			m_componentBitset[getComponentTypeId<ComponentType>()] = true;

			return *component;
		}

		template <class ComponentType>
		bool hasComponent() const
		{
			return m_componentBitset[getComponentTypeId<ComponentType>()];
		}

		template <class ComponentType>
		NodeComponent& getComponent()
		{
			assert(hasComponent<ComponentType>());
			auto ptr = m_componentArray[getComponentTypeId<ComponentType>()];
			return *reinterpret_cast<ComponentType*>(ptr);
		}

	protected:
		friend class Renderer;

		virtual void onStartCurrent();
		void onStartChildren();

		virtual void updateCurrent(f32 dt);
		void updateChildren(f32 dt);

		void draw(Renderer& renderer, Transform t = Transform()) const;
		virtual void drawCurrent(Renderer& renderer, Transform t) const;
		void drawChildren(Renderer& renderer, Transform t) const;

		std::deque<UPtr> m_children;
		//GroupedComponentMap m_groupedComponents;
		
		std::vector<NodeComponent::UPtr> m_components;
		ComponentArray m_componentArray;
		ComponentBitset m_componentBitset;
	};
}


#endif // !DUNJUN_SCENE_SCENENODE_HPP
