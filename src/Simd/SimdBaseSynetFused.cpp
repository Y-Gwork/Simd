/*
* Simd Library (http://ermig1979.github.io/Simd).
*
* Copyright (c) 2011-2019 Yermalayeu Ihar.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#include "Simd/SimdMemory.h"
#include "Simd/SimdSynet.h"

namespace Simd
{
    namespace Base
    {
        void SynetFusedLayerForward0Nchw(const float * src, const float * bias, const float * scale, size_t channels, size_t spatial, float * dst)
        {
            size_t aligned = Simd::AlignLo(spatial, 4);
            for (size_t c = 0; c < channels; ++c)
            {
                float _bias = bias[c];
                float _scale = scale[c];
                size_t s = 0;
                for (; s < aligned; s += 4)
                {
                    dst[s + 0] = SynetFusedLayerForward0(src[s + 0] + _bias, _scale);
                    dst[s + 1] = SynetFusedLayerForward0(src[s + 1] + _bias, _scale);
                    dst[s + 2] = SynetFusedLayerForward0(src[s + 2] + _bias, _scale);
                    dst[s + 3] = SynetFusedLayerForward0(src[s + 3] + _bias, _scale);
                }
                for (; s < spatial; ++s)
                    dst[s] = SynetFusedLayerForward0(src[s] + _bias, _scale);
                src += spatial;
                dst += spatial;
            }
        }

        void SynetFusedLayerForward0Nhwc(const float * src, const float * bias, const float * scale, size_t channels, size_t spatial, float * dst)
        {
            size_t aligned = Simd::AlignLo(channels, 4);
            for (size_t s = 0; s < spatial; ++s)
            {
                size_t c = 0;
                for (; c < aligned; c += 4)
                {
                    dst[c + 0] = SynetFusedLayerForward0(src[c + 0] + bias[c + 0], scale[c + 0]);
                    dst[c + 1] = SynetFusedLayerForward0(src[c + 1] + bias[c + 1], scale[c + 1]);
                    dst[c + 2] = SynetFusedLayerForward0(src[c + 2] + bias[c + 2], scale[c + 2]);
                    dst[c + 3] = SynetFusedLayerForward0(src[c + 3] + bias[c + 3], scale[c + 3]);
                }
                for (; c < channels; ++c)
                    dst[c] = SynetFusedLayerForward0(src[c] + bias[c], scale[c]);
                src += channels;
                dst += channels;
            }
        }

        template<int N> void SynetFusedLayerForward0NchwXc(const float * src, const float * bias, const float * scale, size_t channels, size_t spatial, float * dst)
        {
            for (size_t c = 0; c < channels; c += N)
            {
                for (size_t s = 0; s < spatial; ++s)
                {
                    for (size_t i = 0; i < N; ++i)
                        dst[i] = SynetFusedLayerForward0(src[i] + bias[i], scale[i]);
                    src += N;
                    dst += N;
                }
                bias += N;
                scale += N;
            }
        }

        void SynetFusedLayerForward0(const float * src, const float * bias, const float * scale, size_t channels, size_t spatial, float * dst, SimdTensorFormatType format)
        {
            if (Base::NchwCompatible(channels, spatial, format))
                SynetFusedLayerForward0Nchw(src, bias, scale, channels, spatial, dst);
            else if (Base::NhwcCompatible(channels, spatial, format))
                SynetFusedLayerForward0Nhwc(src, bias, scale, channels, spatial, dst);
            else if (format == SimdTensorFormatNchw4c)
                SynetFusedLayerForward0NchwXc<4>(src, bias, scale, channels, spatial, dst);
            else if (format == SimdTensorFormatNchw8c)
                SynetFusedLayerForward0NchwXc<8>(src, bias, scale, channels, spatial, dst);
            else if (format == SimdTensorFormatNchw16c)
                SynetFusedLayerForward0NchwXc<16>(src, bias, scale, channels, spatial, dst);
            else
                assert(0);
        }

        //---------------------------------------------------------------------

        void SynetFusedLayerForward1Nchw(const float * src, const float * bias0, const float * scale1, const float * bias1, size_t channels, size_t spatial, float * dst)
        {
            size_t aligned = Simd::AlignLo(spatial, 4);
            for (size_t c = 0; c < channels; ++c)
            {
                float _bias0 = bias0[c];
                float _scale1 = scale1[c];
                float _bias1 = bias1[c];
                size_t s = 0;
                for (; s < aligned; s += 4)
                {
                    dst[s + 0] = SynetFusedLayerForward1(src[s + 0] + _bias0, _scale1, _bias1);
                    dst[s + 1] = SynetFusedLayerForward1(src[s + 1] + _bias0, _scale1, _bias1);
                    dst[s + 2] = SynetFusedLayerForward1(src[s + 2] + _bias0, _scale1, _bias1);
                    dst[s + 3] = SynetFusedLayerForward1(src[s + 3] + _bias0, _scale1, _bias1);
                }
                for (; s < spatial; ++s)
                    dst[s] = SynetFusedLayerForward1(src[s] + _bias0, _scale1, _bias1);
                src += spatial;
                dst += spatial;
            }
        }

        void SynetFusedLayerForward1Nhwc(const float * src, const float * bias0, const float * scale1, const float * bias1, size_t channels, size_t spatial, float * dst)
        {
            size_t aligned = Simd::AlignLo(channels, 4);
            for (size_t s = 0; s < spatial; ++s)
            {
                size_t c = 0;
                for (; c < aligned; c += 4)
                {
                    dst[c + 0] = SynetFusedLayerForward1(src[c + 0] + bias0[c + 0], scale1[c + 0], bias1[c + 0]);
                    dst[c + 1] = SynetFusedLayerForward1(src[c + 1] + bias0[c + 1], scale1[c + 1], bias1[c + 1]);
                    dst[c + 2] = SynetFusedLayerForward1(src[c + 2] + bias0[c + 2], scale1[c + 2], bias1[c + 2]);
                    dst[c + 3] = SynetFusedLayerForward1(src[c + 3] + bias0[c + 3], scale1[c + 3], bias1[c + 3]);
                }
                for (; c < channels; ++c)
                    dst[c] = SynetFusedLayerForward1(src[c] + bias0[c], scale1[c], bias1[c]);
                src += channels;
                dst += channels;
            }
        }

        template<int N> void SynetFusedLayerForward1NchwXc(const float * src, const float * bias0, const float * scale1, const float * bias1, size_t channels, size_t spatial, float * dst)
        {
            for (size_t c = 0; c < channels; c += N)
            {
                for (size_t s = 0; s < spatial; ++s)
                {
                    for (size_t i = 0; i < N; ++i)
                        dst[i] = SynetFusedLayerForward1(src[i] + bias0[i], scale1[i], bias1[i]);
                    src += N;
                    dst += N;
                }
                bias0 += N;
                scale1 += N;
                bias1 += N;
            }
        }

        void SynetFusedLayerForward1(const float * src, const float * bias0, const float * scale1, const float * bias1, size_t channels, size_t spatial, float * dst, SimdTensorFormatType format)
        {
            if (Base::NchwCompatible(channels, spatial, format))
                SynetFusedLayerForward1Nchw(src, bias0, scale1, bias1, channels, spatial, dst);
            else if (Base::NhwcCompatible(channels, spatial, format))
                SynetFusedLayerForward1Nhwc(src, bias0, scale1, bias1, channels, spatial, dst);
            else if (format == SimdTensorFormatNchw4c)
                SynetFusedLayerForward1NchwXc<4>(src, bias0, scale1, bias1, channels, spatial, dst);
            else if (format == SimdTensorFormatNchw8c)
                SynetFusedLayerForward1NchwXc<8>(src, bias0, scale1, bias1, channels, spatial, dst);
            else if (format == SimdTensorFormatNchw16c)
                SynetFusedLayerForward1NchwXc<16>(src, bias0, scale1, bias1, channels, spatial, dst);
            else
                assert(0);
        }

        //---------------------------------------------------------------------

        void SynetFusedLayerForward2Nchw(const float * src, const float * scale, const float * bias, size_t channels, size_t spatial, const float * slope, float * dst)
        {
            float _slope = slope[0];
            size_t aligned = Simd::AlignLo(spatial, 4);
            for (size_t c = 0; c < channels; ++c)
            {
                float _scale = scale[c];
                float _bias = bias[c];
                size_t s = 0;
                for (; s < aligned; s += 4)
                {
                    dst[s + 0] = SynetFusedLayerForward2(src[s + 0], _scale, _bias, _slope);
                    dst[s + 1] = SynetFusedLayerForward2(src[s + 1], _scale, _bias, _slope);
                    dst[s + 2] = SynetFusedLayerForward2(src[s + 2], _scale, _bias, _slope);
                    dst[s + 3] = SynetFusedLayerForward2(src[s + 3], _scale, _bias, _slope);
                }
                for (; s < spatial; ++s)
                    dst[s] = SynetFusedLayerForward2(src[s], _scale, _bias, _slope);
                src += spatial;
                dst += spatial;
            }
        }

        void SynetFusedLayerForward2Nhwc(const float * src, const float * scale, const float * bias, size_t channels, size_t spatial, const float * slope, float * dst)
        {
            float _slope = slope[0];
            size_t aligned = Simd::AlignLo(channels, 4);
            for (size_t s = 0; s < spatial; ++s)
            {
                size_t c = 0;
                for (; c < aligned; c += 4)
                {
                    dst[c + 0] = SynetFusedLayerForward2(src[c + 0], scale[c + 0], bias[c + 0], _slope);
                    dst[c + 1] = SynetFusedLayerForward2(src[c + 1], scale[c + 1], bias[c + 1], _slope);
                    dst[c + 2] = SynetFusedLayerForward2(src[c + 2], scale[c + 2], bias[c + 2], _slope);
                    dst[c + 3] = SynetFusedLayerForward2(src[c + 3], scale[c + 3], bias[c + 3], _slope);
                }
                for (; c < channels; ++c)
                    dst[c] = SynetFusedLayerForward2(src[c], scale[c], bias[c], _slope);
                src += channels;
                dst += channels;
            }
        }

        template<int N> void SynetFusedLayerForward2NchwXc(const float * src, const float * scale, const float * bias, size_t channels, size_t spatial, const float * slope, float * dst)
        {
            float _slope = slope[0];
            for (size_t c = 0; c < channels; c += N)
            {
                for (size_t s = 0; s < spatial; ++s)
                {
                    for (size_t i = 0; i < N; ++i)
                        dst[i] = SynetFusedLayerForward2(src[i], scale[i], bias[i], _slope);
                    src += N;
                    dst += N;
                }
                scale += N;
                bias += N;
            }
        }

        void SynetFusedLayerForward2(const float * src, const float * scale, const float * bias, size_t channels, size_t spatial, const float * slope, float * dst, SimdTensorFormatType format)
        {
            if (Base::NchwCompatible(channels, spatial, format))
                SynetFusedLayerForward2Nchw(src, scale, bias, channels, spatial, slope, dst);
            else if (Base::NhwcCompatible(channels, spatial, format))
                SynetFusedLayerForward2Nhwc(src, scale, bias, channels, spatial, slope, dst);
            else if (format == SimdTensorFormatNchw4c)
                SynetFusedLayerForward2NchwXc<4>(src, scale, bias, channels, spatial, slope, dst);
            else if (format == SimdTensorFormatNchw8c)
                SynetFusedLayerForward2NchwXc<8>(src, scale, bias, channels, spatial, slope, dst);
            else if (format == SimdTensorFormatNchw16c)
                SynetFusedLayerForward2NchwXc<16>(src, scale, bias, channels, spatial, slope, dst);
            else
                assert(0);
        }

        //---------------------------------------------------------------------

        void SynetFusedLayerForward3Nchw(const float * src, const float * bias, const float * scale, size_t channels, size_t spatial, float * dst)
        {
            size_t aligned = Simd::AlignLo(spatial, 4);
            for (size_t c = 0; c < channels; ++c)
            {
                float _bias = bias[c];
                float _scale = scale[c];
                size_t s = 0;
                for (; s < aligned; s += 4)
                {
                    dst[s + 0] = SynetFusedLayerForward3(src[s + 0] + _bias, _scale);
                    dst[s + 1] = SynetFusedLayerForward3(src[s + 1] + _bias, _scale);
                    dst[s + 2] = SynetFusedLayerForward3(src[s + 2] + _bias, _scale);
                    dst[s + 3] = SynetFusedLayerForward3(src[s + 3] + _bias, _scale);
                }
                for (; s < spatial; ++s)
                    dst[s] = SynetFusedLayerForward3(src[s] + _bias, _scale);
                src += spatial;
                dst += spatial;
            }
        }

        void SynetFusedLayerForward3Nhwc(const float * src, const float * bias, const float * scale, size_t channels, size_t spatial, float * dst)
        {
             size_t aligned = Simd::AlignLo(channels, 4);
            for (size_t s = 0; s < spatial; ++s)
            {
                size_t c = 0;
                for (; c < aligned; c += 4)
                {
                    dst[c + 0] = SynetFusedLayerForward3(src[c + 0] + bias[c + 0], scale[c + 0]);
                    dst[c + 1] = SynetFusedLayerForward3(src[c + 1] + bias[c + 1], scale[c + 1]);
                    dst[c + 2] = SynetFusedLayerForward3(src[c + 2] + bias[c + 2], scale[c + 2]);
                    dst[c + 3] = SynetFusedLayerForward3(src[c + 3] + bias[c + 3], scale[c + 3]);
                }
                for (; c < channels; ++c)
                    dst[c] = SynetFusedLayerForward3(src[c] + bias[c], scale[c]);
                src += channels;
                dst += channels;
            }
        }

        template<int N> void SynetFusedLayerForward3NchwXc(const float * src, const float * bias, const float * scale, size_t channels, size_t spatial, float * dst)
        {
            for (size_t c = 0; c < channels; c += N)
            {
                for (size_t s = 0; s < spatial; ++s)
                {
                    for (size_t i = 0; i < N; ++i)
                        dst[i] = SynetFusedLayerForward3(src[i] + bias[i], scale[i]);
                    src += N;
                    dst += N;
                }
                bias += N;
                scale += N;
            }
        }

        void SynetFusedLayerForward3(const float * src, const float * bias, const float * scale, size_t channels, size_t spatial, float * dst, SimdTensorFormatType format)
        {
            if (Base::NchwCompatible(channels, spatial, format))
                SynetFusedLayerForward3Nchw(src, bias, scale, channels, spatial, dst);
            else if (Base::NhwcCompatible(channels, spatial, format))
                SynetFusedLayerForward3Nhwc(src, bias, scale, channels, spatial, dst);
            else if (format == SimdTensorFormatNchw4c)
                SynetFusedLayerForward3NchwXc<4>(src, bias, scale, channels, spatial, dst);
            else if (format == SimdTensorFormatNchw8c)
                SynetFusedLayerForward3NchwXc<8>(src, bias, scale, channels, spatial, dst);
            else if (format == SimdTensorFormatNchw16c)
                SynetFusedLayerForward3NchwXc<16>(src, bias, scale, channels, spatial, dst);
            else
                assert(0);
        }

        //---------------------------------------------------------------------

        void SynetFusedLayerForward4(const float * src, const float * bias0, const float * scale1, const float * bias1, size_t count, size_t size, float * dst, SimdBool trans)
        {
            float s1 = scale1[0], b1 = bias1[0];
            if ((trans || size == 1) && count != 1)
            {
                size_t aligned = Simd::AlignLo(count, 4);
                float * dst0 = dst, * dst1 = dst + count;
                for (size_t j = 0; j < size; ++j)
                {
                    size_t i = 0;
                    for (; i < aligned; i += 4)
                    {
                        SynetFusedLayerForward4(src[i + 0], bias0[i + 0], s1, b1, dst0 + i + 0, dst1 + i + 0);
                        SynetFusedLayerForward4(src[i + 1], bias0[i + 1], s1, b1, dst0 + i + 1, dst1 + i + 1);
                        SynetFusedLayerForward4(src[i + 2], bias0[i + 2], s1, b1, dst0 + i + 2, dst1 + i + 2);
                        SynetFusedLayerForward4(src[i + 3], bias0[i + 3], s1, b1, dst0 + i + 3, dst1 + i + 3);
                    }
                    for (; i < count; ++i)
                        SynetFusedLayerForward4(src[i], bias0[i], s1, b1, dst0 + i, dst1 + i);
                    src += count;
                    dst0 += 2*count;
                    dst1 += 2 * count;
                }
            }
            else
            {
                size_t aligned = Simd::AlignLo(size, 4);
                float * dst0 = dst, * dst1 = dst + count * size;
                for (size_t i = 0; i < count; ++i)
                {
                    float b0 = bias0[i];
                    size_t j = 0;
                    for (; j < aligned; j += 4)
                    {
                        SynetFusedLayerForward4(src[j + 0], b0, s1, b1, dst0 + j + 0, dst1 + j + 0);
                        SynetFusedLayerForward4(src[j + 1], b0, s1, b1, dst0 + j + 1, dst1 + j + 1);
                        SynetFusedLayerForward4(src[j + 2], b0, s1, b1, dst0 + j + 2, dst1 + j + 2);
                        SynetFusedLayerForward4(src[j + 3], b0, s1, b1, dst0 + j + 3, dst1 + j + 3);
                    }
                    for (; j < size; ++j)
                        SynetFusedLayerForward4(src[j], b0, s1, b1, dst0 + j, dst1 + j);
                    src += size;
                    dst0 += size;
                    dst1 += size;
                }
            }
        }

        //---------------------------------------------------------------------

        void SynetFusedLayerForward8(const float * src0, const float * src1, const float * src2, size_t count, size_t size, float * dst, SimdBool trans)
        {
            if ((trans || size == 1) && count != 1)
            {
                size_t aligned = Simd::AlignLo(count, 4);
                for (size_t j = 0; j < size; ++j)
                {
                    size_t i = 0;
                    for (; i < aligned; i += 4)
                    {
                        dst[i + 0] = SynetFusedLayerForward8(src0[i + 0], src1[i + 0], src2[i + 0]);
                        dst[i + 1] = SynetFusedLayerForward8(src0[i + 1], src1[i + 1], src2[i + 1]);
                        dst[i + 2] = SynetFusedLayerForward8(src0[i + 2], src1[i + 2], src2[i + 2]);
                        dst[i + 3] = SynetFusedLayerForward8(src0[i + 3], src1[i + 3], src2[i + 3]);
                    }
                    for (; i < count; ++i)
                        dst[i] = SynetFusedLayerForward8(src0[i], src1[i], src2[i]);
                    src0 += count;
                    src1 += count;
                    dst += count;
                }
            }
            else
            {
                size_t aligned = Simd::AlignLo(size, 4);
                for (size_t i = 0; i < count; ++i)
                {
                    float s2 = src2[i];
                    size_t j = 0;
                    for (; j < aligned; j += 4)
                    {
                        dst[j + 0] = SynetFusedLayerForward8(src0[j + 0], src1[j + 0], s2);
                        dst[j + 1] = SynetFusedLayerForward8(src0[j + 1], src1[j + 1], s2);
                        dst[j + 2] = SynetFusedLayerForward8(src0[j + 2], src1[j + 2], s2);
                        dst[j + 3] = SynetFusedLayerForward8(src0[j + 3], src1[j + 3], s2);
                    }
                    for (; j < size; ++j)
                        dst[j] = SynetFusedLayerForward8(src0[j], src1[j], s2);
                    src0 += size;
                    src1 += size;
                    dst += size;
                }
            }
        }

        //---------------------------------------------------------------------

        void SynetFusedLayerForward9(const float * src0, const float * src1, const float * scale0, const float * bias0, size_t count0, size_t count1, size_t size, float * dst0, float * dst1, SimdBool trans)
        {
            const float * scale1 = scale0 + count0;
            const float * bias1 = bias0 + count0;
            if (trans || size == 1)
            {
                if (dst1)
                {
                    for (size_t j = 0; j < size; ++j)
                    {
                        for (size_t i = 0; i < count0; ++i)
                            dst0[i] = SynetFusedLayerForward9(src0[i], scale0[i], bias0[i]), dst1[i] = src0[i];
                        src0 += count0, dst0 += count0, dst1 += count0;
                        for (size_t i = 0; i < count1; ++i)
                            dst0[i] = SynetFusedLayerForward9(src1[i], scale1[i], bias1[i]), dst1[i] = src1[i];
                        src1 += count1, dst0 += count1, dst1 += count1;
                    }
                }
                else
                {
                    for (size_t j = 0; j < size; ++j)
                    {
                        for (size_t i = 0; i < count0; ++i)
                            dst0[i] = SynetFusedLayerForward9(src0[i], scale0[i], bias0[i]);
                        src0 += count0, dst0 += count0;
                        for (size_t i = 0; i < count1; ++i)
                            dst0[i] = SynetFusedLayerForward9(src1[i], scale1[i], bias1[i]);
                        src1 += count1, dst0 += count1;
                    }
                }
            }
            else
            {
                if (dst1)
                {
                    for (size_t i = 0; i < count0; ++i, src0 += size, dst0 += size, dst1 += size)
                        for (size_t j = 0; j < size; ++j)
                            dst0[j] = SynetFusedLayerForward9(src0[j], scale0[i], bias0[i]), dst1[j] = src0[j];
                    for (size_t i = 0; i < count1; ++i, src1 += size, dst0 += size, dst1 += size)
                        for (size_t j = 0; j < size; ++j)
                            dst0[j] = SynetFusedLayerForward9(src1[j], scale1[i], bias1[i]), dst1[j] = src1[j];
                }
                else
                {
                    for (size_t i = 0; i < count0; ++i, src0 += size, dst0 += size)
                        for (size_t j = 0; j < size; ++j)
                            dst0[j] = SynetFusedLayerForward9(src0[j], scale0[i], bias0[i]);
                    for (size_t i = 0; i < count1; ++i, src1 += size, dst0 += size)
                        for (size_t j = 0; j < size; ++j)
                            dst0[j] = SynetFusedLayerForward9(src1[j], scale1[i], bias1[i]);
                }
            }
        }
    }
}
