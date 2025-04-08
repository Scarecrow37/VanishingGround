#pragma once

class Component
{
protected:
	explicit Component();
	virtual ~Component() = default;

public:
	const unsigned int GetID() const { return _ID; }

private:
	static unsigned int	_globalID;
	unsigned int		_ID{ 0 };
};