/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2003-01-17
 * Description : Haar 2d transform
 *               Wavelet algorithms, metric and query ideas based on the paper
 *               "Fast Multiresolution Image Querying"
 *               by Charles E. Jacobs, Adam Finkelstein and David H. Salesin.
 *               https://grail.cs.washington.edu/wp-content/uploads/2015/08/jacobs-1995.pdf
 *
 * SPDX-FileCopyrightText: 2003      by Ricardo Niederberger Cabral <nieder at mail dot ru>
 * SPDX-FileCopyrightText: 2008-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2008-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// C++ includes

#include <cstring>

// Qt includes

#include <QtGlobal>

class QImage;

namespace Digikam
{

class DImg;

namespace Haar
{

/**
 * Weights for the Haar coefficients. Straight from the referenced paper
 * "Fast Multiresolution Image Querying"
 * by Charles E. Jacobs, Adam Finkelstein and David H. Salesin.
 * https://www.cs.washington.edu/homes/salesin/abstracts.html
 */
static const float s_haar_weights[2][6][3] =
{
    // For scanned picture (sketch=0):
    //       Y      I      Q         idx    total       occurs

    {
        { 5.00F, 19.21F, 34.37F },   // 0   58.58       1 (`DC' component)
        { 0.83F,  1.26F,  0.36F },   // 1    2.45       3
        { 1.01F,  0.44F,  0.45F },   // 2    1.90       5
        { 0.52F,  0.53F,  0.14F },   // 3    1.19       7
        { 0.47F,  0.28F,  0.18F },   // 4    0.93       9
        { 0.30F,  0.14F,  0.27F }
    },  // 5      0.71    16384-25=16359

    // For handdrawn/painted sketch (sketch=1):
    //       Y      I      Q

    {
        { 4.04F, 15.14F, 22.62F },
        { 0.78F,  0.92F,  0.40F },
        { 0.46F,  0.53F,  0.63F },
        { 0.42F,  0.26F,  0.25F },
        { 0.41F,  0.14F,  0.15F },
        { 0.32F,  0.07F,  0.38F }
    }
};

/**
 * Number of pixels on one side of image; required to be a power of 2.
 */
enum { NumberOfPixels = 128 };

/**
 * Total pixels in a square image.
 */
enum { NumberOfPixelsSquared = NumberOfPixels * NumberOfPixels };

/**
 * Number of Haar coefficients we retain as signature for an image.
 */
enum { NumberOfCoefficients = 40 };

typedef double Unit;

/**
 * Keep this definition constant at qint32 (guaranteed binary size!)
 */
typedef qint32 Idx;

// ---------------------------------------------------------------------------------

class ImageData
{
public:

    Unit data1[NumberOfPixelsSquared] = { 0.0 };
    Unit data2[NumberOfPixelsSquared] = { 0.0 };
    Unit data3[NumberOfPixelsSquared] = { 0.0 };

    void fillPixelData(const QImage& image);
    void fillPixelData(const DImg& image);
};

// ---------------------------------------------------------------------------------

class SignatureData
{
public:

    /**
     * Y/I/Q positions with largest magnitude
     */
    Haar::Idx sig[3][Haar::NumberOfCoefficients] = { { 0 } };

    /**
     * YIQ for position [0,0]
     */
    double    avg[3] = { 0.0 };
};

// ---------------------------------------------------------------------------------

/**
 * This class provides very fast lookup if a certain pixel
 * is set (positive or negative) in the loaded coefficient set.
 */
class SignatureMap
{
public:

    SignatureMap()
    {
        m_indexList = new MapIndexType[2 * Haar::NumberOfPixelsSquared];
    }

    ~SignatureMap()
    {
        delete[] m_indexList;
    }

    /// Load a set of coefficients

    void fill(const Haar::Idx* const coefs)
    {
        // For maximum performance, we use a flat array.
        // First 16k for negative values, second 16k for positive values.
        // All values or false, only 2*40 are true.

        memset(m_indexList, 0, sizeof(MapIndexType[2 * Haar::NumberOfPixelsSquared]));
        int x = 0;

        for (int i = 0 ; i < Haar::NumberOfCoefficients ; ++i)
        {
            x              = coefs[i] + Haar::NumberOfPixelsSquared;
            m_indexList[x] = true;
        }
    }

    /// Query if the given index is set.
    /// Index must be in the range -16383..16383.

    bool operator[](Haar::Idx index) const
    {
        return m_indexList[index + Haar::NumberOfPixelsSquared];
    }

private:

    // To prevent cppcheck warnings.

    explicit SignatureMap(const SignatureMap& other)
    {
        m_indexList = new MapIndexType[2 * Haar::NumberOfPixelsSquared];
        memcpy(m_indexList, other.m_indexList, sizeof(MapIndexType[2 * Haar::NumberOfPixelsSquared]));
    }

public:

    typedef bool  MapIndexType;
    MapIndexType* m_indexList  = nullptr;

private:

    SignatureMap& operator=(const SignatureMap&); // Disable
};

// ---------------------------------------------------------------------------------

class WeightBin
{
public:

    WeightBin();

    unsigned char bin(int index)    const
    {
        if ((index >= 0) && (index < 16384))
        {
            return m_bin[index];
        }

        return 0;
    }

    unsigned char binAbs(int index) const
    {
        if      ((index > 0) && (index < 16384))
        {
           return m_bin[index];
        }
        else if ((index > -16384) && (index <= 0))
        {
           return m_bin[-index];
        }

        return 0;
    }

public:

    /**
     * Fixed weight mask for pixel positions (i,j).
     * Each entry x = i*NUM_PIXELS + j, gets value max(i,j) saturated at 5.
     * To be treated as a constant.
     */
    unsigned char m_bin[16384] = { 0 };
};

// ---------------------------------------------------------------------------------

class Weights
{
public:

    enum SketchType
    {
        ScannedSketch = 0,
        PaintedSketch = 1
    };

public:

    explicit Weights(SketchType type = ScannedSketch)
        : m_type(type)
    {
    }

    float weight(int weight, int channel) const
    {
        return (s_haar_weights[(int)m_type][weight][channel]);
    }

    float weightForAverage(int channel)   const
    {
        return (s_haar_weights[(int)m_type][0][channel]);
    }

private:

    SketchType m_type = ScannedSketch;
};

// ---------------------------------------------------------------------------------

class Calculator
{

public:

    Calculator()  = default;
    ~Calculator() = default;

    int  calcHaar(ImageData* const imageData, SignatureData* const sigData);

    void transform(ImageData* const data);

private:

    void        haar2D(Unit a[]);
    inline void getmLargests(Unit* const cdata, Idx* const sig);
};

} // namespace Haar

} // namespace Digikam
