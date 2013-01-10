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

#include "Bar.h"

#include <QtDebug>

Bar::Bar ()
{
}

void
Bar::setDate (QDateTime)
{
}

QDateTime
Bar::date ()
{
  return QDateTime();
}

void
Bar::setColor (QColor)
{
}

QColor
Bar::color ()
{
  return QColor();
}

void
Bar::set (QString, double)
{
}

int
Bar::get (QString, double &)
{
  return 0;
}

int
Bar::highLow (double &, double &)
{
/*  
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
*/

  return 0;
}

void
Bar::setOpen (double)
{
}

double
Bar::open ()
{
  return 0;
}

void
Bar::setHigh (double)
{
}

double
Bar::high ()
{
  return 0;
}

void
Bar::setLow (double)
{
}

double
Bar::low ()
{
  return 0;
}

void
Bar::setClose (double)
{
}

double
Bar::close ()
{
  return 0;
}

void
Bar::setValue (double)
{
}

double
Bar::value ()
{
  return 0;
}

void
Bar::setBase (double)
{
}

double
Bar::base ()
{
  return 0;
}
