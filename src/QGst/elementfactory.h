/*
    Copyright (C) 2009-2010  George Kiagiadakis <kiagiadakis.george@gmail.com>

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
#ifndef QGST_ELEMENTFACTORY_H
#define QGST_ELEMENTFACTORY_H

#include "pluginfeature.h"
#include "element.h"

namespace QGst {

/*! \headerfile elementfactory.h <QGst/ElementFactory>
 * \brief Wrapper class for GstElementFactory
 */
class QTGSTREAMER_EXPORT ElementFactory : public PluginFeature
{
    QGST_WRAPPER(ElementFactory)
public:
    static ElementFactoryPtr find(const char *factoryName);
    static inline ElementFactoryPtr find(const QString & factoryName);

    static ElementPtr make(const char *factoryName, const char *elementName = NULL);
    static inline ElementPtr make(const QString & factoryName, const char *elementName = NULL);

    QGlib::Type elementType() const;
    QString longName() const;
    QString klass() const;
    QString description() const;
    QString author() const;
    QString documentationUri() const;
    QString iconName() const;

    uint padTemplatesCount() const;
    int uriType() const;
    bool hasInterface(const char *interfaceName) const;
    bool canSinkCaps(const CapsPtr & caps) const;
    bool canSrcCaps(const CapsPtr & caps) const;

    ElementPtr create(const char *elementName = NULL) const;
};

//static
inline ElementFactoryPtr ElementFactory::find(const QString & factoryName)
{
    return find(factoryName.toUtf8().constData());
}

//static
inline ElementPtr ElementFactory::make(const QString & factoryName, const char *elementName)
{
    return make(factoryName.toUtf8().constData(), elementName);
}

} //namespace QGst

QGST_REGISTER_TYPE(QGst::ElementFactory)

#endif
