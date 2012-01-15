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
#ifndef QGST_UI_GRAPHICSVIDEOSURFACE_H
#define QGST_UI_GRAPHICSVIDEOSURFACE_H

#include "global.h"
#include "../element.h"
#include <QtGui/QGraphicsView>

namespace QGst {
namespace Ui {

class GraphicsVideoWidget;
class GraphicsVideoSurfacePrivate;

/*! \headerfile graphicsvideosurface.h <QGst/Ui/GraphicsVideoSurface>
 * \brief Helper class for painting video on a QGraphicsView
 *
 * \code
 * QGraphicsView *view = new QGraphicsView;
 * view->setViewport(new QGLWidget); //recommended
 * QGst::Ui::GraphicsVideoSurface *surface = new QGst::Ui::GraphicsVideoSurface(view);
 * ...
 * QGst::Ui::GraphicsVideoWidget *widget = new QGst::Ui::GraphicsVideoWidget;
 * widget->setSurface(surface);
 * view->addItem(widget);
 * \endcode
 */
class QTGSTREAMERUI_EXPORT GraphicsVideoSurface : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(GraphicsVideoSurface)
public:
    explicit GraphicsVideoSurface(QGraphicsView *parent);
    virtual ~GraphicsVideoSurface();

    /*! Returns the video sink element that provides this surface's image.
     * The element will be constructed the first time that this function
     * is called. The surface will always keep a reference to this element.
     */
    ElementPtr videoSink() const;

private:
    QTGSTREAMERUI_NO_EXPORT void onUpdate();

private:
    friend class GraphicsVideoWidget;
    GraphicsVideoSurfacePrivate * const d;
};

} // namespace Ui
} // namespace QGst

Q_DECLARE_METATYPE(QGst::Ui::GraphicsVideoSurface*)

#endif // QGST_UI_GRAPHICSVIDEOSURFACE_H
