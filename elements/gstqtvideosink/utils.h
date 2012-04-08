/*
    Copyright (C) 2011-2012 Collabora Ltd. <info@collabora.com>

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 2.1
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef UTILS_H
#define UTILS_H

#include <QtCore/QRectF>
#include <QtCore/QSize>
#include <QtCore/QMetaType>

struct Fraction
{
    inline Fraction() {}
    inline Fraction(int numerator, int denominator)
        : numerator(numerator), denominator(denominator) {}

    inline bool operator==(const Fraction & other) const
    { return numerator == other.numerator && denominator == other.denominator; }
    inline bool operator!=(const Fraction & other) const
    { return !operator==(other); }

    inline qreal ratio() const
    { return (qreal) numerator / (qreal) denominator; }
    inline qreal invRatio() const
    { return (qreal) denominator / (qreal) numerator; }

    int numerator;
    int denominator;
};

struct PaintAreas
{
    void calculate(const QRectF & targetArea, const QSize & frameSize,
                   const Fraction & pixelAspectRatio, const Fraction & displayAspectRatio);

    // the area that we paint on
    QRectF targetArea;

    // the area where the video should be painted on (subrect of or equal to targetArea)
    QRectF videoArea;

    // these are small subrects of targetArea that are not
    // covered by videoArea to keep the video's aspect ratio
    QRectF blackArea1;
    QRectF blackArea2;
};

Q_DECLARE_METATYPE(Fraction)
Q_DECLARE_METATYPE(PaintAreas)

#endif
