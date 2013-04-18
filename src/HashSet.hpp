#pragma once

#include <boost/integer.hpp>
#include <cstring>

// TODO move to utility class
#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

template<unsigned int BIT_WIDTH>
struct HashSetTraits {
    static const int bitWidth = BIT_WIDTH;
    static const int minCapacity = 1 << 5;
    typedef typename boost::int_t<BIT_WIDTH>::least valueType;
    static const int maxCapacity = std::numeric_limits<valueType>::max();
};

template<class hashSetT>
class HashSet {
public:
    typedef typename hashSetT::valueType valueType;
    static const int bitWidth = hashSetT::bitWidth;
    static const int invalidElement = 0;
    static const int minCapacity = hashSetT::minCapacity;
    static const int maxCapacity = hashSetT::maxCapacity;
private:
    valueType m_size;
    valueType m_capacity;
    valueType* m_table;
    bool m_containsInvalidElement;

    // TODO move to trait!
    valueType getIndex(valueType value) const
    {
        valueType h = value;
        // Copied from Apache's AbstractHashedMap; prevents power-of-two collisions.
        h += ~(h << 9);
        h ^= (h >> 14); // TODO was >>>
        h += (h << 4);
        h ^= (h >> 10); // TODO was >>>
        // Power of two trick.
        return h & (m_capacity-1); // OPTIMIZE precompute m_capacity-1
    }

    valueType findOrEmpty(valueType value) const
    {
        valueType index = getIndex(value);
        do {
            const valueType existing = m_table[index];
            if (value == existing || existing == invalidElement)
                return index;
            index++;
            index = index % m_capacity; // TODO use shift operations
        } while (true);
    }

    valueType find(valueType value) const
    {
        valueType index = findOrEmpty(value);
        if (m_table[index] == invalidElement)
            return -1; // TODO correct value?
        return index;
    }

    bool internalAddNoCount(valueType value)
    {
        valueType index = findOrEmpty(value);
        if (m_table[index] == invalidElement) {
            m_table[index] = value;
            return true;
        }
        return false;
    }

    bool internalAddInvalidNoCount()
    {
        bool result = !m_containsInvalidElement;
        m_containsInvalidElement = true;
        return result;
    }

    bool internalAdd(valueType value) {
        bool result = value==invalidElement ? internalAddInvalidNoCount() : internalAddNoCount(value);
        if (result)
            m_size++;
        return result;
    }

    void ensureCapacityFor(valueType expectedSize)
    {
        // compute capacity
        if (likely(m_table != 0 && 3 * m_capacity > 4 * expectedSize))
            return; // nothing to do
        int newCapacity = expectedSize;
        if (newCapacity < 1)
            newCapacity = 1;
        while (3 * newCapacity <= 4 * expectedSize) {
            int tmp = newCapacity * 2;
            if (tmp < newCapacity || tmp > maxCapacity) {
                // overflow detected!
                newCapacity = maxCapacity;
                break;
            }
            newCapacity = tmp;
        }

        // replace table
        valueType* oldTable = m_table;
        valueType oldCapacity = m_capacity;
        m_table = new valueType[newCapacity];
        m_capacity = newCapacity;
        memset(m_table, 0, m_capacity * sizeof(valueType));
        // now the table is correctly set up and empty (except m_size and m_containsInvalidElement)

        // fill new table with old values
        if (oldTable)
        {
            for (int i=0; i<oldCapacity; ++i) { // TODO replace by boost::iterator (if it's as performant)
                valueType element = oldTable[i];
                if (element != invalidElement)
                    internalAddNoCount(element);
            }
            // free old table
            delete[] oldTable;
        }
    }

public:

    HashSet() : HashSet(minCapacity) {}

    HashSet(int capacity) : m_size(0), m_table(0), m_containsInvalidElement(false)
    {
        ensureCapacityFor(capacity);
    }

    ~HashSet()
    {
        if (m_table)
            delete[] m_table;
    }

    valueType size() const {
        return m_size;
    }

    valueType capacity() const {
        return m_capacity;

    }

    void clear()
    {
        m_containsInvalidElement = false;
        if (m_table)
            delete[] m_table;
        m_size = 0;
        m_capacity = minCapacity;
        m_table = new valueType[capacity];
    }

    bool operator[](valueType value) const
    {
        return m_table[findOrEmpty(value)] != invalidElement || unlikely(value == invalidElement && m_containsInvalidElement);
    }

    bool operator+=(valueType value)
    {
        ensureCapacityFor(value);
        return internalAdd(value);
    }

};
