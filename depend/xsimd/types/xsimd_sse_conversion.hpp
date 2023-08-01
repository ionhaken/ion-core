/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay, Wolf Vollprecht and         *
* Martin Renou                                                             *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XSIMD_SSE_CONVERSION_HPP
#define XSIMD_SSE_CONVERSION_HPP

#include "xsimd_sse_double.hpp"
#include "xsimd_sse_float.hpp"
#include "xsimd_sse_int8.hpp"
#include "xsimd_sse_int16.hpp"
#include "xsimd_sse_int32.hpp"
#include "xsimd_sse_int64.hpp"

namespace xsimd
{

    /************************
     * conversion functions *
     ************************/

    batch<int32_t, 4> to_int(const batch<float, 4>& x);
    batch<int64_t, 2> to_int(const batch<double, 2>& x);

    batch<float, 4> to_float(const batch<int32_t, 4>& x);
    batch<double, 2> to_float(const batch<int64_t, 2>& x);

    batch<uint16_t, 8> u8_to_u16(const batch<uint8_t, 16>& x);
    batch<uint8_t, 16> u16_to_u8(const batch<uint16_t, 8>& x);
    batch<uint32_t, 4> u8_to_u32(const batch<uint8_t, 16>& x);
    batch<uint8_t, 16> u32_to_u8(const batch<uint32_t, 4>& x);
    batch<uint64_t, 2> u8_to_u64(const batch<uint8_t, 16>& x);
    batch<uint8_t, 16> u64_to_u8(const batch<uint64_t, 2>& x);

    /**************************
     * boolean cast functions *
     **************************/

    batch_bool<int32_t, 4> bool_cast(const batch_bool<float, 4>& x);
    batch_bool<int64_t, 2> bool_cast(const batch_bool<double, 2>& x);
    batch_bool<float, 4> bool_cast(const batch_bool<int32_t, 4>& x);
    batch_bool<double, 2> bool_cast(const batch_bool<int64_t, 2>& x);

    /***************************************
     * conversion functions implementation *
     ***************************************/

    inline batch<int32_t, 4> to_int(const batch<float, 4>& x)
    {
        return _mm_cvttps_epi32(x);
    }

    inline batch<int64_t, 2> to_int(const batch<double, 2>& x)
    {
#if defined(XSIMD_AVX512VL_AVAILABLE) & defined(XSIMD_AVX512DQ_AVAILABLE)
        return _mm_cvttpd_epi64(x);
#else
        return batch<int64_t, 2>(static_cast<int64_t>(x[0]), static_cast<int64_t>(x[1]));
#endif
    }

    inline batch<float, 4> to_float(const batch<int32_t, 4>& x)
    {
        return _mm_cvtepi32_ps(x);
    }

    inline batch<double, 2> to_float(const batch<int64_t, 2>& x)
    {
#if defined(XSIMD_AVX512VL_AVAILABLE) & defined(XSIMD_AVX512DQ_AVAILABLE)
        return _mm_cvtepi64_pd(x);
#else
        return batch<double, 2>(static_cast<double>(x[0]), static_cast<double>(x[1]));
#endif
    }

    inline batch<uint16_t, 8> u8_to_u16(const batch<uint8_t, 16>& x)
    {
        return static_cast<batch<uint16_t, 8>>(x);
    }

    inline batch<uint8_t, 16> u16_to_u8(const batch<uint16_t, 8>& x)
    {
        return static_cast<batch<uint8_t, 16>>(x);
    }

    inline batch<uint32_t, 4> u8_to_u32(const batch<uint8_t, 16>& x)
    {
        return static_cast<batch<uint32_t, 4>>(x);
    }

    inline batch<uint8_t, 16> u32_to_u8(const batch<uint32_t, 4>& x)
    {
        return static_cast<batch<uint8_t, 16>>(x);
    }

    inline batch<uint64_t, 2> u8_to_u64(const batch<uint8_t, 16>& x)
    {
        return static_cast<batch<uint64_t, 2>>(x);
    }

    inline batch<uint8_t, 16> u64_to_u8(const batch<uint64_t, 2>& x)
    {
        return static_cast<batch<uint8_t, 16>>(x);
    }

    /*****************************************
     * batch cast functions implementation *
     *****************************************/

    XSIMD_BATCH_CAST_IMPLICIT(int8_t, uint8_t, 16)
    XSIMD_BATCH_CAST_IMPLICIT(uint8_t, int8_t, 16)
    XSIMD_BATCH_CAST_IMPLICIT(int16_t, uint16_t, 8)
    XSIMD_BATCH_CAST_IMPLICIT(uint16_t, int16_t, 8)
    XSIMD_BATCH_CAST_IMPLICIT(int32_t, uint32_t, 4)
    XSIMD_BATCH_CAST_INTRINSIC(int32_t, float, 4, _mm_cvtepi32_ps)
    XSIMD_BATCH_CAST_IMPLICIT(uint32_t, int32_t, 4)
    XSIMD_BATCH_CAST_IMPLICIT(int64_t, uint64_t, 2)
    XSIMD_BATCH_CAST_IMPLICIT(uint64_t, int64_t, 2)
    XSIMD_BATCH_CAST_INTRINSIC(float, int32_t, 4, _mm_cvttps_epi32)
#if defined(XSIMD_AVX512VL_AVAILABLE)

#if defined(_MSC_VER)
    namespace detail {
        static inline __m128 xsimd_mm_cvtepu32_ps(__m128i a)
        {
          return _mm512_castps512_ps128(_mm512_cvtepu32_ps(_mm512_castsi128_si512(a)));
        }
    }
    XSIMD_BATCH_CAST_INTRINSIC(uint32_t, float, 4, detail::xsimd_mm_cvtepu32_ps)
#else
    XSIMD_BATCH_CAST_INTRINSIC(uint32_t, float, 4, _mm_cvtepu32_ps)
#endif

    XSIMD_BATCH_CAST_INTRINSIC(float, uint32_t, 4, _mm_cvttps_epu32)
#if defined(XSIMD_AVX512DQ_AVAILABLE)
    XSIMD_BATCH_CAST_INTRINSIC(int64_t, double, 2, _mm_cvtepi64_pd)
    XSIMD_BATCH_CAST_INTRINSIC(uint64_t, double, 2, _mm_cvtepu64_pd)
    XSIMD_BATCH_CAST_INTRINSIC(double, int64_t, 2, _mm_cvttpd_epi64)
    XSIMD_BATCH_CAST_INTRINSIC(double, uint64_t, 2, _mm_cvttpd_epu64)
#endif
#endif

    /**************************
     * boolean cast functions *
     **************************/

    inline batch_bool<int32_t, 4> bool_cast(const batch_bool<float, 4>& x)
    {
        return _mm_castps_si128(x);
    }

    inline batch_bool<int64_t, 2> bool_cast(const batch_bool<double, 2>& x)
    {
        return _mm_castpd_si128(x);
    }

    inline batch_bool<float, 4> bool_cast(const batch_bool<int32_t, 4>& x)
    {
        return _mm_castsi128_ps(x);
    }

    inline batch_bool<double, 2> bool_cast(const batch_bool<int64_t, 2>& x)
    {
        return _mm_castsi128_pd(x);
    }

    /*****************************************
     * bitwise cast functions implementation *
     *****************************************/

    XSIMD_BITWISE_CAST_IMPLICIT(int8_t, 16, int8_t, 16)
    XSIMD_BITWISE_CAST_IMPLICIT(int8_t, 16, uint8_t, 16)
    XSIMD_BITWISE_CAST_IMPLICIT(int8_t, 16, int16_t, 8)
    XSIMD_BITWISE_CAST_IMPLICIT(int8_t, 16, uint16_t, 8)
    XSIMD_BITWISE_CAST_IMPLICIT(int8_t, 16, int32_t, 4)
    XSIMD_BITWISE_CAST_IMPLICIT(int8_t, 16, uint32_t, 4)
    XSIMD_BITWISE_CAST_IMPLICIT(int8_t, 16, int64_t, 2)
    XSIMD_BITWISE_CAST_IMPLICIT(int8_t, 16, uint64_t, 2)
    XSIMD_BITWISE_CAST_INTRINSIC(int8_t, 16, float, 4, _mm_castsi128_ps)
    XSIMD_BITWISE_CAST_INTRINSIC(int8_t, 16, double, 2, _mm_castsi128_pd)
    XSIMD_BITWISE_CAST_IMPLICIT(uint8_t, 16, int8_t, 16)
    XSIMD_BITWISE_CAST_IMPLICIT(uint8_t, 16, uint8_t, 16)
    XSIMD_BITWISE_CAST_IMPLICIT(uint8_t, 16, int16_t, 8)
    XSIMD_BITWISE_CAST_IMPLICIT(uint8_t, 16, uint16_t, 8)
    XSIMD_BITWISE_CAST_IMPLICIT(uint8_t, 16, int32_t, 4)
    XSIMD_BITWISE_CAST_IMPLICIT(uint8_t, 16, uint32_t, 4)
    XSIMD_BITWISE_CAST_IMPLICIT(uint8_t, 16, int64_t, 2)
    XSIMD_BITWISE_CAST_IMPLICIT(uint8_t, 16, uint64_t, 2)
    XSIMD_BITWISE_CAST_INTRINSIC(uint8_t, 16, float, 4, _mm_castsi128_ps)
    XSIMD_BITWISE_CAST_INTRINSIC(uint8_t, 16, double, 2, _mm_castsi128_pd)
    XSIMD_BITWISE_CAST_IMPLICIT(int16_t, 8, int8_t, 16)
    XSIMD_BITWISE_CAST_IMPLICIT(int16_t, 8, uint8_t, 16)
    XSIMD_BITWISE_CAST_IMPLICIT(int16_t, 8, int16_t, 8)
    XSIMD_BITWISE_CAST_IMPLICIT(int16_t, 8, uint16_t, 8)
    XSIMD_BITWISE_CAST_IMPLICIT(int16_t, 8, int32_t, 4)
    XSIMD_BITWISE_CAST_IMPLICIT(int16_t, 8, uint32_t, 4)
    XSIMD_BITWISE_CAST_IMPLICIT(int16_t, 8, int64_t, 2)
    XSIMD_BITWISE_CAST_IMPLICIT(int16_t, 8, uint64_t, 2)
    XSIMD_BITWISE_CAST_INTRINSIC(int16_t, 8, float, 4, _mm_castsi128_ps)
    XSIMD_BITWISE_CAST_INTRINSIC(int16_t, 8, double, 2, _mm_castsi128_pd)
    XSIMD_BITWISE_CAST_IMPLICIT(uint16_t, 8, int8_t, 16)
    XSIMD_BITWISE_CAST_IMPLICIT(uint16_t, 8, uint8_t, 16)
    XSIMD_BITWISE_CAST_IMPLICIT(uint16_t, 8, int16_t, 8)
    XSIMD_BITWISE_CAST_IMPLICIT(uint16_t, 8, uint16_t, 8)
    XSIMD_BITWISE_CAST_IMPLICIT(uint16_t, 8, int32_t, 4)
    XSIMD_BITWISE_CAST_IMPLICIT(uint16_t, 8, uint32_t, 4)
    XSIMD_BITWISE_CAST_IMPLICIT(uint16_t, 8, int64_t, 2)
    XSIMD_BITWISE_CAST_IMPLICIT(uint16_t, 8, uint64_t, 2)
    XSIMD_BITWISE_CAST_INTRINSIC(uint16_t, 8, float, 4, _mm_castsi128_ps)
    XSIMD_BITWISE_CAST_INTRINSIC(uint16_t, 8, double, 2, _mm_castsi128_pd)
    XSIMD_BITWISE_CAST_IMPLICIT(int32_t, 4, int8_t, 16)
    XSIMD_BITWISE_CAST_IMPLICIT(int32_t, 4, uint8_t, 16)
    XSIMD_BITWISE_CAST_IMPLICIT(int32_t, 4, int16_t, 8)
    XSIMD_BITWISE_CAST_IMPLICIT(int32_t, 4, uint16_t, 8)
    XSIMD_BITWISE_CAST_IMPLICIT(int32_t, 4, int32_t, 4)
    XSIMD_BITWISE_CAST_IMPLICIT(int32_t, 4, uint32_t, 4)
    XSIMD_BITWISE_CAST_IMPLICIT(int32_t, 4, int64_t, 2)
    XSIMD_BITWISE_CAST_IMPLICIT(int32_t, 4, uint64_t, 2)
    XSIMD_BITWISE_CAST_INTRINSIC(int32_t, 4, float, 4, _mm_castsi128_ps)
    XSIMD_BITWISE_CAST_INTRINSIC(int32_t, 4, double, 2, _mm_castsi128_pd)
    XSIMD_BITWISE_CAST_IMPLICIT(uint32_t, 4, int8_t, 16)
    XSIMD_BITWISE_CAST_IMPLICIT(uint32_t, 4, uint8_t, 16)
    XSIMD_BITWISE_CAST_IMPLICIT(uint32_t, 4, int16_t, 8)
    XSIMD_BITWISE_CAST_IMPLICIT(uint32_t, 4, uint16_t, 8)
    XSIMD_BITWISE_CAST_IMPLICIT(uint32_t, 4, int32_t, 4)
    XSIMD_BITWISE_CAST_IMPLICIT(uint32_t, 4, uint32_t, 4)
    XSIMD_BITWISE_CAST_IMPLICIT(uint32_t, 4, int64_t, 2)
    XSIMD_BITWISE_CAST_IMPLICIT(uint32_t, 4, uint64_t, 2)
    XSIMD_BITWISE_CAST_INTRINSIC(uint32_t, 4, float, 4, _mm_castsi128_ps)
    XSIMD_BITWISE_CAST_INTRINSIC(uint32_t, 4, double, 2, _mm_castsi128_pd)
    XSIMD_BITWISE_CAST_IMPLICIT(int64_t, 2, int8_t, 16)
    XSIMD_BITWISE_CAST_IMPLICIT(int64_t, 2, uint8_t, 16)
    XSIMD_BITWISE_CAST_IMPLICIT(int64_t, 2, int16_t, 8)
    XSIMD_BITWISE_CAST_IMPLICIT(int64_t, 2, uint16_t, 8)
    XSIMD_BITWISE_CAST_IMPLICIT(int64_t, 2, int32_t, 4)
    XSIMD_BITWISE_CAST_IMPLICIT(int64_t, 2, uint32_t, 4)
    XSIMD_BITWISE_CAST_IMPLICIT(int64_t, 2, int64_t, 2)
    XSIMD_BITWISE_CAST_IMPLICIT(int64_t, 2, uint64_t, 2)
    XSIMD_BITWISE_CAST_INTRINSIC(int64_t, 2, float, 4, _mm_castsi128_ps)
    XSIMD_BITWISE_CAST_INTRINSIC(int64_t, 2, double, 2, _mm_castsi128_pd)
    XSIMD_BITWISE_CAST_IMPLICIT(uint64_t, 2, int8_t, 16)
    XSIMD_BITWISE_CAST_IMPLICIT(uint64_t, 2, uint8_t, 16)
    XSIMD_BITWISE_CAST_IMPLICIT(uint64_t, 2, int16_t, 8)
    XSIMD_BITWISE_CAST_IMPLICIT(uint64_t, 2, uint16_t, 8)
    XSIMD_BITWISE_CAST_IMPLICIT(uint64_t, 2, int32_t, 4)
    XSIMD_BITWISE_CAST_IMPLICIT(uint64_t, 2, uint32_t, 4)
    XSIMD_BITWISE_CAST_IMPLICIT(uint64_t, 2, int64_t, 2)
    XSIMD_BITWISE_CAST_IMPLICIT(uint64_t, 2, uint64_t, 2)
    XSIMD_BITWISE_CAST_INTRINSIC(uint64_t, 2, float, 4, _mm_castsi128_ps)
    XSIMD_BITWISE_CAST_INTRINSIC(uint64_t, 2, double, 2, _mm_castsi128_pd)
    XSIMD_BITWISE_CAST_INTRINSIC(float, 4, int8_t, 16, _mm_castps_si128)
    XSIMD_BITWISE_CAST_INTRINSIC(float, 4, uint8_t, 16, _mm_castps_si128)
    XSIMD_BITWISE_CAST_INTRINSIC(float, 4, int16_t, 8, _mm_castps_si128)
    XSIMD_BITWISE_CAST_INTRINSIC(float, 4, uint16_t, 8, _mm_castps_si128)
    XSIMD_BITWISE_CAST_INTRINSIC(float, 4, int32_t, 4, _mm_castps_si128)
    XSIMD_BITWISE_CAST_INTRINSIC(float, 4, uint32_t, 4, _mm_castps_si128)
    XSIMD_BITWISE_CAST_INTRINSIC(float, 4, int64_t, 2, _mm_castps_si128)
    XSIMD_BITWISE_CAST_INTRINSIC(float, 4, uint64_t, 2, _mm_castps_si128)
    XSIMD_BITWISE_CAST_IMPLICIT(float, 4, float, 4)
    XSIMD_BITWISE_CAST_INTRINSIC(float, 4, double, 2, _mm_castps_pd)
    XSIMD_BITWISE_CAST_INTRINSIC(double, 2, int8_t, 16, _mm_castpd_si128)
    XSIMD_BITWISE_CAST_INTRINSIC(double, 2, uint8_t, 16, _mm_castpd_si128)
    XSIMD_BITWISE_CAST_INTRINSIC(double, 2, int16_t, 8, _mm_castpd_si128)
    XSIMD_BITWISE_CAST_INTRINSIC(double, 2, uint16_t, 8, _mm_castpd_si128)
    XSIMD_BITWISE_CAST_INTRINSIC(double, 2, int32_t, 4, _mm_castpd_si128)
    XSIMD_BITWISE_CAST_INTRINSIC(double, 2, uint32_t, 4, _mm_castpd_si128)
    XSIMD_BITWISE_CAST_INTRINSIC(double, 2, int64_t, 2, _mm_castpd_si128)
    XSIMD_BITWISE_CAST_INTRINSIC(double, 2, uint64_t, 2, _mm_castpd_si128)
    XSIMD_BITWISE_CAST_INTRINSIC(double, 2, float, 4, _mm_castpd_ps)
    XSIMD_BITWISE_CAST_IMPLICIT(double, 2, double, 2)
}

#endif
