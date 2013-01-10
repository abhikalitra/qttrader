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


#include "GUIWidget.h"
#include "Global.h"
#include "DataBase.h"
#include "PluginFactory.h"

#include "../pics/add.xpm"
#include "../pics/delete.xpm"
#include "../pics/select_all.xpm"
#include "../pics/unselect_all.xpm"
#include "../pics/new.xpm"
#include "../pics/edit.xpm"
#include "../pics/save.xpm"
#include "../pics/find.xpm"

#include <QtDebug>
#include <QtGui>
#include <QSettings>


GUIWidget::GUIWidget ()
{
  _modified = FALSE;
  
  _group.set(QString("list"), new QVariant(QStringList()));

  createActions();
  
  createGUI();

  loadSettings();
}

GUIWidget::~GUIWidget ()
{
  saveSettings();
}

void
GUIWidget::createActions ()
{
  QAction *a = new QAction(QIcon(new_xpm), tr("New Group"), this);
  a->setToolTip(tr("New Group"));
  a->setStatusTip(tr("New Group"));
  connect(a, SIGNAL(triggered()), this, SLOT(newGroup()));
  _actions.insert(_CREATE_GROUP, a);
  
  a = new QAction(QIcon(edit_xpm), tr("Edit Group"), this);
  a->setToolTip(tr("Edit Group"));
  a->setStatusTip(tr("Edit Group"));
  connect(a, SIGNAL(triggered()), this, SLOT(editGroup()));
  _actions.insert(_EDIT_GROUP, a);
  
  a = new QAction(QIcon(delete_xpm), tr("Delete Group"), this);
  a->setToolTip(tr("Delete Group"));
  a->setStatusTip(tr("Delete Group"));
  connect(a, SIGNAL(triggered()), this, SLOT(deleteGroup()));
  _actions.insert(_DELETE_GROUP, a);
  
  a = new QAction(QIcon(save_xpm), tr("Save Group"), this);
  a->setToolTip(tr("Save Group"));
  a->setStatusTip(tr("Save Group"));
  connect(a, SIGNAL(triggered()), this, SLOT(saveGroup()));
  a->setEnabled(FALSE);
  _actions.insert(_SAVE, a);
  
  a = new QAction(QIcon(add_xpm), tr("Add Selected"), this);
  a->setToolTip(tr("Add Selected"));
  a->setStatusTip(tr("Add Selected"));
  connect(a, SIGNAL(triggered()), this, SLOT(addButtonPressed()));
  a->setEnabled(FALSE);
  _actions.insert(_ADD_SELECTED, a);

  a = new QAction(QIcon(delete_xpm), tr("Remove Selected"), this);
  a->setToolTip(tr("Remove Selected"));
  a->setStatusTip(tr("Remove Selected"));
  connect(a, SIGNAL(triggered()), this, SLOT(deleteButtonPressed()));
  a->setEnabled(FALSE);
  _actions.insert(_DELETE_SELECTED, a);

  a = new QAction(QIcon(select_all_xpm), tr("Select All"), this);
  a->setToolTip(tr("Select All"));
  a->setStatusTip(tr("Select All"));
  a->setEnabled(FALSE);
  _actions.insert(_SELECT_ALL, a);
  
  a = new QAction(QIcon(unselect_all_xpm), tr("Unselect All"), this);
  a->setToolTip(tr("Unselect All"));
  a->setStatusTip(tr("Unselect All"));
  a->setEnabled(FALSE);
  _actions.insert(_UNSELECT_ALL, a);
  
  a = new QAction(QIcon(find_xpm), tr("Perform Search"), this);
  a->setToolTip(tr("Perform Search"));
  a->setStatusTip(tr("Perform Search"));
  connect(a, SIGNAL(triggered()), this, SLOT(search()));
  a->setEnabled(FALSE);
  _actions.insert(_SEARCH, a);
}

void
GUIWidget::createGUI ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  vbox->setMargin(5);
  setLayout(vbox);
  
  // search area
  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setSpacing(2);
  hbox->setMargin(0);
  vbox->addLayout(hbox);
  
  QLabel *label = new QLabel(tr("Search"));
  hbox->addWidget(label);
  
  _search = new QLineEdit;
  _search->setText("%");
  hbox->addWidget(_search);
  
  // results area
  hbox = new QHBoxLayout;
  hbox->setSpacing(0);
  hbox->setMargin(0);
  vbox->addLayout(hbox);
  
  QGroupBox *gbox = new QGroupBox;
  gbox->setTitle(tr("Search Results"));
  hbox->addWidget(gbox);
  
  QVBoxLayout *tvbox = new QVBoxLayout;
  tvbox->setSpacing(0);
  tvbox->setMargin(0);
  gbox->setLayout(tvbox);

  _results = new QListWidget;
  _results->setSortingEnabled(TRUE);
  _results->setSelectionMode(QAbstractItemView::ExtendedSelection);
  connect(_results, SIGNAL(itemSelectionChanged()), this, SLOT(buttonStatus()));
  tvbox->addWidget(_results);
  
  // middle toolbar
  
  tvbox = new QVBoxLayout;
  tvbox->setSpacing(0);
  tvbox->setMargin(0);
  hbox->addLayout(tvbox);
  tvbox->addStretch(1);

  QSize bsize(16, 16);
  
  QToolBar *tb = new QToolBar;
  tb->setOrientation(Qt::Vertical);
  tb->setIconSize(bsize);
  tvbox->addWidget(tb);
  
  tb->addAction(_actions.value(_SEARCH));
  tb->addAction(_actions.value(_ADD_SELECTED));
  tb->addAction(_actions.value(_DELETE_SELECTED));
  tb->addSeparator();
  tb->addAction(_actions.value(_SAVE));
  tb->addAction(_actions.value(_CREATE_GROUP));
  tb->addAction(_actions.value(_EDIT_GROUP));
  tb->addAction(_actions.value(_DELETE_GROUP));
  
  tvbox->addStretch(1);
  
  // group list area  
  
  gbox = new QGroupBox;
  gbox->setTitle(tr("Group Contents"));
  hbox->addWidget(gbox);

  tvbox = new QVBoxLayout;
  tvbox->setSpacing(0);
  tvbox->setMargin(0);
  gbox->setLayout(tvbox);

  _list = new QListWidget;
  _list->setSortingEnabled(TRUE);
  _list->setSelectionMode(QAbstractItemView::ExtendedSelection);
  connect(_list, SIGNAL(itemSelectionChanged()), this, SLOT(buttonStatus()));
  tvbox->addWidget(_list);

  // defer until object created
  connect(_actions.value(_SELECT_ALL), SIGNAL(triggered()), _list, SLOT(selectAll()));
  connect(_actions.value(_UNSELECT_ALL), SIGNAL(triggered()), _list, SLOT(clearSelection()));
}

void
GUIWidget::saveGroup ()
{
  QStringList l;
  int loop = 0;
  for (; loop < _list->count(); loop++)
    l << _list->item(loop)->text();
  
  QVariant *set = _group.get(QString("list"));
  set->setValue(l);

  DataBase db(QString("groups"));
  db.transaction();
  if (! db.set(&_group))
  {
    qDebug() << "DialogGroupEdit::done: GroupDataBase error";
    return;
  }
  db.commit();

  _modified = FALSE;
  
  buttonStatus();
  
  emit signalMessage(tr("Group saved"));
}

void
GUIWidget::addButtonPressed ()
{
  QList<QListWidgetItem *> sl = _results->selectedItems();
  
  for (int pos = 0; pos < sl.size(); pos++)
    _list->addItem(sl.at(pos)->text());

  _modified = TRUE;
  
  buttonStatus();
}

void
GUIWidget::deleteButtonPressed ()
{
  QList<QListWidgetItem *> l = _list->selectedItems();

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    QListWidgetItem *item = l.at(loop);
    delete item;
  }

  _modified = TRUE;
  
  buttonStatus();
}

void
GUIWidget::loadGroup (QString name)
{
  if (name.isEmpty())
    return;
  
  _group.setName(name);

  DataBase db(QString("groups"));
  if (! db.get(&_group))
  {
    qDebug() << "DialogGroupEdit::loadGroup: database error";
    return;
  }

  _list->clear();
  
  QVariant *set = _group.get(QString("list"));
  _list->addItems(set->toStringList());

  _modified = FALSE;
  
  buttonStatus();
  
  QStringList tl;
  tl << "QtTrader" << "-" << tr("Edit Group") << _group.name();
  emit signalTitle(tl.join(" "));
}

void
GUIWidget::loadSettings ()
{
/*  
  QSettings settings(g_settings);

  settings.beginGroup(QString("settings"));
  settings.endGroup();  
*/
}

void
GUIWidget::saveSettings ()
{
/*  
  QSettings settings(g_settings);

  settings.beginGroup(QString("settings"));
  settings.endGroup();  
  
  settings.sync();
*/
}

void
GUIWidget::newGroup ()
{
  saveDialog();
  
  bool ok;
  QString name = QInputDialog::getText(this,
                                       tr("New Group Name"),
                                       tr("Group name"),
                                       QLineEdit::Normal,
                                       tr("New Group"),
                                       &ok);
  
  if (! ok || name.isEmpty())
    return;

  QStringList l;
  DataBase db(QString("groups"));
  db.names(l);

  if (l.indexOf(name) != -1)
  {
    QMessageBox::StandardButton rc = QMessageBox::question(this,
                                                           tr("Group Already Exists"),
                                                           tr("Do you want to overwrite existing group?"),
                                                           QMessageBox::Yes | QMessageBox::No,
                                                           QMessageBox::No);
    
    if (rc == QMessageBox::No)
      return;

    db.transaction();
    db.remove(QStringList() << name);
    db.commit();
  }
  
  _group.setName(name);
  
  QVariant *set = _group.get(QString("list"));
  set->setValue(QStringList());
  
  _list->clear();

  _modified = FALSE;
  
  buttonStatus();
  
  QStringList tl;
  tl << "QtTrader" << "-" << tr("Edit") << _group.name();
  emit signalTitle(tl.join(" "));
}

void
GUIWidget::editGroup ()
{
  saveDialog();

  QStringList l;
  DataBase db(QString("groups"));
  db.names(l);

  bool ok;
  QString name = QInputDialog::getItem(this,
                                       tr("Edit Group"),
                                       tr("Group name"),
                                       l,
                                       0,
                                       FALSE,
                                       &ok,
                                       0);
  
  if (! ok || name.isEmpty())
    return;

  _group.setName(name);
  
  loadGroup(name);

  _modified = FALSE;
  
  buttonStatus();
  
  QStringList tl;
  tl << "QtTrader" << "-" << tr("Edit") << _group.name();
  emit signalTitle(tl.join(" "));
}

void
GUIWidget::deleteGroup ()
{
  QStringList l;
  DataBase db(QString("groups"));
  db.names(l);

  bool ok;
  QString name = QInputDialog::getItem(this,
                                       tr("Delete Group"),
                                       tr("Group name"),
                                       l,
                                       0,
                                       FALSE,
                                       &ok,
                                       0);
  
  if (! ok || name.isEmpty())
    return;

  db.transaction();
  db.remove(QStringList() << name);
  db.commit();
  
  emit signalMessage(tr("Group deleted"));
}

void
GUIWidget::search ()
{
  PluginFactory fac;
  Plugin *plug = fac.load(QString("DBSymbol"));
  if (! plug)
    return;

  PluginData pd;
  pd.command = QString("search");
  pd.search = _search->text();
  if (! plug->command(&pd))
    return;
  
  QStringList tl;
  for (int pos = 0; pos < pd.symbols.size(); pos++)
  {
    Bars sym = pd.symbols.at(pos);
    tl << sym.symbol();
  }

  _results->clear();
  _results->addItems(tl);
  
  buttonStatus();
}

void
GUIWidget::saveDialog ()
{
  if (! _modified)
    return;
  
  QMessageBox::StandardButton rc = QMessageBox::question(this,
                                                         tr("Group Modified"),
                                                         tr("Do you want to save changes?"),
                                                         QMessageBox::Yes | QMessageBox::No,
                                                         QMessageBox::Yes);
    
  if (rc == QMessageBox::No)
    return;

  saveGroup();
}

void
GUIWidget::buttonStatus ()
{
  // search button
  if (_group.name().isEmpty())
    _actions.value(_SEARCH)->setEnabled(FALSE);
  else
    _actions.value(_SEARCH)->setEnabled(TRUE);
  
  // save button
  if (_modified)
    _actions.value(_SAVE)->setEnabled(TRUE);
  else
    _actions.value(_SAVE)->setEnabled(FALSE);

  // delete selected button
  QList<QListWidgetItem *> sl = _list->selectedItems();
  if (sl.count())
    _actions.value(_DELETE_SELECTED)->setEnabled(TRUE);
  else
    _actions.value(_DELETE_SELECTED)->setEnabled(FALSE);

  // add selected button
  sl = _results->selectedItems();
  if (sl.count())
    _actions.value(_ADD_SELECTED)->setEnabled(TRUE);
  else
    _actions.value(_ADD_SELECTED)->setEnabled(FALSE);
}
