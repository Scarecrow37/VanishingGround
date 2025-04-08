#pragma once

class Base abstract
{
protected:
	explicit Base() = default;
	virtual ~Base() = default;

public:
	inline void Release()
	{
		Free();
		delete this;
	}

protected:
	virtual void Free() = 0;
};