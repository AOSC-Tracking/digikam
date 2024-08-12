/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *
 * SPDX-FileCopyrightText: 2007-2022 Marble Team
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#pragma once

/**
 * 3-Dim Vector and Matrix Definitions and Operations
 */

// C++ includes

#include <iostream>

// Local includes

#include "digikam_export.h"

namespace Marble
{

DIGIKAM_EXPORT double atan20(double y, double x);

class DIGIKAM_EXPORT Vec3
{
private:

    double v[3];

public:

    friend class Mat3;

    Vec3(double x = 0, double y = 0, double z = 0);
    Vec3(const Vec3& c);
    void assign(double x = 0, double y = 0, double z = 0);
    double& operator [](unsigned index);
    Vec3& operator = (const Vec3& c);
    Vec3& operator += (const Vec3& c);
    Vec3& operator -= (const Vec3& c);
    Vec3& operator *= (const Vec3& c);   // cross product
    Vec3& operator *= (double r);
    Vec3& operator /= (double r);
    friend double abs(const Vec3& c);               // absolute value
    friend double dot(const Vec3& c1, const Vec3& c2);    // dot product
    friend Vec3 operator + (const Vec3& c1, const Vec3& c2);
    friend Vec3 operator - (const Vec3& c1, const Vec3& c2);
    friend Vec3 operator * (double r, const Vec3& c1);
    friend Vec3 operator * (const Vec3& c1, double r);
    friend Vec3 operator * (const Vec3& c1, const Vec3& c2);  // cross product
    friend Vec3 operator / (const Vec3& c1, double r);
    friend Vec3 vnorm(const Vec3& c);       // norm vector
    friend Vec3 carpol(const Vec3& c);     // Cartesian -> Polar
    friend Vec3 polcar(const Vec3& c);     // Polar -> Cartesian
    friend std::ostream& operator << (std::ostream& os, const Vec3& c);
};

/********************************************************************/

// class Mat3: public Vec3
class DIGIKAM_EXPORT Mat3
{
public:

    double m[3][3];

    explicit Mat3(double x = 0);
    Mat3(const Mat3& c);
    void assign(double x11, double x12, double x13,  double x21, double x22,
                double x23, double x31, double x32, double x33);
    void assign(double x[3][3]);              // assign matrix
    void PutMij(double x, int i, int j);     // put single matrix element
    double GetMij(int i, int j);             // get single matrix element
    Mat3& operator = (const Mat3& c);
    Mat3& operator += (const Mat3& c);
    Mat3& operator -= (const Mat3& c);
    Mat3& operator *= (const Mat3& c);
    Mat3& operator *= (double r);
    Mat3& operator /= (double r);
    friend Mat3 mxtrn(const Mat3& m1);   // transposed matrix
    friend double mxdet(const Mat3& c);  // determinant
    friend Mat3 operator + (const Mat3& c1, const Mat3& c2);
    friend Mat3 operator - (const Mat3& c1, const Mat3& c2);
    friend Mat3 operator * (double r, const Mat3& c1);
    friend Mat3 operator * (const Mat3& c1, double r);
    friend Mat3 operator * (const Mat3& c1, const Mat3& c2);
    friend Mat3 operator / (const Mat3& c1, double r);
    friend Vec3 mxvct(const Mat3& m1, Vec3& v1);   // multiply vector with matrix
    friend void gpyr(const Mat3& m1, double& p, double& y, double& r);  // get p/y/r
    friend void mxevc(const Mat3& m, double& a, Vec3& v);   // eigenvector
    friend std::ostream& operator << (std::ostream& os, const Mat3& c);
};

// ****************************************************************************

// defining the following functions here seems to make more compilers happy

Mat3 mxcon(double r);   // constant matrix
Mat3 mxidn();           // identity matrix
//  friend Mat3 mxtrn (const Mat3& m1);  // transposed matrix
Mat3 xrot(double a);   // rotation around x-axis
Mat3 yrot(double a);   // rotation around y-axis
Mat3 zrot(double a);   // rotation around z-axis

Mat3 csmx(double p, double y, double r);  // pitch/yaw/roll matrix
void vcpy(Vec3& v, double& p, double& y);  // get pitch and yaw from vector
void vcrp(Vec3& v, double& p, double& r);  // get pitch and roll from vector
Mat3 mxrox(double& a, Vec3& v);   // get matrix from eigenvector and angle

} // namespace Marble