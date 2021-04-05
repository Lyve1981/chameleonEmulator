#pragma once

#include <algorithm>
#include <array>
#include <atomic>
#include <cassert>

#include "semaphore.h"

template<typename T, size_t C> class RingBuffer
{
public:
	RingBuffer() : m_insertPos(0), m_removePos(0), m_usage(0), m_readSem(0), m_writeSem(C)
	{
	}

	size_t capacity() const		{ return C; }
	bool empty() const			{ return m_usage == 0; }
	bool full() const			{ return m_usage == C; }
	size_t size() const			{ return m_usage; }
	size_t remaining() const	{ return (C - m_usage); }

	void push_back( const T& _val )
	{
//		assert( m_usage < C && "ring buffer is already full!" );

		m_writeSem.wait();

		m_data[m_insertPos] = _val;
		updateCounter(m_insertPos);

		// usage need to be incremented AFTER data has been written, otherwise, reader thread would read incomplete data
		++m_usage;

		m_readSem.notify();
	}

	T pop_front()
	{
		m_readSem.wait();

		const T res = front();
//		assert( m_usage > 0 && "ring buffer is already empty!" );

		updateCounter(m_removePos);

		--m_usage;

		m_writeSem.notify();

		return res;
	}

	void removeAt( size_t i )
	{
		if( !i )
		{
			pop_front();
			return;
		}

		convertIdx(i);

		std::swap( m_data[i], m_data[m_removePos] );

		pop_front();
	}

	T& operator[](size_t i)
	{
		return get(i);
	}

	const T& operator[](size_t i) const
	{
		return const_cast< RingBuffer<T,C>* >(this)->get(i);
	}

	const T& front() const
	{
		return m_data[m_removePos];
	}
	
	T& front()
	{
		return m_data[m_removePos];
	}

	void clear()
	{
		while( !empty() )
			pop_front();
	}

private:
	static void updateCounter( size_t& _counter )
	{
		if( (++_counter) == C )
			_counter = 0;
	}

	T& get( size_t i )
	{
		convertIdx( i );

		return m_data[i];
	}

	void convertIdx( size_t& _i ) const
	{
		_i += m_removePos;

		if( _i >= C )
			_i -= C;
	}

	std::array<T,C>		m_data;

	size_t				m_insertPos;
	size_t				m_removePos;
	std::atomic<size_t>	m_usage;
	ceLib::Semaphore	m_readSem;
	ceLib::Semaphore	m_writeSem;

public:
	static void test()
	{
		RingBuffer<int,10>	rb;

		assert( rb.size() == 0 );
		assert( rb.empty() );
		assert( rb.remaining() == 10 );

		rb.push_back( 3 );
		rb.push_back( 4 );
		rb.push_back( 5 );
		rb.push_back( 6 );

		assert( rb.size() == 4 );
		assert( !rb.empty() );
		assert( rb.remaining() == 6 );

		assert( rb[2] == 5 );

		rb.pop_front();

		assert( rb.size() == 3 );
		assert( !rb.empty() );
		assert( rb.remaining() == 7 );

		assert( rb[2] == 6 );
		assert( rb[0] == 4 );
		assert( rb.front() == 4 );

		rb.pop_front();
		rb.pop_front();
		rb.pop_front();

		assert( rb.size() == 0 );
		assert( rb.empty() );
		assert( rb.remaining() == 10 );

		rb.push_back(77);

		assert( rb.size() == 1 );
		assert( !rb.empty() );
		assert( rb.remaining() == 9 );

		assert( rb.front() == 77 );
		assert( rb[0] == 77 );
	}
};
