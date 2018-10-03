#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

unsigned int AtomicAdd(unsigned int volatile *ptr, unsigned int const value) 
{
	return (unsigned int) ::InterlockedAddNoFence( (LONG volatile*)ptr, (LONG)value );
}

unsigned int AtomicIncrement(unsigned int *ptr) 
{
	return (unsigned int) ::InterlockedIncrementNoFence( (LONG volatile*)ptr );
}

unsigned int AtomicDecrement(unsigned int *ptr) 
{
	return (unsigned int) ::InterlockedDecrementNoFence( (LONG volatile*)ptr );
}

unsigned int CompareAndSet(unsigned int volatile *ptr, unsigned int const comparand, unsigned int const value)
{
	return ::InterlockedCompareExchange( ptr, value, comparand );
}

// bool CompareAndSet128(unsigned long long volatile data[2], unsigned long long comparand[2], unsigned long long value[2] )
// {
// 	return 1 == ::InterlockedCompareExchange128( (long long volatile*)data, value[1], value[0], (long long*)comparand );  
// }

template <typename T> T* CompareAndSetPointer( T *volatile *ptr, T *comparand, T *value ) 
{
	return (T*)::InterlockedCompareExchangePointerNoFence( (PVOID volatile*)ptr, (PVOID)value, (PVOID)comparand );
}

