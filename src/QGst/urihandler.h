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
#ifndef QGST_URIHANDLER_H
#define QGST_URIHANDLER_H

#include "element.h"
class QUrl;

namespace QGst {

class UriHandler : public virtual Element
{
    QGST_WRAPPER_DIFFERENT_C_CLASS(UriHandler, URIHandler)
public:
    static bool protocolIsSupported(UriType type, const char *protocol);
    static UriHandlerPtr makeFromUri(UriType type, const QUrl & uri, const char *elementName = NULL);

    UriType uriType() const;
    QStringList supportedProtocols() const;
    QUrl uri() const;
    bool setUri(const QUrl & uri);
};

} //namespace QGst

QGLIB_REGISTER_TYPE(QGst::UriHandler)

#endif // QGST_URIHANDLER_H
