#pragma once
namespace GE
{
	class IBase
	{	
	protected:
		explicit IBase() = default;
		virtual ~IBase() = default;
		IBase(const IBase&) = delete;
		IBase(IBase&&) = delete;
		IBase& operator=(const IBase&) = delete;
		IBase& operator=(IBase&&) = delete;

	public:
		virtual void Release() = 0;
	};
}