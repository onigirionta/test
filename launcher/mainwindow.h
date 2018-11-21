#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_rbFile_toggled(bool checked);

    void on_rbCOMPort_toggled(bool checked);

    void setInputFromFile(bool checked);

    void on_btFile_clicked();

    void on_btVectors_clicked();

    void on_btMatrix_clicked();

    void on_btGo_clicked();

    void on_process_started();
    void on_process_finished();

private:
    void startProcess();
    void stopProcess();

    QProcess* process{};
    Ui::MainWindow *ui;
    bool _running{};
};

#endif // MAINWINDOW_H
