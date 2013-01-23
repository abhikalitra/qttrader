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

#ifndef PLOT_WIDGET_HPP
#define PLOT_WIDGET_HPP

#include <QWidget>
#include <QScrollBar>
#include <QSplitter>
#include <QHash>

#include "Plot.h"
#include "Widget.h"
#include "ControlWidget.h"
#include "DataBase.h"

// Plots everything
// Some sort of main class
class PlotWidget : public Widget
{
  Q_OBJECT
  
  signals:
    void signalClear ();
    void signalSetDates ();
    void signalDraw ();
    void signalBarLength (int);
    void signalIndex (int);

  public:
    PlotWidget ();
    ~PlotWidget ();
    void removePlot (QString);
    void setPanScrollBarSize ();
    void saveSettings ();
    void loadMarkers (DataBase &);
    void saveMarkers (DataBase &);
    void addPlotSettings (Entity *e);
    Entity * settings ();
    
  public slots:
    void addPlot (QString plugin, int row, QString name);
    void scrollBarChanged (int);
    void refresh ();
    void indicatorDialog ();
    void indicatorDialog2 (QString plugin, int row, QString name);
    void editIndicator ();
    void editIndicator2 (QString);
    void removeIndicator ();
    void removeIndicator2 (QString);
    void loadSettings ();
    void setBarLength ();
    void deleteMarkers (QStringList);
   
  private:
    /**
      * Loads symbol data if needed.
      * New symbol data is stored in PluginData
      *
      * @return true if new data was loaded
      */
    bool loadSymbolData();


    /**
      * Updates the scrollbars size for pan and zoom
      *
      */
    void updateScrollBars();

    QSplitter *_csplitter;
    QHash<QString, Plot *> _plots;
    QHash<QString, Entity *> _settings;
    ControlWidget *_controlWidget;
    QString _removeName;
    //TODO remove
    bool first;
};

#endif
