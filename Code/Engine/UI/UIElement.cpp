#include "Engine/UI/UIElement.hpp"
#include "Engine/Math/Matrix4.hpp"

UIElement::UIElement()
	: m_parent(nullptr)
{

}

UIElement::~UIElement()
{

}

UIElement* UIElement::GetParent()
{
	return m_parent;
}

void UIElement::AddChild(UIElement* newChild)
{
	m_children.push_back(newChild);
	newChild->m_parent = this;
}

void UIElement::RemoveChild(UIElement *oldChild)
{
	std::vector<UIElement*>::iterator elementIter;
	for (elementIter = m_children.begin(); elementIter != m_children.end(); ++elementIter)
	{
		UIElement *currentElement = *elementIter;
		if (currentElement == oldChild)
		{
			elementIter = m_children.erase(elementIter);
		}
	}
}

void UIElement::RemoveSelf()
{
	m_parent->RemoveChild(this);
}

void UIElement::SetPosition(Vector2 ratio, Vector2 unit)
{
	m_position.ratio = ratio;
	m_position.offset = unit;
}

void UIElement::SetSize(Vector2 ratio, Vector2 unit)
{
	m_size.ratio = ratio;
	m_size.offset = unit;
}

void UIElement::SetPivot(Vector2 newPivot)
{
	m_pivot = newPivot;
}

Vector2 UIElement::GetSize()
{
	if (m_parent != nullptr)
		return (m_parent->GetSize() * m_size.ratio) + m_size.offset;
	else
		return m_size.offset;
}

AABB2D UIElement::GetElementBounds()
{
	Vector2 mySize = GetSize();
	return AABB2D(Vector2::ZERO, mySize);
}

AABB2D UIElement::GetLocalBounds()
{
	AABB2D elementBounds = GetElementBounds();

	AABB2D localBounds = elementBounds;

	if (m_parent != nullptr)
	{
		Vector2 parentSize = m_parent->GetSize();
		AABB2D parentLocalBounds = m_parent->GetLocalBounds();
		Vector2 position = (parentLocalBounds.maxs * m_position.ratio) + m_position.offset + parentLocalBounds.mins;
		Vector2 pivotPosition = (GetSize() * m_pivot) + localBounds.mins;
		localBounds.Translate(position - pivotPosition);
	}
	return localBounds;
}

Vector2 UIElement::GetLocalPosition()
{
	AABB2D localBounds = GetLocalBounds();
	Vector2 position = localBounds.GetPointFromNormalizedPoint(m_pivot);

	if (m_parent == nullptr)
		return position;

	AABB2D parentBounds = m_parent->GetLocalBounds();
	Vector2 localPosition = RangeMap2D(position, Vector2(localBounds.mins.x, localBounds.maxs.x), Vector2(localBounds.mins.y, localBounds.maxs.y), 
		Vector2(parentBounds.mins.x, parentBounds.maxs.x), Vector2(parentBounds.mins.y, parentBounds.maxs.y)); 

	return localPosition;
}

Matrix4 UIElement::GetLocalTransform()
{
	AABB2D localBounds = GetLocalBounds();
	Vector2 position = localBounds.GetPointFromNormalizedPoint(m_pivot);

	Matrix4 newMatrix;
	newMatrix.Translate(position);
	return newMatrix;
}

Matrix4 UIElement::GetWorldTransform()
{
	Matrix4 worldTransform = GetLocalTransform();

	if (m_parent != nullptr)
	{
		Vector3 parentTranslation = m_parent->GetWorldTransform().GetTranslation();
		worldTransform.Translate(parentTranslation);
	}
	
	return worldTransform;
}

void UIElement::RenderChildElements(SimpleRenderer *renderer)
{
	std::vector<UIElement*>::iterator elementIter;
	for (elementIter = m_children.begin(); elementIter != m_children.end(); ++elementIter)
	{
		UIElement *currentElement = *elementIter;
		currentElement->Render(renderer);
	}
}

