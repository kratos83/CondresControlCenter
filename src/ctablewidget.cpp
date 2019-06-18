#include "ctablewidget.h"

CTableWidget::CTableWidget(QWidget* parent) :
    QTableWidget(parent)
{
}

void CTableWidget::mousePressEvent(QMouseEvent *event){

    if (event->button() == Qt::RightButton) {
            this->Popup(event ->pos());
            event->accept();
    }
    else QTableWidget::mousePressEvent(event);
}

void CTableWidget::Popup(const QPoint& pt)
{
    QPoint global = mapToGlobal(pt);
    QMenu* pPopup = new QMenu(this);
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/images/install.png"));
    QIcon rem;
    rem.addFile(QString::fromUtf8(":/images/remove.png"));
    QAction* pInstall = new QAction(tr("Install group"), this);
    pInstall->setIcon(icon);
    pInstall->setIconVisibleInMenu(true);
    QAction *pRemove = new QAction(tr("Remove group"),this);
    pRemove->setIcon(rem);
    pRemove->setIconVisibleInMenu(true);
    pPopup->addAction(pInstall);
    pPopup->addSeparator();
    pPopup->addAction(pRemove);
    QAction* pItem = pPopup->exec(global);
    if(pItem == pInstall)
    {
        selectAll();
        QModelIndexList selectedRows = selectionModel()->selectedRows();
        foreach( QModelIndex index, selectedRows )
        {
            QString name = model()->index(index.row(),1,QModelIndex()).data(Qt::DisplayRole).toString();
            emit addInstallPkg(name,"install");
        }
    }
    else if(pItem == pRemove)
    {
        selectAll();
        QModelIndexList selectedRows = selectionModel()->selectedRows();
        foreach( QModelIndex index, selectedRows )
        {
            QString name = model()->index(index.row(),1,QModelIndex()).data(Qt::DisplayRole).toString();
            emit addInstallPkg(name,"remove");
        }
    }
}

CTableWidget::~CTableWidget(){}
