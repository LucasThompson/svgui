/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*-  vi:set ts=8 sts=4 sw=4: */

/*
    Sonic Visualiser
    An audio file viewer and annotation editor.
    Centre for Digital Music, Queen Mary, University of London.
    This file copyright 2006 Chris Cannam.
    
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/

#include "Panner.h"

#include <QMouseEvent>
#include <QPaintEvent>
#include <QWheelEvent>
#include <QPainter>

#include <iostream>

Panner::Panner(QWidget *parent) :
    QWidget(parent),
    m_rectX(0),
    m_rectY(0),
    m_rectWidth(1),
    m_rectHeight(1),
    m_defaultCentreX(0),
    m_defaultCentreY(0),
    m_defaultsSet(false),
    m_clicked(false)
{
}

Panner::~Panner()
{
}

void
Panner::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        m_clicked = true;
        m_clickPos = e->pos();
        m_dragStartX = m_rectX;
        m_dragStartY = m_rectY;
    } else if (e->button() == Qt::MidButton) {
        resetToDefault();
    }
}

void
Panner::mouseDoubleClickEvent(QMouseEvent *e)
{
    resetToDefault();
}

void
Panner::mouseMoveEvent(QMouseEvent *e)
{
    if (!m_clicked) return;

    float dx = float(e->pos().x() - m_clickPos.x()) / float(width());
    float dy = float(e->pos().y() - m_clickPos.y()) / float(height());
    
    m_rectX = m_dragStartX + dx;
    m_rectY = m_dragStartY + dy;
    
    normalise();
    emitAndUpdate();
}

void
Panner::mouseReleaseEvent(QMouseEvent *e)
{
    if (!m_clicked) return;

    mouseMoveEvent(e);
    m_clicked = false;
}

void
Panner::wheelEvent(QWheelEvent *e)
{
    if (e->delta() > 0) {
        m_rectY += 0.1;
    } else {
        m_rectY -= 0.1;
    }

    normalise();
    emitAndUpdate();
}

void
Panner::paintEvent(QPaintEvent *e)
{
    QPainter paint(this);
    paint.setRenderHint(QPainter::Antialiasing, false);

    QColor bg(palette().background().color());
    bg.setAlpha(80);

    paint.setPen(palette().dark().color());
    paint.setBrush(bg);
    paint.drawRect(0, 0, width(), height());

    QColor hl(palette().highlight().color());
    hl.setAlpha(80);

    paint.setBrush(hl);

    paint.drawRect(lrintf(width() * m_rectX),
                   lrintf(height() * m_rectY),
                   lrintf(width() * m_rectWidth),
                   lrintf(height() * m_rectHeight));
}

void
Panner::normalise()
{
    if (m_rectWidth > 1.0) m_rectWidth = 1.0;
    if (m_rectHeight > 1.0) m_rectHeight = 1.0;
    if (m_rectX + m_rectWidth > 1.0) m_rectX = 1.0 - m_rectWidth;
    if (m_rectX < 0) m_rectX = 0;
    if (m_rectY + m_rectHeight > 1.0) m_rectY = 1.0 - m_rectHeight;
    if (m_rectY < 0) m_rectY = 0;

    if (!m_defaultsSet) {
        m_defaultCentreX = centreX();
        m_defaultCentreY = centreY();
        m_defaultsSet = true;
    }
}

void
Panner::emitAndUpdate()
{
    emit rectExtentsChanged(m_rectX, m_rectY, m_rectWidth, m_rectHeight);
    emit rectCentreMoved(centreX(), centreY());
    update();
}  

void
Panner::setRectExtents(float x0, float y0, float width, float height)
{
    if (m_rectX == x0 &&
        m_rectY == y0 &&
        m_rectWidth == width &&
        m_rectHeight == height) {
        return;
    }

    m_rectX = x0;
    m_rectY = y0;
    m_rectWidth = width;
    m_rectHeight = height;

    normalise();
    emitAndUpdate();
}

void
Panner::setRectWidth(float width)
{
    if (m_rectWidth == width) return;
    m_rectWidth = width;
    normalise();
    emitAndUpdate();
}

void
Panner::setRectHeight(float height)
{
    if (m_rectHeight == height) return;
    m_rectHeight = height;
    normalise();
    emitAndUpdate();
}

void
Panner::setRectCentreX(float x)
{
    float x0 = x - m_rectWidth/2;
    if (x0 == m_rectX) return;
    m_rectX = x0;
    normalise();
    emitAndUpdate();
}

void
Panner::setRectCentreY(float y)
{
    float y0 = y - m_rectHeight/2;
    if (y0 == m_rectY) return;
    m_rectY = y0;
    normalise();
    emitAndUpdate();
}

QSize
Panner::sizeHint() const
{
    return QSize(30, 30);
}

void
Panner::setDefaultRectCentre(float cx, float cy)
{
    m_defaultCentreX = cx;
    m_defaultCentreY = cy;
    m_defaultsSet = true;
}

void
Panner::resetToDefault()
{
    float x0 = m_defaultCentreX - m_rectWidth/2;
    float y0 = m_defaultCentreY - m_rectHeight/2;
    if (x0 == m_rectX && y0 == m_rectY) return;
    m_rectX = x0;
    m_rectY = y0;
    normalise();
    emitAndUpdate();
}


