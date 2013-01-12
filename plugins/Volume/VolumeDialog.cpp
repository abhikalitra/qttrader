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

#include "VolumeDialog.h"
#include "Doc.h"
#include "Global.h"
#include "MAType.h"
#include "CurveLineType.h"

#include <QtDebug>
#include <QSettings>
#include <QLayout>
#include <QFormLayout>


VolumeDialog::VolumeDialog (QWidget *p) : QDialog (p)
{
  _settings = 0;
  _modified = 0;
  _helpFile = "main.html";
  connect(this, SIGNAL(finished(int)), this, SLOT(deleteLater()));

  // main vbox
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  vbox->setMargin(5);
  setLayout(vbox);
  
  _tabs = new QTabWidget;
  vbox->addWidget(_tabs);

  // buttonbox
  _buttonBox = new QDialogButtonBox(QDialogButtonBox::Help);
  connect(_buttonBox, SIGNAL(accepted()), this, SLOT(done()));
  connect(_buttonBox, SIGNAL(rejected()), this, SLOT(cancel()));
  vbox->addWidget(_buttonBox);

  // ok button
  _okButton = _buttonBox->addButton(QDialogButtonBox::Ok);
  _okButton->setText(tr("&OK"));

  // cancel button
  _cancelButton = _buttonBox->addButton(QDialogButtonBox::Cancel);
  _cancelButton->setText(tr("&Cancel"));
  _cancelButton->setDefault(TRUE);
  _cancelButton->setFocus();

  // help button
  QPushButton *b = _buttonBox->button(QDialogButtonBox::Help);
  b->setText(tr("&Help"));
  connect(b, SIGNAL(clicked()), this, SLOT(help()));
}

void VolumeDialog::done ()
{
  saveSettings();
  save();
  accept();
}

void VolumeDialog::help ()
{
  Doc *doc = new Doc;
  doc->showDocumentation(_helpFile);
}

void VolumeDialog::cancel ()
{
  saveSettings();
  reject();
}

void VolumeDialog::loadSettings ()
{
  QSettings settings(g_settings);
  settings.beginGroup(g_session);

  QSize sz = settings.value("volume_dialog_size", QSize(200,200)).toSize();
  resize(sz);

  // restore the position of the app
  QPoint p = settings.value("volume_dialog_pos").toPoint();
  if (! p.isNull())
    move(p);
}

void VolumeDialog::saveSettings ()
{
  QSettings settings(g_settings);
  settings.beginGroup(g_session);
  settings.setValue("volume_dialog_size", size());
  settings.setValue("volume_dialog_pos", pos());
}

void VolumeDialog::setGUI (Entity *settings)
{
  _settings = settings;

  // Volume page
  QWidget *w = new QWidget;
  
  QFormLayout *form = new QFormLayout;
  w->setLayout(form);
  
  // up color
  QVariant *set = _settings->get(QString("upColor"));
  
  _upColor = new ColorButton(this, QColor(set->toString()));
  connect(_upColor, SIGNAL(valueChanged(QColor)), this, SLOT(modified()));
  form->addRow(tr("Up Color"), _upColor);
  
  // down color
  set = _settings->get(QString("downColor"));
  
  _downColor = new ColorButton(this, QColor(set->toString()));
  connect(_downColor, SIGNAL(valueChanged(QColor)), this, SLOT(modified()));
  form->addRow(tr("Down Color"), _downColor);
  
  // neutral color
  set = _settings->get(QString("neutralColor"));
  
  _neutralColor = new ColorButton(this, QColor(set->toString()));
  connect(_neutralColor, SIGNAL(valueChanged(QColor)), this, SLOT(modified()));
  form->addRow(tr("Neutral Color"), _neutralColor);
  
  _tabs->addTab(w, QString("Volume"));

  // MA page
  w = new QWidget;
  
  form = new QFormLayout;
  w->setLayout(form);

  // type
  set = _settings->get(QString("maType"));

  MAType mat;
  _maType = new QComboBox;
  _maType->addItems(mat.list());
  _maType->setCurrentIndex(_maType->findText(set->toString(), Qt::MatchExactly));
  connect(_maType, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Type"), _maType);
  
  // style
  set = _settings->get(QString("maStyle"));

  CurveLineType clt;
  _maStyle = new QComboBox;
  _maStyle->addItems(clt.list());
  _maStyle->setCurrentIndex(_maStyle->findText(set->toString(), Qt::MatchExactly));
  connect(_maStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Style"), _maStyle);

  // width
  set = _settings->get(QString("maWidth"));

  _maWidth = new QSpinBox;
  _maWidth->setRange(1, 99);
  _maWidth->setValue(set->toInt());
  connect(_maWidth, SIGNAL(valueChanged(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Width"), _maWidth);
  
  // color
  set = _settings->get(QString("maColor"));
  
  _maColor = new ColorButton(this, QColor(set->toString()));
  connect(_maColor, SIGNAL(valueChanged(QColor)), this, SLOT(modified()));
  form->addRow(tr("Color"), _maColor);
  
  // period
  set = _settings->get(QString("maPeriod"));

  _maPeriod = new QSpinBox;
  _maPeriod->setRange(1, 999999);
  _maPeriod->setValue(set->toInt());
  connect(_maPeriod, SIGNAL(valueChanged(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Period"), _maPeriod);
  
  _tabs->addTab(w, QString("MA"));
  
}

void VolumeDialog::save ()
{
  if (! _modified)
    return;

  QVariant *set = _settings->get(QString("upColor"));
  set->setValue(_upColor->color().name());

  set = _settings->get(QString("downColor"));
  set->setValue(_downColor->color().name());

  set = _settings->get(QString("neutralColor"));
  set->setValue(_neutralColor->color().name());

  set = _settings->get(QString("maType"));
  set->setValue(_maType->currentText());

  set = _settings->get(QString("maStyle"));
  set->setValue(_maStyle->currentText());

  set = _settings->get(QString("maWidth"));
  set->setValue(_maWidth->value());

  set = _settings->get(QString("maColor"));
  set->setValue(_maColor->color().name());

  set = _settings->get(QString("maPeriod"));
  set->setValue(_maPeriod->value());
}

void VolumeDialog::modified ()
{
  _modified = 1;
}
