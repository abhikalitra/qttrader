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


#include "MarkerHLine.h"
#include "Strip.h"
#include "Marker.h"
#include "Plot.h"
#include "PlotStatus.h"

#include <QtGui>


int
MarkerHLine::command (PluginData *pd)
{
  int rc = 0;

  QStringList cl;
  cl << "type" << "info" << "highLow" << "move" << "click" << "create" << "settings";
  
  switch (cl.indexOf(pd->command))
  {
    case 0: // type
      pd->type = QString("marker");
      rc = 1;
      break;
    case 1: // info
      rc = info(pd);
      break;
    case 2: // highLow
      rc = highLow(pd);
      break;
    case 3: // move
      rc = move(pd);
      break;
    case 4: // click
      rc = click(pd);
      break;
    case 5: // create
      rc = create(pd);
      break;
    case 6: // settings
      rc = settings(pd);
      break;
    default:
      break;
  }
  
  return rc;
}

int
MarkerHLine::draw (QPainter *p, const QwtScaleMap &, const QwtScaleMap &yMap, const QRect &, void *m)
{
  Marker *hl = (Marker *) m;
  Entity *e = hl->settings();
  
  QVariant *price = e->get(QString("price"));
  if (! price)
  {
    qDebug() << "MarkerHLine::draw: price missing";
    return 0;
  }
  
  QVariant *tset = e->get(QString("color"));
  if (! tset)
  {
    qDebug() << "MarkerHLine::draw: color missing";
    return 0;
  }
  QColor color(tset->toString());
  
  p->setPen(color);

  int y = yMap.transform(price->toDouble());

  // test start
  Strip strip;
  QString ts;
  strip.strip(price->toDouble(), 4, ts);
  QString s = " " + ts; // prepend space so we can clearly read text
  QFontMetrics fm = p->fontMetrics();
  QRect rc = p->boundingRect(0, y - (fm.height() / 2), 1, 1, 0, s);
  p->fillRect(rc, hl->plot()->canvasBackground()); // fill in behind text first
  p->drawText(rc, s); // draw text
  p->drawRect(rc); // draw a nice little box around text

  p->drawLine (rc.width(), y, p->window().width(), y);

  hl->clearSelectionArea();

  QPolygon array;
  array.putPoints(0,
                  4,
                  0,
                  y - 4,
                  0,
                  y + 4,
                  p->window().width(),
                  y + 4,
                  p->window().width(),
                  y - 4);

  hl->appendSelectionArea(QRegion(array));

  if (hl->selected())
  {
    hl->clearGrabHandles();

    int hw = hl->handleWidth();
    int t = (int) p->window().width() / 4;
    int loop;
    for (loop = 0; loop < 5; loop++)
    {
      hl->appendGrabHandle(QRegion(t * loop,
                                   y - (hw / 2),
                                   hw,
                                   hw,
                                   QRegion::Rectangle));

      p->fillRect(t * loop,
                  y - (hw / 2),
                  hw,
                  hw,
                  color);
    }
  }
  
  return 1;
}

int
MarkerHLine::info (PluginData *pd)
{
  if (! pd->data)
    return 0;
  
  Marker *hl = (Marker *) pd->data;
  
  Entity *e = hl->settings();
  QVariant *price = e->get(QString("price"));
  if (! price)
    return 0;

  Strip strip;
  QString ts;
  strip.strip(price->toDouble(), 4, ts);
  
  pd->info << QString("HLine=") + ts;
  
  return 1;
}

int
MarkerHLine::move (PluginData *pd)
{
  if (! pd->data)
    return 0;
  
  Marker *hl = (Marker *) pd->data;
  
  Entity *e = hl->settings();
  
  switch (pd->status)
  {
    case PlotStatus::_MOVE:
    {
      QVariant *price = e->get(QString("price"));
      if (! price)
        return 0;
      
      QwtScaleMap map = hl->plot()->canvasMap(QwtPlot::yRight);
      price->setValue(map.invTransform((double) pd->point.y()));

      hl->plot()->replot();

      hl->setModified(TRUE);
      break;
    }
    default:
      break;
  }
  
  return 1;
}

int
MarkerHLine::highLow (PluginData *pd)
{
  if (! pd->data)
    return 0;
  
  Marker *hl = (Marker *) pd->data;
  
  Entity *e = hl->settings();
  QVariant *price = e->get(QString("price"));
  if (! price)
    return 0;

  pd->high = price->toDouble();
  pd->low = price->toDouble();

  return 1;
}

int
MarkerHLine::click (PluginData *pd)
{
  if (! pd->data)
    return 0;
  
  Marker *hl = (Marker *) pd->data;
  
  switch ((PlotStatus::Key) pd->status)
  {
    case PlotStatus::_SELECTED:
    {
      switch (pd->button)
      {
        case Qt::LeftButton:
          if (hl->isGrabSelected(pd->point))
          {
            pd->status = PlotStatus::_MOVE;
            return 1;
          }

          if (! hl->isSelected(pd->point))
          {
            pd->status = PlotStatus::_NONE;
            hl->setSelected(FALSE);

            Plot *tplot = (Plot *) hl->plot();
            tplot->unselectMarker();

            hl->plot()->replot();
            return 1;
          }
          break;
        case Qt::RightButton:
        {
          Plot *p = (Plot *) hl->plot();
          p->showMarkerMenu();
          break;
        }
        default:
          break;
      }

      break;
    }
    case PlotStatus::_MOVE:
    {
      switch (pd->button)
      {
        case Qt::LeftButton:
          pd->status = PlotStatus::_SELECTED;
          return 1;
        default:
          break;
      }

      break;
    }
    default: // _None
    {
      switch (pd->button)
      {
        case Qt::LeftButton:
        {
          if (hl->isSelected(pd->point))
          {
            pd->status = PlotStatus::_SELECTED;
            hl->setSelected(TRUE);

            Plot *tplot = (Plot *) hl->plot();
            tplot->selectMarker(hl->ID());

            hl->plot()->replot();
            return 1;
          }
          break;
        }
        default:
          break;
      }

      break;
    }
  }
  
  return 1;
}

int
MarkerHLine::create (PluginData *pd)
{
  if (! pd->data)
    return 0;
  
  Marker *hl = (Marker *) pd->data;
  
  pd->status = PlotStatus::_MOVE;
  hl->setSelected(TRUE);
  emit signalMessage(QObject::tr("Place HLine marker"));
  return 1;
}

int
MarkerHLine::settings (PluginData *pd)
{
  Entity *e = new Entity;
  e->set(QString("plot"), new QVariant(QString()));
  e->set(QString("type"), new QVariant(QString("marker")));
  e->set(QString("symbol"), new QVariant(QString()));
  e->set(QString("plugin"), new QVariant(QString("MarkerHLine")));
  e->set(QString("price"), new QVariant(0.0));
  e->set(QString("color"), new QVariant(QString("red")));
  pd->settings = e;
  
  return 1;
}

// do not remove
Q_EXPORT_PLUGIN2(markerhline, MarkerHLine);
