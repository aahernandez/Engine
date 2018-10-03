#pragma once
#include "Engine/Math/Matrix4.hpp"
#include <vector>

class MatrixStack
{
public:
	std::vector<Matrix4> m_matrixStack;

	MatrixStack();
	~MatrixStack();

	void Push(const Matrix4 &matrix);
	void PushDirect(const Matrix4 &newTop);
	void Pop();

	Matrix4 GetTop();
};