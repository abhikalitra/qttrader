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


#include "GUIWidget.h"
#include "DateRange.h"
#include "YahooHistoryDownload.h"
#include "Global.h"

#include <QtGui>
#include <QSettings>


GUIWidget::GUIWidget ()
{
  createGUI();
  loadSettings();
}

GUIWidget::~GUIWidget ()
{
  saveSettings();
}

void
GUIWidget::createGUI ()
{
  _timer = new QTimer(this);
  connect(_timer, SIGNAL(timeout()), this, SLOT(updateGUI()));

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  vbox->setMargin(5);
  setLayout(vbox);
  
  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(0);
  vbox->addLayout(form);
  
  // templates
  _templates = new QComboBox;
  form->addRow (tr("Templates"), _templates);
  
  // range
  DateRange dr;
  _range = new QComboBox;
  _range->addItems(dr.list());
  _range->setCurrentIndex(5);
  form->addRow (tr("Range"), _range);
  
  // symbol file
  _symbolButton = new FileButton(0);
  connect(_symbolButton, SIGNAL(signalSelectionChanged(QStringList)), this, SLOT(buttonStatus()));
  _symbolButton->setFiles(QStringList() << "/tmp/yahoo_symbols");
  form->addRow (tr("Symbol File"), _symbolButton);
  
  // log
  QGroupBox *gbox = new QGroupBox;
  gbox->setTitle(tr("Log"));
  vbox->addWidget(gbox);

  QVBoxLayout *tvbox = new QVBoxLayout;
  gbox->setLayout(tvbox);
  
  _log = new QTextEdit;
  _log->setReadOnly(TRUE);
  tvbox->addWidget(_log);

  // buttonbox
  QDialogButtonBox *bb = new QDialogButtonBox(QDialogButtonBox::Help);
  vbox->addWidget(bb);

  // ok button
  _okButton = bb->addButton(QDialogButtonBox::Ok);
  _okButton->setText(tr("&OK"));
  connect(_okButton, SIGNAL(clicked()), this, SLOT(downloadHistory()));

  // cancel button
  _cancelButton = bb->addButton(QDialogButtonBox::Cancel);
  _cancelButton->setText(tr("&Cancel"));
  _cancelButton->setDefault(TRUE);
  _cancelButton->setFocus();
  _cancelButton->setEnabled(FALSE);

  // help button
  QPushButton *b = bb->button(QDialogButtonBox::Help);
  b->setText(tr("&Help"));
  connect(b, SIGNAL(clicked()), this, SIGNAL(signalHelp()));
}

void
GUIWidget::downloadHistory ()
{
  _log->clear();
  setEnabled(FALSE);
  _okButton->setEnabled(FALSE);
  _cancelButton->setEnabled(TRUE);
  
  DateRange dr;
  QDateTime ed = QDateTime::currentDateTime();
  QDateTime sd = dr.interval(ed, _range->currentIndex());
  
  _timer->start(100);
  
  YahooHistoryDownload function(this);
  connect(&function, SIGNAL(signalMessage(QString)), _log, SLOT(append(const QString &)));
  connect(_cancelButton, SIGNAL(clicked()), &function, SLOT(stop()));
  function.download(_symbolButton->files(), sd, ed);
  
  _timer->stop();

  setEnabled(TRUE);
  _okButton->setEnabled(TRUE);
  _cancelButton->setEnabled(FALSE);
}

void
GUIWidget::buttonStatus ()
{
  int count = 0;
  
  if (_symbolButton->fileCount())
    count++;
  
  switch (count)
  {
    case 1:
      _okButton->setEnabled(TRUE);
      break;
    default:
      _okButton->setEnabled(FALSE);
      break;
  }
}

void
GUIWidget::updateGUI ()
{
  QCoreApplication::processEvents();
}

void
GUIWidget::loadSettings ()
{
  QSettings settings(g_settings);
  settings.beginGroup(g_session);
  _range->setCurrentIndex(settings.value(QString("dateRange"), DateRange::_YEAR).toInt());
  _symbolButton->setFiles(settings.value(QString("symbolFiles"), QStringList() << "/tmp/yahoo_symbols").toStringList());
}

void
GUIWidget::saveSettings ()
{
  QSettings settings(g_settings);
  settings.beginGroup(g_session);
  settings.setValue(QString("dateRange"), _range->currentIndex());
  settings.setValue(QString("symbolFiles"), _symbolButton->files());
}
