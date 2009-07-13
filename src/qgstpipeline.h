/*
    Copyright (C) 2009  George Kiagiadakis <kiagiadakis.george@gmail.com>

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
#ifndef _QTGSTREAMER_QGSTPIPELINE_H
#define _QTGSTREAMER_QGSTPIPELINE_H

#include "qgstbin.h"
typedef struct _GstPipeline GstPipeline;

namespace QtGstreamer {

class QGstPipeline;
typedef QSharedPointer<QGstPipeline> QGstPipelinePtr;

class QGstPipeline : public QGstBin
{
    Q_OBJECT
    Q_DISABLE_COPY(QGstPipeline)
public:
    static QGstPipelinePtr newPipeline(const char *name = 0);
    static QGstPipelinePtr fromGstPipeline(GstPipeline *gstPipeline);
    virtual ~QGstPipeline();

protected:
    QGstPipeline(const char *name = 0);
    QGstPipeline(GstPipeline *gstPipeline);
};

}

Q_DECLARE_METATYPE(QtGstreamer::QGstPipelinePtr)

#endif
