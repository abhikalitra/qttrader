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

#ifndef PLOT_HPP
#define PLOT_HPP

#include <QString>
#include <QList>
#include <QDateTime>
#include <QColor>
#include <QFont>
#include <QPoint>
#include <QHash>
#include <QMenu>
#include <QAction>
#include <qwt_plot_grid.h>
#include <qwt_plot.h>

#include "Entity.h"
#include "Curve.h"
#include "PlotDateScaleDraw.h"
#include "PlotScaleDraw.h"
#include "PlotPicker.h"
#include "PlotInfo.h"
#include "PlotSettings.h"
#include "Bars.h"

class Plot : public QwtPlot
{
  Q_OBJECT

  signals:
    void signalMessage (QString);
    void signalInfoMessage (Entity *);
    void signalIndex (int);
    void signalResizePanScrollBar (Plot *);
    void signalInfo (QStringList &);
    void signalDeleteMarkers (QStringList);

  public:
    Plot (QString, QWidget *);
    ~Plot ();
    void setCurve (Curve *);
    void setHighLow ();
    int index ();
    QMenu * menu ();
    void setYPoints ();
    void setMarker (Marker *);
    void unselectMarker ();
    void selectMarker (QString);
    void showMarkerMenu ();
    void setName (QString);
    QString name ();
    void panScrollBarSize (int &page, int &max);
    void createMenu ();
    bool date ();
    bool grid ();
    bool info ();
    QHash<QString, Marker *> markers ();
    int getNumberOfBars();
//TODO fix getter setter etc
    int high;
    int low;

  public slots:
    virtual void clear ();
    void setDates ();
    void updatePlot ();
    void setStartIndex (int index);
    void setBackgroundColor (QColor);
    void setFont (QFont);
    void setGridColor (QColor);
    void setGrid (bool);
    void setInfo (bool);
    void showDate (bool);
    void setLogScaling (bool);
    void showContextMenu ();
    void mouseMove (QPoint);
    void mouseClick (int, QPoint);
    void mouseDoubleClick (int, QPoint);
    void newMarker (QString);
    void setCrossHairs (bool);
    void setCrossHairsColor (QColor);
    void setBarSpacing (int);//void
    //Plot::calculatePlot(){

    //}
    void setBarLength (int);
    void markerDialog ();
    void markerDialog2 ();
    void deleteMarker ();
    void deleteMarker2 ();
    void markerMenuSelected (QAction *);
    void deleteAllMarkersDialog ();
    void deleteAllMarkers ();
    void setPage(int);
  protected:
    void resizeEvent (QResizeEvent *event);    

  private:
    int getStartPosition();
    PlotSettings _plotSettings;
    PlotDateScaleDraw *_dateScaleDraw;
    PlotScaleDraw *_plotScaleDraw;
    QwtPlotGrid *_grid;
    PlotPicker *_picker;
    QMenu *_menu;
    QMenu *_markerMenu;
    PlotInfo *_plotInfo;
    QAction *_dateAction;
    QAction *_gridAction;
    QAction *_infoAction;
    int mpage;
    bool destroying;
};

#endif
