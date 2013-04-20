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
    typedef typename boost::int_t<2*BIT_WIDTH>::least indexType; // TODO this is a quick hack.
    static const int maxCapacity = std::numeric_limits<valueType>::max();
};

template<class hashSetT>
class HashSet {
public:
    typedef typename hashSetT::valueType valueType;
    typedef typename hashSetT::indexType indexType;
    static const int bitWidth = hashSetT::bitWidth;
    static const int invalidElement = 0;
    static const int minCapacity = hashSetT::minCapacity;
private:
    indexType m_size;
    indexType m_capacity;
    valueType* m_table;
    bool m_containsInvalidElement;

    // TODO move to trait!
    indexType getIndex(valueType value) const
    {
        indexType h = value;
        // Copied from Apache's AbstractHashedMap; prevents power-of-two collisions.
        h += ~(h << 9);
        h ^= (h >> 14);
        h += (h << 4);
        h ^= (h >> 10);
        // Power of two trick.
        return h & (m_capacity-1);
    }

    indexType findOrEmpty(indexType value) const
    {
        indexType index = getIndex(value);
        do {
            const valueType existing = m_table[index];
            if (value == existing || existing == invalidElement)
                return index;
            index++;
            index = index % m_capacity; // TODO use bit operations
        } while (true);
    }

    indexType find(valueType value) const
    {
        indexType index = findOrEmpty(value);
        if (m_table[index] == invalidElement)
            return -1; // TODO correct value?
        return index;
    }

    bool internalAddValidNoCount(valueType value)
    {
        indexType index = findOrEmpty(value);
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

    void internalAddOnlyValidNoCount(valueType* values, indexType n)
    {
        for (indexType i=0; i<n; ++i)
        {
            valueType value = values[i];
            if (value != invalidElement)
                internalAddValidNoCount(value);
        }
    }

    inline bool sizeFitsIntoCapacity(indexType expectedSize, indexType capacity)
    {
        // expectedSize < 0.75 * m_capacity
        return 4 * expectedSize < 3 * capacity;
    }

    int computeCapacityForSize(indexType expectedSize)
    {
        indexType newCapacity = m_capacity;
        while (!sizeFitsIntoCapacity(expectedSize, newCapacity)) {
            newCapacity = newCapacity * 2;
        }
        return newCapacity;
    }

    void adjustCapacityTo(indexType newCapacity)
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

        indexType oldCapacity = m_capacity;
        indexType newCapacity = computeCapacityForSize(expectedSize);
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

    HashSet(indexType capacity) : m_size(0), m_capacity(capacity), m_table(nullptr), m_containsInvalidElement(false)
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

    indexType size() const {
        return m_size;
    }

    indexType capacity() const {
        return m_capacity;

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
