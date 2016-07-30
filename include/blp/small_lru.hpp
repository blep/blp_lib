#pragma once

#include <stdint.h>
#include <assert.h>

namespace blp {

/// Inserts nbBits bitsToInsert in value's bits at position insertAt.
/// bitsToInsert must be less than (1 << nbBits)
template<typename T, typename U>
inline T bitsInsert( T value, U bitsToInsert, int insertAt, int nbBits )
{
    T preserveMask = (1 << insertAt) - 1;
    T shiftedBits = (value & ~preserveMask) << nbBits;
    return (value & preserveMask) | shiftedBits | ( static_cast<T>(bitsToInsert) << insertAt );
}


template<typename T>
inline T bitsRemove( T value, int removeAt, int nbBits )
{
    T preserveMask = ( 1 << removeAt ) - 1;
    T shiftedBits = (value  >> nbBits) & ~preserveMask;
    return ( value & preserveMask ) | shiftedBits;
}


namespace small_lru {
    
    namespace impl {

        template<const int size>
        struct BitPerIndex
        {
            static const int value = 1 + BitPerIndex< (size >> 1) >::value;
        };

        template<>
        struct BitPerIndex<0>
        {
            static const int value = 0;
        };

        template<>
        struct BitPerIndex<1>
        {
            static const int value = 1;
        };

        template<>
        struct BitPerIndex<8>
        {
            static const int value = 3;
        };

        template<>
        struct BitPerIndex<16>
        {
            static const int value = 4;
        };

    } // namespace impl



    /**
     * Represents the indexes of LRU entries for quick eviction.
     *
     * The indexes are packed in a single integer and eviction is a matter of masking and shifting.
     * uint32_t: max 8 entries
     * uint64_t: max 16 entries
     *
     * Inspired from:
     * https://fgiesen.wordpress.com/2016/03/07/repeated-match-offsets-in-bitknit/
     *
     * @param T storage type, MUST be unsigned (either uint32_t or uint64_t)
     */
    template<typename T=int32_t, const int nbEntries=8, const int bitsPerEntry=impl::BitPerIndex<nbEntries>::value>
    class SmallLRUIndex
    {
    public:
        static const int bitsPerIndex = bitsPerEntry;
        static_assert( sizeof(T)*8 >= size_t(bitsPerIndex * nbEntries), "nbEntries does not fit on storage type T" );
        static const T entryMask = (1 << bitsPerEntry) - 1;
        static const int size = nbEntries;
        
        inline explicit SmallLRUIndex()
        {
            T indexes = 0;
            for ( int index=nbEntries-1; index >= 0; --index )
            {
                indexes = (indexes << bitsPerEntry) | index;
            }
            indexes_ = indexes;
        }

        inline explicit SmallLRUIndex( T indexes )
            : indexes_( indexes )
        {
        }
        
        inline int operator[](int pos) const
        {
            int entryBitOffset = pos * bitsPerIndex;
            return (indexes_ >> entryBitOffset) & entryMask;
        }
        
        T indexes_;
    };


    
    /// Move the oldest index to the front and return it.
    template<typename T, const int nbEntries, const int bitsPerEntry>
    inline int evict( SmallLRUIndex<T, nbEntries, bitsPerEntry> &lru )
    {
        using Lru = SmallLRUIndex<T, nbEntries, bitsPerEntry>;
        T indexes = lru.indexes_;
        int entryBitOffset = (nbEntries-1) * bitsPerEntry;
        int evictedIndex = (indexes >> entryBitOffset) & Lru::entryMask;
        indexes = (indexes << bitsPerEntry) | T(evictedIndex);
        lru.indexes_ = indexes;
        return evictedIndex;
    }
    
    /// Move the oldest index at the given position and return it
    template<typename T, const int nbEntries, const int bitsPerEntry>
    inline int evictAt(SmallLRUIndex<T, nbEntries, bitsPerEntry> &lru, int insertPos)
    {
        using Lru = SmallLRUIndex<T, nbEntries, bitsPerEntry>;
        T indexes = lru.indexes_;
        int lastEntryBitOffset = ( nbEntries - 1 ) * bitsPerEntry;
        int evictedIndex = (indexes >> lastEntryBitOffset) & Lru::entryMask;
        // Remove evicted index
        indexes = indexes & ~(Lru::entryMask << lastEntryBitOffset);

        // insert the index: preserve all bits before the index and shift the other
        int insertBitOffset = insertPos * Lru::bitsPerIndex;
        T insertMask = ~T( 0 ) << insertBitOffset;
        T oldShiftedBits = ( indexes & insertMask) << Lru::bitsPerIndex;
        indexes = oldShiftedBits | ( indexes & ~insertMask ) | ( T(evictedIndex) << insertBitOffset );

        lru.indexes_ = indexes;
        return evictedIndex;
    }
    
    /// Search for the position of the given index and move it to the given position
    template<typename T, const int nbEntries, const int bitsPerEntry>
    inline void moveIndexToFront( SmallLRUIndex<T, nbEntries, bitsPerEntry> &lru, int index )
    {
        using Lru = SmallLRUIndex<T, nbEntries, bitsPerEntry>;
        int originalPos = findIndexPos( lru, index );
        assert( originalPos >= 0 );
        // 1) remove the index: preserve all bits before the index and shift the other
        T indexes = lru.indexes_;
        T removeMask = ~T(0) << (originalPos * Lru::bitsPerIndex);
        indexes = ((indexes >> Lru::bitsPerIndex) & removeMask) | (indexes & ~removeMask);
        // 2) insert the index at the front
        indexes = (indexes << Lru::bitsPerIndex) | T(index);
        lru.indexes_ = indexes;
    }
    
    /// Search for the position of the given index and move it to the given position
    template<typename T, const int nbEntries, const int bitsPerEntry>
    inline void moveIndexTo( SmallLRUIndex<T, nbEntries, bitsPerEntry> &lru, int index, int insertPos )
    {
        using Lru = SmallLRUIndex<T, nbEntries, bitsPerEntry>;
        int originalPos = findIndexPos( lru, index );
        assert( originalPos >= 0 );
        // 1) remove the index: preserve all bits before the index and shift the other
        T indexes = lru.indexes_;
        T removeMask = ~T(0) << (originalPos * Lru::bitsPerIndex);
        indexes = ((indexes >> Lru::bitsPerIndex) & removeMask) | (indexes & ~removeMask);
        // 2) insert the index: preserve all bits before the index and shift the other
        int insertBitOffset = insertPos * Lru::bitsPerIndex;
        T insertMask = ~T(0) << insertBitOffset;
        T shiftedBits = (indexes & insertMask ) << Lru::bitsPerIndex;
        indexes = shiftedBits | (indexes & ~insertMask) | ( T(index) << insertBitOffset );
        lru.indexes_ = indexes;
    }
    
    template<typename T, const int nbEntries, const int bitsPerEntry>
    inline int findIndexPos( SmallLRUIndex<T, nbEntries, bitsPerEntry> lru, int index )
    {
        using Lru = SmallLRUIndex<T, nbEntries, bitsPerEntry>;
        T indexes = lru.indexes_;
        for ( int pos = 0; pos < Lru::size; ++pos )
        {
            if ( static_cast<int>(indexes & Lru::entryMask) == index )
            {
                return pos;
            }
            indexes >>= Lru::bitsPerIndex;
        }
        return -1;
    }

} // namespace small_lru

using small_lru::SmallLRUIndex;
using SmallLRUIndex8 = SmallLRUIndex<uint32_t,8,4>;
using SmallLRUIndex16 = SmallLRUIndex<uint64_t,16>;


} // namespace blp
