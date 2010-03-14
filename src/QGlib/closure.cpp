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
#include "closure.h"
#include <glib-object.h>

namespace QGlib {

//BEGIN Closure

void Closure::ref()
{
    g_closure_ref(static_cast<GClosure*>(m_object));
}

void Closure::unref()
{
    g_closure_unref(static_cast<GClosure*>(m_object));
}

void Closure::setMarshaller(void (*marshaller)(GClosure*, GValue*, uint, const GValue*, void*, void*))
{
    g_closure_set_marshal(static_cast<GClosure*>(m_object), marshaller);
}

//END Closure

//BEGIN CppClosure

//static
ClosurePtr CppClosure::newCppClosure(ClosureDataBase *closureData)
{
    GClosure *closure = g_closure_new_simple(sizeof(GClosure), closureData);
    g_closure_add_finalize_notifier(closure, NULL, &closureDestroyNotify);
    ClosurePtr result = ClosurePtr::wrap(closure);
    g_closure_sink(closure);
    return result;
}

//static
void CppClosure::closureDestroyNotify(void *data, GClosure *closure)
{
    Q_UNUSED(data);
    delete static_cast<CppClosure::ClosureDataBase*>(closure->data);
}

//static
CppClosure::ClosureDataBase* CppClosure::getClosureData(GClosure* closure)
{
    return static_cast<CppClosure::ClosureDataBase*>(closure->data);
}

QList<Value> CppClosure::convertParamValues(const GValue* values, uint number)
{
    QList<Value> result;
    for(uint i=0; i<number; ++i) {
        result.append(Value(values[i]));
    }
    return result;
}

//END CppClosure

}
