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
#include "object.h"
#include <glib-object.h>

namespace QGlib {
namespace Private {

template <class T>
QList< RefPointer<T> > arrayToList(typename T::CType **array, uint n)
{
    QList< RefPointer<T> > result;
    for(uint i = 0; i<n; ++i) {
        result.append(RefPointer<T>::wrap(array[i]));
    }
    return result;
}

} //namespace Private


ParamSpecPtr Object::findProperty(const char *name) const
{
    GObjectClass *klass = G_OBJECT_CLASS(g_type_class_ref(Type::fromInstance(object<void>())));
    GParamSpec *param = g_object_class_find_property(klass, name);
    g_type_class_unref(klass);
    if (param) {
        return ParamSpecPtr::wrap(g_param_spec_ref_sink(param), false);
    } else {
        return ParamSpecPtr();
    }
}

QList<ParamSpecPtr> Object::listProperties() const
{
    GObjectClass *klass = G_OBJECT_CLASS(g_type_class_ref(Type::fromInstance(object<void>())));
    uint n;
    GParamSpec **param = g_object_class_list_properties(klass, &n);
    g_type_class_unref(klass);
    QList<ParamSpecPtr> result = QGlib::Private::arrayToList<ParamSpec>(param, n);
    g_free(param);
    return result;
}

Value Object::property(const char *name) const
{
    Value result;
    ParamSpecPtr param = findProperty(name);
    if (param && (param->flags() & ParamSpec::Readable)) {
        result.init(param->valueType());
        g_object_get_property(object<GObject>(), name, result);
    }
    return result;
}

void Object::setPropertyValue(const char *name, const Value & value)
{
    g_object_set_property(object<GObject>(), name, value);
}

void Object::ref(bool increaseRef)
{
    if (increaseRef) {
        g_object_ref(m_object);
    }
}

void Object::unref()
{
    g_object_unref(m_object);
}

}
