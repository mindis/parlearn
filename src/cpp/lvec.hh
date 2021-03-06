#pragma once

#include <cassert>
#include <amd64.hh>

namespace impl {
  template <size_t Size> struct uint_sel {};
  template <> struct uint_sel<4> { typedef uint32_t type; };
  template <> struct uint_sel<8> { typedef uint64_t type; };
}

/**
 * Locking vector
 */
template <typename T>
class standard_lvec {
public:
  standard_lvec(size_t n)
    : impl_(n) {}

  typedef typename impl::uint_sel<sizeof(T)>::type uint_type;
  static const uint_type LOCK_MASK = 0x1;

  inline T
  unsaferead(size_t idx) const
  {
    assert(idx < impl_.size());
    return impl_[idx];
  }

  inline void
  unsafewrite(size_t idx, const T &t)
  {
    assert(idx < impl_.size());
    impl_[idx] = t;
  }

  inline void
  lock(size_t idx)
  {
    lockandread(idx);
  }

  inline T
  lockandread(size_t idx)
  {
    assert(idx < impl_.size());
    volatile uint_type * const px =
      reinterpret_cast<volatile uint_type *>(&impl_[idx]);
    union {
      uint_type v;
      T f;
    } u;
    u.v = *px;
    while ((u.v & LOCK_MASK) ||
           !__sync_bool_compare_and_swap(px, u.v, u.v | LOCK_MASK)) {
      nop_pause();
      u.v = *px;
    }
    compiler_barrier();
    assert(*px & LOCK_MASK);
    return u.f;
  }

  inline void
  writeandunlock(size_t idx, const T &t)
  {
    assert(idx < impl_.size());
    compiler_barrier();
    uint_type * const px = reinterpret_cast<uint_type *>(&impl_[idx]);
    assert(*px & LOCK_MASK);
    const uint_type * const tpx = reinterpret_cast<const uint_type *>(&t);
    *px = (*tpx & ~LOCK_MASK);
  }

  inline void
  unlock(size_t idx)
  {
    compiler_barrier();
    uint_type * const px = reinterpret_cast<uint_type *>(&impl_[idx]);
    assert(*px & LOCK_MASK);
    *px &= ~LOCK_MASK;
  }

  template <typename Vec>
	inline void
	unsafesnapshot(Vec &v) const
	{
		v.resize(impl_.size());
		for (size_t i = 0; i < impl_.size(); i++)
			v[i] = impl_[i];
	}

private:
  std::vector< T > impl_;
};
