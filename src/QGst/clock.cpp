/*
    Copyright (C) 2010  Collabora Multimedia.
      @author Mauricio Piacentini <mauricio.piacentini@collabora.co.u

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
#include "clock.h"

#include <QtCore/QTime>
#include <gst/gstclock.h>
#include <gst/gstsystemclock.h>

namespace QGst {

ClockPtr Clock::systemClock()
{
    return ClockPtr::wrap(gst_system_clock_obtain());
}

ClockTime Clock::clockTime()
{
    GstClockTime t = gst_clock_get_time(object<GstClock>());
    return t;
}

ClockTime Clock::resolution()
{
    GstClockTime t = gst_clock_get_resolution(object<GstClock>());
    return t;
}

QTime Clock::time()
{
    return Clock::timeFromClockTime(clockTime());
}

QTime Clock::timeFromClockTime(ClockTime clocktime)
{
    ClockTime asSeconds = GST_TIME_AS_SECONDS(clocktime);
    ClockTime h = (asSeconds / 3600) % 24;
    ClockTime min = (asSeconds / 60) % 60;
    ClockTime sec = asSeconds % 60;
    ClockTime msec = GST_TIME_AS_MSECONDS(clocktime) % 1000;
    return QTime(h, min, sec, msec);
}

ClockTime Clock::clockTimeFromTime(const QTime & time)
{
    return (time.hour()*3600*1000000000LL) + (time.minute()*60*1000000000LL)
            + (time.second()*1000000000LL) + (time.msec()*1000000LL);
}

}
