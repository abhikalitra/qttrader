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

#ifndef PLOT_INFO_HPP
#define PLOT_INFO_HPP

#include <QPainter>
#include <QStringList>
#include <QColor>
#include <qwt_plot_item.h>
#include <qwt_scale_map.h>

class PlotInfo : public QwtPlotItem
{
  public:
    PlotInfo ();
    ~PlotInfo ();
    void draw (QPainter *, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const;
    int rtti () const;
    void setColor (QColor);
    void setData (QStringList);

  protected:
    QColor _color;
    QStringList _text;
};

#endif
