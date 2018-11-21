#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <windows.h>

#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    process = new QProcess(this);
    connect(process, SIGNAL(finished(int)), this, SLOT(on_process_finished()));
    connect(process, SIGNAL(started()), this, SLOT(on_process_started()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setInputFromFile(bool checked)
{
    ui->edFile->setEnabled(checked);
    ui->btFile->setEnabled(checked);
    ui->cbCOMPort->setEnabled(!checked);
}

void MainWindow::on_rbFile_toggled(bool checked)
{
    setInputFromFile(checked);
}

void MainWindow::on_rbCOMPort_toggled(bool checked)
{
    setInputFromFile(!checked);
}

void MainWindow::on_btFile_clicked()
{
    QFileDialog dialog;
    if (dialog.exec()) {
        const auto path = dialog.selectedFiles().first();
        ui->edFile->setText(path);
        if (ui->edVectors->text().isEmpty()) {
            QFileInfo info(path);
            ui->edVectors->setText(info.dir().path() + "/vectors.txt");
        }
        if (ui->edMatrix->text().isEmpty()) {
            QFileInfo info(path);
            ui->edMatrix->setText(info.dir().path() + "/matrix.txt");
        }
    }
}

void MainWindow::on_btVectors_clicked()
{
    QFileDialog dialog;
    if (dialog.exec()) {
        ui->edVectors->setText(dialog.selectedFiles().first());
    }
}

void MainWindow::on_btMatrix_clicked()
{
    QFileDialog dialog;
    if (dialog.exec()) {
        ui->edMatrix->setText(dialog.selectedFiles().first());
    }
}

void MainWindow::on_btGo_clicked()
{
    ui->btGo->setEnabled(false);

    if (_running) {
        ui->statusBar->showMessage(tr("Сбор данных завершается..."));
        stopProcess();
    } else {
        ui->statusBar->showMessage(tr("Сбор данных запускается..."));
        startProcess();
    }

    ui->btGo->setEnabled(true);
}

void MainWindow::on_process_started()
{
    _running = true;
    ui->statusBar->showMessage(tr("Сбор данных запущен."));
    ui->btGo->setText(tr("Стоп"));
}

void MainWindow::on_process_finished()
{
    _running = false;
    ui->tbLog->setPlainText(process->readAllStandardError());
    ui->statusBar->showMessage(tr("Сбор данных остановлен."));
    ui->btGo->setText(tr("Старт"));
}

void MainWindow::startProcess()
{
    process->setWorkingDirectory(QApplication::applicationDirPath());
    process->setProgram("tes.exe");

    QStringList args;

    if (ui->rbCOMPort->isChecked()) {
        args.append("-c");
        args.append(ui->cbCOMPort->currentText());
    } else {
        args.append("-f");
        args.append(ui->edFile->text());
    }

    args.append(ui->edVectors->text());
    args.append(ui->edMatrix->text());

    process->setArguments(args);

    process->start();
}

void MainWindow::stopProcess()
{
    const auto handle = ::OpenEventA(EVENT_MODIFY_STATE, FALSE, "Startrap_StopRequested");
    if (handle == INVALID_HANDLE_VALUE) {
        QMessageBox::warning(this,
                             tr("Проблема"),
                             tr("Не удалось связаться с запущенным процессом сбора данных.\n"
                                "Если вы уверены, что процесс запущен, попробуйте еще раз."));
        return;
    }

    if (!::SetEvent(handle)) {
        QMessageBox::warning(
                    this,
                    tr("Прооблема"),
                    tr("Не удается остановить процесс сбора данных.\n"
                       "Можно остановить его вручную через диспетчер задач (Ctrl+Shift+Esc)."));
    }

    ::CloseHandle(handle);
}
