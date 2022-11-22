// by stevenso aka v3ssel
#ifndef DEPOSIT_H
#define DEPOSIT_H

#include <QBoxLayout>
#include <QCalendarWidget>
#include <QCheckBox>
#include <QDialog>
#include <QListWidget>
#include <QScrollArea>
#include <QToolButton>
#include <QtAlgorithms>

#include "Controller/Controller.h"

namespace Ui {
class deposit;
}

class ViewDeposit : public QDialog {
  Q_OBJECT

 public:
  QHash<QPushButton *, QHBoxLayout *> mButtonToLayoutMap;
  QDate current;
  QFont font;
  explicit ViewDeposit(QWidget *parent = nullptr);
  ~ViewDeposit();

 private slots:
  void on_pushButton_back_clicked();
  void on_pushButton_termType_clicked();
  void on_pushButton_rateType_clicked();
  void on_pushButton_addMore_clicked();
  void on_pushButton_periodType_clicked();
  void on_pushButton_addMoreRep_clicked();
  void on_pushButton_addMoreWith_clicked();
  void on_pushButton_calculate_clicked();
  void itemDClickedTERM();
  void itemDClickedPERIOD();
  void itemDClickedRATE();
  void delOneRate();
  void delAllRate();
  void delOneRep();
  void delOneWith();

 private:
  void createLists();
  void maxSize();
  bool RepAndWithFill(size_t count, QDate *RepWithDates,
                      long double *RepWithValues, QVBoxLayout *vbox_rep,
                      size_t *toI, bool withdraw);
  bool RateFill(int64_t *daysOrAmountRate, size_t rateCounter,
                long double *interestRate);
  void SortAndDelDuples(QDate *RepWithDates, long double *RepWithValues,
                        size_t *size);
  void setPeriod(int64_t term, short *period);
  QPushButton *createWidgets(QHBoxLayout *&, QFrame *&);

  Ui::deposit *ui_;
  s21::Controller controller_;
};

#endif  // DEPOSIT_H
