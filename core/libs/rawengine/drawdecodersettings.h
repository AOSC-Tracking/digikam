/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-12-09
 * Description : Raw decoding settings
 *
 * SPDX-FileCopyrightText: 2006-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2006-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2007-2008 by Guillaume Castagnino <casta at xwing dot info>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QRect>
#include <QString>
#include <QDebug>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT DRawDecoderSettings
{

public:

    /**
     * RAW decoding Interpolation methods
     *
     * NOTE: from original dcraw demosaic
     *
     *  Bilinear: use high-speed but low-quality bilinear
     *            interpolation (default - for slow computer). In this method,
     *            the red value of a non-red pixel is computed as the average of
     *            the adjacent red pixels, and similar for blue and green.
     *  VNG:      use Variable Number of Gradients interpolation.
     *            This method computes gradients near the pixel of interest and uses
     *            the lower gradients (representing smoother and more similar parts
     *            of the image) to make an estimate.
     *  PPG:      use Patterned Pixel Grouping interpolation.
     *            Pixel Grouping uses assumptions about natural scenery in making estimates.
     *            It has fewer color artifacts on natural images than the Variable Number of
     *            Gradients method.
     *  AHD:      use Adaptive Homogeneity-Directed interpolation.
     *            This method selects the direction of interpolation so as to
     *            maximize a homogeneity metric, thus typically minimizing color artifacts.
     *  DCB:      DCB interpolation (see www.linuxphoto.org/html/dcb.html for details)
     *  DHT:      DHT interpolation.
     *  AAHD:     Enhanced Adaptive AHD interpolation.
     */
    enum DecodingQuality
    {
        BILINEAR = 0,
        VNG      = 1,
        PPG      = 2,
        AHD      = 3,
        DCB      = 4,
        DHT      = 11,
        AAHD     = 12
    };

    /**
     * White balances alternatives
     *  NONE:     no white balance used : reverts to standard daylight D65 WB.
     *  CAMERA:   Use the camera embedded WB if available. Reverts to NONE if not.
     *  AUTO:     Averages an auto WB on the entire image.
     *  CUSTOM:   Let use set it's own temperature and green factor (later converted to RGBG factors).
     *  AERA:     Let use an area from image to average white balance (see whiteBalanceArea for details).
     */
    enum WhiteBalance
    {
        NONE    = 0,
        CAMERA  = 1,
        AUTO    = 2,
        CUSTOM  = 3,
        AERA    = 4
    };

    /**
     * Noise Reduction method to apply before demosaicing
     *  NONR:       No noise reduction.
     *  WAVELETSNR: wavelets correction to erase noise while preserving real detail. It's applied after interpolation.
     *  FBDDNR:     Fake Before Demosaicing Denoising noise reduction. It's applied before interpolation.
     */
    enum NoiseReduction
    {
        NONR = 0,
        WAVELETSNR,
        FBDDNR
    };

    /**
     * Input color profile used to decoded image
     *  NOINPUTCS:     No input color profile.
     *  EMBEDDED:      Use the camera profile embedded in RAW file if exist.
     *  CUSTOMINPUTCS: Use a custom input color space profile.
     */
    enum InputColorSpace
    {
        NOINPUTCS = 0,
        EMBEDDED,
        CUSTOMINPUTCS
    };

    /**
     * Output RGB color space used to decoded image
     *  RAWCOLOR:       No output color profile (Linear RAW).
     *  SRGB:           Use standard sRGB color space.
     *  ADOBERGB:       Use standard Adobe RGB color space.
     *  WIDEGAMMUT:     Use standard RGB Wide Gamut color space.
     *  PROPHOTO:       Use standard RGB Pro Photo color space.
     *  CUSTOMOUTPUTCS: Use a custom workspace color profile.
     */
    enum OutputColorSpace
    {
        RAWCOLOR = 0,
        SRGB,
        ADOBERGB,
        WIDEGAMMUT,
        PROPHOTO,
        CUSTOMOUTPUTCS
    };

public:

    /**
     * Standard constructor with default settings
     */
    DRawDecoderSettings() = default;

    /**
     * Equivalent to the copy constructor
     */
    DRawDecoderSettings(const DRawDecoderSettings& o);
    DRawDecoderSettings& operator=(const DRawDecoderSettings& o);

    /**
     * Compare for equality
     */
    bool operator==(const DRawDecoderSettings& o) const;

    /**
     * Standard destructor
     */
    ~DRawDecoderSettings() = default;

    /**
     * Method to use a settings to optimize time loading, for example to compute image histogram
     */
    void optimizeTimeLoading();

public:

    /**
     * If true, images with overblown channels are processed much more accurate,
     * without 'pink clouds' (and blue highlights under tungsten lamps).
     */
    bool fixColorsHighlights            = false;

    /**
     * If false, use a fixed white level, ignoring the image histogram.
     */
    bool autoBrightness                 = true;

    /**
     * Turn on RAW file decoding in 16 bits per color per pixel instead 8 bits.
     */
    bool sixteenBitsImage               = false;

    /**
     * Half-size color image decoding (twice as fast as "enableRAWQuality").
     * Turn on this option to reduce time loading to render histogram for example,
     * no to render an image to screen.
     */
    bool halfSizeColorImage             = false;

    /**
     * White balance type to use. See WhiteBalance values for detail
     */
    WhiteBalance whiteBalance           = CAMERA;

    /**
     * The temperature and the green multiplier of the custom white balance
     */
    int    customWhiteBalance           = 6500;
    double customWhiteBalanceGreen      = 1.0;

    /**
     * Turn on RAW file decoding using RGB interpolation as four colors.
     */
    bool RGBInterpolate4Colors          = false;

    /**
     * For cameras with non-square pixels, do not stretch the image to its
     * correct aspect ratio. In any case, this option guarantees that each
     * output pixel corresponds to one RAW pixel.
     */
    bool DontStretchPixels              = false;

    /**
     * Unclip Highlight color level:
     *  0   = Clip all highlights to solid white.
     *  1   = Leave highlights unclipped in various shades of pink.
     *  2   = Blend clipped and unclipped values together for a gradual
     *        fade to white.
     *  3-9 = Reconstruct highlights. Low numbers favor whites; high numbers
     *        favor colors.
     */
    int unclipColors                    = 0;

    /**
     * RAW quality decoding factor value. See DecodingQuality values
     * for details.
     */
    DecodingQuality RAWQuality          = BILINEAR;

    /**
     * After interpolation, clean up color artifacts by repeatedly applying
     * a 3x3 median filter to the R-G and B-G channels.
     */
    int medianFilterPasses              = 0;

    /**
     * Noise reduction method to apply before demosaicing.
     */
    NoiseReduction NRType               = NONR;

    /**
     * Noise reduction threshold value. Null value disable NR. Range is between 100 and 1000.
     * For IMPULSENR : set the amount of Luminance impulse denoise.
     */
    int NRThreshold                     = 0;

    /**
     * Brightness of output image.
     */
    double brightness                   = 1.0;

    /**
     * Turn on the black point setting to decode RAW image.
     */
    bool enableBlackPoint               = false;

    /**
     * Black Point value of output image.
     */
    int blackPoint                      = 0;

    /**
     * Turn on the white point setting to decode RAW image.
     */
    bool enableWhitePoint               = false;

    /**
     * White Point value of output image.
     */
    int whitePoint                      = 0;

    /**
     * The input color profile used to decoded RAW data. See OutputColorProfile
     * values for details.
     */
    InputColorSpace inputColorSpace     = NOINPUTCS;

    /**
     * Path to custom input ICC profile to define the camera's raw colorspace.
     */
    QString inputProfile;

    /**
     * The output color profile used to decoded RAW data. See OutputColorProfile
     * values for details.
     */
    OutputColorSpace outputColorSpace   = SRGB;

    /**
     * Path to custom output ICC profile to define the color workspace.
     */
    QString outputProfile;

    /**
     * Path to text file including dead pixel list.
     */
    QString deadPixelMap;

    /**
     * Rectangle used to calculate the white balance by averaging the region of image.
     */
    QRect whiteBalanceArea;

    //-- Extended demosaicing settings ----------------------------------------------------------

    /// For DCB interpolation.

    /**
     * Number of DCB median filtering correction passes.
     * -1   : disable (default)
     * 1-10 : DCB correction passes
     */
    int dcbIterations                   = -1;

    /**
     * Turn on the DCB interpolation with enhance interpolated colors.
     */
    bool dcbEnhanceFl                   = false;

    /**
     * Turn on the Exposure Correction before interpolation.
     */
    bool expoCorrection                 = false;

    /**
     * Shift of Exposure Correction before interpolation in linear scale.
     * Usable range is from 0.25 (darken image 1 stop : -2EV) to 8.0 (lighten ~1.5 photographic stops : +3EV).
     */
    double expoCorrectionShift          = 1.0;

    /**
     * Amount of highlight preservation for exposure correction before interpolation in E.V.
     * Usable range is from 0.0 (linear exposure shift, highlights may blow) to 1.0 (maximum highlights preservation)
     * This settings can only take effect if expoCorrectionShift > 1.0.
     */
    double expoCorrectionHighlight      = 0.0;
};

//! qDebug() stream operator. Writes settings @a s to the debug output in a nicely formatted way.
QDebug operator<<(QDebug dbg, const DRawDecoderSettings& s);

} // namespace Digikam
