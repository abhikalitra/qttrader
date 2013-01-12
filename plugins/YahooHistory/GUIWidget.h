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


#ifndef PLUGIN_YAHOO_HISTORY_GUI_WIDGET_H
#define PLUGIN_YAHOO_HISTORY_GUI_WIDGET_H

#include <QStringList>
#include <QPushButton>
#include <QTextEdit>
#include <QComboBox>
#include <QTimer>

#include "Widget.h"
#include "FileButton.h"

class GUIWidget : public Widget
{
  Q_OBJECT

  public:
    GUIWidget ();
    ~GUIWidget ();
    void createGUI ();
    void loadSettings ();
    void saveSettings ();

  public slots:
    void downloadHistory ();
    void buttonStatus ();
    void updateGUI ();
   
  private:
    QPushButton *_okButton;
    QPushButton *_cancelButton;
    QComboBox *_templates;
    QComboBox *_range;
    FileButton *_symbolButton;
    QTextEdit *_log;
    QTimer *_timer;
};

#endif
