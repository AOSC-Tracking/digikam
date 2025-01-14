/*****************************************************************************/
// Copyright 2006-2019 Adobe Systems Incorporated
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in
// accordance with the terms of the Adobe license agreement accompanying it.
/*****************************************************************************/

/** \file
 *  Standard gamma functions and color spaces used within the DNG SDK.
 */

#ifndef __dng_color_space__
#define __dng_color_space__

/*****************************************************************************/

#include "dng_1d_function.h"
#include "dng_classes.h"
#include "dng_matrix.h"
#include "dng_types.h"

/*****************************************************************************/

/// \brief A dng_1d_function for gamma encoding in sRGB color space

class dng_function_GammaEncode_sRGB: public dng_1d_function
    {

    public:

        virtual real64 Evaluate (real64 x) const;

        virtual real64 EvaluateInverse (real64 y) const;

        static const dng_1d_function & Get ();

    };

/*****************************************************************************/

/// \brief A dng_1d_function for gamma encoding with 1.8 gamma.

class dng_function_GammaEncode_1_8: public dng_1d_function
    {

    public:

        virtual real64 Evaluate (real64 x) const;

        virtual real64 EvaluateInverse (real64 y) const;

        static const dng_1d_function & Get ();

    };

/*****************************************************************************/

/// \brief A dng_1d_function for gamma encoding with 2.2 gamma.

class dng_function_GammaEncode_2_2: public dng_1d_function
    {

    public:

        virtual real64 Evaluate (real64 x) const;

        virtual real64 EvaluateInverse (real64 y) const;

        static const dng_1d_function & Get ();

    };

/*****************************************************************************/

/// \brief Base class for two-part gamma encoding with a linear + power function.

class dng_function_GammaEncode_TwoPart: public dng_1d_function
    {

    protected:

        real64 fAlpha = 1.0;
        real64 fBeta  = 0.0;
        real64 fSlope = 4.5;
        real64 fGamma = 1.0;

    public:

        real64 Evaluate (real64 x) const override
            {

            if (x <= fBeta)
                return fSlope * x;

            return fAlpha * pow (x, fGamma) - (fAlpha - 1.0);

            }

        real64 EvaluateInverse (real64 y) const override
            {

            if (y <= fSlope * fBeta)
                return y / fSlope;

            else
                return pow ((y + (fAlpha - 1.0)) / fAlpha, 1.0 / fGamma);

            }

    protected:

        dng_function_GammaEncode_TwoPart ()
            {
            }

    };

/*****************************************************************************/

// CICP Transfer Curve Code Value 1, 6, 14, and 15.
//
// Also used by Rec. 2020.
//
// See Recommendation ITU-R BT.2020-2 (10/2015).
//
// https://en.wikipedia.org/wiki/Rec._2020

class dng_function_GammaEncode_Rec709: public dng_function_GammaEncode_TwoPart
    {

    public:

        dng_function_GammaEncode_Rec709 ()
            {
            fAlpha = 1.0992968268094429;
            fBeta  = 0.0180539685108078;
            fSlope = 4.5;
            fGamma = 0.45;
            }

        static const dng_1d_function & Get ()
            {

            static dng_function_GammaEncode_Rec709 static_function;

            return static_function;

            }

    };

typedef dng_function_GammaEncode_Rec709 dng_function_GammaEncode_Rec2020;

/*****************************************************************************/

/// \brief An abstract color space

class dng_color_space
    {

    protected:

        dng_matrix fMatrixToPCS;

        dng_matrix fMatrixFromPCS;

    public:

        virtual ~dng_color_space ();

        /// Return a matrix which transforms source data in this color space into the
        /// Profile Connection Space.

        const dng_matrix & MatrixToPCS () const
            {
            return fMatrixToPCS;
            }

        /// Return a matrix which transforms Profile Connection Space data into this
        /// color space.

        const dng_matrix & MatrixFromPCS () const
            {
            return fMatrixFromPCS;
            }

        /// Predicate which is true if this color space is monochrome (has only a
        /// single column).

        bool IsMonochrome () const
            {
            return fMatrixToPCS.Cols () == 1;
            }

        /// Getter for the gamma function for this color space.

        virtual const dng_1d_function & GammaFunction () const;

        /// Returns true if this color space is linear. (I.e. has gamma 1.0.)

        bool IsLinear () const
            {
            return GammaFunction ().IsIdentity ();
            }

        /// Map an input value through this color space's encoding gamma.

        real64 GammaEncode (real64 x) const
            {
            return GammaFunction ().Evaluate (x);
            }

        /// Map an input value through this color space's decoding gamma (inverse of
        /// the encoding gamma).

        real64 GammaDecode (real64 y) const
            {
            return GammaFunction ().EvaluateInverse (y);
            }

        /// Getter for ICC profile, if this color space has one.
        /// \param size Out parameter which receives size on return.
        /// \param data Receives bytes of profile.
        /// \retval Returns true if this color space has an ICC profile, false otherwise.

        virtual bool ICCProfile (uint32 &size,
                                 const uint8 *&data) const;

    protected:

        dng_color_space ();

        void SetMonochrome ();

        void SetMatrixToPCS (const dng_matrix_3by3 &M);

    };

/*****************************************************************************/

/// Singleton class for sRGB color space.

class dng_space_sRGB: public dng_color_space
    {

    protected:

        dng_space_sRGB ();

    public:

        /// Returns dng_function_GammaEncode_sRGB

        virtual const dng_1d_function & GammaFunction () const;

        /// Returns sRGB IEC61966-2.1 ICC profile

        virtual bool ICCProfile (uint32 &size,
                                 const uint8 *&data) const;

        /// Static method for getting single global instance of this color space.

        static const dng_color_space & Get ();

    };

/*****************************************************************************/

/// \brief Singleton class for AdobeRGB color space.

class dng_space_AdobeRGB: public dng_color_space
    {

    protected:

        dng_space_AdobeRGB ();

    public:

        /// Returns dng_function_GammaEncode_2_2

        virtual const dng_1d_function & GammaFunction () const;

        /// Returns AdobeRGB (1998) ICC profile

        virtual bool ICCProfile (uint32 &size,
                                 const uint8 *&data) const;

        /// Static method for getting single global instance of this color space.

        static const dng_color_space & Get ();

    };

/*****************************************************************************/

/// \brief Singleton class for ColorMatch color space.

class dng_space_ColorMatch: public dng_color_space
    {

    protected:

        dng_space_ColorMatch ();

    public:

        /// Returns dng_function_GammaEncode_1_8

        virtual const dng_1d_function & GammaFunction () const;

        /// Returns ColorMatch RGB ICC profile

        virtual bool ICCProfile (uint32 &size,
                                 const uint8 *&data) const;

        /// Static method for getting single global instance of this color space.

        static const dng_color_space & Get ();

    };

/*****************************************************************************/

/// \brief Singleton class for Display P3 color space.

class dng_space_DisplayP3: public dng_color_space
    {

    protected:

        dng_space_DisplayP3 ();

    public:

        virtual bool ICCProfile (uint32 &size,
                                 const uint8 *&data) const;

        virtual const dng_1d_function & GammaFunction () const;

        static const dng_color_space & Get ();

    };

/*****************************************************************************/

/// \brief Singleton class for Rec. 2020 color space.

class dng_space_Rec2020: public dng_color_space
    {

    protected:

        dng_space_Rec2020 ();

    public:

        virtual bool ICCProfile (uint32 &size,
                                 const uint8 *&data) const;

        virtual const dng_1d_function & GammaFunction () const;

        static const dng_color_space & Get ();

    };

/*****************************************************************************/

/// \brief Singleton class for ProPhoto RGB color space.

class dng_space_ProPhoto: public dng_color_space
    {

    protected:

        dng_space_ProPhoto ();

    public:

        /// Returns dng_function_GammaEncode_1_8

        virtual const dng_1d_function & GammaFunction () const;

        /// Returns ProPhoto RGB ICC profile

        virtual bool ICCProfile (uint32 &size,
                                 const uint8 *&data) const;

        /// Static method for getting single global instance of this color space.

        static const dng_color_space & Get ();

    };

/*****************************************************************************/

/// \brief Singleton class for gamma 1.8 grayscale color space.

class dng_space_GrayGamma18: public dng_color_space
    {

    protected:

        dng_space_GrayGamma18 ();

    public:

        /// Returns dng_function_GammaEncode_1_8

        virtual const dng_1d_function & GammaFunction () const;

        /// Returns simple grayscale gamma 1.8 ICC profile

        virtual bool ICCProfile (uint32 &size,
                                 const uint8 *&data) const;

        /// Static method for getting single global instance of this color space.

        static const dng_color_space & Get ();

    };

/*****************************************************************************/

/// \brief Singleton class for gamma 2.2 grayscale color space.

class dng_space_GrayGamma22: public dng_color_space
    {

    protected:

        dng_space_GrayGamma22 ();

    public:

        /// Returns dng_function_GammaEncode_2_2

        virtual const dng_1d_function & GammaFunction () const;

        /// Returns simple grayscale gamma 2.2 ICC profile

        virtual bool ICCProfile (uint32 &size,
                                 const uint8 *&data) const;

        /// Static method for getting single global instance of this color space.

        static const dng_color_space & Get ();

    };

/*****************************************************************************/

class dng_space_sRGB_Linear: public dng_color_space
    {

    protected:

        dng_space_sRGB_Linear ();

    public:

        virtual bool ICCProfile (uint32 &size,
                                 const uint8 *&data) const;

        static const dng_color_space & Get ();

    };

/*****************************************************************************/

class dng_space_AdobeRGB_Linear: public dng_color_space
    {

    protected:

        dng_space_AdobeRGB_Linear ();

    public:

        virtual bool ICCProfile (uint32 &size,
                                 const uint8 *&data) const;

        static const dng_color_space & Get ();

    };

/*****************************************************************************/

class dng_space_ProPhoto_Linear: public dng_color_space
    {

    protected:

        dng_space_ProPhoto_Linear ();

    public:

        virtual bool ICCProfile (uint32 &size,
                                 const uint8 *&data) const;

        static const dng_color_space & Get ();

    };

/*****************************************************************************/

class dng_space_Gray_Linear: public dng_color_space
    {

    protected:

        dng_space_Gray_Linear ();

    public:

        virtual bool ICCProfile (uint32 &size,
                                 const uint8 *&data) const;

        static const dng_color_space & Get ();

    };

/*****************************************************************************/

class dng_space_LinearP3: public dng_color_space
    {

    protected:

        dng_space_LinearP3 ();

    public:

        virtual bool ICCProfile (uint32 &size,
                                 const uint8 *&data) const;

        static const dng_color_space & Get ();

    };

/*****************************************************************************/

class dng_space_Rec2020_Linear: public dng_color_space
    {

    protected:

        dng_space_Rec2020_Linear ();

    public:

        virtual bool ICCProfile (uint32 &size,
                                 const uint8 *&data) const;

        static const dng_color_space & Get ();

    };

/*****************************************************************************/

class dng_space_fakeRGB: public dng_color_space
    {

    protected:

        dng_space_fakeRGB ();

    public:

        static const dng_color_space & Get ();

    };

/*****************************************************************************/

#endif

/*****************************************************************************/
