#pragma once

namespace csl::ut
{
	template<typename TOp>
	class HashMap
	{
		inline static constexpr size_t INVALID_KEY = static_cast<size_t>(-1);
		
	protected:
		struct Elem
		{
			size_t m_Hash;
			size_t m_Key;
			size_t m_Value;
		};
		
		Elem* m_pElements;
		size_t m_Length;
		size_t m_Capacity;
		size_t m_CapacityMax;
		fnd::IAllocator* m_pAllocator;
		TOp m_Operation{};

	protected:
		size_t CalcResize(size_t capacity)
		{
			size_t result;
			for (result = 16; result < capacity; result *= 2);

			if (result < 16)
				result = 16;

			return result;
		}

		Elem* AllocateMemory(size_t count)
		{
			return static_cast<Elem*>(m_pAllocator->Alloc(sizeof(Elem), alignof(Elem)));
		}

		size_t GetCapacity()
		{
			return m_Capacity & ~csl::ut::SIGN_BIT;
		}

		void ResizeTbl(size_t capacity)
		{
			size_t oldCap = GetCapacity();
			Elem* oldElements = m_pElements;

			m_Capacity = capacity;
			m_CapacityMax = capacity - 1;
			m_Length = 0;

			m_pElements = AllocateMemory(capacity);
			memset(m_pElements, INVALID_KEY, sizeof(Elem) * capacity);

			if (oldElements)
			{
				for (size_t i = 0; i < oldCap; i++)
				{
					const Elem* pElem = &oldElements[i];

					if (pElem->m_Key != INVALID_KEY)
					{
						Insert(pElem->m_Key, pElem->m_Value);
					}
				}

				if (m_pAllocator)
					m_pAllocator->Free(oldElements);
			}
		}

		void Reserve(size_t capacity)
		{
			size_t cap = CalcResize(capacity);
			if (GetCapacity() < cap)
				ResizeTbl(cap);
		}
		
		HashMap(fnd::IAllocator* pAllocator)
		{
			m_pAllocator = pAllocator;
		}

	public:
		void reserve(size_t capacity)
		{
			Reserve(capacity);
		}

	protected:
		void Insert(size_t key, size_t value)
		{
			size_t hash = m_Operation.hash(key);
			if (m_Length || GetCapacity())
			{
				if (2 * m_Length >= GetCapacity())
				{
					ResizeTbl(2 * GetCapacity());
				}
			}
			else
			{
				ResizeTbl(CalcResize(GetCapacity()));
			}

			size_t idx = hash & m_CapacityMax;
			Elem* pElem = &m_pElements[idx];

			if (pElem->m_Key == INVALID_KEY)
			{
				pElem->m_Hash = hash;
				pElem->m_Key = key;
				pElem->m_Value = value;
				m_Length++;
			}
			else
			{
				while (pElem->m_Hash != hash || pElem->m_Key != key)
				{
					idx = m_CapacityMax & (idx + 1);
					pElem = &m_pElements[idx];

					if (pElem->m_Key == INVALID_KEY)
					{
						pElem->m_Hash = hash;
						pElem->m_Key = key;
						pElem->m_Value = value;
						m_Length++;
						break;
					}
				}
			}
		}

	public:
		struct iterator
		{
			const HashMap* m_pOwner;
			size_t m_CurIdx;

			friend bool operator==(const iterator& a, const iterator& b) { return a.m_CurIdx == b.m_CurIdx; }
			friend bool operator!=(const iterator& a, const iterator& b) { return a.m_CurIdx != b.m_CurIdx; }
		};

		iterator end() const
		{
			return iterator{ this, m_CapacityMax + 1 };
		}

	protected:
		iterator Find(size_t key) const
		{
			if (!m_pElements)
				return end();

			const size_t hash = const_cast<TOp&>(m_Operation).hash(key);
			size_t idx = hash & m_CapacityMax;
			const Elem* pElem = &m_pElements[idx];
			
			if (pElem->m_Key == INVALID_KEY)
				return end();
			
			while (pElem->m_Hash != hash || pElem->m_Key != key)
			{
				idx = m_CapacityMax & (idx + 1);
				pElem = &m_pElements[idx];

				if (pElem->m_Hash == INVALID_KEY)
					return end();
			}

			return iterator{ this, idx };
		}

		size_t GetValue(iterator iter) const
		{
			return iter.m_pOwner->m_pElements[iter.m_CurIdx].m_Value;
		}

		void Erase(size_t key)
		{
			auto result = Find(key);

			if (result == end())
				return;

			Elem* pElem = &m_pElements[result.m_CurIdx];
			pElem->m_Hash = INVALID_KEY;
			pElem->m_Key = INVALID_KEY;
			pElem->m_Value = INVALID_KEY;
			m_Length--;
		}
	};
}