#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <windows.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onStartClicked();
    void onStopClicked();
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_actionQuit_triggered();

    void on_pushButton_3_clicked();

    void on_checkBoxAutostop_checkStateChanged(const Qt::CheckState &arg1);

    void on_lineEditDelay_textChanged(const QString &arg1);

private:
    static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
    void SetHook();
    void ReleaseHook();
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
