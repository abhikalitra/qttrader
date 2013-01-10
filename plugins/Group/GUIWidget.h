/*
 *  Qtstalker stock charter
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


#ifndef PLUGIN_GROUP_GUI_WIDGET_H
#define PLUGIN_GROUP_HISTORY_GUI_WIDGET_H

#include <QStringList>
#include <QToolButton>
#include <QListWidget>

#include "Widget.h"
#include "Entity.h"

class GUIWidget : public Widget
{
  Q_OBJECT

  public:
    enum Actions
    {
      _CREATE_GROUP,
      _SEARCH,
      _EDIT_GROUP,
      _DELETE_GROUP,
      _SAVE,
      _DELETE_SELECTED,
      _ADD_SELECTED,
      _SELECT_ALL,
      _UNSELECT_ALL
    };
    
    GUIWidget ();
    ~GUIWidget ();
    void createActions ();
    void createGUI ();
    void loadSettings ();
    void saveSettings ();
    void loadGroup (QString);
    void saveDialog ();

  public slots:
    void newGroup ();
    void editGroup ();
    void deleteGroup ();
    void saveGroup ();
    void addButtonPressed ();
    void deleteButtonPressed ();
    void search ();
    void buttonStatus ();
   
  private:
    QLineEdit *_search;
    QListWidget *_results;
    QListWidget *_list;
    QHash<int, QAction *> _actions;
    Entity _group;
    bool _modified;
};

#endif
