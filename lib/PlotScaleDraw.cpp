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

#include "PlotScaleDraw.h"
#include "Strip.h"

#include <QString>
#include <QDebug>
#include <qwt_scale_map.h>

PlotScaleDraw::PlotScaleDraw ()
{
}

//Convert the scale from double to QwtText
QwtText PlotScaleDraw::label (double v) const
{
  Strip strip;

  double tv = v;

  int flag = 0;
  if (tv < 0)
  {
    flag = 1;
    tv = tv * -1;
  }

  QString s;
  if (tv < 1000)
    strip.strip(tv, 4, s);
  else
  {
    if (tv >= 1000 && tv < 1000000)
    {
      s = QString::number(tv / 1000, 'f', 2);
      while (s.endsWith("0"))
        s.chop(1);
      while (s.endsWith("."))
        s.chop(1);
      s.append("K");
    }
    else if (tv >= 1000000 && tv < 1000000000)
    {
      s = QString::number(tv / 1000000, 'f', 2);
      while (s.endsWith("0"))
        s.chop(1);
      while (s.endsWith("."))
        s.chop(1);
      s.append("M");
    }
    else if (tv >= 1000000000)
    {
      s = QString::number(tv / 1000000000, 'f', 2);
      while (s.endsWith("0"))
        s.chop(1);
      while (s.endsWith("."))
        s.chop(1);
      s.append("B");
    }
  }

  if (flag)
    s.prepend("-");

  return s;
}

// Draws the scale to the window
void PlotScaleDraw::draw (QPainter *p, const QPalette &pal) const
{
  // first draw the original label
  QwtAbstractScaleDraw::draw(p, pal);

  // now we overwrite with our own color version
  QFontMetrics fm = p->fontMetrics();

  Strip strip;
  int offset = 8;
  int loop = 0;
  int x = 0;
  for (; loop < _colors.count(); loop++)
  {
    QColor color = _colors.at(loop);

    double v = _values.at(loop);
    QString s;
    strip.strip(v, 4, s);

    int y = map().transform(v);

    QRect rc = p->boundingRect(x + offset,
                               y - (fm.height() / 2),
                               0,
                               0,
                               0,
                               s);

    // draw the left arrow portion of the box shape
    QPolygon arrow(4);
    arrow.setPoint(0, x, y);
    arrow.setPoint(1, x + offset, rc.top() - 1);
    arrow.setPoint(2, x + offset, rc.bottom() + 1);
    arrow.setPoint(3, x, y);

    p->setBrush(color);
    p->drawPolygon(arrow, Qt::OddEvenFill);

    p->fillRect(rc, color);

    QBrush bbrush = p->background();
    color.setRed(color.red() || bbrush.color().red());
    color.setGreen(color.green() || bbrush.color().green());
    color.setBlue(color.blue() || bbrush.color().blue());
    p->setPen(color);
    p->drawText(rc, s);
  }
}

/*
void PlotScaleDraw::drawPoints (QwtScaleWidget *w)
{
  QPainter p;
  p.begin(w);

  // now we overwrite with our own color version
  QFontMetrics fm = p.fontMetrics();

  Strip strip;
  int offset = 8;
  int loop = 0;
  int x = 0;
  for (; loop < _colors.count(); loop++)
  {
    QColor color = _colors.at(loop);

    double v = _values.at(loop);
    QString s;
    strip.strip(v, 2, s);

    int y = map().transform(v);

    QRect rc = p.boundingRect(x + offset,
                               y - (fm.height() / 2),
                               0,
                               0,
                               0,
                               s);

    // draw the left arrow portion of the box shape
    QPolygon arrow(4);
    arrow.setPoint(0, x, y);
    arrow.setPoint(1, x + offset, rc.top() - 1);
    arrow.setPoint(2, x + offset, rc.bottom() + 1);
    arrow.setPoint(3, x, y);

    p.setBrush(color);
    p.drawPolygon(arrow, Qt::OddEvenFill);

    p.fillRect(rc, color);

    QBrush bbrush = p.background();
    color.setRed(color.red() || bbrush.color().red());
    color.setGreen(color.green() || bbrush.color().green());
    color.setBlue(color.blue() || bbrush.color().blue());
    p.setPen(color);
    p.drawText(rc, s);
  }
}
*/

void PlotScaleDraw::clearPoints ()
{
  _colors.clear();
  _values.clear();

  // force a replot
  QwtScaleDiv sd = scaleDiv();
  sd.invalidate();
  setScaleDiv(sd);
}

void PlotScaleDraw::addPoint (QColor c, double v)
{
  _colors.append(c);
  _values.append(v);
}
