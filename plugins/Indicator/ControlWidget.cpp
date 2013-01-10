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

#include "ControlWidget.h"
#include "PluginFactory.h"
#include "BarLength.h"
#include "DateRange.h"
#include "PluginData.h"

#include "../../pics/prev.xpm"
#include "../../pics/next.xpm"
#include "../../pics/search.xpm"
#include "../../pics/add.xpm"
#include "../../pics/configure.xpm"
#include "../../pics/delete.xpm"
#include "../../pics/edit.xpm"
#include "../../pics/group.xpm"

#include <QtDebug>
#include <QSettings>
#include <QLayout>
#include <QMessageBox>
#include <QAction>
#include <QMenu>
#include <QTimer>
#include <QInputDialog>

ControlWidget::ControlWidget ()
{
  _search = "%";
  createGUI();
  updateSymbols();
}

void ControlWidget::createGUI ()
{
  QSize bs(25, 25);
  
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(5);
  vbox->setMargin(5);

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setSpacing(0);
  hbox->setMargin(0);
  hbox->addLayout(vbox, 1);
  setLayout(hbox);

  // pan scrollbar
  _panScrollbar = new QScrollBar(Qt::Horizontal, 0);
  _panScrollbar->setRange(0, 0);
  _panScrollbar->setValue(0);
  connect(_panScrollbar, SIGNAL(valueChanged(int)), this, SIGNAL(signalScrollBarChanged(int)));
  vbox->addWidget(_panScrollbar);

  // zoom scrollbar
  _zoomScrollbar = new QScrollBar(Qt::Horizontal, 0);
  _zoomScrollbar->setRange(0, 0);
  _zoomScrollbar->setValue(0);
  connect(_zoomScrollbar, SIGNAL(valueChanged(int)), this, SIGNAL(signalRange(int)));
  connect(_zoomScrollbar, SIGNAL(valueChanged(int)), this, SLOT(rangeChanged(int)));
  vbox->addWidget(_zoomScrollbar);

  // bar length
  _lengthButton = new LengthButton;
  connect(_lengthButton, SIGNAL(signalLength()), this, SIGNAL(signalLength()));
  hbox->addWidget(_lengthButton);
  
  // symbol combo
  _list = new QComboBox;
  connect(_list, SIGNAL(currentIndexChanged(int)), this, SLOT(listSelected()));
  hbox->addWidget(_list);
  
  // create actions for options menu
  QMenu *menu = new QMenu(this);

  QAction *a = new QAction(QIcon(search_xpm), tr("Symbol &Search"), this);
  a->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
  a->setToolTip(tr("Symbol Search"));
  a->setStatusTip(tr("Symbol Search"));
  connect(a, SIGNAL(triggered()), this, SLOT(search()));
  menu->addAction(a);

  a = new QAction(QIcon(group_xpm), tr("&Group"), this);
  a->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_G));
  a->setToolTip(tr("Group"));
  a->setStatusTip(tr("Group"));
  connect(a, SIGNAL(triggered()), this, SLOT(group()));
  menu->addAction(a);
  
  menu->addSeparator();

  a = new QAction(QIcon(add_xpm), tr("&Add Indicator"), this);
  a->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_A));
  a->setToolTip(tr("Add Indicator"));
  a->setStatusTip(tr("Add Indicator"));
  connect(a, SIGNAL(triggered()), this, SIGNAL(signalIndicator()));
  menu->addAction(a);
  
  a = new QAction(QIcon(edit_xpm), tr("&Edit Indicator"), this);
  a->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
  a->setToolTip(tr("Edit Indicator"));
  a->setStatusTip(tr("Edit Indicator"));
  connect(a, SIGNAL(activated()), this, SIGNAL(signalEditPlot()));
  menu->addAction(a);

  a = new QAction(QIcon(delete_xpm), tr("&Remove Indicator"), this);
  a->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
  a->setToolTip(tr("Remove Indicator"));
  a->setStatusTip(tr("Remove Indicator"));
  connect(a, SIGNAL(activated()), this, SIGNAL(signalRemovePlot()));
  menu->addAction(a);
  
  // options button
  _optionButton = new QToolButton;
  _optionButton->setPopupMode(QToolButton::InstantPopup);
  _optionButton->setIcon(QIcon(configure_xpm));
  _optionButton->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
  _optionButton->setToolTip(tr("Options"));
  _optionButton->setStatusTip(tr("Options"));
  _optionButton->setMaximumSize(bs);
  _optionButton->setMenu(menu);
  hbox->addWidget(_optionButton);
  
  // prev button
  _prevButton = new QToolButton;
  _prevButton->setIcon(QIcon(prev_xpm));
  _prevButton->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));
  _prevButton->setToolTip(tr("Previous Symbol"));
  _prevButton->setStatusTip(tr("Previous Symbol"));
  _prevButton->setMaximumSize(bs);
  connect(_prevButton, SIGNAL(clicked()), this, SLOT(previousSymbol()));
  hbox->addWidget(_prevButton);
  
  // next button
  _nextButton = new QToolButton;
  _nextButton->setIcon(QIcon(next_xpm));
  _nextButton->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
  _nextButton->setToolTip(tr("Next Symbol"));
  _nextButton->setStatusTip(tr("Next Symbol"));
  _nextButton->setMaximumSize(bs);
  connect(_nextButton, SIGNAL(clicked()), this, SLOT(nextSymbol()));
  hbox->addWidget(_nextButton);
}

void ControlWidget::updateSymbols ()
{
  PluginFactory fac;
  Plugin *plug = fac.load(QString("DBSymbol"));
  if (! plug)
    return;

  _symbols.clear();
  
  PluginData pd;
  pd.command = QString("search");
  pd.search = _search;
  
  if (! plug->command(&pd))
    return;
  
  _symbols = pd.symbols;
  
  QStringList tl;
  for (int pos = 0; pos < _symbols.size(); pos++)
  {
    Bars sym = _symbols.at(pos);
    tl << sym.symbol();
  }

  _list->blockSignals(TRUE);
  _list->clear();
  _list->addItems(tl);
  _list->blockSignals(FALSE);
  
  buttonStatus();
}

void ControlWidget::previousSymbol ()
{
  _list->setCurrentIndex(_list->currentIndex() - 1);
  buttonStatus();
}

void ControlWidget::nextSymbol ()
{
  _list->setCurrentIndex(_list->currentIndex() + 1);
  buttonStatus();  
}

void ControlWidget::buttonStatus ()
{
  // update symbol navigation buttons
  if (_symbols.size() < 2)
  {
    _prevButton->setEnabled(FALSE);
    _nextButton->setEnabled(FALSE);
  }
  else
  {
    if (_list->currentIndex() == 0)
    {
      _prevButton->setEnabled(FALSE);
      _nextButton->setEnabled(TRUE);
    }
    else
    {
      if (_list->currentIndex() == _symbols.size() - 1)
      {
        _prevButton->setEnabled(TRUE);
        _nextButton->setEnabled(FALSE);
      }
      else
      {
        _prevButton->setEnabled(TRUE);
        _nextButton->setEnabled(TRUE);
      }
    }
  }
}

int ControlWidget::count ()
{
  return _list->count();
}

Bars ControlWidget::currentSymbol ()
{
  if (! _list->count() || _list->currentIndex() < 0)
    return Bars();
  
  return _symbols.at(_list->currentIndex());
}

void ControlWidget::search ()
{
  QStringList ml;
  ml << QString("QtTrader -") << tr("Symbol Search");
  
  QInputDialog *dialog = new QInputDialog(this);
  dialog->setWindowTitle(ml.join(" "));
  dialog->setLabelText(tr("Enter a partial search like %OOG% or % for all"));
  dialog->setTextValue(_search);
  connect(dialog, SIGNAL(textValueSelected(const QString &)), this, SLOT(search2(QString)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void ControlWidget::search2 (QString d)
{
  if (d.isEmpty())
    return;
  
  _search = d;
  updateSymbols();
}

int ControlWidget::length ()
{
  return _lengthButton->length();
}

QString ControlWidget::lengthText ()
{
  return _lengthButton->lengthText();
}

void ControlWidget::setLength (int d)
{
  _lengthButton->setLength(d);
}

int ControlWidget::range ()
{
  qDebug() << "range()";
  return mPlotRange;
}

int ControlWidget::scrollBarValue ()
{
  return _panScrollbar->value();
}

void ControlWidget::resizePanScrollBar (Plot *p)
{
    qDebug() << "ControlWidget->resizePanScrollBar";
  int page, max;
  p->panScrollBarSize(page, max);
  _panScrollbar->setRange(0, max);
  _panScrollbar->setPageStep(page);
  qDebug() << "Number of bars " << p->getNumberOfBars();

}

void ControlWidget::setPan (int min, int max, int page)
{
    qDebug() << "ControlWidget->setPan";
    qDebug() << min;
    qDebug() << max;
    qDebug() << page;
  _panScrollbar->setRange(min, max);
  _panScrollbar->setPageStep(page);
  _panScrollbar->setValue(max);
}

void ControlWidget::setZoom (int min, int max, int page){
    qDebug() << "ControlWidget->setZoom";
    qDebug() << min;
    qDebug() << max;
    qDebug() << page;
  _zoomScrollbar->setRange(min,max);
  _zoomScrollbar->setValue(max);
  _zoomScrollbar->setPageStep(page);
}

void ControlWidget::resizeZoom(int max){
    _zoomScrollbar->setRange(0,max);
}

void ControlWidget::listSelected ()
{
  emit signalSelected();
  buttonStatus();
}

void ControlWidget::group ()
{
  QStringList l;
  DataBase db(QString("groups"));
  db.names(l);

  QStringList ml;
  ml << "QtTrader" << "-" << tr("Select Group");
  
  QInputDialog *dialog = new QInputDialog(this);
  dialog->setWindowTitle(ml.join(" "));
  dialog->setLabelText(tr("Group:"));
  dialog->setComboBoxItems(l);
  dialog->setComboBoxEditable(FALSE);
  connect(dialog, SIGNAL(textValueSelected(const QString &)), this, SLOT(group2(QString)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void ControlWidget::group2 (QString name)
{
  Entity e;
  e.setName(name);
  e.set(QString("list"), new QVariant(QStringList()));
  
  DataBase db(QString("groups"));
  if (! db.get(&e))
  {
    qDebug() << "ControlWidget::group2: error loading group" << name;
    return;
  }

  QVariant *set = e.get(QString("list"));
  QStringList tl = set->toStringList();
  
  _symbols.clear();

  PluginFactory fac;
  Plugin *plug = fac.load(QString("DBSymbol"));
  if (! plug)
    return;
  
  for (int pos = 0; pos < tl.size(); pos++)
  {
    PluginData pd;
    pd.command = QString("search");
    pd.search = tl.at(pos);
    
    if (! plug->command(&pd))
      continue;
    
    if (! pd.symbols.size())
      continue;
    
    _symbols << pd.symbols.at(0);
  }

  _list->blockSignals(TRUE);
  _list->clear();
  _list->addItems(set->toStringList());
  _list->blockSignals(FALSE);
  
  buttonStatus();
}

int ControlWidget::saveSettings (DataBase *db)
{
  Entity *e = settings();
  int rc = db->set(e);
  delete e;
  return rc;
}

int ControlWidget::loadSettings (DataBase *db)
{
    qDebug() << "ControlWidget::loadSettings";
  
  Entity *e = settings();
  
  if (! db->get(e))
  {
    delete e;
    return 0;
  }
  
  QVariant *tset = e->get(QString("panning"));
  if (tset){
    qDebug() << "panningScrollbar: " << tset->toInt();
    _panScrollbar->setValue(tset->toInt());
  }
  tset = e->get(QString("range"));
  if (tset){
      qDebug() << "range: " << tset->toInt();
    _zoomScrollbar->setValue(tset->toInt());
  }
  tset = e->get(QString("length"));
  if (tset)
    _lengthButton->setLength(tset->toInt());
  
  tset = e->get(QString("search"));
  if (tset)
    _search = tset->toString();
  
  delete e;
  
  return 1;
}

Entity* ControlWidget::settings ()
{
  qDebug() << "ControlWidget::settings";
  Entity *e = new Entity;
  e->setName(QString("controlWidget"));

  e->set(QString("panning"), new QVariant(_panScrollbar->value()));
  qDebug() << "panningScrollbar: " << _panScrollbar->value();

  e->set(QString("range"), new QVariant(_zoomScrollbar->value()));
  qDebug() << "rangeScrollbar: " << _zoomScrollbar->value();

  e->set(QString("length"), new QVariant(_lengthButton->length()));
  e->set(QString("search"), new QVariant(_search));
  return e;
}

void ControlWidget::rangeChanged(int i){
  qDebug() << "ControlWidget::rangeChanged " << i;
  mPlotRange = i;
  emit signalRefresh();
}

int ControlWidget::getRange(){
    qDebug() << "ControlWidget::getRange() : " << mPlotRange;
    qDebug() << "_zoomScrollbar->value() : " << _zoomScrollbar->value();

  return _zoomScrollbar->value();
}
