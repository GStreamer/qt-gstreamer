/*
    Copyright (C) 2011-2012 Collabora Ltd. <info@collabora.com>

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 2.1
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "utils.h"

/*
 * Modified version of gst_video_sink_center_rect
 * Copyright (C) Julien Moutte <julien@moutte.net>
 */
static QRectF centerRect(const QRectF & src, const QRectF & dst)
{
    QRectF result = dst;
    qreal srcRatio = src.width() / src.height();
    qreal dstRatio = dst.width() / dst.height();

    if (srcRatio > dstRatio) {
        result.setWidth(dst.width());
        result.setHeight(dst.width() / srcRatio);
        result.moveTop(result.top() + ((dst.height() - result.height()) / 2));
    } else if (srcRatio < dstRatio) {
        result.setWidth(dst.height() * srcRatio);
        result.setHeight(dst.height());
        result.moveLeft(result.left() + ((dst.width() - result.width()) / 2));
    }

    return result;
}

void PaintAreas::calculate(const QRectF & _targetArea,
        const QSize & frameSize,
        const Fraction & pixelAspectRatio,
        const Fraction & displayAspectRatio)
{
    targetArea = _targetArea;

    qreal aspectRatio = pixelAspectRatio.ratio() * displayAspectRatio.invRatio();
    QRectF srcRect(QPointF(0,0), QSizeF(frameSize.width() * aspectRatio, frameSize.height()));

    videoArea = centerRect(srcRect, targetArea);

    if (videoArea == targetArea) {
        blackArea1 = blackArea2 = QRectF();
    } else {
        blackArea1 = QRectF(
            targetArea.left(),
            targetArea.top(),
            videoArea.left() == targetArea.left() ?
                targetArea.width() : videoArea.left() - targetArea.left(),
            videoArea.top() == targetArea.top() ?
                targetArea.height() : videoArea.top() - targetArea.top()
        );

        blackArea2 = QRectF(
            videoArea.right() == targetArea.right() ?
                targetArea.left() : videoArea.right(),
            videoArea.bottom() == targetArea.bottom() ?
                targetArea.top() : videoArea.bottom(),
            videoArea.right() == targetArea.right() ?
                targetArea.width() : targetArea.right() - videoArea.right(),
            videoArea.bottom() == targetArea.bottom() ?
                targetArea.height() : targetArea.bottom() - videoArea.bottom()
        );
    }
}
