/*
    Copyright (C) 2010 George Kiagiadakis <kiagiadakis.george@gmail.com>

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "propertyprobe.h"
#include <gst/interfaces/propertyprobe.h>

namespace QGst {

QList<QGlib::ParamSpecPtr> PropertyProbe::properties() const
{
    QList<QGlib::ParamSpecPtr> result;
    const GList *list = gst_property_probe_get_properties(object<GstPropertyProbe>());
    while(list) {
        result.append(QGlib::ParamSpecPtr::wrap(G_PARAM_SPEC(list->data)));
        list = list->next;
    }
    return result;
}

bool PropertyProbe::propertySupportsProbe(const QGlib::ParamSpecPtr & property) const
{
    const GList *list = gst_property_probe_get_properties(object<GstPropertyProbe>());
    while(list) {
        GParamSpec *param = G_PARAM_SPEC(list->data);
        if (param == property) {
            return true;
        }
        list = list->next;
    }
    return false;
}

bool PropertyProbe::propertySupportsProbe(const char *property) const
{
    const GParamSpec *param = gst_property_probe_get_property(object<GstPropertyProbe>(), property);
    return param != NULL;
}

bool PropertyProbe::needsProbe(const QGlib::ParamSpecPtr & property) const
{
    return gst_property_probe_needs_probe(object<GstPropertyProbe>(), property);
}

bool PropertyProbe::needsProbe(const char *property) const
{
    return gst_property_probe_needs_probe_name(object<GstPropertyProbe>(), property);
}

void PropertyProbe::probe(const QGlib::ParamSpecPtr & property)
{
    gst_property_probe_probe_property(object<GstPropertyProbe>(), property);
}

void PropertyProbe::probe(const char *property)
{
    gst_property_probe_probe_property_name(object<GstPropertyProbe>(), property);
}

static QList<QGlib::Value> valueArrayToList(GValueArray *array)
{
    QList<QGlib::Value> result;
    if (array) {
        for(uint i = 0; i < array->n_values; ++i) {
            const GValue *v = g_value_array_get_nth(array, i);
            result.append(QGlib::Value(v));
        }
        g_value_array_free(array);
    }
    return result;
}

QList<QGlib::Value> PropertyProbe::values(const QGlib::ParamSpecPtr & property) const
{
    GValueArray *array = gst_property_probe_get_values(object<GstPropertyProbe>(), property);
    return valueArrayToList(array);
}

QList<QGlib::Value> PropertyProbe::values(const char *property) const
{
    GValueArray *array = gst_property_probe_get_values_name(object<GstPropertyProbe>(), property);
    return valueArrayToList(array);
}

QList<QGlib::Value> PropertyProbe::probeAndGetValues(const QGlib::ParamSpecPtr & property)
{
    GValueArray *array = gst_property_probe_probe_and_get_values(object<GstPropertyProbe>(), property);
    return valueArrayToList(array);
}

QList<QGlib::Value> PropertyProbe::probeAndGetValues(const char *property)
{
    GValueArray *array = gst_property_probe_probe_and_get_values_name(object<GstPropertyProbe>(), property);
    return valueArrayToList(array);
}

}
