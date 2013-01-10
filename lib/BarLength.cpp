/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 *  USA.
 */

#include "BarLength.h"

#include <QtDebug>

BarLength::BarLength ()
{
  _list << "1";
  _list << "5";
  _list << "10";
  _list << "15";
  _list << "30";
  _list << "60";
  _list << "D";
  _list << "W";
  _list << "M";
}

QStringList
BarLength::list ()
{
  return _list;
}

int
BarLength::stringToIndex (QString d)
{
  return _list.indexOf(d);
}

QString
BarLength::indexToString (int d)
{
  return _list.at(d);
}

void
BarLength::interval (QDateTime dt, int length, QDateTime &startDate, QDateTime &endDate)
{
  startDate = dt;

  switch ((BarLength::Key) length)
  {
    case BarLength::_MINUTE1:
      startDate.setTime(QTime(startDate.time().hour(), startDate.time().minute(), 0, 0));
      endDate = startDate;
      endDate = endDate.addSecs(60);
      endDate = endDate.addSecs(-1);
      break;
    case BarLength::_MINUTE5:
    {
      int tint = startDate.time().minute() / 5;
      startDate.setTime(QTime(startDate.time().hour(), tint * 5, 0, 0));
      endDate = startDate;
      endDate = endDate.addSecs(300);
      endDate = endDate.addSecs(-1);
      break;
    }
    case BarLength::_MINUTE10:
    {
      int tint = startDate.time().minute() / 10;
      startDate.setTime(QTime(startDate.time().hour(), tint * 10, 0, 0));
      endDate = startDate;
      endDate = endDate.addSecs(600);
      endDate = endDate.addSecs(-1);
      break;
    }
    case BarLength::_MINUTE15:
    {
      int tint = startDate.time().minute() / 15;
      startDate.setTime(QTime(startDate.time().hour(), tint * 15, 0, 0));
      endDate = startDate;
      endDate = endDate.addSecs(900);
      endDate = endDate.addSecs(-1);
      break;
    }
    case BarLength::_MINUTE30:
    {
      int tint = startDate.time().minute() / 30;
      startDate.setTime(QTime(startDate.time().hour(), tint * 30, 0, 0));
      endDate = startDate;
      endDate = endDate.addSecs(1800);
      endDate = endDate.addSecs(-1);
      break;
    }
    case BarLength::_MINUTE60:
      startDate.setTime(QTime(startDate.time().hour(), 0, 0, 0));
      endDate = startDate;
      endDate = endDate.addSecs(3600);
      endDate = endDate.addSecs(-1);
      break;
    case BarLength::_DAILY:
      startDate.setTime(QTime(0, 0, 0, 0));
      endDate = startDate;
      endDate = endDate.addDays(1);
      endDate = endDate.addSecs(-1);
      break;
    case BarLength::_WEEKLY:
      startDate.setTime(QTime(0, 0, 0, 0));
      startDate = startDate.addDays(- startDate.date().dayOfWeek());
      endDate = startDate;
      endDate = endDate.addDays(7);
      endDate = endDate.addSecs(-1);
      break;
    case BarLength::_MONTHLY:
      startDate.setTime(QTime(0, 0, 0, 0));
      startDate = startDate.addDays(- (startDate.date().day() - 1));
      endDate = startDate;
      endDate = endDate.addDays(endDate.date().daysInMonth());
      endDate = endDate.addSecs(-1);
      break;
    default:
      break;
  }
}
