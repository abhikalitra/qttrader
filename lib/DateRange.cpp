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

#include "DateRange.h"

#include <QDebug>
#include <QObject>

DateRange::DateRange ()
{
  _list << QString("1D");
  _list << QString("1W");
  _list << QString("1M");
  _list << QString("3M");
  _list << QString("6M");
  _list << QString("1Y");
  _list << QString("2Y");
  _list << QString("5Y");
  _list << QString("10Y");
  _list << QString("25Y");
  _list << QString("50Y");
  _list << QString("*");
}

QStringList
DateRange::list ()
{
  return _list;
}

int
DateRange::stringToIndex (QString d)
{
  return _list.indexOf(d);
}

QString
DateRange::indexToString (int d)
{
  return _list.at(d);
}

QDateTime
DateRange::interval (QDateTime ed, int type)
{
  QDateTime sd;
  
  switch ((Key) type)
  {
    case _DAY:
      sd = ed.addDays(-1);
      break;
    case _WEEK:
      sd = ed.addDays(-7);
      break;
    case _MONTH:
      sd = ed.addDays(-31);
      break;
    case _MONTH3:
      sd = ed.addDays(-63);
      break;
    case _MONTH6:
      sd = ed.addDays(-186);
      break;
    case _YEAR:
      sd = ed.addYears(-1);
//      sd = ed.addDays(-365);
      break;
    case _YEAR2:
      sd = ed.addYears(-2);
//      sd = ed.addDays(-365*2);
      break;
    case _YEAR5:
      sd = ed.addYears(-5);
      break;
    case _YEAR10:
      sd = ed.addYears(-10);
      break;
    case _YEAR25:
      sd = ed.addYears(-25);
      break;
    case _YEAR50:
      sd = ed.addYears(-50);
      break;
    case _ALL:
      sd = ed.addYears(-1000);
      break;
    default:
      break;
  }
  
  return sd;
}

QString
DateRange::rangeKey (QDateTime sd, QDateTime ed)
{
  QString s = sd.toString(Qt::ISODate) + ed.toString(Qt::ISODate);
  return s;
}
