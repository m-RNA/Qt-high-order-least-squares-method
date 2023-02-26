#ifndef CV_TABLE_H
#define CV_TABLE_H

#include <QWidget>
#include <QTableWidget>

class CVTable : public QTableWidget
{
    Q_OBJECT
public:
    explicit CVTable(QWidget *parent = nullptr);

private:
    void keyPressEvent(QKeyEvent *event);

    void copySelectedCells();
    void pasteSelectedCells();
    void removeSelectedCells();
};

#endif // CV_TABLE_H
