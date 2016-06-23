/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*-  vi:set ts=8 sts=4 sw=4: */

/*
    Sonic Visualiser
    An audio file viewer and annotation editor.
    Centre for Digital Music, Queen Mary, University of London.
    This file copyright 2006-2016 Chris Cannam and QMUL.
    
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/

#ifndef COLOUR_SCALE_H
#define COLOUR_SCALE_H

#include "ColourMapper.h"

/**
 * Map values within a range onto a set of colours, with a given
 * distribution (linear, log etc) and optional colourmap rotation.
 */
class ColourScale
{
public:
    enum Scale {
	LinearColourScale,
	MeterColourScale,
	LogColourScale,
	PhaseColourScale,
        PlusMinusOneScale,
        AbsoluteScale
    };

    struct Parameters {
	Parameters() : colourMap(0), scale(LinearColourScale),
		       minValue(0.0), maxValue(1.0),
		       threshold(0.0), gain(1.0) { }

	/** A colour map index as used by ColourMapper */
	int colourMap;
	
	/** Distribution for the scale */
	Scale scale;
	
	/** Minimum value in source range */
	double minValue;
	
	/** Maximum value in source range. Must be > minValue */
	double maxValue;

	/** Threshold below which every value is mapped to background
	    pixel 0 */
	double threshold;

	/** Gain to apply before clamping and mapping */
	double gain;
    };
    
    /**
     * Create a ColourScale with the given parameters.
     *
     * Note that some parameters may be ignored for some scale
     * distribution settings. For example, min and max are ignored for
     * PlusMinusOneScale and PhaseColourScale and threshold and gain
     * are ignored for PhaseColourScale.
     */
    ColourScale(Parameters parameters);

    /**
     * Return a pixel number (in the range 0-255 inclusive)
     * corresponding to the given value.  The pixel 0 is used only for
     * values below the threshold supplied in the constructor. All
     * other values are mapped onto the range 1-255.
     */
    int getPixel(double value);

    /**
     * Return the colour for the given pixel number (which must be in
     * the range 0-255). The pixel 0 is always the background
     * colour. Other pixels are mapped taking into account the given
     * colourmap rotation (which is also a value in the range 0-255).
     */
    QColor getColourForPixel(int pixel, int rotation);
    
    /**
     * Return the colour corresponding to the given value. This is
     * equivalent to getColourForPixel(getPixel(value), rotation).
     */
    QColor getColour(double value, int rotation) {
	return getColourForPixel(getPixel(value), rotation);
    }

private:
    Parameters m_params;
    ColourMapper m_mapper;
    double m_mappedMin;
    double m_mappedMax;
    static int m_maxPixel;
};

#endif
