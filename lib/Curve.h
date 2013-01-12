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

#ifndef CURVE_HPP
#define CURVE_HPP

#include <qwt_plot_curve.h>
#include <QMap>

//#include "Plugin.h"
#include "Bar.h"

class Curve : public QwtPlotCurve
{
  public:
    Curve (QString);
    Curve ();
    ~Curve ();
    void setPlugin (QString);
    void draw (QPainter *, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const; //plugin
    int info (int, QStringList &); // plugin
    int scalePoint (int, QColor &, double &); // plugin
    int highLowRange (int start, int end, double &h, double &l); // plugin
    int fill (QString k1, QString k2, QString k3, QString k4, QColor);
    int rtti () const;
    void init ();
    void setType (int);
    int type ();
    void setLabel (QString);
    QString label ();
    void setPen (int);
    int pen ();
    void setStyle (int);
    int style ();
    void setColor (QColor);
    QColor color ();
    void setBar (int, Bar *);
    Bar * bar (int);
    int bars ();

  protected:
//    Plugin *_plugin;
    QString _pluginString;
    int _type;
    QString _label;
    int _pen;
    int _style;
    QString _name;
    QColor _color;
    QMap<int, Bar *> _bars;
};

#endif
