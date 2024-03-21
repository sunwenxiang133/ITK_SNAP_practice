#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMenuBar>
#include <QToolBar>
#include <QFont>
#include <QDockWidget>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QStatusBar>
#include <QHBoxLayout>
#include <QPixmap>
#include <QFile>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // NOTE:初始化UI文件，并把他挂在实例上
    ui->setupUi(this);

    // NOTE:因为继承自QMainWindow，所以setWindowTitle之类的方法都可以使用
    //标题
    setWindowTitle("ITK-SNAP");
    setWindowIcon(QIcon(":/res/icon.jpg"));
    //菜单栏
    QMenuBar * bar = menuBar();
    setMenuBar(bar);
    QMenu * fileMenu = bar -> addMenu("File");
    QMenu * editMenu = bar -> addMenu("Edit");
    QMenu * segMenu = bar -> addMenu("Segmentation");
    QMenu * workMenu = bar -> addMenu("Workspace");
    QMenu * toolMenu = bar -> addMenu("Tools");
    QMenu * helpMenu = bar -> addMenu("Help");
    QFont menuFont("Arial",15);
    bar -> setFont(menuFont);

    //菜单项
    // QFont menuFont("Arial",15);
    fileMenu -> setFont(menuFont);
    editMenu -> setFont(menuFont);
    segMenu -> setFont(menuFont);
    workMenu -> setFont(menuFont);
    toolMenu -> setFont(menuFont);
    helpMenu -> setFont(menuFont);

    //fileMenu
    //fileMenu -> addAction("Recent Main Images");
    //历史记录保存
    // NB:这里千万别写QStringList historyPath了,否则相当于内部的变量了，mainwindow的函数也根本访问不到
    historyPath=(QStringList());
    // MainWindow::writeFile(historyPath);
    MainWindow::readFile();
    recentMainImages=new QMenu(tr("Recent Main Images"),this);



    //fileMenu -> addAction("Open Main Image ...");
    QAction *openImagesAction=new QAction(tr("Open Main Image ..."),this);
    openImagesAction->setShortcut(QKeySequence("Ctrl+G"));
    connect(openImagesAction,&QAction::triggered,this,&MainWindow::on_openImagesAction_clicked);
    fileMenu->addAction(openImagesAction);

    fileMenu->addMenu(recentMainImages);


    for(int i=0;i<historyPath.size();++i){
        // NOTE:注意这里不用tr() 这个是转换为字符串的过程
        QAction *recentFilePath=new QAction(historyPath.at(i),this);
        recentFilePath->setShortcut(QKeySequence(&"Ctrl+"[i]));
        recentMainImages->addAction(recentFilePath);
        //recentMainImages->addAction(tr(historyPath.at(i)),recentMainImages);
    }

    fileMenu -> addSeparator();

    QAction *addImageAction = new QAction(tr("Add Another Image ..."),this);
    addImageAction -> setShortcut(QKeySequence("Ctrl+Shift+G"));
    fileMenu -> addAction(addImageAction);
    // fileMenu -> addAction("Add Another Image ... Ctrl+Shift+G");
    fileMenu -> addSeparator();
    fileMenu -> addAction("Save Image");
    fileMenu -> addSeparator();
    fileMenu -> addAction("Export");
    fileMenu -> addSeparator();
    fileMenu -> addAction("Close All Images Ctrl+Shift+W");
    fileMenu -> addSeparator();
    fileMenu -> addAction("New ITK-SNAP Window");
    fileMenu -> addSeparator();

    // 退出按钮
    QAction *exitAction = new QAction(tr("Quit Ctrl+Q"),this);
    exitAction->setShortcut(QKeySequence("Ctrl+Q"));
    exitAction->setStatusTip(tr("Exit the application"));
    connect(exitAction,&QAction::triggered,this,&QApplication::quit);

    fileMenu -> addAction(exitAction);
    //editMenu
    QAction * undoAction = editMenu -> addAction("Undo Ctrl+Z");
    undoAction -> setIcon(QIcon(":/res/undo.png"));

    QAction * redoAction = editMenu -> addAction("Redo Ctrl+Shift+Z");
    redoAction -> setIcon(QIcon(":/res/redo.png"));

    editMenu -> addSeparator();
    editMenu -> addAction("Foreground Label");
    editMenu -> addAction("Background Label");
    editMenu -> addSeparator();
    editMenu -> addAction("Zoom");
    editMenu -> addAction("Layers");
    editMenu -> addAction("Slice Annotations");
    editMenu -> addSeparator();
    editMenu -> addAction("Views");
    //segMenu
    segMenu -> addAction("Open Segmentation ... Ctrl+O");
    segMenu -> addSeparator();
    segMenu -> addAction("Add Another Segmentation");
    segMenu -> addSeparator();
    segMenu -> addAction("Save Ctrl+S");
    segMenu -> addAction("Save as...");
    segMenu -> addSeparator();
    segMenu -> addAction("Unload Segmentation");
    segMenu -> addSeparator();
    segMenu -> addAction("Export as Surface Mesh...");
    segMenu -> addSeparator();
    segMenu -> addAction("Import Label Descriptions...");
    segMenu -> addAction("Export Label Descriptions...");
    segMenu -> addSeparator();
    QAction * labelAction = segMenu -> addAction("Label Editor ... Ctrl+L");
    labelAction -> setIcon(QIcon(":/res/tiaosepan.png"));
    segMenu -> addSeparator();
    segMenu -> addAction("Volumes and Statistics...");
    segMenu -> addSeparator();
    segMenu -> addAction("Appearance");
    //workMenu
    workMenu -> addAction("Open Workspace ...");
    workMenu -> addSeparator();
    workMenu -> addAction("Save Worksapce ...");
    workMenu -> addAction("Save Workspace As ...");
    //toolMenu
    QAction * layerAction = toolMenu -> addAction("Layer Inspector... Ctrl+I");
    layerAction -> setIcon(QIcon(":/res/layers.png"));
    toolMenu -> addSeparator();
    toolMenu -> addAction("Image Contrast");
    toolMenu -> addAction("Color Map Editor...");
    toolMenu -> addAction("Image Information...");
    toolMenu -> addSeparator();
    toolMenu -> addAction("Active Main Tool");
    toolMenu -> addAction("Active 3D Tool");
    toolMenu -> addSeparator();
    toolMenu -> addAction("Reorient Image...");
    toolMenu -> addAction("Registration ... Ctrl+R");
    toolMenu -> addSeparator();
    toolMenu -> addAction("Distributed Segmentation Service... Ctrl+D");
    toolMenu -> addSeparator();
    toolMenu -> addAction("Interpolate Labels...");
    toolMenu -> addSeparator();
    toolMenu -> addAction("Preferences ...");
    //helpMenu
    helpMenu -> addAction("About ITK-SNAP");
    helpMenu -> addSeparator();
    helpMenu -> addAction("Check for Software Updates ...");
    helpMenu -> addSeparator();
    helpMenu -> addAction("Documentation Home ...");
    // //工具栏
    QToolBar * toolBar = new QToolBar(this);
    addToolBar(Qt::TopToolBarArea, toolBar);
    toolBar -> setAllowedAreas(Qt::TopToolBarArea);
    toolBar -> setFloatable(false);
    toolBar -> setMovable(false);
    QPushButton * white1 = new QPushButton("                                                                          ", this);
    toolBar -> addWidget(white1);
    QPushButton * btn1 = new QPushButton("Getting Started", this);
    toolBar -> addWidget(btn1);
    QPushButton * white2 = new QPushButton("                ", this);
    toolBar -> addWidget(white2);
    QPushButton * btn2 = new QPushButton("Recent Images", this);
    toolBar -> addWidget(btn2);
    QPushButton * white3 = new QPushButton("                ", this);
    toolBar -> addWidget(white3);
    QPushButton * btn3 = new QPushButton("Recent Workspaces", this);
    toolBar -> addWidget(btn3);
    btn1 -> setStyleSheet("QPushButton{background-color: transparent;font-size:13pt;color:gray;}"
                        "QPushButton:hover{text-decoration:underline;}");
    btn2 -> setStyleSheet("QPushButton{background-color: transparent;font-size:13pt;color:gray;}"
                        "QPushButton:hover{text-decoration:underline;}");
    btn3 -> setStyleSheet("QPushButton{background-color: transparent;font-size:13pt;color:gray;}"
                        "QPushButton:hover{text-decoration:underline;}");
    white1 -> setStyleSheet("QPushButton{background-color: transparent;}");
    white2 -> setStyleSheet("QPushButton{background-color: transparent;}");
    white3 -> setStyleSheet("QPushButton{background-color: transparent;}");
    // //铆接部件（浮动窗口）
    // QListWidget * listWidget;
    QDockWidget * dockWidget = new QDockWidget(tr("ITK-SNAP Toolbox"), this);
    addDockWidget(Qt::LeftDockWidgetArea, dockWidget);
    dockWidget -> setAllowedAreas(Qt::LeftDockWidgetArea);

    //透明标题栏
    dockWidget->setStyleSheet("QDockWidget { background-color: transparent; }");
    // 创建布局
    QVBoxLayout * layout = new QVBoxLayout;
    // 创建标签并设置文本
    QLabel * label1 = new QLabel("\nITK-SNAP\n");
    label1->setStyleSheet("font-family:宋体; font-size: 24pt; text-align: center;");
    QLabel * label2 = new QLabel("Version 3.8.0\nJun 12， 2019\n\n\nCpoyright（C） 1998-2019\nPaul a. Yushkevich\nGuido Gerig");
    label2->setStyleSheet("color:gray; font-family:宋体; font-size: 12pt; text-align: center;");
    label1->setAlignment(Qt::AlignCenter);
    label2->setAlignment(Qt::AlignCenter);
    QLabel * label3 = new QLabel("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nSupported by grants RO1\nEBO14346，RO1 EB017255, \nR03 EB008200, and Po 467-\nMZ-202446-1 from the\n US National Insitutes of\n Health");
    label3->setStyleSheet("color:gray; font-family:宋体; font-size: 10pt;text-align: center;");
    label3->setAlignment(Qt::AlignCenter);
    // 将标签添加到布局中
    layout -> addWidget(label1);
    layout -> addWidget(label2);
    layout -> addWidget(label3);
    // 在标签下方添加弹性空间
    layout->addStretch(1);
    // 创建一个QWidget作为容器，并将布局设置给它
    QWidget * container = new QWidget;
    container -> setLayout(layout);
    // 将容器设置为浮动窗口的内容
    dockWidget->setWidget(container);


    // QAction * dockAction1 = dockWidget -> addAction("ITK-SNAP");
    //浮动窗口设置为仅可移动和可浮动
    dockWidget -> setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);

    // listWidget = new QListWidget(dockWidget);
    // listWidget -> addItems(QStringList()
    //                      << ""
    //                      << "ITK-SNAP"
    //                      << ""
    //                      << "Version 3.8.0"
    //                      << "Jun 12,2019"
    //                      << ""
    //                      << "Cpoyright（C） 1998-2019"
    //                      << "Paul a. Yushkevich"
    //                      << "Guido Gerig"
    //                      << ""
    //                      << "");
    // dockWidget -> setWidget(listWidget);

    // QFont dockFont1("宋体",24);
    // dockAction1 -> setFont(dockFont1);


    //状态栏
    QStatusBar * stBar = statusBar();
    setStatusBar(stBar);
    // 创建两个按钮
    QPushButton *button1 = new QPushButton("Open Image ...");
    QPushButton *button2 = new QPushButton("Open Workspace ...");
    button1->setStyleSheet("QPushButton { min-width: 150px; min-height: 30px; }");
    button2->setStyleSheet("QPushButton { min-width: 200px; min-height: 30px; }");

    // 创建一个水平布局来管理按钮
    QHBoxLayout * layout2 = new QHBoxLayout;
    layout2->addWidget(button1);
    layout2->addWidget(button2);
    //layout2->addStretch(2); // 添加一个弹性空间来填充剩余空间

    // 创建一个QWidget作为容器，并将布局设置给它
    QWidget * container1 = new QWidget;
    container1->setLayout(layout2);

    // 将按钮的容器作为永久widget添加到状态栏，并设置适当的拉伸因子
    stBar->addPermanentWidget(container1, 0); // 0表示该widget不伸展

    //主界面
    QLabel * mainLabel = new QLabel();
    QPixmap pixmap(":/res/mainwindow.jpg");
    QPixmap scaledPixmap = pixmap.scaled(1800, 800, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    mainLabel->setPixmap(scaledPixmap);
    setCentralWidget(mainLabel);




}

MainWindow::~MainWindow()
{
    delete ui;

}
// void MainWindow::on_cancelButton_clicked()
// {
//     this->close();
// }

void MainWindow::on_openImagesAction_clicked()
{
    QString fileName=QFileDialog::getOpenFileName(this,"打开文件",QCoreApplication::applicationFilePath(),"*");
    if(fileName.isEmpty()){
        //好像用不到
        QMessageBox::warning(this,"警告","请选择正确的文件");
    }else{
        qDebug()<<historyPath;
        for(const QString &path:historyPath){
            if(path!=fileName){
                qDebug()<<"path"<<path;
                qDebug()<<"path123";
                qDebug()<<fileName;
                MainWindow::historyPath.append(fileName);
                QAction *recentFilePath=new QAction(fileName,this);
                recentMainImages->addAction(recentFilePath);
                MainWindow::writeFile(historyPath);
            }
        }

        // MainWindow::historyPath.append(fileName);
        // QAction *recentFilePath=new QAction(fileName,this);
        // recentMainImages->addAction(recentFilePath);
        // MainWindow::writeFile(historyPath);

        // qDebug()<<MainWindow::historyPath;
    }
}

// void MainWindow::addPathToHistory(const QString &path,QMenu &historyMenu){
//     if(!historyPath.contains(path)){
//         historyPath.append(path);

//         QAction *pathAction=new QAction(path,this);
//         connect(pathAction, &QAction::triggered, this, [this, path]() {
//             // 这里可以添加当点击历史路径时的处理逻辑
//             qDebug() << "Selected history path:" << path;
//         });
//         historyMenu.addAction(pathAction);
//     }
// }

void MainWindow::writeFile(QStringList &pathList){
    QFile file("D:\\Users\\chillin\\Documents\\code\\Qt\\ITK-SNAP\\res\\recentPath.txt");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        // 如果文件打开失败，输出错误信息并返回
        qWarning("Cannot open file for writing");
        return;
    }

    QTextStream out(&file);
    qDebug()<<historyPath;
    // out<< historyPath.data();
    for(const QString &line:historyPath){
        out<<line<<"\n";
    }
    file.close(); // 完成写入后关闭文件
}

void MainWindow::readFile(){
    QFile file("D:\\Users\\chillin\\Documents\\code\\Qt\\ITK-SNAP\\res\\recentPath.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Cannot open file for reading";
    }
    QTextStream in(&file);
    while(!in.atEnd()){
        QString line=in.readLine();
        historyPath<<line;
    }


}
