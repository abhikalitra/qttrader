/*
 *  QtTrader stock charter
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

#include "CBar.h"

#include <QtDebug>

CBar::CBar ()
{
}

void
CBar::setDate (QDateTime d)
{
  _date = d;
}

QDateTime
CBar::date ()
{
  return _date;
}

void
CBar::set (QString k, double d)
{
  _values.insert(k, d);
}

int
CBar::get (QString k, double &d)
{
  if (! _values.contains(k))
    return 0;
  
  d = _values.value(k);
  return 1;
}

int
CBar::highLow (double &h, double &l)
{
  bool flag = FALSE;
  QHashIterator<QString, double> it(_values);
  while (it.hasNext())
  {
    it.next();
    
    if (! flag)
    {
      h = it.value();
      l = it.value();
      flag = TRUE;
    }
    else
    {
      if (it.value() > h)
        h = it.value();
      if (it.value() < l)
        l = it.value();
    }
  }

  return 0;
}
