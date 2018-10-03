#include "Engine/Math/MatrixStack.hpp"

MatrixStack::MatrixStack()
{

}

MatrixStack::~MatrixStack()
{

}

void MatrixStack::Push(const Matrix4 &matrix)
{
	Matrix4 topMatrix = m_matrixStack.back();
	topMatrix.ConcatenateTransform(matrix);
	m_matrixStack.push_back(topMatrix);
}

void MatrixStack::Pop()
{
	m_matrixStack.pop_back();
}

void MatrixStack::PushDirect(const Matrix4 &newTop)
{
	m_matrixStack.push_back(newTop);
}

Matrix4 MatrixStack::GetTop()
{
	return m_matrixStack.back();
}

