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
#include "player.h"
#include <cstdlib>
#include <QApplication>
#include <QDeclarativeView>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <QGst/Ui/GraphicsVideoSurface>
#include <QGst/Init>

#ifndef QMLPLAYER_NO_OPENGL
# include <QGLWidget>
#endif

int main(int argc, char **argv)
{
#if defined(QTVIDEOSINK_PATH)
    //this allows the example to run from the QtGStreamer build tree without installing QtGStreamer
    qputenv("GST_PLUGIN_PATH", QTVIDEOSINK_PATH);
#endif

    QApplication app(argc, argv);
    QGst::init(&argc, &argv);

    QDeclarativeView view;

#if !defined(QMLPLAYER_NO_OPENGL)
    /*
     * Setting a QGLWidget as the viewport is highly recommended as
     * it enables hardware scaling & color conversion on the video sink
     */
    view.setViewport(new QGLWidget);
#endif

    QGst::Ui::GraphicsVideoSurface *surface = new QGst::Ui::GraphicsVideoSurface(&view);
    view.rootContext()->setContextProperty(QLatin1String("videoSurface1"), surface);

    Player *player = new Player(&view);
    player->setVideoSink(surface->videoSink());
    view.rootContext()->setContextProperty(QLatin1String("player"), player);

#if defined(UNINSTALLED_IMPORTS_DIR)
    //this allows the example to run from the QtGStreamer build tree without installing QtGStreamer
    view.engine()->addImportPath(QLatin1String(UNINSTALLED_IMPORTS_DIR));
#endif

    view.setSource(QUrl(QLatin1String("qrc:///qmlplayer.qml")));
    view.show();

    return app.exec();
}
