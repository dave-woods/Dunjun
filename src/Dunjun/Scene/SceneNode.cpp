#include <Dunjun/Scene/SceneNode.hpp>

#include <algorithm>
#include <sstream>

namespace Dunjun
{
	namespace Impl
	{
		inline SceneNode::ID getUniqueSceneNodeId()
		{
			LOCAL_PERSIST SceneNode::ID lastId = 0;
			return lastId++;
		}
	}

	SceneNode::SceneNode()
		: m_children()
		, id(Impl::getUniqueSceneNodeId())
		, name("")
		, transform()
		, parent(nullptr)
		, visible(true)
	{
		std::stringstream ss;
		ss << "node_" << id;
		name = ss.str();
	}
	
	SceneNode& SceneNode::attachChild(UPtr child)
	{
		child->parent = this;
		m_children.push_back(std::move(child));
		return *this;
	}

	SceneNode::UPtr SceneNode::detachChild(const SceneNode& node)
	{
		auto found = std::find_if(
			m_children.begin(),
			m_children.end(),
			[&node](UPtr& child) //lambda expression
			{
				return child.get() == &node;
			});

		if (found != m_children.end()) //child was found
		{
			UPtr result = std::move(*found);

			result->parent = nullptr;
			m_children.erase(found);

			return result;
		}
		// no child found
		return nullptr;
	}
	
	SceneNode* SceneNode::findChildById(const usize id) const
	{
		for (const UPtr& child : m_children)
		{
			if (child->id == id)
				return child.get();
		}
		return nullptr;
	}

	SceneNode* SceneNode::findChildByName(const std::string& name) const
	{
		for (const UPtr& child : m_children)
		{
			if (child->name == name)
				return child.get();
		}
		return nullptr;
	}

	Transform SceneNode::getGlobalTransform() const
	{
		Transform result;

		for (const SceneNode* node = this; node != nullptr; node = node->parent)
			result *= node->transform;


		return result;
	}

	void SceneNode::onStart()
	{
		onStartCurrent();
		onStartChildren();
		for (auto& component : m_components)
			component->onStart();
	}

	void SceneNode::update(f32 dt)
	{
		updateCurrent(dt);
		updateChildren(dt);
		for (auto& component : m_components)
			component->update(dt);
	}

	void SceneNode::draw(SceneRenderer& renderer, Transform t) const
	{
		if (!visible)
			return;

		t *= this->transform;
		drawCurrent(renderer, t);
		drawChildren(renderer, t);
		for (auto& component : m_components)
			component->draw(renderer, t);
	}

	void SceneNode::onStartCurrent()
	{
		// Do nothing by default
	}

	void SceneNode::onStartChildren()
	{
		for (UPtr& child : m_children)
			child->onStart();
	}

	void SceneNode::updateCurrent(f32 dt)
	{
		// Do nothing by default
	}

	void SceneNode::updateChildren(f32 dt)
	{
		for (UPtr& child : m_children)
			child->update(dt);
	}

	void SceneNode::drawCurrent(SceneRenderer& renderer, Transform t) const
	{
		// Do nothing by default
	}

	void SceneNode::drawChildren(SceneRenderer& renderer, Transform t) const
	{
		for (const UPtr& child : m_children)
			child->draw(renderer, t);
	}
}