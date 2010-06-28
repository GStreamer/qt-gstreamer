/*
    Copyright (C) 2010  George Kiagiadakis <kiagiadakis.george@gmail.com>

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
#include "structs.h"
#include <gst/gstvalue.h>

QGLIB_REGISTER_VALUEIMPL_IMPLEMENTATION(
    QGst::Fourcc,
    QGst::Fourcc(gst_value_get_fourcc(value)),
    gst_value_set_fourcc(value, data.value.as_integer)
)

QGLIB_REGISTER_VALUEIMPL_IMPLEMENTATION(
    QGst::Fraction,
    QGst::Fraction(gst_value_get_fraction_numerator(value), gst_value_get_fraction_denominator(value)),
    gst_value_set_fraction(value, data.numerator, data.denominator)
)

QGLIB_REGISTER_VALUEIMPL_IMPLEMENTATION(
    QGst::IntRange,
    QGst::IntRange(gst_value_get_int_range_min(value), gst_value_get_int_range_max(value)),
    gst_value_set_int_range(value, data.start, data.end)
)

QGLIB_REGISTER_VALUEIMPL_IMPLEMENTATION(
    QGst::DoubleRange,
    QGst::DoubleRange(gst_value_get_double_range_min(value), gst_value_get_double_range_max(value)),
    gst_value_set_double_range(value, data.start, data.end)
)

QGLIB_REGISTER_VALUEIMPL_IMPLEMENTATION(
    QGst::FractionRange,
    QGst::FractionRange(QGlib::Value(*gst_value_get_fraction_range_min(value)).get<QGst::Fraction>(),
                        QGlib::Value(*gst_value_get_fraction_range_max(value)).get<QGst::Fraction>()),
    gst_value_set_fraction_range_full(value, data.start.numerator, data.start.denominator,
                                             data.end.numerator, data.end.denominator)
)
