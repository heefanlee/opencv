/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2013, OpenCV Foundation, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the OpenCV Foundation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

#include "test_precomp.hpp"

#include <string>
#include <iostream>
#include <fstream>
#include <iterator>
#include <limits>
#include <numeric>
#include "opencv2/core/ocl.hpp"

using namespace cv;
using namespace std;

class CV_UMatTest : public cvtest::BaseTest
{
public:
    CV_UMatTest() {}
    ~CV_UMatTest() {}
protected:
    void run(int);

    struct test_excep
    {
        test_excep(const string& _s=string("")) : s(_s) {};
        string s;
    };

    bool TestUMat();

    void checkDiff(const Mat& m1, const Mat& m2, const string& s)
    {
        if (norm(m1, m2, NORM_INF) != 0)
            throw test_excep(s);
    }
    void checkDiffF(const Mat& m1, const Mat& m2, const string& s)
    {
        if (norm(m1, m2, NORM_INF) > 1e-5)
            throw test_excep(s);
    }
};

#define STR(a) STR2(a)
#define STR2(a) #a

#define CHECK_DIFF(a, b) checkDiff(a, b, "(" #a ")  !=  (" #b ")  at l." STR(__LINE__))
#define CHECK_DIFF_FLT(a, b) checkDiffF(a, b, "(" #a ")  !=(eps)  (" #b ")  at l." STR(__LINE__))


bool CV_UMatTest::TestUMat()
{
    try
    {
        Mat a(100, 100, CV_16S), b;
        randu(a, Scalar::all(-100), Scalar::all(100));
        Rect roi(1, 3, 10, 20);
        Mat ra(a, roi), rb;
        UMat ua, ura;
        a.copyTo(ua);
        ua.copyTo(b);
        CHECK_DIFF(a, b);

        ura = ua(roi);
        ura.copyTo(rb);

        CHECK_DIFF(ra, rb);

        ra += Scalar::all(1.f);
        {
        Mat temp = ura.getMat(ACCESS_RW);
        temp += Scalar::all(1.f);
        }
        ra.copyTo(rb);
        CHECK_DIFF(ra, rb);
    }
    catch (const test_excep& e)
    {
        ts->printf(cvtest::TS::LOG, "%s\n", e.s.c_str());
        ts->set_failed_test_info(cvtest::TS::FAIL_MISMATCH);
        return false;
    }
    return true;
}

void CV_UMatTest::run( int /* start_from */)
{
    printf("Use OpenCL: %s\nHave OpenCL: %s\n",
           ocl::useOpenCL() ? "TRUE" : "FALSE",
           ocl::haveOpenCL() ? "TRUE" : "FALSE" );

    if (!TestUMat())
        return;

    ts->set_failed_test_info(cvtest::TS::OK);
}

TEST(Core_UMat, base) { CV_UMatTest test; test.safe_run(); }
