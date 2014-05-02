/*
    Copyright (C) 2012-2013 Collabora Ltd. <info@collabora.com>
      @author George Kiagiadakis <george.kiagiadakis@collabora.com>
    Copyright (C) 2013 basysKom GmbH <info@basyskom.com>
      @author Benjamin Federau <benjamin.federau@basyskom.com>

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
#include <QtGui/QGuiApplication>
#include <QtQuick/QQuickView>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>
#include <QGst/Init>
#include <QGst/Quick/VideoSurface>

int main(int argc, char **argv)
{
#if defined(QTVIDEOSINK_PATH)
    //this allows the example to run from the QtGStreamer build tree without installing QtGStreamer
    qputenv("GST_PLUGIN_PATH", QTVIDEOSINK_PATH);
#endif

    QGuiApplication app(argc, argv);
    QGst::init(&argc, &argv);

    QQuickView view;

    QGst::Quick::VideoSurface *surface = new QGst::Quick::VideoSurface;
    view.rootContext()->setContextProperty(QLatin1String("videoSurface1"), surface);

    Player *player = new Player(&view);
    player->setVideoSink(surface->videoSink());
    if (argc > 1)
        player->setUri(QString::fromLocal8Bit(argv[1]));
    else
        player->setUri(QLatin1Literal("http://download.blender.org/peach/bigbuckbunny_movies/big_buck_bunny_480p_surround-fix.avi"));
    view.rootContext()->setContextProperty(QLatin1String("player"), player);

#if defined(UNINSTALLED_IMPORTS_DIR)
    //this allows the example to run from the QtGStreamer build tree without installing QtGStreamer
    view.engine()->addImportPath(QLatin1String(UNINSTALLED_IMPORTS_DIR));
#endif

    view.setSource(QUrl(QLatin1String("qrc:///qmlplayer2.qml")));
    view.show();

    return app.exec();
}
