/*
    Copyright (C) 2009  George Kiagiadakis <kiagiadakis.george@gmail.com>

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef _QTGSTREAMER_QGVALUE_H
#define _QTGSTREAMER_QGVALUE_H

#include "qgstdeclarations.h"
#include <QtCore/QVariant>

namespace QtGstreamer {

class QGValue
{
public:
    QGValue();

#define FUNDAMENTAL_TYPE_FUNCTION_DECLARATIONS(type, typeName) \
    QGValue(type val); \
    bool holds ##typeName () const; \
    type get ##typeName () const; \
    void set ##typeName (type val);

    FUNDAMENTAL_TYPE_FUNCTION_DECLARATIONS(char, Char)
    FUNDAMENTAL_TYPE_FUNCTION_DECLARATIONS(uchar, UChar)
    FUNDAMENTAL_TYPE_FUNCTION_DECLARATIONS(int, Int)
    FUNDAMENTAL_TYPE_FUNCTION_DECLARATIONS(uint, UInt)
    FUNDAMENTAL_TYPE_FUNCTION_DECLARATIONS(qint64, Int64)
    FUNDAMENTAL_TYPE_FUNCTION_DECLARATIONS(quint64, UInt64)
    FUNDAMENTAL_TYPE_FUNCTION_DECLARATIONS(bool, Boolean)
    FUNDAMENTAL_TYPE_FUNCTION_DECLARATIONS(double, Double)

#undef FUNDAMENTAL_TYPE_FUNCTION_DECLARATIONS

    QGValue(const char *val);
    QGValue(const QByteArray & val);
    bool holdsString() const;
    QByteArray getString() const;
    void setString(const char *val);

    QGValue(const QGstObjectPtr & val);
    QGValue(const QGstElementPtr & val);
    QGValue(const QGstPadPtr & val);
    QGValue(const QGstCapsPtr & val);
    QGValue(const QGValue & other);
    virtual ~QGValue();

    QGValue & operator=(const QGValue & other);

    bool isValid() const;

    static QGValue fromGValue(const GValue *val);
    GValue *toGValue() const;

    static QGValue fromQVariant(const QVariant & variant, quint32 gtype = 0);
    QVariant toQVariant() const;

    template<class T> static QGValue fromValue(const T & value, quint32 gtype = 0);
    template<class T> T value() const;

    bool holdsFourcc() const;
    quint32 getFourcc() const;
    void setFourcc(quint32 fourcc);
    void setFourcc(char a, char b, char c, char d);
    void setFourcc(const char fourccStr[4]);

    bool holdsIntRange() const;
    int getIntRangeMin() const;
    int getIntRangeMax() const;
    void setIntRange(int start, int end);

    bool holdsDoubleRange() const;
    double getDoubleRangeMin() const;
    double getDoubleRangeMax() const;
    void setDoubleRange(double start, double end);

    bool holdsList() const;
    uint listGetSize() const;
    QGValue listGetValue(uint index) const;
    //void listAppendValue(const QGValue & value);
    //void listPrependValue(const QGValue & value);
    //void listConcat(const QGValue & value1, const QGValue & value2);

    bool holdsArray() const;
    uint arrayGetSize() const;
    QGValue arrayGetValue(uint index) const;
    //void arrayAppendValue(const QGValue & value);
    //void arrayPrependValue(const QGValue & value);

    bool holdsFraction() const;
    int getFractionNumerator() const;
    int getFractionDenominator() const;
    void setFraction(int numerator, int denominator);
    bool fractionMultiply(const QGValue & factor1, const QGValue & factor2);
    bool fractionSubtract(const QGValue & minuend, const QGValue & subtrahend);

    bool holdsFractionRange() const;
    QGValue getFractionRangeMin() const;
    QGValue getFractionRangeMax() const;
    void setFractionRange(const QGValue & start, const QGValue & end);
    void setFractionRange(int numerator_start, int denominator_start,
                          int numerator_end, int denominator_end);

    bool holdsCaps() const;
    QGstCapsPtr getCaps() const;
    void setCaps(const QGstCapsPtr & val);

    bool holdsStructure() const;
    //QGstStructure getStructure() const;
    //void setStructure(const QGstStructure & val);

    bool isFixed() const;
    QByteArray serialize() const;
    bool deserialize(const char *src);

    static bool canCompare(const QGValue & val1, const QGValue & val2);
    bool operator<(const QGValue & val2) const;
    bool operator<=(const QGValue & val2) const;
    bool operator>(const QGValue & val2) const;
    bool operator>=(const QGValue & val2) const;
    bool operator==(const QGValue & val2) const;

private:
    void reinit(unsigned long gtype);
    GValue *m_value;
    friend QDebug operator<<(QDebug debug, const QGValue & qgvalue);
};

QDebug operator<<(QDebug debug, const QGValue & qgvalue);

template<class T>
QGValue QGValue::fromValue(const T & value, quint32 gtype)
{
    return QGValue::fromQVariant(QVariant::fromValue(value), gtype);
}

template<class T>
T QGValue::value() const
{
    return toQVariant().value<T>();
}

}

#endif
