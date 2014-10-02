/*
 *  This file is a part of TiledArray.
 *  Copyright (C) 2013  Virginia Tech
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  justus
 *  Department of Chemistry, Virginia Tech
 *
 *  vector_op.h
 *  Nov 17, 2013
 *
 */

#ifndef TILEDARRAY_MATH_VECTOR_OP_H__INCLUDED
#define TILEDARRAY_MATH_VECTOR_OP_H__INCLUDED

#include <TiledArray/math/math.h>
#include <TiledArray/math/eigen.h>
#include <TiledArray/madness.h>

#ifndef TILEARRAY_ALIGNMENT
#define TILEARRAY_ALIGNMENT 16
#endif // TILEARRAY_ALIGNMENT

#ifndef TILEDARRAY_CACHELINE_SIZE
#define TILEDARRAY_CACHELINE_SIZE 64
#endif // TILEDARRAY_CACHELINE_SIZE

#define TILEDARRAY_LOOP_UNWIND ::TiledArray::math::LoopUnwind::value

// Add macro TILEDARRAY_FORCE_INLINE which does as the name implies.
#if (defined _MSC_VER) || (defined __INTEL_COMPILER)

#define TILEDARRAY_FORCE_INLINE __forceinline

#elif(__clang__)

#define TILEDARRAY_FORCE_INLINE __attribute__((always_inline)) inline

#elif defined(__GNUC__)

#if (__GNUC__ >= 4)
#define TILEDARRAY_FORCE_INLINE __attribute__((always_inline)) inline
#else
#define TILEDARRAY_FORCE_INLINE inline
#endif // (__GNUC__ >= 4)

#else

#define TILEDARRAY_FORCE_INLINE inline

#endif


#if (defined __GNUC__) || (defined __PGI) || (defined __IBMCPP__) || (defined __ARMCC_VERSION)

#define TILEDARRAY_ALIGNED_STORAGE __attribute__((aligned(TILEARRAY_ALIGNMENT)))

#elif (defined _MSC_VER)

#define TILEDARRAY_ALIGNED_STORAGE __declspec(align(TILEARRAY_ALIGNMENT))

#else

#define TILEDARRAY_ALIGNED_STORAGE
#warning FIXEME!!! TiledArray alignment attribute is not definded for this platform.

#endif


namespace TiledArray {
  namespace math {

    // Define compile time constant for loop unwinding.
    typedef std::integral_constant<std::size_t, TILEDARRAY_CACHELINE_SIZE / sizeof(double)> LoopUnwind;
    typedef std::integral_constant<std::size_t, ~std::size_t(TILEDARRAY_LOOP_UNWIND - 1ul)> index_mask;

    template <std::size_t> struct VectorOpUnwind;

    /// Vector loop unwind helper class

    /// This object will unwind \c 1 step of a vector operation loop, and
    /// terminate the loop
    template <>
    struct VectorOpUnwind<0ul> {

      static const std::size_t offset = TILEDARRAY_LOOP_UNWIND - 1ul;

      template <typename Arg, typename Result>
      static TILEDARRAY_FORCE_INLINE void
      copy(const Arg* restrict const arg, Result* restrict const result) {
        result[offset] = arg[offset];
      }

      template <typename Arg, typename Result>
      static TILEDARRAY_FORCE_INLINE void
      fill(const Arg& restrict arg, Result* restrict const result) {
        result[offset] = arg;
      }

      template <typename Arg, typename Result>
      static TILEDARRAY_FORCE_INLINE void
      scatter(const Arg* restrict const arg, Result* restrict const result, const std::size_t /*stride*/) {
        *result = arg[offset];
      }

      template <typename Arg, typename Result>
      static TILEDARRAY_FORCE_INLINE void
      gather(const Arg* restrict const arg, Result* restrict const result, const std::size_t /*stride*/) {
        result[offset] = *arg;
      }

      template <typename Left, typename Right, typename Result, typename Op>
      static TILEDARRAY_FORCE_INLINE void
      binary(const Left* restrict const left, const Right* restrict const right,
          Result* restrict const result, const Op& op)
      {
        result[offset] = op(left[offset], right[offset]);
      }

      template <typename Arg, typename Result, typename Op>
      static TILEDARRAY_FORCE_INLINE void
      binary(const Arg* restrict const arg, Result* restrict const result, const Op& op) {
        op(result[offset], arg[offset]);
      }

      template <typename Arg, typename Result, typename Op>
      static TILEDARRAY_FORCE_INLINE void
      unary(const Arg* restrict const arg, Result* restrict const result, const Op& op) {
        result[offset] = op(arg[offset]);
      }

      template <typename Result, typename Op>
      static TILEDARRAY_FORCE_INLINE void
      unary(Result* restrict const result, const Op& op) {
        op(result[offset]);
      }

      template <typename Left, typename Right, typename Result, typename Op>
      static TILEDARRAY_FORCE_INLINE
      typename madness::disable_if<std::is_pointer<Result> >::type
      reduce(const Left* restrict const left, const Right* restrict const right,
          Result& restrict result, const Op& op)
      {
        op(result, left[offset], right[offset]);
      }

      template <typename Arg, typename Result, typename Op>
      static TILEDARRAY_FORCE_INLINE
      typename madness::disable_if<std::is_pointer<Result> >::type
      reduce(const Arg* restrict const arg, Result& restrict result, const Op& op) {
        op(result, arg[offset]);
      }

      template <typename Left, typename Right, typename Result, typename Op>
      static TILEDARRAY_FORCE_INLINE void
      reduce(const Left* restrict const left, const Right* restrict const right,
          Result* restrict result, const Op& op)
      {
        op(result[offset], left[offset], right[offset]);
      }

      template <typename Arg, typename Result, typename Op>
      static TILEDARRAY_FORCE_INLINE void
      reduce(const Arg* restrict const arg, Result* restrict result, const Op& op) {
        op(result[offset], arg[offset]);
      }

      template <typename Arg, typename Result>
      static TILEDARRAY_FORCE_INLINE void
      uninitialized_copy(const Arg* restrict const arg, Result* restrict const result) {
        new(result + offset) Result(arg[offset]);
      }

      template <typename Arg, typename Result>
      static TILEDARRAY_FORCE_INLINE void
      uninitialized_fill(const Arg& restrict arg, Result* restrict const result) {
        new(result + offset) Result(arg);
      }

      template <typename Left, typename Right, typename Result, typename Op>
      static TILEDARRAY_FORCE_INLINE void
      uninitialized_binary(const Left* restrict const left, const Right* restrict const right,
          Result* restrict const result, const Op& op)
      {
        new(result + offset) Result(op(left[offset], right[offset]));
      }

      template <typename Arg, typename Result, typename Op>
      static TILEDARRAY_FORCE_INLINE void
      uninitialized_unary(const Arg* restrict const arg, Result* restrict const result, const Op& op) {
        new(result + offset) Result(op(arg[offset]));
      }

      template <typename Arg>
      static TILEDARRAY_FORCE_INLINE void
      destroy(Arg* restrict const arg) {
        arg[offset].~Arg();
      }

    }; //  struct VectorOpUnwind

    /// Vector loop unwind helper class

    /// This object will unwind \c N steps of a vector operation loop.
    /// \tparam N The number of steps to unwind
    template <std::size_t N>
    struct VectorOpUnwind : public VectorOpUnwind<N - 1ul> {

      typedef VectorOpUnwind<N - 1ul> VectorOpUnwindN1;

      static const std::size_t offset = TILEDARRAY_LOOP_UNWIND - N - 1ul;

      template <typename Arg, typename Result>
      static TILEDARRAY_FORCE_INLINE void
      copy(const Arg* restrict const arg, Result* restrict const result) {
        result[offset] = arg[offset];
        VectorOpUnwindN1::copy(arg, result);
      }

      template <typename Arg, typename Result>
      static TILEDARRAY_FORCE_INLINE void
      fill(const Arg& restrict arg, Result* restrict const result) {
        result[offset] = arg;
        VectorOpUnwindN1::fill(arg, result);
      }

      template <typename Arg, typename Result>
      static TILEDARRAY_FORCE_INLINE void
      scatter(const Arg* restrict const arg, Result* restrict const result, const std::size_t stride) {
        *result = arg[offset];
        VectorOpUnwindN1::scatter(arg, result + stride, stride);
      }

      template <typename Arg, typename Result>
      static TILEDARRAY_FORCE_INLINE void
      gather(const Arg* restrict const arg, Result* restrict const result, const std::size_t stride) {
        result[offset] = *arg;
        VectorOpUnwindN1::gather(arg + stride, result, stride);
      }

      template <typename Left, typename Right, typename Result, typename Op>
      static TILEDARRAY_FORCE_INLINE void
      binary(const Left* restrict const left, const Right* restrict const right,
          Result* restrict const result, const Op& op)
      {
        result[offset] = op(left[offset], right[offset]);
        VectorOpUnwindN1::binary(left, right, result, op);
      }

      template <typename Arg, typename Result, typename Op>
      static void TILEDARRAY_FORCE_INLINE
      binary(const Arg* restrict const arg, Result* restrict const result, const Op& op) {
        op(result[offset], arg[offset]);
        VectorOpUnwindN1::binary(arg, result, op);
      }

      template <typename Arg, typename Result, typename Op>
      static TILEDARRAY_FORCE_INLINE void
      unary(const Arg* restrict const arg, Result* restrict const result, const Op& op) {
        result[offset] = op(arg[offset]);
        VectorOpUnwindN1::unary(arg, result, op);
      }

      template <typename Result, typename Op>
      static void TILEDARRAY_FORCE_INLINE
      unary(Result* restrict const result, const Op& op) {
        op(result[offset]);
        VectorOpUnwindN1::unary(result, op);
      }

      template <typename Left, typename Right, typename Result, typename Op>
      static TILEDARRAY_FORCE_INLINE
      typename madness::disable_if<std::is_pointer<Result> >::type
      reduce(const Left* restrict const left, const Right* restrict const right,
          Result& restrict result, const Op& op)
      {
        op(result, left[offset], right[offset]);
        VectorOpUnwindN1::reduce(left, right, result, op);
      }

      template <typename Arg, typename Result, typename Op>
      static TILEDARRAY_FORCE_INLINE
      typename madness::disable_if<std::is_pointer<Result> >::type
      reduce(const Arg* restrict const arg, Result& restrict result, const Op& op) {
        op(result, arg[offset]);
        VectorOpUnwindN1::reduce(arg, result, op);
      }

      template <typename Left, typename Right, typename Result, typename Op>
      static TILEDARRAY_FORCE_INLINE void
      reduce(const Left* restrict const left, const Right* restrict const right,
          Result* restrict result, const Op& op)
      {
        op(result[offset], left[offset], right[offset]);
        VectorOpUnwindN1::reduce(left, right, result, op);
      }

      template <typename Arg, typename Result, typename Op>
      static TILEDARRAY_FORCE_INLINE void
      reduce(const Arg* restrict const arg, Result* restrict result, const Op& op) {
        op(result[offset], arg[offset]);
        VectorOpUnwindN1::reduce(arg, result, op);
      }

      template <typename Arg, typename Result>
      static TILEDARRAY_FORCE_INLINE void
      uninitialized_copy(const Arg* restrict const arg, Result* restrict const result) {
        new(result + offset) Result(arg[offset]);
        VectorOpUnwindN1::uninitialized_copy(arg, result);
      }

      template <typename Arg, typename Result>
      static TILEDARRAY_FORCE_INLINE void
      uninitialized_fill(const Arg& restrict arg, Result* restrict const result) {
        new(result + offset) Result(arg);
        VectorOpUnwindN1::uninitialized_fill(arg, result);
      }

      template <typename Left, typename Right, typename Result, typename Op>
      static TILEDARRAY_FORCE_INLINE void
      uninitialized_binary(const Left* restrict const left, const Right* restrict const right,
          Result* restrict const result, const Op& op)
      {
        new(result + offset) Result(op(left[offset], right[offset]));
        VectorOpUnwindN1::uninitialized_binary(left, right, result, op);
      }

      template <typename Arg, typename Result, typename Op>
      static TILEDARRAY_FORCE_INLINE void
      uninitialized_unary(const Arg* restrict const arg, Result* restrict const result, const Op& op) {
        new(result + offset) Result(op(arg[offset]));
        VectorOpUnwindN1::uninitialized_unary(arg, result, op);
      }

      template <typename Arg>
      static TILEDARRAY_FORCE_INLINE void
      destroy(Arg* restrict const arg) {
        arg[offset].~Arg();
        VectorOpUnwindN1::destroy(arg);
      }

    }; //  struct VectorOpUnwind

    typedef VectorOpUnwind<TILEDARRAY_LOOP_UNWIND - 1> VecOpUnwindN;


    template <typename Arg, typename Result, typename Op>
    void binary_vector_op(const std::size_t n, const Arg* restrict const arg,
        Result* restrict const result, const Op& op)
    {
      std::size_t i = 0ul;

      // Compute block iteration limit
      const std::size_t nx = n & index_mask::value;

      for(; i < nx; i += TILEDARRAY_LOOP_UNWIND) {
        Result* restrict const result_i = result + i;

        TILEDARRAY_ALIGNED_STORAGE Result result_block[TILEDARRAY_LOOP_UNWIND];
        VecOpUnwindN::copy(result_i, result_block);
        TILEDARRAY_ALIGNED_STORAGE Arg arg_block[TILEDARRAY_LOOP_UNWIND];
        VecOpUnwindN::copy(arg + i, arg_block);

        VecOpUnwindN::binary(arg_block, result_block, op);

        VecOpUnwindN::copy(result_block, result_i);

      }

      for(; i < n; ++i) {

        Result result_block = result[i];
        const Arg arg_block = arg[i];

        op(result_block, arg_block);

        result[i] = result_block;
      }
    }

    template <typename Left, typename Right, typename Result, typename Op>
    void binary_vector_op(const std::size_t n, const Left* restrict const left,
        const Right* restrict const right, Result* restrict const result, const Op& op)
    {
      std::size_t i = 0ul;

      // Compute block iteration limit
      const std::size_t nx = n & index_mask::value;

      for(; i < nx; i += TILEDARRAY_LOOP_UNWIND) {

        TILEDARRAY_ALIGNED_STORAGE Left left_block[TILEDARRAY_LOOP_UNWIND];
        VecOpUnwindN::copy(left + i, left_block);
        TILEDARRAY_ALIGNED_STORAGE Right right_block[TILEDARRAY_LOOP_UNWIND];
        VecOpUnwindN::copy(right + i, right_block);

        TILEDARRAY_ALIGNED_STORAGE Result result_block[TILEDARRAY_LOOP_UNWIND];
        VecOpUnwindN::binary(left_block, right_block, result_block, op);

        VecOpUnwindN::copy(result_block, result + i);
      }

      for(; i < n; ++i) {

        const Left left_i = left[i];
        const Right right_i = right[i];

        const Result temp_i = op(left_i, right_i);

        result[i] = temp_i;

      }
    }

    template <typename Result, typename Op>
    void unary_vector_op(const std::size_t n, Result* restrict const result, const Op& op) {
      std::size_t i = 0ul;

      // Compute block iteration limit
      const std::size_t nx = n & index_mask::value;

      for(; i < nx; i += 8ul) {
        Result* restrict const result_i = result + i;

        TILEDARRAY_ALIGNED_STORAGE Result result_block[TILEDARRAY_LOOP_UNWIND];
        VecOpUnwindN::copy(result_i, result_block);

        VecOpUnwindN::unary(result_block, op);

        VecOpUnwindN::copy(result_block, result_i);

      }

      for(; i < n; ++i) {

        Result temp_i = result[i];

        op(temp_i);

        result[i] = temp_i;

      }
    }

    template <typename Arg, typename Result, typename Op>
    void unary_vector_op(const std::size_t n, const Arg* restrict const arg,
        Result* restrict const result, const Op& op)
    {
      std::size_t i = 0ul;

      // Compute block iteration limit
      const std::size_t nx = n & index_mask::value;

      for(; i < nx; i += TILEDARRAY_LOOP_UNWIND) {

        TILEDARRAY_ALIGNED_STORAGE Arg arg_block[TILEDARRAY_LOOP_UNWIND];
        VecOpUnwindN::copy(arg + i, arg_block);

        TILEDARRAY_ALIGNED_STORAGE Result result_block[TILEDARRAY_LOOP_UNWIND];
        VecOpUnwindN::unary(arg_block, result_block, op);

        VecOpUnwindN::copy(result_block, result + i);

      }

      for(; i < n; ++i) {

        const Arg arg_i = arg[i];

        const Result temp_i = op(arg_i);

        result[i] = temp_i;
      }
    }

    template <typename Arg, typename Result>
    typename madness::disable_if_c<std::is_same<Arg, Result>::value && std::is_scalar<Arg>::value>::type
    copy_vector(const std::size_t n, const Arg* restrict const arg,
        Result* restrict const result)
    {
      std::size_t i = 0ul;

      // Compute block iteration limit
      const std::size_t nx = n & index_mask::value;

      for(; i < nx; i += TILEDARRAY_LOOP_UNWIND)
        VecOpUnwindN::copy(arg + i, result + i);

      for(; i < n; ++i)
        result[i] = arg[i];
    }

    template <typename T>
    inline typename madness::enable_if<std::is_scalar<T> >::type
    copy_vector(const std::size_t n, const T* restrict const arg, T* restrict const result) {
      std::memcpy(result, arg, n * sizeof(T));
    }

    template <typename Arg, typename Result>
    void fill_vector(const std::size_t n, const Arg& restrict arg, Result* restrict const result) {
      std::size_t i = 0ul;

      // Compute block iteration limit
      const std::size_t nx = n & index_mask::value;

      for(; i < nx; i += TILEDARRAY_LOOP_UNWIND)
        VecOpUnwindN::fill(arg, result + i);

      for(; i < n; ++i)
        result[i] = arg;
    }


    template <typename Arg, typename Result>
    typename madness::disable_if_c<std::is_scalar<Arg>::value && std::is_scalar<Result>::value>::type
    uninitialized_copy_vector(const std::size_t n, const Arg* restrict const arg,
        Result* restrict const result)
    {
      std::size_t i = 0ul;

      // Compute block iteration limit
      const std::size_t nx = n & index_mask::value;

      for(; i < nx; i += TILEDARRAY_LOOP_UNWIND)
        VecOpUnwindN::uninitialized_copy(arg + i, result + i);

      for(; i < n; ++i)
        new(result + i) Result(arg[i]);
    }

    template <typename Arg, typename Result>
    inline typename madness::enable_if_c<std::is_scalar<Arg>::value && std::is_scalar<Result>::value>::type
    uninitialized_copy_vector(const std::size_t n, const Arg* restrict const arg, Result* restrict const result) {
      copy_vector(n, arg, result);
    }

    template <typename Arg, typename Result>
    typename madness::disable_if_c<std::is_scalar<Arg>::value && std::is_scalar<Result>::value>::type
    uninitialized_fill_vector(const std::size_t n, const Arg& restrict arg,
        Result* restrict const result)
    {
      std::size_t i = 0ul;

      // Compute block iteration limit
      const std::size_t nx = n & index_mask::value;

      for(; i < nx; i += TILEDARRAY_LOOP_UNWIND)
        VecOpUnwindN::uninitialized_fill(arg, result + i);

      for(; i < n; ++i)
        new(result + i) Result(arg);
    }


    template <typename Arg, typename Result>
    inline typename madness::enable_if_c<std::is_scalar<Arg>::value && std::is_scalar<Result>::value>::type
    uninitialized_fill_vector(const std::size_t n, const Arg& restrict arg,
        Result* restrict const result)
    { fill_vector(n, arg, result); }


    template <typename Arg>
    typename madness::disable_if<std::is_scalar<Arg> >::type
    destroy_vector(const std::size_t n, const Arg* restrict const arg) {
      std::size_t i = 0ul;

      // Compute block iteration limit
      const std::size_t nx = n & index_mask::value;

      for(; i < nx; i += TILEDARRAY_LOOP_UNWIND)
        VecOpUnwindN::destroy(arg + i);

      for(; i < n; ++i)
        arg[i].~Arg();
    }

    template <typename Arg>
    inline typename madness::enable_if<std::is_scalar<Arg> >::type
    destroy_vector(const std::size_t, const Arg* restrict const) { }


    template <typename Arg, typename Result, typename Op>
    typename madness::disable_if_c<std::is_scalar<Arg>::value && std::is_scalar<Result>::value>::type
    uninitialized_unary_vector_op(const std::size_t n, const Arg* restrict const arg,
        Result* restrict const result, const Op& op)
    {
      std::size_t i = 0ul;

      // Compute block iteration limit
      const std::size_t nx = n & index_mask::value;

      for(; i < nx; i += TILEDARRAY_LOOP_UNWIND) {

        TILEDARRAY_ALIGNED_STORAGE Arg arg_block[TILEDARRAY_LOOP_UNWIND];
        VecOpUnwindN::copy(arg + i, arg_block);

        TILEDARRAY_ALIGNED_STORAGE Result result_block[TILEDARRAY_LOOP_UNWIND];
        VecOpUnwindN::uninitialized_unary(arg_block, result_block, op);

        VecOpUnwindN::copy(result_block, result + i);

      }

      for(; i < n; ++i) {

        const Arg arg_i = arg[i];

        const Result temp_i = op(arg_i);

        new(result + i) Result(temp_i);
      }
    }

    template <typename Arg, typename Result, typename Op>
    inline typename madness::enable_if_c<std::is_scalar<Arg>::value && std::is_scalar<Result>::value>::type
    uninitialized_unary_vector_op(const std::size_t n, const Arg* restrict const arg,
        Result* restrict const result, const Op& op)
    {
      unary_vector_op(n, arg, result, op);
    }


    template <typename Left, typename Right, typename Result, typename Op>
    typename madness::disable_if_c<std::is_scalar<Left>::value &&
        std::is_scalar<Right>::value && std::is_scalar<Result>::value>::type
    uninitialized_binary_vector_op(const std::size_t n, const Left* restrict const left,
        const Right* restrict const right, Result* restrict const result, const Op& op)
    {
      std::size_t i = 0ul;

      // Compute block iteration limit
      const std::size_t nx = n & index_mask::value;

      for(; i < nx; i += TILEDARRAY_LOOP_UNWIND) {

        TILEDARRAY_ALIGNED_STORAGE Left left_block[TILEDARRAY_LOOP_UNWIND];
        VecOpUnwindN::copy(left + i, left_block);
        TILEDARRAY_ALIGNED_STORAGE Right right_block[TILEDARRAY_LOOP_UNWIND];
        VecOpUnwindN::copy(right + i, right_block);

        TILEDARRAY_ALIGNED_STORAGE Result result_block[TILEDARRAY_LOOP_UNWIND];
        VecOpUnwindN::uninitialized_binary(left_block, right_block, result_block, op);

        VecOpUnwindN::copy(result_block, result + i);
      }

      for(; i < n; ++i) {

        const Left left_i = left[i];
        const Right right_i = right[i];

        const Result temp_i = op(left_i, right_i);

        new(result + i) Result(temp_i);

      }
    }

    template <typename Left, typename Right, typename Result, typename Op>
    typename madness::enable_if_c<std::is_scalar<Left>::value &&
        std::is_scalar<Right>::value && std::is_scalar<Result>::value>::type
    uninitialized_binary_vector_op(const std::size_t n, const Left* restrict const left,
        const Right* restrict const right, Result* restrict const result, const Op& op)
    {
      binary_vector_op(n, left, right, result, op);
    }

    template <typename Left, typename Right, typename Result, typename Op>
    typename madness::disable_if<std::is_pointer<Result> >::type
    reduce_vector_op(const std::size_t n, const Left* restrict const left,
        const Right* restrict const right, Result& restrict result, const Op& op)
    {
      std::size_t i = 0ul;

      // Compute block iteration limit
      const std::size_t nx = n & index_mask::value;

      for(; i < nx; i += 8ul) {

        TILEDARRAY_ALIGNED_STORAGE Left left_block[TILEDARRAY_LOOP_UNWIND];
        VecOpUnwindN::copy(left + i, left_block);
        TILEDARRAY_ALIGNED_STORAGE Right right_block[TILEDARRAY_LOOP_UNWIND];
        VecOpUnwindN::copy(right + i, right_block);

        VecOpUnwindN::reduce(left_block, right_block, result, op);

      }

      for(; i < n; ++i) {

        const Left left_block = left[i];
        const Right right_block = right[i];

        op(result, left_block, right_block);

      }
    }

    template <typename Arg, typename Result, typename Op>
    typename madness::disable_if<std::is_pointer<Result> >::type
    reduce_vector_op(const std::size_t n, const Arg* restrict const arg,
        Result& restrict result, const Op& op)
    {
      std::size_t i = 0ul;

      // Compute block iteration limit
      const std::size_t nx = n & index_mask::value;

      for(; i < nx; i += TILEDARRAY_LOOP_UNWIND) {

        TILEDARRAY_ALIGNED_STORAGE Arg arg_block[TILEDARRAY_LOOP_UNWIND];
        VecOpUnwindN::copy(arg + i, arg_block);

        VecOpUnwindN::reduce(arg_block, result, op);

      }

      for(; i < n; ++i) {

        const Arg arg_i = arg[i];

        op(result, arg_i);

      }
    }


    template <typename Left, typename Right, typename Result, typename Op>
    void reduce_vector_op(const std::size_t n, const Left* restrict const left,
        const Right* restrict const right, Result* restrict result, const Op& op)
    {
      std::size_t i = 0ul;

      // Compute block iteration limit
      const std::size_t nx = n & index_mask::value;

      for(; i < nx; i += 8ul) {

        TILEDARRAY_ALIGNED_STORAGE Result result_block[TILEDARRAY_LOOP_UNWIND];
        VecOpUnwindN::copy(result + i, result_block);
        TILEDARRAY_ALIGNED_STORAGE Left left_block[TILEDARRAY_LOOP_UNWIND];
        VecOpUnwindN::copy(left + i, left_block);
        TILEDARRAY_ALIGNED_STORAGE Right right_block[TILEDARRAY_LOOP_UNWIND];
        VecOpUnwindN::copy(right + i, right_block);

        VecOpUnwindN::reduce(left_block, right_block, static_cast<Result*>(result_block), op);

        VecOpUnwindN::copy(result_block, result + i);
      }

      for(; i < n; ++i) {

        Result result_block = result[i];
        const Left left_block = left[i];
        const Right right_block = right[i];

        op(result_block, left_block, right_block);

        result[i] = result_block;

      }
    }

    template <typename Arg, typename Result, typename Op>
    void reduce_vector_op(const std::size_t n, const Arg* restrict const arg,
        Result* restrict result, const Op& op)
    {
      std::size_t i = 0ul;

      // Compute block iteration limit
      const std::size_t nx = n & index_mask::value;

      for(; i < nx; i += TILEDARRAY_LOOP_UNWIND) {

        TILEDARRAY_ALIGNED_STORAGE Result result_block[TILEDARRAY_LOOP_UNWIND];
        VecOpUnwindN::copy(result + i, result_block);
        TILEDARRAY_ALIGNED_STORAGE Arg arg_block[TILEDARRAY_LOOP_UNWIND];
        VecOpUnwindN::copy(arg + i, arg_block);

        VecOpUnwindN::reduce(arg_block, static_cast<Result*>(result_block), op);

        VecOpUnwindN::copy(result_block, result + i);
      }

      for(; i < n; ++i) {

        Result result_block = result[i];
        const Arg arg_i = arg[i];

        op(result_block, arg_i);

        result[i] = result_block;

      }
    }

  }  // namespace math
} // namespace TiledArray

#endif // TILEDARRAY_MATH_VECTOR_OP_H__INCLUDED
