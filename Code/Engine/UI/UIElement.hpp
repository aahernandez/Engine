#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/AABB2D.hpp"

#include <vector>

class SimpleRenderer;

struct UIMetric_t
{
	Vector2 ratio;
	Vector2 offset;
};

class UIElement
{
public:
	UIMetric_t m_position;
	UIMetric_t m_size;
	Vector2 m_pivot;

	UIElement *m_parent;
	std::vector<UIElement*> m_children;

	UIElement();
	~UIElement();

	UIElement* GetParent();
	void AddChild(UIElement* newChild);
	void RemoveChild(UIElement *oldChild);
	void RemoveSelf();

	template <typename T>
	T* CreateChild()
	{
		T *newChild = new T();
		AddChild(newChild);
		return newChild;
	}

	void SetPosition(Vector2 ratio, Vector2 unit);
	void SetSize(Vector2 ratio, Vector2 unit);
	void SetPivot(Vector2 newPivot);
	Vector2 GetSize();
	AABB2D GetElementBounds();
	AABB2D GetLocalBounds();
	Vector2 GetLocalPosition();
	Matrix4 GetLocalTransform();
	Matrix4 GetWorldTransform();

	virtual void Render(SimpleRenderer *renderer) = 0;
	void RenderChildElements(SimpleRenderer *renderer);
};