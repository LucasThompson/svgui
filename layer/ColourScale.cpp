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

#include "ColourScale.h"

#include "base/AudioLevel.h"
#include "base/LogRange.h"

#include <cmath>

int ColourScale::m_maxPixel = 255;

ColourScale::ColourScale(int colourMap,
			 Scale scale,
			 double minValue,
			 double maxValue,
			 double threshold,
			 double gain) :
    m_mapper(colourMap, 1.f, double(m_maxPixel)),
    m_scale(scale),
    m_min(minValue),
    m_max(maxValue),
    m_threshold(threshold),
    m_gain(gain)
{
    if (minValue >= maxValue) {
	throw std::logic_error("maxValue must be greater than minValue");
    }

    m_mappedMin = m_min;
    m_mappedMax = m_max;

    if (m_scale == LogColourScale) {

	LogRange::mapRange(m_mappedMin, m_mappedMax);
	
    } else if (m_scale == PlusMinusOneScale) {
	
	m_mappedMin = -1.0;
	m_mappedMax =  1.0;

    } else if (m_scale == AbsoluteScale) {

	m_mappedMin = fabs(m_mappedMin);
	m_mappedMax = fabs(m_mappedMax);
	if (m_mappedMin >= m_mappedMax) {
	    std::swap(m_mappedMin, m_mappedMax);
	}
    }

    if (m_mappedMin >= m_mappedMax) {
	throw std::logic_error("maxValue must be greater than minValue [after mapping]");
    }
}

int
ColourScale::getPixel(double value)
{
    double maxPixF = m_maxPixel;

    if (m_scale == PhaseColourScale) {
	double half = (maxPixF - 1.f) / 2.f;
	return 1 + int((value * half) / M_PI + half);
    }
    
    value *= m_gain;

    if (value < m_threshold) return 0;

    double mapped = value;

    if (m_scale == LogColourScale) {
	mapped = LogRange::map(value);
    } else if (m_scale == PlusMinusOneScale) {
	if (mapped < -1.f) mapped = -1.f;
	if (mapped > 1.f) mapped = 1.f;
    } else if (m_scale == AbsoluteScale) {
	if (mapped < 0.f) mapped = -mapped;
    }
	
    if (mapped < m_mappedMin) {
	mapped = m_mappedMin;
    }
    if (mapped > m_mappedMax) {
	mapped = m_mappedMax;
    }

    double proportion = (mapped - m_mappedMin) / (m_mappedMax - m_mappedMin);

    int pixel = 0;

    if (m_scale == MeterColourScale) {
	pixel = AudioLevel::multiplier_to_preview(proportion, m_maxPixel-1) + 1;
    } else {
	pixel = int(proportion * maxPixF) + 1;
    }

    if (pixel > m_maxPixel) pixel = m_maxPixel;
    if (pixel < 0) pixel = 0;
    return pixel;
}

QColor
ColourScale::getColourForPixel(int pixel, int rotation)
{
    if (pixel < 0) {
	pixel = 0;
    }
    if (pixel > m_maxPixel) {
	pixel = m_maxPixel;
    }
    if (pixel == 0) {
	if (m_mapper.hasLightBackground()) {
	    return Qt::white;
	} else {
	    return Qt::black;
	}
    } else {
	int target = int(pixel) + rotation;
	while (target < 1) target += m_maxPixel;
	while (target > m_maxPixel) target -= m_maxPixel;
	return m_mapper.map(double(target));
    }
}
