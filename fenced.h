#ifndef __FENCED_H__
#define __FENCED_H__

#include <atomic_ops.h>

// http://www.hpl.hp.com/research/linux/atomic_ops

/* Note that if some data is initialized with                   */
/*      data.x = ...; data.y = ...; ...                         */
/*      AO_store_release_write(&data_is_initialized, 1)         */
/* then data is guaranteed to be initialized after the test     */
/*      if (AO_load_release_read(&data_is_initialized)) ...     */
/* succeeds.  Furthermore, this should generate near-optimal    */
/* code on all common platforms.                                */
/*                                                              */
/* All operations operate on unsigned AO_t, which               */
/* is the natural word size, and usually unsigned long.         */

#include <boost/static_assert.hpp>

template <typename T>
struct Fenced
{
    T Get()
    {
//         return const_cast<T&>(t);
        BOOST_STATIC_ASSERT(sizeof(AO_t) == sizeof(T));
        AO_t value = AO_load_read(reinterpret_cast<volatile AO_t*>(&t));
        return reinterpret_cast<T&>(value);
    }
    void Set(T value)
    {
//         const_cast<T&>(t) = value; return;
        AO_store_write(reinterpret_cast<volatile AO_t*>(&t), reinterpret_cast<AO_t&>(value));
    }
private:
    volatile T t;
};

#endif
