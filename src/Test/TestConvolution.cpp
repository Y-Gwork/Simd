/*
* Tests for Simd Library (http://ermig1979.github.io/Simd).
*
* Copyright (c) 2011-2018 Yermalayeu Ihar.
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
#include "Test/TestUtils.h"
#include "Test/TestPerformance.h"
#include "Test/TestData.h"
#include "Test/TestTensor.h"

#include "Simd/SimdConvolution.h"

namespace Test
{
    namespace
    {
        struct Param
        {
            size_t srcC, srcH, srcW, dstC, kernelY, kernelX, dilationY, dilationX, strideY, strideX, padY, padX, padH, padW, group;

            Param(size_t sC, size_t sH, size_t sW, size_t dC, size_t kY, size_t kX, size_t dY, size_t dX, size_t sY, size_t sX, size_t pY, size_t pX, size_t pH, size_t pW, size_t g)
                : srcC(sC), srcH(sH), srcW(sW), dstC(dC), kernelY(kY), kernelX(kX), dilationY(dY), dilationX(dX), strideY(sY), strideX(sX), padY(pY), padX(pX), padH(pH), padW(pW), group(g)
            {}

            Param(size_t sC, size_t sH, size_t sW, size_t dC, Size k, Size d, Size s, Size b, Size e, size_t g)
                : srcC(sC), srcH(sH), srcW(sW), dstC(dC), kernelY(k.y), kernelX(k.x), dilationY(d.y), dilationX(d.x), strideY(s.y), strideX(s.x), padY(b.y), padX(b.x), padH(e.y), padW(e.x), group(g)
            {}
        };

        struct FuncC
        {
            typedef void*(*FuncPtr)(size_t srcC, size_t srcH, size_t srcW, size_t dstC, size_t kernelY, size_t kernelX, size_t dilationY, size_t dilationX, size_t strideY, size_t strideX, size_t padY, size_t padX, size_t padH, size_t padW, size_t group);

            FuncPtr func;
            String description;

            FuncC(const FuncPtr & f, const String & d) : func(f), description(d) {}

            void Update(const Param & p)
            {
                std::stringstream ss;
                ss << description;
                ss << "[" << p.srcC << "x" << p.srcH << "x" << p.srcW;
                ss << "-" << p.dstC << "-" << p.kernelX << "-" << p.strideX << "-" << Simd::Max(p.padX, p.padW) << "-" << p.group;
                ss << "]";
                description = ss.str();
            }

            void Call(const Param & p, const Tensor32f & weight, const Tensor32f & bias, const Tensor32f & src, Tensor32f & buf, Tensor32f & dst) const
            {
                void * convolution = func(p.srcC, p.srcH, p.srcW, p.dstC, p.kernelY, p.kernelX, p.dilationY, p.dilationX, p.strideY, p.strideX, p.padY, p.padX, p.padH, p.padW, p.group);
                buf.Extend({ ::SimdConvolutionBufferSize(convolution) });
                ::SimdConvolutionSetWeight(convolution, weight.Data(), bias.Data());
                {
                    TEST_PERFORMANCE_TEST(description);
                    ::SimdConvolutionForward(convolution, src.Data(), buf.Data(), dst.Data());
                }
               ::SimdRelease(convolution);
            }
        };
    }

#define FUNC_C(function) \
    FuncC(function, std::string(#function))

    bool ConvolutionForwardAutoTest(const Param & p, FuncC f1, FuncC f2)
    {
        bool result = true;

        f1.Update(p);
        f2.Update(p);

        TEST_LOG_SS(Info, "Test " << f1.description << " & " << f2.description << "].");

        Tensor32f src({ p.srcC, p.srcH, p.srcW });
        FillRandom(src.Data(), src.Size(), -1.0, 1.0f);

        Tensor32f weight({p.dstC, p.srcC/p.group, p.kernelY, p.kernelX });
        FillRandom(weight.Data(), weight.Size(), -1.0, 1.0f);

        Tensor32f bias({ p.dstC });
        FillRandom(bias.Data(), bias.Size(), -1.0, 1.0f);

        Tensor32f buf;

        size_t dstH = (p.srcH + p.padY + p.padH - (p.dilationY * (p.kernelY - 1) + 1)) / p.strideY + 1;
        size_t dstW = (p.srcW + p.padX + p.padW - (p.dilationX * (p.kernelX - 1) + 1)) / p.strideX + 1;
        Tensor32f dst1({ p.dstC, dstH, dstW });
        Tensor32f dst2({ p.dstC, dstH, dstW });

        TEST_ALIGN(SIMD_ALIGN);

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(p, weight, bias, src, buf, dst1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(p, weight, bias, src, buf, dst2));

        result = result && Compare(dst1, dst2, EPS, true, 64, DifferenceAbsolute);

        return result;
    }

    bool ConvolutionForwardAutoTest(const FuncC & f1, const FuncC & f2)
    {
        bool result = true;

        Size _0(0, 0), _1(1, 1), _2(2, 2), _3(3, 3), _5(5, 5), _7(7, 7);

        result = result && ConvolutionForwardAutoTest(Param(3, 300, 300, 32, _3, _1, _2, _0, _1, 1), f1, f2);
        result = result && ConvolutionForwardAutoTest(Param(32, 150, 150, 32, _3, _1, _1, _1, _1, 32), f1, f2);

        return result;
    }

    bool ConvolutionForwardAutoTest()
    {
        bool result = true;

        result = result && ConvolutionForwardAutoTest(FUNC_C(Simd::Base::ConvolutionInit), FUNC_C(SimdConvolutionInit));

//#ifdef SIMD_SSE_ENABLE
//        if (Simd::Sse::Enable)
//            result = result && ConvolutionForwardAutoTest(FUNC_C(Simd::Sse::ConvolutionInit), FUNC_C(SimdConvolutionInit));
//#endif 

        return result;
    }
}
