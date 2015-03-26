#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QFileDialog>
#include <stdint.h>
#include <fstream>
#include <openslide/openslide.h>
#include <openslide/openslide-features.h>

openslide_t * osr; // the openslide image, this is what openslide will create and manipulate

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;

    if(osr!=0) //TODO: nullptr gave compile errors, change C++ version
    {
        //close image
        openslide_close (osr);
        qDebug()<< "image closed";
    }

}

void MainWindow::on_loadButton_clicked()
{
    qDebug() << "load button clicked";
    QString qfilename = QFileDialog::getOpenFileName(this, tr("Open File"),"","All files (*.*)" );
    QByteArray ba= qfilename.toLatin1();
    char* filename =ba.data();
    loadImageFile(filename);

    //Eventually, call RegionRead on the most zoomed out level on loading the image
}

void MainWindow::on_ViewRegionButton_clicked()
{
    qDebug() << "View Region button clicked";
    // first we extract the desired region size, position and level
    int64_t wtarget,htarget,xtarget,ytarget; // variables to read
    int level; // variables to read

    qDebug() << "Calling input data parsing function";
    extractRegionData(wtarget,htarget,xtarget,ytarget,level);

    //now we actually read the region and visualize it
    qDebug() << "calling region reading function and visualization";
    Regionread(wtarget,htarget,xtarget,ytarget,level);

}

void MainWindow::loadImageFile(const char *filename)
{

    qDebug()<< "load image file function invoked";
    qDebug()<< "about to load filename:";
    qDebug()<< filename;
    const char* information = openslide_detect_vendor(filename);
    qDebug()<< "openslide_detect_vendor output:";
    qDebug()<< information;

    // will we succeed in opening the image?
    bool success= openslide_can_open (filename);
    qDebug()<< "openslide_can_open output:";
    qDebug()<< success;
    // open the whole slide image
    osr=	openslide_open (filename); // load the image into the openslide_t osr variable

    // get level count
    int32_t level_count= openslide_get_level_count (osr);
    qDebug()<< "openslide_get_level_count:";
    qDebug()<< level_count;

    //get level 0 dimensions
    int64_t w;
    int64_t h;

    openslide_get_level0_dimensions (osr, &w, &h);
    qDebug()<< "openslide_get_level0_dimensions:";
    qDebug()<< "width:";
    qDebug()<< w;
    qDebug()<< "height:";
    qDebug()<< h;

    //get the most zoomed out level dimensions, and observe the entire image on the poorest resolution
    int64_t initw,inith;
    openslide_get_layer_dimensions(osr, level_count-1 ,&initw,&inith);
    qDebug()<< "openslide_get_level_dimensions, poorest resolution:";
    qDebug()<< "width:";
    qDebug()<< initw;
    qDebug()<< "height:";
    qDebug()<< inith;

    //call the Regionread method
    MainWindow::Regionread(initw,inith,0,0,static_cast<int>(level_count-1));

}

void MainWindow::extractRegionData(int64_t& wtarget, int64_t& htarget, int64_t& xtarget,  int64_t& ytarget, int& level)
{
     //safely cast and assign wtarget variable
     QString wtargetString = ui->widthlineEdit->text();
     wtarget = static_cast<int64_t>(wtargetString.toLong());
     qDebug()<< wtarget;

     //safely cast and assign htarget variable
     QString htargetString = ui->heightlineEdit->text();
     htarget = static_cast<int64_t>(htargetString.toLong());
     qDebug()<< htarget;

     //safely cast and assign xtarget variable
     QString xtargetString = ui->xCoordlineEdit->text();
     xtarget = static_cast<int64_t>(xtargetString.toLong());
     qDebug()<< xtarget;

     //safely cast and assign  ytarget variable
     QString ytargetString = ui->yCoordlineEdit->text();
     ytarget = static_cast<int64_t>(ytargetString.toLong());
     qDebug()<< ytarget;

     //safely cast and assign level variable
     QString levelString = ui->levellineEdit->text();
     level = static_cast<int>(levelString.toInt());
     qDebug()<< level;

}

void MainWindow::Regionread(const int64_t wtarget, const int64_t htarget, const int64_t xtarget, const int64_t ytarget, const int level)
{
    //TODO: raise error OR load image if osr is null

    //TODO: changing the level but keeping the corner constant creates an unexpected zoom functionality (zooms to the corner, not to the center)
    // change that!

    //open the requested region
    uint32_t* dst = new uint32_t[wtarget*htarget]; //preallocate memory to write the desired region to (4 bytes per pixel)
    openslide_read_region(osr,dst,xtarget,ytarget,level,wtarget,htarget); // this outputs the image as premutliplied ARGB values

    // set the dst array to a Qimage for visualization
    //First, we create a new destination array of 4 uint8 values for each uint32 value (this is the required formate for QImage)
    uint8_t* dst_8b = new uint8_t[wtarget*htarget*4];
    // now we assing each element to the corresponding A R G or B value
    for(int i=0; i<wtarget*htarget; i++)
    {
        uint32_t value= dst[i];
        dst_8b[4*i+0] = (value & 0x000000ff);
        dst_8b[4*i+1] = (value & 0x0000ff00) >> 8;
        dst_8b[4*i+2] = (value & 0x00ff0000) >> 16;
        dst_8b[4*i+3] = (value & 0xff000000) >> 24;
    }
    delete dst; //we delete the dst variable, as it is no longer necessary

    // Finally, we generate the Qimage with the available pixel data
    QImage regionImage= QImage(dst_8b, wtarget,htarget, QImage::Format_ARGB32_Premultiplied);


    // Now we add the image to the GraphicsView
    ui->Imagelabel->setPixmap(QPixmap::fromImage(regionImage));





    //save the opened region to disk, just for additional verification
    //QString filenameOutput =QString("/home/martin/QTGsoc/imagenPrueba/output.bmp");
    //testImage.save(filenameOutput);


    //TODO:
    //Figure out an efficient way to delete dst_8b when it is no longer needed
}
