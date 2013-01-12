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


#ifndef PLUGIN_CSV_GUI_WIDGET_H
#define PLUGIN_CSV_GUI_WIDGET_H

#include <QAction>
#include <QStringList>
#include <QTextEdit>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>

#include "Widget.h"
#include "FileButton.h"

class GUIWidget : public Widget
{
  Q_OBJECT

  signals:
    void signalMessage (QString);
    
  public:
    GUIWidget ();
    ~GUIWidget ();
    void createGUI ();
    void loadSettings ();
    void saveSettings ();
   
  public slots:
    void importStart ();
    void importThread ();
    void importDone ();
    void buttonStatus ();
   
  private:
    FileButton *_csvButton;
    QPushButton *_okButton;
    QPushButton *_cancelButton;
    QLineEdit *_format;
    QLineEdit *_dateFormat;
    QComboBox *_delimiter;
    QComboBox *_type;
    QLineEdit *_exchange;
    QCheckBox *_filename;
    QAction *_startAction;
    QTextEdit *_log;
    QTimer *_timer;
};

#endif
