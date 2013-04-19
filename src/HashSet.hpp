#pragma once

#include <boost/integer.hpp>
#include <cstring>
#include <iostream>

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
        h ^= (h >> 14);
        h += (h << 4);
        h ^= (h >> 10);
        // Power of two trick.
        return h & (m_capacity-1);
    }

    valueType findOrEmpty(valueType value) const
    {
        valueType index = getIndex(value);
        do {
            const valueType existing = m_table[index];
            if (value == existing || existing == invalidElement)
                return index;
            index++;
            index = index % m_capacity; // TODO use bit operations
        } while (true);
    }

    valueType find(valueType value) const
    {
        valueType index = findOrEmpty(value);
        if (m_table[index] == invalidElement)
            return -1; // TODO correct value?
        return index;
    }

    bool internalAddValidNoCount(valueType value)
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

    bool internalAdd(valueType value)
    {
        bool result = value==invalidElement ? internalAddInvalidNoCount() : internalAddValidNoCount(value);
        if (result)
            m_size++;
        return result;
    }

    void internalAddOnlyValidNoCount(valueType* values, valueType n)
    {
        for (valueType i=0; i<n; ++i)
        {
            valueType value = values[i];
            if (value != invalidElement)
                internalAddValidNoCount(value);
        }
    }

    inline bool sizeFitsIntoCapacity(valueType expectedSize, valueType capacity)
    {
        return 3 * m_capacity > 4 * expectedSize;
    }

    int computeCapacityForSize(valueType expectedSize)
    {
        long newCapacity = m_capacity;
        if (newCapacity < 1)
            newCapacity = 1;
        while (!sizeFitsIntoCapacity(expectedSize, newCapacity)) {
            int tmp = newCapacity * 2;
            if (tmp < newCapacity || tmp > maxCapacity) {
                // overflow detected!
                newCapacity = maxCapacity;
                break;
            }
            newCapacity = tmp;
        }
        return newCapacity;
    }

    void adjustCapacityTo(valueType newCapacity)
    {
        m_table = new valueType[newCapacity];
        m_capacity = newCapacity;
        memset(m_table, 0, m_capacity * sizeof(valueType));
    }


    void ensureCapacityFor(valueType expectedSize)
    {
        // fast-path
        if (likely(m_table != 0 && sizeFitsIntoCapacity(expectedSize, m_capacity)))
            return; // nothing to do

        valueType oldCapacity = m_capacity;
        valueType newCapacity = computeCapacityForSize(expectedSize);
        if (newCapacity == oldCapacity)
            return;
        valueType* oldTable = m_table;
        adjustCapacityTo(newCapacity);
        if (oldTable)
        {
            internalAddOnlyValidNoCount(oldTable, oldCapacity);
            delete[] oldTable;
        }
    }

    HashSet(int capacity) : m_size(0), m_capacity(capacity), m_table(nullptr), m_containsInvalidElement(false)
    {
        adjustCapacityTo(capacity); // TODO wrong!!!
    }

public:

    HashSet() : HashSet(minCapacity) {}

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
