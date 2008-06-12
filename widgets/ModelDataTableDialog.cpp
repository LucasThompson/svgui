/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*-  vi:set ts=8 sts=4 sw=4: */

/*
    Sonic Visualiser
    An audio file viewer and annotation editor.
    Centre for Digital Music, Queen Mary, University of London.
    This file copyright 2008 QMUL.
    
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/

#include "ModelDataTableDialog.h"

#include "data/model/ModelDataTableModel.h"
#include "data/model/TabularModel.h"
#include "data/model/Model.h"

#include "CommandHistory.h"

#include <QTableView>
#include <QGridLayout>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QApplication>
#include <QDesktopWidget>

#include <iostream>

ModelDataTableDialog::ModelDataTableDialog(TabularModel *model, QString title, QWidget *parent) :
    QMainWindow(parent)
{
    setWindowTitle(tr("Data Editor"));

    QToolBar *toolbar = addToolBar(tr("Toolbar"));
    CommandHistory::getInstance()->registerToolbar(toolbar);

    QFrame *mainFrame = new QFrame;
    setCentralWidget(mainFrame);

    QGridLayout *grid = new QGridLayout;
    mainFrame->setLayout(grid);
    
    QGroupBox *box = new QGroupBox;
    if (title != "") {
        box->setTitle(title);
    } else {
        box->setTitle(tr("Data in Layer"));
    }
    grid->addWidget(box, 0, 0);
    grid->setRowStretch(0, 15);

    QGridLayout *subgrid = new QGridLayout;
    box->setLayout(subgrid);

    subgrid->setSpacing(0);
    subgrid->setMargin(5);

    m_tableView = new QTableView;
    subgrid->addWidget(m_tableView);

    m_tableView->verticalHeader()->hide();
//    m_tableView->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    m_tableView->setSortingEnabled(true);
    m_tableView->sortByColumn(0, Qt::AscendingOrder);

    m_table = new ModelDataTableModel(model);
    m_tableView->setModel(m_table);

    connect(m_tableView, SIGNAL(clicked(const QModelIndex &)),
            this, SLOT(viewClicked(const QModelIndex &)));
    connect(m_tableView, SIGNAL(pressed(const QModelIndex &)),
            this, SLOT(viewPressed(const QModelIndex &)));
    connect(m_table, SIGNAL(executeCommand(Command *)),
            this, SLOT(executeCommand(Command *)));

    QDialogButtonBox *bb = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(bb, SIGNAL(rejected()), this, SLOT(close()));
    grid->addWidget(bb, 2, 0);
    grid->setRowStretch(2, 0);
    
    QDesktopWidget *desktop = QApplication::desktop();
    QRect available = desktop->availableGeometry();

    int width = available.width() / 3;
    int height = available.height() / 2;
    if (height < 370) {
        if (available.height() > 500) height = 370;
    }
    if (width < 500) {
        if (available.width() > 650) width = 500;
    }

    resize(width, height);
}

ModelDataTableDialog::~ModelDataTableDialog()
{
    delete m_table;
}

void
ModelDataTableDialog::scrollToFrameRequested(unsigned long frame)
{
    m_tableView->scrollTo(m_table->getModelIndexForFrame(frame));
}

void
ModelDataTableDialog::viewClicked(const QModelIndex &index)
{
    std::cerr << "ModelDataTableDialog::viewClicked: " << index.row() << ", " << index.column() << std::endl;
    emit scrollToFrame(m_table->getFrameForModelIndex(index));
}

void
ModelDataTableDialog::viewPressed(const QModelIndex &index)
{
    std::cerr << "ModelDataTableDialog::viewPressed: " << index.row() << ", " << index.column() << std::endl;
}

void
ModelDataTableDialog::executeCommand(Command *command)
{
    std::cerr << "ModelDataTableDialog::executeCommand(" << command << ")" << std::endl;
    CommandHistory::getInstance()->addCommand(command, false, true);
}
