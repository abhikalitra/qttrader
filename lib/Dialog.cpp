/*
 *  Qtstalker stock charter
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

#include "Dialog.h"
#include "Doc.h"
#include "Global.h"

#include <QtDebug>
#include <QSettings>

Dialog::Dialog (QWidget *p) : QDialog (p)
{
  _helpFile = "main.html";
  connect(this, SIGNAL(finished(int)), this, SLOT(deleteLater()));

  // main vbox
  _vbox = new QVBoxLayout;
  _vbox->setSpacing(5);
  _vbox->setMargin(5);
  setLayout(_vbox);

  // form layout
  _form = new QFormLayout;
  _form->setSpacing(2);
  _form->setMargin(0);
  _vbox->addLayout(_form);

  // status message
  _message = new QLabel;
  _vbox->addWidget(_message);

  // buttonbox
  _buttonBox = new QDialogButtonBox(QDialogButtonBox::Help);
  connect(_buttonBox, SIGNAL(accepted()), this, SLOT(done()));
  connect(_buttonBox, SIGNAL(rejected()), this, SLOT(cancel()));
  _vbox->addWidget(_buttonBox);

  // ok button
  _okButton = _buttonBox->addButton(QDialogButtonBox::Ok);
  _okButton->setText(tr("&OK"));

  // cancel button
  _cancelButton = _buttonBox->addButton(QDialogButtonBox::Cancel);
  _cancelButton->setText(tr("&Cancel"));
  _cancelButton->setDefault(TRUE);
  _cancelButton->setFocus();

  // help button
  _helpButton = _buttonBox->button(QDialogButtonBox::Help);
  _helpButton->setText(tr("&Help"));
  connect(_helpButton, SIGNAL(clicked()), this, SLOT(help()));
}

Dialog::~Dialog ()
{
  saveSettings();
}

// virtual
void Dialog::done ()
{
  accept();
}

// virtual
void Dialog::help ()
{
  Doc *doc = new Doc;
  doc->showDocumentation(_helpFile);
}

// virtual
void Dialog::cancel ()
{
  reject();
}

// virtual
void Dialog::loadSettings ()
{
  QSettings settings(g_settings);
  settings.beginGroup(g_session);

  QSize sz = settings.value(_keySize, QSize(200,200)).toSize();
  resize(sz);

  // restore the position of the app
  QPoint p = settings.value(_keyPos).toPoint();
  if (! p.isNull())
    move(p);
}

// virtual
void Dialog::saveSettings ()
{
  QSettings settings(g_settings);
  settings.beginGroup(g_session);
  settings.setValue(_keySize, size());
  settings.setValue(_keyPos, pos());
}

void Dialog::setWidget (QWidget *w)
{
  _vbox->insertWidget(0, w);
}
