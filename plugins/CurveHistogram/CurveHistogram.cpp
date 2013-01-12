/*
 *  QtTrader stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#include <qwt_scale_div.h>
#include <QtGui>

#include "CurveHistogram.h"
#include "CHBar.h"
#include "Strip.h"
#include "Curve.h"
#include "CurveHistogramType.h"
#include "Global.h"


int
CurveHistogram::command (PluginData *pd)
{
  int rc = 0;

  QStringList cl;
  cl << "type" << "info" << "scalePoint" << "highLowRange" << "fill";
  
  switch (cl.indexOf(pd->command))
  {
    case 0: // type
      pd->type = QString("curve");
      rc = 1;
      break;
    case 1: // info
      rc = info(pd);
      break;
    case 2: // scalePoint
      rc = scalePoint(pd);
      break;
    case 3: // highLowRange
      rc = highLowRange(pd);
      break;
    case 4: // fill
      rc = fill(pd);
      break;
    default:
      break;
  }
  
  return rc;
}

int
CurveHistogram::draw (QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &, void *c)
{
  Curve *curve = (Curve *) c;
  QwtScaleDiv *sd = curve->plot()->axisScaleDiv(QwtPlot::xBottom);
  if (! g_symbol)
  {
    qDebug() << "CurveHistogram::draw: bars missing";
    return 0;
  }

  int loop = sd->lowerBound();
  int size = sd->upperBound();
  
  for (; loop < size; loop++)
  {
    CHBar *b = (CHBar *) curve->bar(loop);
    if (! b)
      continue;

    QPolygon poly;
    int width = xMap.transform(loop + 1) - xMap.transform(loop);
    
    // base left
    int x = xMap.transform(loop);
    int by = yMap.transform(b->base());
    poly << QPoint(x, by);
    
    // value left
    int vy = yMap.transform(b->value());
    poly << QPoint(x, vy);
    
    // value right
    x += width;
    poly << QPoint(x, vy);
    
    // base right
    poly << QPoint(x, by);
    
    painter->setBrush(b->color());
    painter->drawPolygon(poly, Qt::OddEvenFill);
  }
  
  return 1;
}

int
CurveHistogram::info (PluginData *pd)
{
  if (! pd->data)
    return 0;
  
  Curve *curve = (Curve *) pd->data;
  
  CHBar *bar = (CHBar *) curve->bar(pd->index);
  if (! bar)
    return 0;
  
  Strip strip;
  QString d;
  strip.strip(bar->value(), 4, d);
  
  pd->info << curve->label() + "=" + d;

  return 1;
}

int
CurveHistogram::scalePoint (PluginData *pd)
{
  if (! pd->data)
    return 0;
  
  Curve *curve = (Curve *) pd->data;
  
  CHBar *bar = (CHBar *) curve->bar(pd->index - 1); // -1 fix for alignment issue
  if (! bar)
    return 0;

  pd->value = bar->value();
  pd->color = bar->color();

  return 1;
}

int
CurveHistogram::highLowRange (PluginData *pd)
{
  if (! pd->data)
    return 0;
  
  Curve *curve = (Curve *) pd->data;
  
  int flag = 0;
  for (int pos = pd->start; pos <= pd->end; pos++)
  {
    CHBar *r = (CHBar *) curve->bar(pos);
    if (! r)
      continue;

    if (! flag)
    {
      pd->high = r->base();
      pd->low = r->base();
      
      if (r->value() > pd->high)
        pd->high = r->value();
      if (r->value() < pd->low)
        pd->low = r->value();

      flag++;
    }
    else
    {
      if (r->base() > pd->high)
        pd->high = r->base();
      if (r->base() < pd->low)
        pd->low = r->base();

      if (r->value() > pd->high)
        pd->high = r->value();
      if (r->value() < pd->low)
        pd->low = r->value();
    }
  }

  return flag;
}

int
CurveHistogram::fill (PluginData *pd)
{
  if (pd->key1.isEmpty())
    return 0;
  
  if (! g_symbol)
  {
    qDebug() << "CurveHistogram::fill: bars missing";
    return 0;
  }

  if (! pd->data)
    return 0;
  
  Curve *curve = (Curve *) pd->data;
  
  QList<int> keys = g_symbol->keys();

  for (int pos = 0; pos < keys.size(); pos++)
  {
    CBar *r = g_symbol->bar(keys.at(pos));
    
    double v = 0.0;
    if (! r->get(pd->key1, v))
      continue;

    curve->setBar(keys.at(pos), new CHBar(pd->color, 0.0, v));
  }

  return 1;
}

// do not remove
Q_EXPORT_PLUGIN2(curvehistogram, CurveHistogram);
