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
#include "pluginfeature.h"
#include "helpers_p.h"
#include <gst/gstpluginfeature.h>

namespace QGst {

uint PluginFeature::rank() const
{
    return gst_plugin_feature_get_rank(GST_PLUGIN_FEATURE(m_object));
}

void PluginFeature::setRank(uint rank)
{
    gst_plugin_feature_set_rank(GST_PLUGIN_FEATURE(m_object), rank);
}

QString PluginFeature::name() const
{
    return QString::fromUtf8(gst_plugin_feature_get_name(GST_PLUGIN_FEATURE(m_object)));
}

void PluginFeature::setName(const QString & name)
{
    gst_plugin_feature_set_name(GST_PLUGIN_FEATURE(m_object), qstringToGcharPtr(name));
}

bool PluginFeature::checkVersion(uint major, uint minor, uint micro) const
{
    return gst_plugin_feature_check_version(GST_PLUGIN_FEATURE(m_object), major, minor, micro);
}

PluginFeaturePtr PluginFeature::load() const
{
    return PluginFeaturePtr::wrap(gst_plugin_feature_load(GST_PLUGIN_FEATURE(m_object)), false);
}

}
