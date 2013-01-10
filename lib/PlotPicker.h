/*
 *  Qtstalker stock charter
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

#ifndef PLOT_PICKER_HPP
#define PLOT_PICKER_HPP

#include <qwt_plot_picker.h>
#include <qwt_plot.h>
#include <qwt_picker_machine.h>
#include <QMouseEvent>
#include <QColor>
#include <QPoint>

class PlotPicker : public QwtPlotPicker
{
  Q_OBJECT

  signals:
    void signalMouseMove (QPoint);
    void signalMouseClick (int, QPoint);
    void signalMouseDoubleClick (int, QPoint);
    void signalMouseRelease (int, QPoint);

  public:
    PlotPicker (QwtPlot *);
    void setCrossHairs (int);
    void setColor (QColor);

  protected:
    virtual void widgetMouseMoveEvent (QMouseEvent *);
    virtual void widgetMousePressEvent (QMouseEvent *);
    virtual void widgetMouseDoubleClickEvent (QMouseEvent *);
    virtual void widgetMouseReleaseEvent (QMouseEvent *);
    virtual QwtText trackerText (const QPoint &) const;
    virtual QwtText trackerText (const QwtDoublePoint &) const;
    virtual void widgetLeaveEvent (QEvent *);
    virtual QwtPickerMachine * stateMachine (int) const;

  private:
    int _crossHairs;
    QColor _color;
};

#endif


