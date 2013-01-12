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

#include "LaunchPluginDialog.h"
#include "Global.h"

#include <QtDebug>
#include <QSettings>
#include <QProcess>
#include <QMessageBox>

LaunchPluginDialog::LaunchPluginDialog (QWidget *p) : Dialog (p)
{
  _keySize = "launch_plugin_dialog_size";
  _keyPos = "launch_plugin_dialog_pos";
  
  // plugin
  QSettings settings(g_settings);
  settings.beginGroup("plugins");
  
  _plugin = new QComboBox;
  _plugin->addItems(settings.value("gui").toStringList());
  _form->addRow(tr("Plugin"), _plugin);
  
  // session
  _session = new QLineEdit(tr("New Session"));
  _form->addRow(tr("Session"), _session);
  
  QStringList tl;
  tl << g_title << tr("New Session Launcher");
  setWindowTitle(tl.join(" "));
  
  loadSettings();
}

void
LaunchPluginDialog::done ()
{
  QSettings settings(g_settings);
  QStringList tl = settings.childGroups();
  if (tl.indexOf(_session->text()) != -1)
  {
    QMessageBox::StandardButton rc = QMessageBox::question(this,
                                                           tr("Session Already Exists"),
                                                           tr("Do you want to overwrite existing session?"),
                                                           QMessageBox::Yes | QMessageBox::No,
                                                           QMessageBox::No);
    
    if (rc == QMessageBox::No)
      return;
  }
  
  settings.beginGroup(_session->text());
  settings.setValue("plugin", _plugin->currentText());
  settings.sync();
  
  tl.clear();
  tl << "ota" << "-session" << _session->text();

  if (! QProcess::startDetached(tl.join(" ")))
  {
    qDebug() << "LaunchPluginDialog::done: error launching process" << tl;
    reject();
    return;
  }
  
  accept();
}
