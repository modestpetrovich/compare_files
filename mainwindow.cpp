#include "mainwindow.h"
#include "piecesmodel.h"
#include "puzzlewidget.h"

#include <QtWidgets>
#include <stdlib.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupMenus();
    setupWidgets();
    model = new PiecesModel(puzzleWidget->pieceSize(), this);
    piecesList->setModel(model);

    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    setWindowTitle(tr("Puzzle"));
}

void MainWindow::openImage()
{
    const QString directory =
        QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).value(0, QDir::homePath());
    QFileDialog dialog(this, tr("Open Image"), directory);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::ExistingFile);
    QStringList mimeTypeFilters;
    for (const QByteArray &mimeTypeName : QImageReader::supportedMimeTypes())
        mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/jpeg");
    if (dialog.exec() == QDialog::Accepted)
        loadImage(dialog.selectedFiles().constFirst());
}

void MainWindow::loadImage(const QString &fileName)
{
    QPixmap newImage;
    if (!newImage.load(fileName)) {
        QMessageBox::warning(this, tr("Open Image"),
                             tr("The image file could not be loaded."),
                             QMessageBox::Close);
        return;
    }
    puzzleImage = newImage;
    setupPuzzle();
}

void MainWindow::setCompleted()
{
    QMessageBox::information(this, tr("Puzzle Completed"),
                             tr("Congratulations! You have completed the puzzle!\n"
                                "Click OK to start again."),
                             QMessageBox::Ok);

    setupPuzzle();
}

void MainWindow::setupPuzzle()
{
    int size = qMin(puzzleImage.width(), puzzleImage.height());
    puzzleImage = puzzleImage.copy((puzzleImage.width() - size) / 2,
        (puzzleImage.height() - size) / 2, size, size).scaled(puzzleWidget->imageSize(),
            puzzleWidget->imageSize(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    model->addPieces(puzzleImage);
    puzzleWidget->clear();
}

void MainWindow::setupMenus()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    QAction *openAction = fileMenu->addAction(tr("&Open..."), this, &MainWindow::openImage);
    openAction->setShortcuts(QKeySequence::Open);

    QAction *exitAction = fileMenu->addAction(tr("E&xit"), qApp, &QCoreApplication::quit);
    exitAction->setShortcuts(QKeySequence::Quit);

    QMenu *gameMenu = menuBar()->addMenu(tr("&Game"));

    gameMenu->addAction(tr("&Restart"), this, &MainWindow::setupPuzzle);
    gameMenu->addAction(tr("&Settings"), this, &MainWindow::setupPieceSize);
}

void MainWindow::setupPieceSize()
{
    QDialog* settings = new QDialog(this);
    settings->setMinimumSize(300, 300);
    settings->setMaximumSize(300, 300);
    settings->setWindowTitle("Size of Puzzle");
    QLabel* lbl = new QLabel(settings);
    lbl->move(40, 0);

    QVBoxLayout* parametersLayout = new QVBoxLayout(settings);
    QRadioButton* threeSize = new QRadioButton("3x3", settings);
    QRadioButton* fiveSize = new QRadioButton("5x5", settings);
    QRadioButton* tenSize = new QRadioButton("10x10", settings);

    connect(tenSize, SIGNAL(clicked()), puzzleWidget, SLOT(puzzleWidget->changeDivider(10)));
    connect(threeSize, SIGNAL(clicked()), puzzleWidget, SLOT(puzzleWidget->changeDivider(3)));
    connect(fiveSize, SIGNAL(clicked()), puzzleWidget, SLOT(puzzleWidget->changeDivider(5)));

    parametersLayout->addWidget(threeSize);
    parametersLayout->addWidget(fiveSize);
    parametersLayout->addWidget(tenSize);

    settings->show();
}

void MainWindow::setupWidgets()
{
    QFrame* frame = new QFrame;
    QHBoxLayout* frameLayout = new QHBoxLayout(frame);
    puzzleWidget = new PuzzleWidget(400);

    piecesList = new QListView;
    piecesList->setDragEnabled(true);
    piecesList->setViewMode(QListView::IconMode);
    piecesList->setIconSize(QSize(puzzleWidget->pieceSize() - 20, puzzleWidget->pieceSize() - 20));
    piecesList->setGridSize(QSize(puzzleWidget->pieceSize(), puzzleWidget->pieceSize()));
    piecesList->setSpacing(5);
    piecesList->setMovement(QListView::Snap);
    piecesList->setAcceptDrops(true);
    piecesList->setDropIndicatorShown(true);

    PiecesModel *model = new PiecesModel(puzzleWidget->pieceSize(), this);
    piecesList->setModel(model);

    connect(puzzleWidget, &PuzzleWidget::puzzleCompleted,
            this, &MainWindow::setCompleted, Qt::QueuedConnection);

    frameLayout->addWidget(piecesList);
    frameLayout->addWidget(puzzleWidget);
    setCentralWidget(frame);
}
