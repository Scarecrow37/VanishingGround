#pragma once
#include <directxtk/SimpleMath.h>

namespace GE
{
	class Matrix4x4 : public DirectX::SimpleMath::Matrix 
	{
	public:
		Matrix4x4() = default;
		Matrix4x4(const DirectX::SimpleMath::Matrix& matrix) : DirectX::SimpleMath::Matrix(matrix)	{}
	};
}