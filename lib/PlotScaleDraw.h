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

// *************************************************************************************************
// Plots the date area of a chart
// *************************************************************************************************

#ifndef PLOT_SCALE_DRAW_HPP
#define PLOT_SCALE_DRAW_HPP

#include <qwt_scale_draw.h>
//#include <qwt_scale_widget.h>
#include <QList>
#include <QColor>
#include <QPainter>

class PlotScaleDraw : public QwtScaleDraw
{
  public:
    PlotScaleDraw ();
    virtual QwtText label (double v) const;
    virtual void draw (QPainter *, const QPalette &) const;
    void clearPoints ();
    void addPoint (QColor, double);

  private:
    QList<QColor> _colors;
    QList<double> _values;
};

#endif


