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

#include "CurveLine.h"
#include "CLBar.h"
#include "Strip.h"
#include "Curve.h"
#include "CurveLineType.h"
#include "Global.h"


int
CurveLine::command (PluginData *pd)
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
CurveLine::draw (QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &, void *c)
{
  Curve *curve = (Curve *) c;
  QwtScaleDiv *sd = curve->plot()->axisScaleDiv(QwtPlot::xBottom);
  if (! g_symbol)
  {
    qDebug() << "CurveLine::draw: bars missing";
    return 0;
  }

  int loop = sd->lowerBound();
  int size = sd->upperBound();

//  painter->setRenderHint(QPainter::Antialiasing, TRUE);

  QPen tpen = painter->pen();
  tpen.setWidth(curve->pen());
  tpen.setColor(curve->color());

  CurveLineType clt;
  int style = curve->style();
  switch ((CurveLineType::Key) style)
  {
    case CurveLineType::_DASH:
      tpen.setStyle((Qt::PenStyle) Qt::DashLine);
      break;
    case CurveLineType::_DOT:
      tpen.setStyle((Qt::PenStyle) Qt::DotLine);
      break;
    default:
      tpen.setStyle((Qt::PenStyle) Qt::SolidLine);
      break;
  }

  painter->setPen(tpen);
  
  switch ((CurveLineType::Key) style)
  {
    case CurveLineType::_DASH:
    case CurveLineType::_SOLID:
    {
      for (; loop < size; loop++)
      {
        CLBar *yb = (CLBar *) curve->bar(loop - 1);
        if (! yb)
          continue;

        CLBar *b = (CLBar *) curve->bar(loop);
        if (! b)
          continue;

        int x = xMap.transform(loop - 1);
        int x2 = xMap.transform(loop);
        int y = yMap.transform(yb->value());
        int y2 = yMap.transform(b->value());

        painter->drawLine (x, y, x2, y2);
      }
      break;
    }
    default:
    {
      for (; loop < size; loop++)
      {
        CLBar *b = (CLBar *) curve->bar(loop);
        if (! b)
          continue;

        int x = xMap.transform(loop);
        int y = yMap.transform(b->value());

        painter->drawPoint (x, y);
      }
      break;
    }
  }
  
  return 1;
}

int
CurveLine::info (PluginData *pd)
{
  if (! pd->data)
    return 0;
  
  Curve *curve = (Curve *) pd->data;
  
  CLBar *bar = (CLBar *) curve->bar(pd->index);
  if (! bar)
    return 0;

  Strip strip;
  QString d;
  strip.strip(bar->value(), 4, d);
  pd->info << curve->label() + "=" + d;

  return 1;
}

int
CurveLine::scalePoint (PluginData *pd)
{
  if (! pd->data)
    return 0;
  
  Curve *curve = (Curve *) pd->data;
  
  CLBar *bar = (CLBar *) curve->bar(pd->index);
  if (! bar)
    return 0;
  
  pd->value = bar->value();
  pd->color = curve->color();

  return 1;
}

int
CurveLine::highLowRange (PluginData *pd)
{
  if (! pd->data)
    return 0;
  
  Curve *curve = (Curve *) pd->data;
  
  int flag = 0;
  for (int pos = pd->start; pos <= pd->end; pos++)
  {
    CLBar *r = (CLBar *) curve->bar(pos);
    if (! r)
      continue;

    if (! flag)
    {
      pd->high = r->value();
      pd->low = r->value();
      flag++;
    }
    else
    {
      if (r->value() > pd->high)
        pd->high = r->value();
      
      if (r->value() < pd->low)
        pd->low = r->value();
    }
  }

  return flag;
}

int
CurveLine::fill (PluginData *pd)
{
  if (pd->key1.isEmpty())
    return 0;
  
  if (! g_symbol)
  {
    qDebug() << "CurveLine::fill: bars missing";
    return 0;
  }

  if (! pd->data)
    return 0;
  
  Curve *curve = (Curve *) pd->data;
  
  QList<int> keys = g_symbol->keys();
  
  for (int pos = 0; pos < keys.size(); pos++)
  {
    CBar *r = g_symbol->bar(keys.at(pos));
    
//qDebug() << "CurveLine::fill:" << k1 << QString::number(pos);
    
    double v = 0;
    if (! r->get(pd->key1, v))
      continue;

    curve->setBar(keys.at(pos), new CLBar(v));
  }

  return 1;
}


// do not remove
Q_EXPORT_PLUGIN2(curveline, CurveLine);
