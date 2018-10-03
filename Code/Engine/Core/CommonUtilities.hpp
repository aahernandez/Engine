#pragma once

template <typename T>
void SwapValues(T& primaryValue, T& secondaryValue)
{
	T tempValue = primaryValue;
	primaryValue = secondaryValue;
	secondaryValue = tempValue;
}
