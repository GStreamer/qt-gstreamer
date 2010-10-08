/*
    Copyright (C) 2010  George Kiagiadakis <kiagiadakis.george@gmail.com>
    Copyright (C) 2010  Collabora Multimedia. Author: Mauricio Piacentini <mauricio.piacentini@collabora.co.uk>

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
#ifndef QGST_VIDEOORIENTATION_H
#define QGST_VIDEOORIENTATION_H

#include "element.h"

namespace QGst {

/*! \interface VideoOrientation videoorientation.h <QGst/VideoOrientation>
 * \brief Wrapper class for GstVideoOrientation
 */
class VideoOrientation : public Element
{
    QGST_WRAPPER(VideoOrientation)
public:
    bool hFlip() const;
    bool vFlip() const;
    int hCenter() const;
    int vCenter() const;
    bool setHFlip(bool enabled);
    bool setVFlip(bool enabled);
    bool setHCenter(int center);
    bool setVCenter(int center);
};

} //namespace QGst

QGLIB_REGISTER_TYPE(QGst::VideoOrientation)

#endif // QGST_VIDEOORIENTATION_H
