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

#include "OHLCBar.h"

#include <QtDebug>

OHLCBar::OHLCBar (QColor color, double open, double high, double low, double close)
{
  _color = color;
  _open = open;
  _high = high;
  _low = low;
  _close = close;
}

void
OHLCBar::setColor (QColor d)
{
  _color = d;
}

QColor
OHLCBar::color ()
{
  return _color;
}

double
OHLCBar::open ()
{
  return _open;
}

double
OHLCBar::high ()
{
  return _high;
}

double
OHLCBar::low ()
{
  return _low;
}

double
OHLCBar::close ()
{
  return _close;
}
