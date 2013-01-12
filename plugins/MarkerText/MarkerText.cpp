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


#include "MarkerText.h"
#include "Strip.h"
#include "Marker.h"
#include "Plot.h"
#include "PlotStatus.h"
#include "PlotDateScaleDraw.h"
#include "Global.h"

#include <QtGui>


int
MarkerText::command (PluginData *pd)
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
MarkerText::draw (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &, void *m)
{
  Marker *text = (Marker *) m;
  Entity *e = text->settings();
  
  PlotDateScaleDraw *dsd = (PlotDateScaleDraw *) text->plot()->axisScaleDraw(QwtPlot::xBottom);
  QVariant *date = e->get(QString("date"));
  if (! date)
  {
    qDebug() << "MarkerText::draw: date missing";
    return 0;
  }
  int x = xMap.transform(dsd->dateToX(date->toDateTime()));
  if (x == -1)
    return 1;

  QVariant *price = e->get(QString("price"));
  if (! price)
  {
    qDebug() << "MarkerText::draw: price missing";
    return 0;
  }
  int y = yMap.transform(price->toDouble());

  QVariant *tset = e->get(QString("color"));
  if (! tset)
  {
    qDebug() << "MarkerText::draw: color missing";
    return 0;
  }
  QColor color(tset->toString());
  
  p->setPen(color);
  
  QVariant *font = e->get(QString("font"));
  if (! font)
  {
    qDebug() << "MarkerText::draw: font missing";
    return 0;
  }
  p->setFont(QFont(font->toString()));
  
  QVariant *string = e->get(QString("text"));
  if (! string)
  {
    qDebug() << "MarkerText::draw: text missing";
    return 0;
  }

  p->drawText(x, y, string->toString());

  QFontMetrics fm = p->fontMetrics();

  text->clearSelectionArea();

  text->appendSelectionArea(QRegion(x,
                            y - fm.height(),
                            fm.width(string->toString(), -1),
                            fm.height(),
                            QRegion::Rectangle));

  if (text->selected())
  {
    int handleWidth = text->handleWidth();
    
    text->clearGrabHandles();

    text->appendGrabHandle(QRegion(x - handleWidth - 1,
                           y - (fm.height() / 2),
                           handleWidth,
                           handleWidth,
                           QRegion::Rectangle));

    p->fillRect(x - handleWidth - 1,
                y - (fm.height() / 2),
                handleWidth,
                handleWidth,
                color);
  }
  
  return 1;
}

int
MarkerText::info (PluginData *pd)
{
  if (! pd->data)
    return 0;
  
  Marker *text = (Marker *) pd->data;
  
  Entity *e = text->settings();
  QVariant *price = e->get(QString("price"));
  if (! price)
    return 0;

  QVariant *date = e->get(QString("date"));
  if (! date)
    return 0;

  QVariant *string = e->get(QString("text"));
  if (! string)
    return 0;

  QDateTime dt = date->toDateTime();
  pd->info << "D=" + dt.toString("yyyy-MM-dd");
  pd->info << "T=" + dt.toString("HH:mm:ss");
  
  Strip strip;
  QString ts;
  strip.strip(price->toDouble(), 4, ts);
  pd->info << QString("Price=") + ts;
  
  pd->info << QString("Text=") + string->toString();
  
  return 1;
}

int
MarkerText::move (PluginData *pd)
{
  if (! pd->data)
    return 0;
  
  Marker *text = (Marker *) pd->data;
  
  switch (pd->status)
  {
    case PlotStatus::_MOVE:
    {
      Entity *e = text->settings();
      
      QVariant *price = e->get(QString("price"));
      if (! price)
        return 0;

      QVariant *date = e->get(QString("date"));
      if (! date)
        return 0;
      
      QwtScaleMap map = text->plot()->canvasMap(QwtPlot::xBottom);
      int x = map.invTransform((double) pd->point.x());

      if (! g_symbol)
        return 0;
      Bar *tbar = g_symbol->bar(x);
      if (! tbar)
        return 0;
      date->setValue(tbar->date());

      map = text->plot()->canvasMap(QwtPlot::yRight);
      price->setValue(map.invTransform((double) pd->point.y()));
      
      text->setModified(TRUE);

      text->plot()->replot();
      break;
    }
    default:
      break;
  }
  
  return 1;
}

int
MarkerText::highLow (PluginData *pd)
{
  if (! pd->data)
    return 0;
  
  Marker *text = (Marker *) pd->data;
  
  Entity *e = text->settings();
  QVariant *price = e->get(QString("price"));
  if (! price)
    return 0;

  pd->high = price->toDouble();
  pd->low = price->toDouble();

  return 1;
}

int
MarkerText::click (PluginData *pd)
{
  if (! pd->data)
    return 0;
  
  Marker *text = (Marker *) pd->data;
  
  switch ((PlotStatus::Key) pd->status)
  {
    case PlotStatus::_SELECTED:
    {
      switch (pd->button)
      {
        case Qt::LeftButton:
          if (text->isGrabSelected(pd->point))
          {
            pd->status = PlotStatus::_MOVE;
            return 1;
          }

          if (! text->isSelected(pd->point))
          {
            pd->status = PlotStatus::_NONE;
            text->setSelected(FALSE);

            Plot *tplot = (Plot *) text->plot();
            tplot->unselectMarker();

            text->plot()->replot();
            return 1;
          }
          break;
        case Qt::RightButton:
        {
          Plot *p = (Plot *) text->plot();
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
          if (text->isSelected(pd->point))
          {
            pd->status = PlotStatus::_SELECTED;
            text->setSelected(TRUE);

            Plot *tplot = (Plot *) text->plot();
            tplot->selectMarker(text->ID());

            text->plot()->replot();
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
MarkerText::create (PluginData *pd)
{
  if (! pd->data)
    return 0;
  
  Marker *text = (Marker *) pd->data;
  
  pd->status = PlotStatus::_MOVE;
  text->setSelected(TRUE);
  emit signalMessage(QObject::tr("Place Text marker"));
  return 1;
}

int
MarkerText::settings (PluginData *pd)
{
  Entity *e = new Entity;
  e->set(QString("plot"), new QVariant(QString()));
  e->set(QString("type"), new QVariant(QString("marker")));
  e->set(QString("symbol"), new QVariant(QString()));
  e->set(QString("plugin"), new QVariant(QString("MarkerText")));
  e->set(QString("date"), new QVariant(QDateTime::currentDateTime()));
  e->set(QString("price"), new QVariant(0.0));
  e->set(QString("color"), new QVariant(QString("white")));
  e->set(QString("font"), new QVariant(QFont()));
  e->set(QString("text"), new QVariant(QString("Text")));
  pd->settings = e;
  
  return 1;
}

// do not remove
Q_EXPORT_PLUGIN2(markertext, MarkerText);
