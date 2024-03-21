#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QtWidgets/QMessageBox>

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

    // 所有的槽都要加上slots关键字
private slots:
    // void on_cancelButton_clicked();
    void on_openImagesAction_clicked();
    // void addPathToHistory(const QString &path,QMenu &historyMenu);
    //QString MainWindow::openFile(QString path);
    void writeFile(QStringList &pathList);
    void readFile();

private:
    Ui::MainWindow *ui;
    QStringList historyPath;
    QMenu *recentMainImages;

};
#endif // MAINWINDOW_H
