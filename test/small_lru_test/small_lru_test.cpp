#include <blp/small_lru.hpp>
#include <cpput3/testing.h>
#include <iostream>



namespace blp { namespace testing {

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
}




} } // namespace blp { namespace testing {
