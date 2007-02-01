/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*-  vi:set ts=8 sts=4 sw=4: */

/*
    Sonic Visualiser
    An audio file viewer and annotation editor.
    Centre for Digital Music, Queen Mary, University of London.
    This file copyright 2006-2007 Chris Cannam and QMUL.
    
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/

#include "ColourMapper.h"

#include <iostream>

ColourMapper::ColourMapper(int map, float min, float max) :
    QObject(),
    m_map(map),
    m_min(min),
    m_max(max)
{
    if (m_min == m_max) {
        std::cerr << "WARNING: ColourMapper: min == max (== " << m_min
                  << "), adjusting" << std::endl;
        m_max = m_min + 1;
    }
}

ColourMapper::~ColourMapper()
{
}

int
ColourMapper::getColourMapCount()
{
    return 10;
}

QString
ColourMapper::getColourMapName(int n)
{
    if (n >= getColourMapCount()) return tr("<unknown>");
    StandardMap map = (StandardMap)n;

    switch (map) {
    case DefaultColours:   return tr("Default");
    case WhiteOnBlack:     return tr("White on Black");
    case BlackOnWhite:     return tr("Black on White");
    case RedOnBlue:        return tr("Red on Blue");
    case YellowOnBlack:    return tr("Yellow on Black");
    case BlueOnBlack:      return tr("Blue on Black");
    case Sunset:           return tr("Sunset");
    case FruitSalad:       return tr("Fruit Salad");
    case Banded:           return tr("Banded");
    case Highlight:        return tr("Highlight");
    }

    return tr("<unknown>");
}

QColor
ColourMapper::map(float value) const
{
    float norm = (value - m_min) / (m_max - m_min);
    if (norm < 0.f) norm = 0.f;
    if (norm > 1.f) norm = 1.f;
    
    float h = 0.f, s = 0.f, v = 0.f, r = 0.f, g = 0.f, b = 0.f;
    bool hsv = true;

    float red = 0.f, green = 0.3333f, blue = 0.6666f, pieslice = 0.3333f;

    if (m_map >= getColourMapCount()) return Qt::black;
    StandardMap map = (StandardMap)m_map;

    switch (map) {

    case DefaultColours:
        h = blue - norm * 2.f * pieslice;
        s = 0.5f + norm/2.f;
        v = norm;
        break;

    case WhiteOnBlack:
        r = g = b = norm;
        hsv = false;
        break;

    case BlackOnWhite:
        r = g = b = 1.f - norm;
        hsv = false;
        break;

    case RedOnBlue:
        h = blue - pieslice/4.f + norm * (pieslice + pieslice/4.f);
        s = 1.f;
        v = norm;
        break;

    case YellowOnBlack:
        h = 0.15f;
        s = 1.f;
        v = norm;
        break;

    case BlueOnBlack:
        h = blue;
        s = 1.f;
        v = norm * 2.f;
        if (v > 1.f) {
            v = 1.f;
            s = 1.f - (sqrtf(norm) - 0.707f) * 3.413f;
            if (s < 0.f) s = 0.f;
            if (s > 1.f) s = 1.f;
        }
        break;

    case Sunset:
        r = (norm - 0.24f) * 2.38f;
        if (r > 1.f) r = 1.f;
        if (r < 0.f) r = 0.f;
        g = (norm - 0.64f) * 2.777f;
        if (g > 1.f) g = 1.f;
        if (g < 0.f) g = 0.f;
        b = (3.6f * norm);
        if (norm > 0.277f) b = 2.f - b;
        if (b > 1.f) b = 1.f;
        if (b < 0.f) b = 0.f;
        hsv = false;
        break;

    case FruitSalad:
        h = blue + (pieslice/6.f) - norm;
        if (h < 0.f) h += 1.f;
        s = 1.f;
        v = 1.f;
        break;

    case Banded:
        if      (norm < 0.125) return Qt::darkGreen;
        else if (norm < 0.25)  return Qt::green;
        else if (norm < 0.375) return Qt::darkBlue;
        else if (norm < 0.5)   return Qt::blue;
        else if (norm < 0.625) return Qt::darkYellow;
        else if (norm < 0.75)  return Qt::yellow;
        else if (norm < 0.875) return Qt::darkRed;
        else                   return Qt::red;
        break;

    case Highlight:
        if (norm > 0.99) return Qt::white;
        else return Qt::darkBlue;
    }

    if (hsv) {
        return QColor::fromHsvF(h, s, v);
    } else {
        return QColor::fromRgbF(r, g, b);
    }
}

QColor
ColourMapper::getContrastingColour() const
{
    if (m_map >= getColourMapCount()) return Qt::white;
    StandardMap map = (StandardMap)m_map;

    switch (map) {

    case DefaultColours:
        return QColor(255, 150, 50);

    case WhiteOnBlack:
        return Qt::red;

    case BlackOnWhite:
        return Qt::darkGreen;

    case RedOnBlue:
        return Qt::green;

    case YellowOnBlack:
        return QColor::fromHsv(240, 255, 255);

    case BlueOnBlack:
        return Qt::red;

    case Sunset:
        return Qt::white;

    case FruitSalad:
        return Qt::white;

    case Banded:
        return Qt::cyan;

    case Highlight:
        return Qt::red;
    }

    return Qt::white;
}


