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

#include "PlotInfo.h"

#include <QDebug>
#include <qwt_plot.h>
#include <qwt_scale_div.h>

PlotInfo::PlotInfo ()
{
  _color = QColor(Qt::white);
  setYAxis(QwtPlot::yRight);
  setZ(9999);
}

PlotInfo::~PlotInfo ()
{
  detach();
}

int
PlotInfo::rtti () const
{
  return Rtti_PlotUserItem;
}

void
PlotInfo::setColor (QColor c)
{
  _color = c;
}

void
PlotInfo::setData (QStringList l)
{
  _text = l;
}

void
PlotInfo::draw (QPainter *p, const QwtScaleMap &, const QwtScaleMap &, const QRect &) const
{
  QFontMetrics fm = p->fontMetrics();

  p->setPen(_color);

  int x = 5;
  int y = fm.height();
  for (int loop = 0; loop < _text.size(); loop++, y += fm.height())
    p->drawText(x, y, _text.at(loop));
}
