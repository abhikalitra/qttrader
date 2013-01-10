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
#include "CSVThread.h"
#include "CSVFunction.h"
#include "Delimiter.h"
#include "Quote.h"
#include "Global.h"

//#include "../../pics/search.xpm"

#include <QDebug>
#include <QtGui>


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
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  vbox->setMargin(5);
  setLayout(vbox);
  
  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(0);
  vbox->addLayout(form);
  
  // csv file
  _csvButton = new FileButton(0);
  connect(_csvButton, SIGNAL(signalSelectionChanged(QStringList)), this, SLOT(buttonStatus()));
//  _csvButton->setFiles(QStringList() << "/tmp/yahoo.csv");
  form->addRow (tr("CSV File"), _csvButton);

  // format
  _format = new QLineEdit;
  form->addRow (tr("Format"), _format);
  
  // date format
  _dateFormat = new QLineEdit;
  form->addRow (tr("Date Format"), _dateFormat);
  
  // delimiter
  Delimiter d;
  _delimiter = new QComboBox;
  _delimiter->addItems(d.list());
  _delimiter->setCurrentIndex(Delimiter::_SEMICOLON);
  form->addRow (tr("Delimiter"), _delimiter);
  
  // type
  Quote q;
  _type = new QComboBox;
  _type->addItems(q.list());
  _type->setCurrentIndex(Quote::_STOCK);
  form->addRow (tr("Quote Type"), _type);
  
  // exchange
  _exchange = new QLineEdit;
  form->addRow (tr("Exchange Override"), _exchange);
  
  // filename as symbol
  _filename = new QCheckBox;
  form->addRow (tr("Use Filename As Symbol"), _filename);

  // log
  _log = new QTextEdit;
  _log->setReadOnly(TRUE);
  vbox->addWidget(_log);

  // buttonbox
  QDialogButtonBox *bb = new QDialogButtonBox(QDialogButtonBox::Help);
  vbox->addWidget(bb);

  // ok button
  _okButton = bb->addButton(QDialogButtonBox::Ok);
  _okButton->setText(tr("&OK"));
  connect(_okButton, SIGNAL(clicked()), this, SLOT(importThread()));

  // cancel button
  _cancelButton = bb->addButton(QDialogButtonBox::Cancel);
  _cancelButton->setText(tr("&Cancel"));
  _cancelButton->setDefault(TRUE);
  _cancelButton->setFocus();
  _cancelButton->setEnabled(TRUE);

  // help button
  QPushButton *b = bb->button(QDialogButtonBox::Help);
  b->setText(tr("&Help"));
  connect(b, SIGNAL(clicked()), this, SIGNAL(signalHelp()));
}

void
GUIWidget::importStart ()
{
  _log->clear();
  setEnabled(FALSE);
  
  QStringList mess;
  mess << tr("Import started") << QDateTime::currentDateTime().toString(Qt::ISODate);
  _log->append(mess.join(" "));
}

void
GUIWidget::importDone ()
{
  setEnabled(TRUE);
  
  QStringList mess;
  mess << tr("Import completed") << QDateTime::currentDateTime().toString(Qt::ISODate);
  _log->append(mess.join(" "));
}

void
GUIWidget::importThread ()
{
  importStart();
  
  CSVFunction f(this);
  connect(&f, SIGNAL(signalMessage(QString)), _log, SLOT(append(const QString &)));
  
  int rc = f.import(_csvButton->files(),
                    _format->text(),
                    _dateFormat->text(),
                    _delimiter->currentText(),
                    _type->currentText(),
                    _exchange->text(),
                    _filename->isChecked());
  importDone();
}

void
GUIWidget::buttonStatus ()
{
  int count = 0;
  
  if (_csvButton->fileCount())
    count++;
  
  if (! _format->text().isEmpty())
    count++;
  
  if (! _dateFormat->text().isEmpty())
    count++;
  
  switch (count)
  {
    case 3:
      _okButton->setEnabled(TRUE);
      break;
    default:
      _okButton->setEnabled(FALSE);
      break;
  }
}

void
GUIWidget::loadSettings ()
{
  QSettings settings(g_settings);
  settings.beginGroup(g_session);
  
  QStringList tl = settings.value("files", QStringList() << "/tmp/yahoo.csv").toStringList();
  _csvButton->setFiles(tl);
  
  _format->setText(settings.value("format", "X,S,N,D,O,H,L,C,V").toString());
  _dateFormat->setText(settings.value("dateFormat", "yyyy-MM-dd").toString());
  _delimiter->setCurrentIndex(settings.value("delimiter", 1).toInt());
  _type->setCurrentIndex(settings.value("type", 0).toInt());
  _exchange->setText(settings.value("exchange").toString());
  _filename->setChecked(settings.value("useFilename").toBool());
}

void
GUIWidget::saveSettings ()
{
  QSettings settings(g_settings);
  settings.beginGroup(g_session);
  
  settings.setValue("files", _csvButton->files());
  settings.setValue("format", _format->text());
  settings.setValue("dateFormat", _dateFormat->text());
  settings.setValue("delimiter", _delimiter->currentIndex());
  settings.setValue("type", _type->currentIndex());
  settings.setValue("exchange", _exchange->text());
  settings.setValue("useFilename", _filename->isChecked());
}
