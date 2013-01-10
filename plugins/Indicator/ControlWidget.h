/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
 *  Copyright (C) 2012 - Mattias Johansson
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

#ifndef PLUGIN_CONTROL_WIDGET_HPP
#define PLUGIN_CONTROL_WIDGET_HPP

#include <QToolButton>
#include <QSplitter>
#include <QListWidget>
#include <QTextEdit>
#include <QScrollBar>
#include <QSettings>
#include <QComboBox>

#include "Bars.h"
#include "LengthButton.h"
#include "Plot.h"
#include "DataBase.h"

class ControlWidget : public QWidget
{
  Q_OBJECT
  
  signals:
    void signalSelected ();
    void signalIndicator ();
    void signalLength ();

    void signalRemovePlot ();
    void signalEditPlot ();

    //Should be replaced by timeFrameUpdated()
    void signalRefresh ();
    void signalRange (int);
    void signalScrollBarChanged (int);

    //emitted when panning and/or zoom have changed
    void timeFrameUpdated();

  public:
    ControlWidget ();
    void createGUI ();
    int count ();
    Bars currentSymbol ();
    int length ();
    QString lengthText ();
    int range ();
    void setLength (int);
    int scrollBarValue ();
    int saveSettings (DataBase *);
    int loadSettings (DataBase *);
    Entity * settings ();
    int getRange();

//  TODO: make -> private:
    void setZoom (int min, int max, int page);
    void setPan (int min, int max, int page);

  public slots:
    void updateSymbols ();
    void previousSymbol ();
    void nextSymbol ();
    void search ();
    void search2 (QString);
    void buttonStatus ();
    void resizePanScrollBar (Plot *);
    void resizeZoom(int max);
    void listSelected ();
    void group ();
    void group2 (QString);
    void rangeChanged(int);

  protected:
    LengthButton *_lengthButton;
    QToolButton *_prevButton;
    QToolButton *_nextButton;
    QToolButton *_optionButton;
    QList<Bars> _symbols;
    QScrollBar *_panScrollbar;
    QScrollBar *_zoomScrollbar;
    QString _search;    
    QComboBox *_list;
    int mPlotRange;
};

#endif
