/*
    Copyright (C) 2010 George Kiagiadakis <kiagiadakis.george@gmail.com>

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
#ifndef QGST_PROPERTYPROBE_H
#define QGST_PROPERTYPROBE_H

#include "global.h"
#include "../QGlib/object.h"

namespace QGst {

/*! \interface PropertyProbe propertyprobe.h <QGst/PropertyProbe>
 * \brief Wrapper class for GstPropertyProbe
 */
class QTGSTREAMER_EXPORT PropertyProbe : public QGlib::Interface
{
    QGST_WRAPPER(PropertyProbe)
public:
    QList<QGlib::ParamSpecPtr> properties() const;

    bool propertySupportsProbe(const QGlib::ParamSpecPtr & property) const;
    bool propertySupportsProbe(const char *property) const;

    bool needsProbe(const QGlib::ParamSpecPtr & property) const;
    bool needsProbe(const char *property) const;

    void probe(const QGlib::ParamSpecPtr & property);
    void probe(const char *property);

    QList<QGlib::Value> values(const QGlib::ParamSpecPtr & property) const;
    QList<QGlib::Value> values(const char *property) const;

    QList<QGlib::Value> probeAndGetValues(const QGlib::ParamSpecPtr & property);
    QList<QGlib::Value> probeAndGetValues(const char *property);
};

}

QGST_REGISTER_TYPE(QGst::PropertyProbe)
QGLIB_REGISTER_INTERFACE(QGst::PropertyProbe)

#endif // QGST_PROPERTYPROBE_H
