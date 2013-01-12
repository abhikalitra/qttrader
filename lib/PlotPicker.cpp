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

#include "PlotPicker.h"
#include "PickerMachine.h"

#include <QString>
#include <QDebug>

PlotPicker::PlotPicker (QwtPlot *p) : QwtPlotPicker (QwtPlot::xBottom, QwtPlot::yLeft,
                                                     QwtPicker::PointSelection | QwtPicker::DragSelection,
                                                     QwtPlotPicker::RectRubberBand,
                                                     QwtPicker::AlwaysOn,
                                                     p->canvas())
{
  _crossHairs = 0;
  _color = QColor(Qt::white);
  p->canvas()->setMouseTracking(true);

  setCrossHairs(_crossHairs);
}

void PlotPicker::setCrossHairs (int d)
{
  if (d)
  {
    setRubberBand(QwtPlotPicker::CrossRubberBand);
    setRubberBandPen(_color);
    setRubberBand(QwtPicker::CrossRubberBand);
  }
  else
  {
    setRubberBandPen(_color);
    setRubberBand(QwtPicker::RectRubberBand);
    setTrackerPen(_color);
  }
}

void PlotPicker::setColor (QColor d)
{
  _color = d;
  setRubberBandPen(_color);
  setTrackerPen(_color);
}

QwtText PlotPicker::trackerText (const QPoint &) const
{
  return QString();
}

QwtText PlotPicker::trackerText (const QwtDoublePoint &) const
{
  return QString();
}

void PlotPicker::widgetMouseMoveEvent (QMouseEvent *event)
{
  if (! isActive())
  {
    setSelectionFlags(QwtPicker::PointSelection);
    begin();
    append(event->pos());
  }

  QwtPicker::widgetMouseMoveEvent(event);

  emit signalMouseMove(event->pos());
}

void PlotPicker::widgetMousePressEvent (QMouseEvent *event)
{
  QwtPicker::widgetMousePressEvent(event);

  emit signalMouseClick(event->button(), event->pos());
}

void PlotPicker::widgetMouseReleaseEvent (QMouseEvent *event)
{
  QwtPicker::widgetMouseReleaseEvent(event);

  emit signalMouseRelease(event->button(), event->pos());
}

void PlotPicker::widgetMouseDoubleClickEvent (QMouseEvent *event)
{
  QwtPicker::widgetMousePressEvent(event);

  emit signalMouseDoubleClick(event->button(), event->pos());
}

void PlotPicker::widgetLeaveEvent(QEvent *)
{
  end();
}

QwtPickerMachine *PlotPicker::stateMachine (int) const
{
  return new PickerMachine;
}

