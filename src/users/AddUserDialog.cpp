/*
 *  This file is part of Manjaro Settings Manager.
 *  
 *  Modify to Angelo Scarnà <angelo.scarnaòcodelinsoft.it>
 *  Roland Singer <roland@manjaro.org>
 *
 *  Manjaro Settings Manager is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Manjaro Settings Manager is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Manjaro Settings Manager.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "AddUserDialog.h"
#include "ui_AddUserDialog.h"

#include <QProcess>

#include <QDebug>

AddUserDialog::AddUserDialog( QWidget* parent ) :
    QDialog( parent ),
    ui( new Ui::AddUserDialog )
{
    ui->setupUi( this );
    dataChanged = false;

    // Connect signals and slots
    connect( ui->buttonCancel, SIGNAL( clicked() ) ,   this, SLOT( close() ) );
    connect( ui->buttonCreate, SIGNAL( clicked() ) ,   this, SLOT( buttonCreate_clicked() ) );
    connect( ui->textBoxPassword, SIGNAL( textChanged( QString ) )   ,   this, SLOT( textbox_textChanged() ) );
    connect( ui->textBoxUsername, SIGNAL( textChanged( QString ) )   ,   this, SLOT( textbox_textChanged() ) );
    connect( ui->textBoxVerifiedPassword, SIGNAL( textChanged( QString ) )   ,   this, SLOT( textbox_textChanged() ) );
}


AddUserDialog::~AddUserDialog()
{
    delete ui;
}


int AddUserDialog::exec()
{
    ui->textBoxPassword->clear();
    ui->textBoxUsername->clear();
    ui->textBoxVerifiedPassword->clear();
    ui->buttonCreate->setEnabled( false );
    ui->buttonCancel->setFocus();
    dataChanged = false;

    return QDialog::exec();
}


void AddUserDialog::textbox_textChanged()
{
    if ( ui->textBoxPassword->text().isEmpty() ||
            ui->textBoxUsername->text().isEmpty() ||
            ui->textBoxVerifiedPassword->text().isEmpty() )
        ui->buttonCreate->setEnabled( false );
    else
        ui->buttonCreate->setEnabled( true );
}


// TODO: Now it ask twice for a password (add user and change password) fix this.
void AddUserDialog::buttonCreate_clicked()
{
    QString errorMessage;
    QString username = ui->textBoxUsername->text();
    QString password = ui->textBoxPassword->text();

    // Check username
    QRegExp rx( "^[a-z][-a-z0-9_]*\\$" );
    QRegExpValidator val( rx );
    int pos = -1;

    if ( val.validate( username, pos ) == QValidator::Invalid )
        errorMessage = tr( "Your username contains invalid characters!" );

    // Check passwords
    if ( password != ui->textBoxVerifiedPassword->text() )
        errorMessage = tr( "Your passwords do not match!" );

    // Show messagebox if error appeared
    if ( !errorMessage.isEmpty() )
    {
        QMessageBox::warning( this, tr( "Error!" ), errorMessage, QMessageBox::Ok, QMessageBox::Ok );
        return;
    }

    dataChanged = true;

    QString defaultUserGroups {"video,audio,power,disk,storage,optical,network,lp,scanner"};
    QStringList args;
    args << "-m" << "-p" << "" << "-U" << "-G" << defaultUserGroups << username;
    QProcess proc;
    if(proc.execute("useradd",args) != 0)
    {
        QMessageBox::warning( this, tr( "Error!" ), QString( tr( "Failed to add user!" ) ), QMessageBox::Ok, QMessageBox::Ok );
        close();
    }
    else {qDebug() << "Add User Job Succesfull";}
    
    QProcess cmd2;
    cmd2.setReadChannel(QProcess::StandardOutput);
    cmd2.setProcessChannelMode(QProcess::MergedChannels);
    cmd2.start("passwd "+username);
    if (!cmd2.waitForStarted()){
        qDebug() << tr("Error: Could not start!") << endl;
        return;
    }
    QByteArray stdinData = QByteArray(password.toUtf8());
    stdinData += "\n";
    stdinData += QByteArray(password.toUtf8());
    stdinData += "\n";
    if(!stdinData.isEmpty()){
        cmd2.waitForStarted();
        cmd2.write(stdinData);
        cmd2.waitForBytesWritten();
        cmd2.closeWriteChannel();
    }
    cmd2.waitForFinished(-1);
    QByteArray pkexec_error = cmd2.readAllStandardError();
    qDebug() << pkexec_error;
    const bool succeeded = cmd2.exitCode() == 0;
    if(!succeeded)
    {
        QMessageBox::warning( this, tr( "Error!" ), QString( tr( "Failed to set user's password!" ) ), QMessageBox::Ok, QMessageBox::Ok );
        close();
    }
    else qDebug() << "Add User Job Succesfull";
    close();
}
