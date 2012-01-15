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
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "player.h"
#include <QtGui/QApplication>
#include <QtDeclarative/QDeclarativeView>
#include <QtDeclarative/QDeclarativeContext>
#include <QtDeclarative/QDeclarativeEngine>
#include <QGst/Ui/GraphicsVideoSurface>
#include <QGst/Init>

#ifndef QMLPLAYER_NO_OPENGL
# include <QtOpenGL/QGLWidget>
#endif

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QGst::init(&argc, &argv);

    QDeclarativeView view;

#ifndef QMLPLAYER_NO_OPENGL
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

    view.engine()->addImportPath(QLatin1String(UNINSTALLED_IMPORTS_DIR));
    view.setSource(QUrl::fromLocalFile(QLatin1String(CMAKE_CURRENT_SOURCE_DIR "/qmlplayer.qml")));
    view.show();

    return app.exec();
}
