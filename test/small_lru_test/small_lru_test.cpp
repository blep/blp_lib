#include <blp/small_lru.hpp>
#include <cpput3/testing.h>
#include <iostream>



namespace blp { namespace testing {

struct BitsTest : CppUT::TestFixture
{
};

// Giving up implementation of bit insert/remove.
// might still consider doing specific one that make genericity compromise 
// handling only LRU use cases (never insert/remove all bits), bits to insert
// guaranted to fit on nbBits.
CPPUT_REGISTERED_TEST( BitsTest, bitsInsert )
{
    // Check that we don't return correct value when bitsToInsert as more than nbBits.
    CPPUT_ASSERT( bitsInsert( 0x00000000, 0xf, 0, 1 ) != 0x00000001 );
    // Check expected bit insertion behavior
    CPPUT_ASSERT( bitsInsert( 0x00000000u, 0x1u, 0, 1 ) == 0x00000001u );
    CPPUT_ASSERT( bitsInsert( 0x00000000u, 0x1u, 1, 1 ) == 0x00000002u );
    CPPUT_ASSERT( bitsInsert( 0xffffffffu, 0x1u, 1, 1 ) == 0xffffffffu );
    CPPUT_ASSERT( bitsInsert( 0x87654321u, 0xau, 0, 4 ) == 0x7654321au );
    CPPUT_ASSERT( bitsInsert( 0x87654321u, 0xau, 32-4, 4 ) == 0xa7654321u );
    CPPUT_ASSERT( bitsInsert( 0x87654321u, 0xau, 16, 4 ) == 0x765a4321u );
    // undefined behavior (shift value 0 by 32)?
    //CPPUT_ASSERT( bitsInsert( 0x87654321u, 0x12345678u, 0, 32 ) == 0x12345678u );
}






struct SmallLRUTest : CppUT::TestFixture
{
    SmallLRUIndex8 lru8_;
    SmallLRUIndex16 lru16_;
    SmallLRUIndex<uint32_t, 1> lru1_;
    SmallLRUIndex<uint32_t, 2> lru2_;
};


template<class T>
static std::vector<int> asVector( const T &lru )
{
    std::vector<int> indexes;
    indexes.reserve(T::size);
    for ( int index=0; index < T::size; ++index )
    {
        indexes.push_back( lru[index] );
    }
    return indexes;
}


CPPUT_REGISTERED_TEST( SmallLRUTest, init8 )
{
    CPPUT_ASSERT( asVector( lru8_ ) == std::vector<int>( { 0,1,2,3,4,5,6,7 } ) );
}


CPPUT_REGISTERED_TEST( SmallLRUTest, init16 )
{
    CPPUT_ASSERT( asVector( lru16_ ) == std::vector<int>( { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 } ) );
}


CPPUT_REGISTERED_TEST( SmallLRUTest, init1 )
{
    CPPUT_ASSERT( asVector( lru1_ ) == std::vector<int>( { 0 } ) );
}


CPPUT_REGISTERED_TEST( SmallLRUTest, init2 )
{
    CPPUT_ASSERT( asVector( lru2_ ) == std::vector<int>( { 0,1 } ) );
}

CPPUT_REGISTERED_TEST( SmallLRUTest, evict8 )
{
    CPPUT_ASSERT( evict( lru8_ ) == 7 );
    CPPUT_ASSERT( asVector( lru8_ ) == std::vector<int>( { 7,0,1,2,3,4,5,6 } ) );
}

CPPUT_REGISTERED_TEST( SmallLRUTest, evict1 )
{
    CPPUT_ASSERT( evict( lru1_ ) == 0 );
    CPPUT_ASSERT( asVector( lru1_ ) == std::vector<int>( { 0 } ) );
}

CPPUT_REGISTERED_TEST( SmallLRUTest, evict16 )
{
    CPPUT_ASSERT( evict( lru16_ ) == 15 );
    CPPUT_ASSERT( asVector( lru16_ ) == std::vector<int>( { 15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14 } ) );
}

CPPUT_REGISTERED_TEST( SmallLRUTest, evict2 )
{
    CPPUT_ASSERT( evict( lru2_ ) == 1 );
    CPPUT_ASSERT( asVector( lru2_ ) == std::vector<int>( { 1,0 } ) );
}

CPPUT_REGISTERED_TEST( SmallLRUTest, evictAt_8 )
{
    CPPUT_ASSERT( evictAt( lru8_, 0 ) == 7 );
    CPPUT_ASSERT( asVector( lru8_ ) == std::vector<int>( { 7,0,1,2,3,4,5,6 } ) );

    CPPUT_ASSERT( evictAt( lru8_, 1 ) == 6 );
    CPPUT_ASSERT( asVector( lru8_ ) == std::vector<int>( { 7,6,0,1,2,3,4,5 } ) );

    CPPUT_ASSERT( evictAt( lru8_, 7 ) == 5 );
    CPPUT_ASSERT( asVector( lru8_ ) == std::vector<int>( { 7,6,0,1,2,3,4,5 } ) );
}

CPPUT_REGISTERED_TEST( SmallLRUTest, evictAt_16 )
{
    CPPUT_ASSERT( evictAt( lru16_, 0 ) == 15 );
    CPPUT_ASSERT( asVector( lru16_ ) == std::vector<int>( { 15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14 } ) );

    CPPUT_ASSERT( evictAt( lru16_, 1 ) == 14 );
    CPPUT_ASSERT( asVector( lru16_ ) == std::vector<int>( { 15,14,0,1,2,3,4,5,6,7,8,9,10,11,12,13 } ) );

    CPPUT_ASSERT( evictAt( lru16_, 15 ) == 13 );
    CPPUT_ASSERT( asVector( lru16_ ) == std::vector<int>( { 15,14,0,1,2,3,4,5,6,7,8,9,10,11,12,13 } ) );
}

CPPUT_REGISTERED_TEST( SmallLRUTest, evictAt_1 )
{
    CPPUT_ASSERT( evictAt( lru1_, 0 ) == 0 );
    CPPUT_ASSERT( asVector( lru1_ ) == std::vector<int>( { 0 } ) );
}

CPPUT_REGISTERED_TEST( SmallLRUTest, evictAt_2 )
{
    CPPUT_ASSERT( evictAt( lru2_, 0 ) == 1 );
    CPPUT_ASSERT( asVector( lru2_ ) == std::vector<int>( { 1,0 } ) );

    CPPUT_ASSERT( evictAt( lru2_, 1 ) == 0 );
    CPPUT_ASSERT( asVector( lru2_ ) == std::vector<int>( { 1,0 } ) );
}

CPPUT_REGISTERED_TEST( SmallLRUTest, moveIndexToFront_8 )
{
    moveIndexToFront( lru8_, 0 );
    CPPUT_ASSERT( asVector( lru8_ ) == std::vector<int>( { 0,1,2,3,4,5,6,7 } ) );

    moveIndexToFront( lru8_, 6 );
    CPPUT_ASSERT( asVector( lru8_ ) == std::vector<int>( { 6,0,1,2,3,4,5,7 } ) );

    moveIndexToFront( lru8_, 7 );
    CPPUT_ASSERT( asVector( lru8_ ) == std::vector<int>( { 7,6,0,1,2,3,4,5 } ) );
}

CPPUT_REGISTERED_TEST( SmallLRUTest, moveIndexToFront_16 )
{
    moveIndexToFront( lru16_, 0 );
    CPPUT_ASSERT( asVector( lru16_ ) == std::vector<int>( { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 } ) );

    moveIndexToFront( lru16_, 11 );
    CPPUT_ASSERT( asVector( lru16_ ) == std::vector<int>( { 11,0,1,2,3,4,5,6,7,8,9,10,12,13,14,15 } ) );

    moveIndexToFront( lru16_, 15 );
    CPPUT_ASSERT( asVector( lru16_ ) == std::vector<int>( { 15,11,0,1,2,3,4,5,6,7,8,9,10,12,13,14 } ) );
}

CPPUT_REGISTERED_TEST( SmallLRUTest, moveIndexToFront_1 )
{
    moveIndexToFront( lru1_, 0 );
    CPPUT_ASSERT( asVector( lru1_ ) == std::vector<int>( { 0 } ) );
}

CPPUT_REGISTERED_TEST( SmallLRUTest, moveIndexToFront_2 )
{
    moveIndexToFront( lru2_, 0 );
    CPPUT_ASSERT( asVector( lru2_ ) == std::vector<int>( { 0,1 } ) );

    moveIndexToFront( lru2_, 1 );
    CPPUT_ASSERT( asVector( lru2_ ) == std::vector<int>( { 1,0 } ) );
}

CPPUT_REGISTERED_TEST( SmallLRUTest, moveIndexTo_8 )
{
    moveIndexTo( lru8_, 0, 0 );
    CPPUT_ASSERT( asVector( lru8_ ) == std::vector<int>( { 0,1,2,3,4,5,6,7 } ) );

    moveIndexTo( lru8_, 2, 0 );
    CPPUT_ASSERT( asVector( lru8_ ) == std::vector<int>( { 2,0,1,3,4,5,6,7 } ) );

    moveIndexTo( lru8_, 3, 0 );
    CPPUT_ASSERT( asVector( lru8_ ) == std::vector<int>( { 3,2,0,1,4,5,6,7 } ) );

    moveIndexTo( lru8_, 7, 1 );
    CPPUT_ASSERT( asVector( lru8_ ) == std::vector<int>( { 3,7,2,0,1,4,5,6 } ) );

    moveIndexTo( lru8_, 0, 7 );
    CPPUT_ASSERT( asVector( lru8_ ) == std::vector<int>( { 3,7,2,1,4,5,6,0 } ) );
}

CPPUT_REGISTERED_TEST( SmallLRUTest, moveIndexTo_16 )
{
    moveIndexTo( lru16_, 0, 0 );
    CPPUT_ASSERT( asVector( lru16_ ) == std::vector<int>( { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 } ) );

    moveIndexTo( lru16_, 2, 0 );
    CPPUT_ASSERT( asVector( lru16_ ) == std::vector<int>( { 2,0,1,3,4,5,6,7,8,9,10,11,12,13,14,15 } ) );

    moveIndexTo( lru16_, 3, 0 );
    CPPUT_ASSERT( asVector( lru16_ ) == std::vector<int>( { 3,2,0,1,4,5,6,7,8,9,10,11,12,13,14,15 } ) );

    moveIndexTo( lru16_, 15, 1 );
    CPPUT_ASSERT( asVector( lru16_ ) == std::vector<int>( { 3,15,2,0,1,4,5,6,7,8,9,10,11,12,13,14 } ) );

    moveIndexTo( lru16_, 0, 15 );
    CPPUT_ASSERT( asVector( lru16_ ) == std::vector<int>( { 3,15,2,1,4,5,6,7,8,9,10,11,12,13,14,0 } ) );
}

CPPUT_REGISTERED_TEST( SmallLRUTest, moveIndexTo_1 )
{
    moveIndexTo( lru1_, 0, 0 );
    CPPUT_ASSERT( asVector( lru1_ ) == std::vector<int>( { 0 } ) );
}


CPPUT_REGISTERED_TEST( SmallLRUTest, moveIndexTo_2 )
{
    moveIndexTo( lru2_, 0, 0 );
    CPPUT_ASSERT( asVector( lru2_ ) == std::vector<int>( { 0,1 } ) );

    moveIndexTo( lru2_, 1, 0 );
    CPPUT_ASSERT( asVector( lru2_ ) == std::vector<int>( { 1,0 } ) );

    moveIndexTo( lru2_, 1, 1 );
    CPPUT_ASSERT( asVector( lru2_ ) == std::vector<int>( { 0,1 } ) );
}

} } // namespace blp { namespace testing {
