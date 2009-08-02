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
#include "qgstpipeline.h"
#include <gst/gstpipeline.h>

namespace QtGstreamer {

QGST_WRAPPER_GTYPE(QGstPipeline, GST_TYPE_PIPELINE)

QGstPipeline::QGstPipeline(const char *name)
    : QGstBin(GST_BIN(gst_pipeline_new(name)))
{
}

//static
QGstPipelinePtr QGstPipeline::newPipeline(const char *name)
{
    return QGstPipelinePtr(new QGstPipeline(name));
}

QGstPipeline::QGstPipeline(GstPipeline *gstPipeline)
    : QGstBin(GST_BIN(gstPipeline))
{
}

//static
QGstPipelinePtr QGstPipeline::fromGstPipeline(GstPipeline *gstPipeline)
{
    return QGstPipelinePtr(new QGstPipeline(gstPipeline));
}

QGstPipeline::~QGstPipeline()
{
}

}

#include "qgstpipeline.moc"
