/*
    Copyright (C) 2011 Collabora Ltd.
      @author George Kiagiadakis <george.kiagiadakis@collabora.co.uk>

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
#ifndef QTGSTREAMERUI_EXPORT_H
#define QTGSTREAMERUI_EXPORT_H

#include <QtCore/QtGlobal>

/* defined by cmake when building this library */
#if defined(QtGStreamerUi_EXPORTS)
# define QTGSTREAMERUI_EXPORT Q_DECL_EXPORT
#else
# define QTGSTREAMERUI_EXPORT Q_DECL_IMPORT
#endif

#if !defined(Q_OS_WIN) && !defined(Q_CC_NOKIAX86) && \
    !defined(Q_CC_RVCT) && defined(QT_VISIBILITY_AVAILABLE)
# define QTGSTREAMERUI_NO_EXPORT __attribute__((visibility("hidden")))
#else
# define QTGSTREAMERUI_NO_EXPORT
#endif

#endif
