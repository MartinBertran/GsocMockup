#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_loadButton_clicked();

    void on_ViewRegionButton_clicked();

private:
    Ui::MainWindow *ui;
    void loadImageFile(const char* filename);
    void extractRegionData(int64_t& wtarget, int64_t& htarget, int64_t& xtarget, int64_t& ytarget, int& level);
    void Regionread(const int64_t wtarget,const int64_t htarget,const int64_t xtarget,const int64_t ytarget,const int level);
};



#endif // MAINWINDOW_H
