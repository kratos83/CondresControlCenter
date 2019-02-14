/*
Copyright 2015 Michaël Lhomme

This file is part of AppSet.

AppSet is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

AppSet is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with AppSet; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "cachecleaner.h"
#include "../ui_cachecleaner.h"

#include <QKeyEvent>

/*
 * CacheCleaner window constructor
 */
CacheCleaner::CacheCleaner(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::CacheCleaner)
{
  //UI initialization
  ui->setupUi(this);

  ui->keepInstalledPackagesSpinner->setValue(0);
  ui->keepUninstalledPackagesSpinner->setValue(0);


  //create package group wrappers
  m_installed = new PackageGroupModel("",
                                      ui->installedPackagesList,
                                      ui->keepInstalledPackagesSpinner,
                                      ui->refreshInstalledButton,
                                      ui->cleanInstalledButton);

  m_uninstalled = new PackageGroupModel("-u",
                                        ui->uninstalledPackagesList,
                                        ui->keepUninstalledPackagesSpinner,
                                        ui->refreshUninstalledButton,                                                                                
                                        ui->cleanUninstalledButton);
}

/*
 * Cache Cleaner destructor
 */
CacheCleaner::~CacheCleaner()
{
  delete m_installed;
  delete m_uninstalled;
  delete ui;
}

/*
 * Save settings when closing window
 */
void CacheCleaner::closeEvent(QCloseEvent *)
{
  QByteArray windowSize=saveGeometry();
  Q_UNUSED(windowSize)
}

/*
 * Whenever user presses ESC, we quit the program
 */
void CacheCleaner::keyPressEvent(QKeyEvent *ke)
{
  if (ke->key() == Qt::Key_Escape)
  {
    close();
  }
}
