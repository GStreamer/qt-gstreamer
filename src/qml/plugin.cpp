/*
    Copyright (C) 2012 Collabora Ltd. <info@collabora.com>
      @author George Kiagiadakis <george.kiagiadakis@collabora.com>

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
#include "videoitem.h"
#include <QtDeclarative/QDeclarativeExtensionPlugin>

class QtGStreamerPlugin : public QDeclarativeExtensionPlugin
{
    Q_OBJECT
public:
    void registerTypes(const char *uri);
};

void QtGStreamerPlugin::registerTypes(const char *uri)
{
    qmlRegisterType<VideoItem>(uri, 0, 10, "VideoItem");
    qmlRegisterUncreatableType<QGst::Ui::GraphicsVideoSurface>(uri, 0, 10, "GraphicsVideoSurface",
        QLatin1String("Creating a QGst::Ui::GraphicsVideoSurface from QML is not supported"));
}

Q_EXPORT_PLUGIN2(qtgstPlugin, QtGStreamerPlugin)

#include "plugin.moc"
