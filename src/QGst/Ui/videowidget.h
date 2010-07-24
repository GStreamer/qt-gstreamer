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
#ifndef QGST_UI_VIDEOWIDGET_H
#define QGST_UI_VIDEOWIDGET_H

#include "../element.h"
#include <QtGui/QWidget>

namespace QGst {
namespace Ui {

class AbstractRenderer;

/*! \headerfile videowidget.h <QGst/Ui/VideoWidget>
 * \brief A generic video widget that can embed a video sink
 *
 * This widget allows you to embed a video sink on a Qt user interface.
 * It supports a wide range of video sinks, including all the sinks that
 * implement the XOverlay interface, the "qwidgetvideosink" and autoplug
 * video sinks, such as "autovideosink" and "gconfvideosink". Because the
 * autoplug sinks are actualy bins, this widget can also accept any kind
 * of bin (to be precise, anything that implements the ChildProxy interface).
 * However, it is not recommended to use it with bins, unless you are
 * completely sure that there can be no child bins and there can be only
 * one video sink element in this bin.
 */
class VideoWidget : public QWidget
{
    Q_OBJECT
public:
    VideoWidget(QWidget *parent = 0, Qt::WindowFlags f = 0);
    virtual ~VideoWidget();

    /*! Returns the video sink element that was set using setVideoSink(),
     * or a null ElementPtr if no sink has been set. */
    ElementPtr videoSink() const;

    /*! Sets the video sink element that is going to be embedded. You can safely
     * pass a null ElementPtr here to remove the previously embedded sink. */
    void setVideoSink(const ElementPtr & sink);

private:
    AbstractRenderer *d;
};

} //namespace Ui
} //namespace QGst

#endif // QGST_UI_VIDEOWIDGET_H
