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
#ifndef _QTGSTREAMER_QGSTVALUE_H
#define _QTGSTREAMER_QGSTVALUE_H

#include "qgstdeclarations.h"
#include <QtCore/QVariant>

namespace QtGstreamer {

class QGstValue
{
public:
    QGstValue();

#define FUNDAMENTAL_TYPE_FUNCTION_DECLARATIONS(type, typeName) \
    QGstValue(type val); \
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

    QGstValue(const char *val);
    QGstValue(const QByteArray & val);
    bool holdsString() const;
    QByteArray getString() const;
    void setString(const char *val);

    QGstValue(const QGstObjectPtr & val);
    QGstValue(const QGstElementPtr & val);
    QGstValue(const QGstPadPtr & val);
    QGstValue(const QGstCapsPtr & val);
    QGstValue(const QGstValue & other);
    virtual ~QGstValue();

    QGstValue & operator=(const QGstValue & other);

    bool isValid() const;

    static QGstValue fromGValue(const GValue *val);
    GValue *toGValue() const;

    static QGstValue fromQVariant(const QVariant & variant, quint32 gtype = 0);
    QVariant toQVariant() const;

    template<class T> static QGstValue fromValue(const T & value, quint32 gtype = 0);
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
    QGstValue listGetValue(uint index) const;
    //void listAppendValue(const QGstValue & value);
    //void listPrependValue(const QGstValue & value);
    //void listConcat(const QGstValue & value1, const QGstValue & value2);

    bool holdsArray() const;
    uint arrayGetSize() const;
    QGstValue arrayGetValue(uint index) const;
    //void arrayAppendValue(const QGstValue & value);
    //void arrayPrependValue(const QGstValue & value);

    bool holdsFraction() const;
    int getFractionNumerator() const;
    int getFractionDenominator() const;
    void setFraction(int numerator, int denominator);
    bool fractionMultiply(const QGstValue & factor1, const QGstValue & factor2);
    bool fractionSubtract(const QGstValue & minuend, const QGstValue & subtrahend);

    bool holdsFractionRange() const;
    QGstValue getFractionRangeMin() const;
    QGstValue getFractionRangeMax() const;
    void setFractionRange(const QGstValue & start, const QGstValue & end);
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

    static bool canCompare(const QGstValue & val1, const QGstValue & val2);
    bool operator<(const QGstValue & val2) const;
    bool operator<=(const QGstValue & val2) const;
    bool operator>(const QGstValue & val2) const;
    bool operator>=(const QGstValue & val2) const;
    bool operator==(const QGstValue & val2) const;

private:
    void reinit(unsigned long gtype);
    GValue *m_value;
    friend QDebug operator<<(QDebug debug, const QGstValue & qgvalue);
};

QDebug operator<<(QDebug debug, const QGstValue & qgvalue);

template<class T>
QGstValue QGstValue::fromValue(const T & value, quint32 gtype)
{
    return QGstValue::fromQVariant(QVariant::fromValue(value), gtype);
}

template<class T>
T QGstValue::value() const
{
    return toQVariant().value<T>();
}

}

#endif
