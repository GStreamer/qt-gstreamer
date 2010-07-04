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
#include "childproxy.h"
#include <gst/gstobject.h>
#include <gst/gstchildproxy.h>

namespace QGst {

uint ChildProxy::childrenCount() const
{
    return gst_child_proxy_get_children_count(object<GstChildProxy>());
}

ObjectPtr ChildProxy::childByName(const char *name) const
{
    return ObjectPtr::wrap(gst_child_proxy_get_child_by_name(object<GstChildProxy>(), name), false);
}

ObjectPtr ChildProxy::childByIndex(uint index) const
{
    return ObjectPtr::wrap(gst_child_proxy_get_child_by_index(object<GstChildProxy>(), index), false);
}

bool ChildProxy::findChildProperty(const char *name, ObjectPtr *obj, QGlib::ParamSpecPtr *paramSpec) const
{
    GstObject *op;
    GParamSpec *pp;
    bool result = gst_child_proxy_lookup(object<GstObject>(), name, &op, &pp);
    if (result) {
        *obj = ObjectPtr::wrap(op, false);
        *paramSpec = QGlib::ParamSpecPtr::wrap(pp, false);
    }
    return result;
}

QGlib::Value ChildProxy::childProperty(const char *name) const
{
    QGlib::ParamSpecPtr param;
    ObjectPtr object;
    if (findChildProperty(name, &object, &param)) {
        return object->property(param->name().toUtf8());
    } else {
        return QGlib::Value();
    }
}

}