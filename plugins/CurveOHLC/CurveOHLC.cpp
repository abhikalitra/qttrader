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

#include "CurveOHLC.h"
#include "OHLCBar.h"
#include "Strip.h"
#include "Curve.h"
#include "CurveOHLCType.h"
#include "Global.h"


int
CurveOHLC::command (PluginData *pd)
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
CurveOHLC::draw (QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &, void *c)
{
  Curve *curve = (Curve *) c;
  QwtScaleDiv *sd = curve->plot()->axisScaleDiv(QwtPlot::xBottom);
  if (! g_symbol)
  {
    qDebug() << "CurveOHLC::draw: bars missing";
    return 0;
  }

  int loop = sd->lowerBound();
  int size = sd->upperBound();
  bool ff = FALSE;
  
  for (; loop < size; loop++)
  {
    OHLCBar *b = (OHLCBar *) curve->bar(loop);
    if (! b)
      continue;

    painter->setPen(b->color());
    
    int x = xMap.transform(loop);
    int width = xMap.transform(loop + 1) - x;
    
    int yo = yMap.transform(b->open());
    int yh = yMap.transform(b->high());
    int yl = yMap.transform(b->low());
    int yc = yMap.transform(b->close());
    
    switch ((CurveOHLCType::Key) curve->style())
    {
      case CurveOHLCType::_OHLC:
      {
        QRect rect(QPoint(x + 1, yh), QPoint(x + width - 1, yl));
        painter->drawLine (rect.center().x(), yh, rect.center().x(), yl); // draw the high/low line
        painter->drawLine (rect.left(), yo, rect.center().x(), yo); // draw the open tick
        painter->drawLine (rect.right(), yc, rect.center().x(), yc); // draw the close tick
        break;
      }
      default: // Candle
      {
        // fill all candles, color is enough. Fill/empty is redundant information.
        QRect rect(QPoint(x + 2, yo), QPoint(x + width - 2, yc));
        painter->drawLine (rect.center().x(), yh, rect.center().x(), yl);
        painter->setBrush(b->color());
        painter->drawRect(rect);

        /*
        ff = FALSE;
        if (b->close() < b->open())
          ff = TRUE;

        if (! ff)
        {
          // empty candle c > o
          QRect rect(QPoint(x + 2, yc), QPoint(x + width - 2, yo));
          painter->drawLine (rect.center().x(), yh, rect.center().x(), yl);
          painter->setBrush(curve->plot()->canvasBackground());
          painter->drawRect(rect);
        }
        else
        {
          // filled candle c < o
          QRect rect(QPoint(x + 2, yo), QPoint(x + width - 2, yc));
          painter->drawLine (rect.center().x(), yh, rect.center().x(), yl);
          painter->setBrush(b->color());
          painter->drawRect(rect);
        }
        break;
        */
      }
    }
  }
  
  return 1;
}

int
CurveOHLC::info (PluginData *pd)
{
  if (! pd->data)
    return 0;
  
  Curve *curve = (Curve *) pd->data;
  
  OHLCBar *bar = (OHLCBar *) curve->bar(pd->index);
  if (! bar)
    return 0;

  Strip strip;
  QString d;
  strip.strip(bar->open(), 4, d);
  pd->info << "O=" + d;

  strip.strip(bar->high(), 4, d);
  pd->info << "H=" + d;

  strip.strip(bar->low(), 4, d);
  pd->info << "L=" + d;

  strip.strip(bar->close(), 4, d);
  pd->info << "C=" + d;

  pd->info << "Index= " + QString::number(pd->index);

  return 1;
}

int
CurveOHLC::scalePoint (PluginData *pd)
{
  if (! pd->data)
    return 0;
  
  Curve *curve = (Curve *) pd->data;
  
  OHLCBar *bar = (OHLCBar *) curve->bar(pd->index - 1); // -1 fix for alignment issue
  if (! bar)
    return 0;

  pd->color = bar->color();
  pd->value = bar->close();

  return 1;
}

int
CurveOHLC::highLowRange (PluginData *pd)
{
  if (! pd->data)
    return 0;
  
  Curve *curve = (Curve *) pd->data;
  
  int flag = 0;
  for (int pos = pd->start; pos <= pd->end; pos++)
  {
    OHLCBar *r = (OHLCBar *) curve->bar(pos);
    if (! r)
      continue;

    if (! flag)
    {
      pd->high = r->high();
      pd->low = r->low();
      flag++;
    }
    else
    {
      if (r->high() > pd->high)
        pd->high = r->high();
      
      if (r->low() < pd->low)
        pd->low = r->low();
    }
  }

  return flag;
}

int
CurveOHLC::fill (PluginData *pd)
{
  if (pd->key1.isEmpty() || pd->key2.isEmpty() || pd->key3.isEmpty() || pd->key4.isEmpty())
    return 0;
  
  if (! g_symbol)
  {
    qDebug() << "CurveOHLC::fill: bars missing";
    return 0;
  }

  if (! pd->data)
    return 0;
  
  Curve *curve = (Curve *) pd->data;
  
  QList<int> keys = g_symbol->keys();
  
  for (int pos = 0; pos < keys.size(); pos++)
  {
    CBar *r = g_symbol->bar(keys.at(pos));
    
    double o = 0;
    if (! r->get(pd->key1, o))
      continue;

    double h = 0;
    if (! r->get(pd->key2, h))
      continue;

    double l = 0;
    if (! r->get(pd->key3, l))
      continue;

    double c = 0;
    if (! r->get(pd->key4, c))
      continue;
    
    curve->setBar(keys.at(pos), new OHLCBar(pd->color, o, h, l, c));
  }

  return 1;
}


// do not remove
Q_EXPORT_PLUGIN2(curveohlc, CurveOHLC);
