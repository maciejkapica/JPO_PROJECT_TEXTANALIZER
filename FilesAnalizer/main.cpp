#include "mainwindow.h"
#include <QApplication>
#include <QTreeView>
#include <QFileSystemModel>
#include <QStringList>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QInputDialog>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Tworzenie modelu dla systemu plików
    QFileSystemModel model;
    model.setRootPath("");

    // Tworzenie widoku drzewa katalogów
    QTreeView* treeView = new QTreeView();
    treeView->setModel(&model);

    // Przycisk do uruchomienia analizy
    QPushButton *analyzeButton = new QPushButton("Analizuj folder");
    QObject::connect(analyzeButton, &QPushButton::clicked, [&]() {

        // Pobranie wybranego katalogu
        QModelIndex currentIndex = treeView->currentIndex();

        // Sprawdzenie czy wybrany element drzewa jest katalogiem
        if (!model.isDir(currentIndex)) {
            QMessageBox::warning(nullptr, "Error", "Proszę wybrać folder.");
            return;
        }

        // Pobranie ścieżki bezwzględnej wybranego katalogu
        QString dir = model.filePath(currentIndex);

        // Pobranie typu pliku od użytkownika
        QString fileType = QInputDialog::getText(nullptr, "Typ pliku", "Wprowadź typ pliku (np. .txt, .cpp):");

        // Inicjalizacja sumy liczby wyrazów, znaków i linii
        int wordCount = 0, charCount = 0, lineCount = 0;

        // Zapisanie nazw wszystkich plików w katalogu w postaci listy
        QStringList files;
        QDirIterator it(dir, QDir::Files, QDirIterator::Subdirectories);
            while (it.hasNext()) {
                it.next();
                files << it.filePath();
            }

        // Otwarcie po kolei wszystkich plików ze zgodnym typem w podanym katalogu
        for (QString file : files) {
            if (file.endsWith(fileType)) {
                QFile f(file);
                f.open(QIODevice::ReadOnly);
                QTextStream in(&f);

                // Iteracja przez wszystkie linie pliku i zliczenie statystyk
                while (!in.atEnd()) {
                    QString line = in.readLine();
                    lineCount++;
                    charCount += line.length();
                    wordCount += line.split(" ").length();
                }

                f.close();
            }
        }

        // Wyświetlenie wyniku w postaci okna
        QMessageBox::information(nullptr, "Licznik statystyk", QString("Liczba słów: %1\nLiczba znaków: %2\nLiczba linii: %3").arg(wordCount).arg(charCount).arg(lineCount));
    });

    // Tworzenie layoutu
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(treeView);
    layout->addWidget(analyzeButton);

    // Tworzenie okna
    QWidget window;
    window.setLayout(layout);
    window.show();

    return a.exec();
}
