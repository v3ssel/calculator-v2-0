// by stevenso aka v3ssel
#include "../ViewDeposit/viewdeposit.h"

#include "../ViewCalc/viewcalc.h"
#include "ui_deposit.h"

ViewDeposit::ViewDeposit(QWidget *parent)
    : QDialog(parent), ui_(new Ui::deposit) {
  ui_->setupUi(this);
  font = QFont("Monaco");
  ui_->listWidget_months_years->hide();
  ui_->listWidget_rateType->hide();
  ui_->listWidget_period->hide();
  ui_->lineEdit_depends->hide();
  ui_->pushButton_addMore->hide();
  ui_->text_dependsOut->hide();
  createLists();

  ui_->lineEdit_dep_term->setValidator(new QIntValidator(0, (int)1e10, this));
  ui_->lineEdit_dep_amount->setValidator(
      new QDoubleValidator(0, 1e10, 3, this));
  ui_->lineEdit_depends->setValidator(new QIntValidator(0, (int)1e10, this));
  ui_->lineEdit_rate->setValidator(new QDoubleValidator(0, 1e10, 3, this));
  ui_->lineEdit_taxRate->setValidator(new QDoubleValidator(0, 1e10, 3, this));

  QTextCharFormat fmt;
  fmt.setForeground(QBrush(Qt::black));
  ui_->dateEdit->calendarWidget()->setWeekdayTextFormat(Qt::Saturday, fmt);
  ui_->dateEdit->calendarWidget()->setWeekdayTextFormat(Qt::Sunday, fmt);
  ui_->dateEdit->calendarWidget()->setFirstDayOfWeek(Qt::Monday);
  current = QDate::currentDate();
  ui_->dateEdit->setDate(current);
}

ViewDeposit::~ViewDeposit() { delete ui_; }

void ViewDeposit::createLists() {
  ui_->listWidget_months_years->addItem("Days");
  ui_->listWidget_months_years->addItem("Months");
  ui_->listWidget_months_years->addItem("Years");
  ui_->listWidget_rateType->addItem("Fixed");
  ui_->listWidget_rateType->addItem("Depends on the amount");
  ui_->listWidget_rateType->addItem("Depends on the term");
  ui_->listWidget_period->addItem("Everyday");
  ui_->listWidget_period->addItem("Every week");
  ui_->listWidget_period->addItem("Once a month");
  ui_->listWidget_period->addItem("Once a quarter");
  ui_->listWidget_period->addItem("Semiannually");
  ui_->listWidget_period->addItem("Once a year");
  ui_->listWidget_period->addItem("At the end of the term");
}

void ViewDeposit::on_pushButton_back_clicked() { this->close(); }

void ViewDeposit::on_pushButton_calculate_clicked() {
  long double amount = ui_->lineEdit_dep_amount->text().toDouble();
  int64_t term = ui_->lineEdit_dep_term->text().toLongLong();
  QDate startTerm = ui_->dateEdit->date();
  if (ui_->pushButton_termType->text().at(0) == 'Y')
    term = startTerm.daysTo(startTerm.addYears(term));
  if (ui_->pushButton_termType->text().at(0) == 'M')
    term = startTerm.daysTo(startTerm.addMonths(term));
  bool rateType =
      ui_->pushButton_rateType->text().contains("term") ? true : false;

  size_t rateCounter = ui_->widgets_frame->layout()->count();
  int64_t daysOrAmountRate[rateCounter];
  long double interestRate[rateCounter + 1];
  interestRate[0] = 0, interestRate[1] = ui_->lineEdit_rate->text().toDouble();
  daysOrAmountRate[0] = 1;

  bool fields = true;
  if (ui_->pushButton_rateType->text().at(0) != 'F') {
    fields = RateFill(daysOrAmountRate, rateCounter, interestRate);
  }

  long double taxRate = ui_->lineEdit_taxRate->text().toDouble();
  short period = 1;
  setPeriod(term, &period);
  bool capitalism = ui_->checkBox_capitalism->isChecked() ? true : false;

  if ((ui_->lineEdit_dep_amount->text() == "") ||
      (ui_->lineEdit_dep_term->text() == "") ||
      (ui_->lineEdit_rate->text() == "") ||
      (ui_->lineEdit_taxRate->text() == "")) {
    fields = false;
  }

  QVBoxLayout *vbox_rep =
      qobject_cast<QVBoxLayout *>(ui_->widgets_frame_rep->layout());
  QVBoxLayout *vbox_with =
      qobject_cast<QVBoxLayout *>(ui_->widgets_frame_with->layout());
  int64_t replCounter = vbox_rep->count();
  QDate RepWithDates[replCounter + vbox_with->count() - 1];
  long double RepWithValues[replCounter + vbox_with->count() - 1];
  size_t toI = 1;
  if (replCounter > 1)
    fields = RepAndWithFill(replCounter - 1, RepWithDates, RepWithValues,
                            vbox_rep, &toI, false);

  if (vbox_with->count() > 1)
    fields = RepAndWithFill(vbox_with->count() - 1, RepWithDates, RepWithValues,
                            vbox_with, &toI, true);

  size_t size = (replCounter - 1 + vbox_with->count() - 1);
  if (replCounter > 1 || vbox_with->count() > 1) {
    SortAndDelDuples(RepWithDates, RepWithValues, &size);
  }
  int64_t DatesInDays[size];
  for (size_t i = 0; i < size; i++) {
    DatesInDays[i] = startTerm.daysTo(RepWithDates[i]);
  }

  if (startTerm > RepWithDates[0] &&
      (replCounter > 1 || vbox_with->count() > 1)) {
    QMessageBox::warning(this, "Error", "Wrong dates");
    return;
  }
  if (!fields) {
    QMessageBox::warning(this, "Error", "All fields must be completed right!");
    return;
  }
  long double AccRate = 0, taxPaid = 0, resultSum = amount;
  if (size) {
    for (size_t i = 0; capitalism && i < size; i++) amount += RepWithValues[i];

    controller_.startCalculationDeposit(
        amount, term, rateType, daysOrAmountRate, rateCounter, size,
        interestRate, taxRate, period, capitalism, DatesInDays, RepWithValues,
        &AccRate, &taxPaid, &resultSum);
  } else {
    controller_.startCalculationDeposit(
        amount, term, rateType, daysOrAmountRate, rateCounter, size,
        interestRate, taxRate, period, capitalism, NULL, NULL, &AccRate,
        &taxPaid, &resultSum);
  }

  QMessageBox::information(
      this, "Result",
      "Accrued interest: " +
          (AccRate < 0 ? "0.00" : QString::number((double)AccRate, 'f', 2)) +
          "\n" + "Tax amount: " +
          (taxPaid < 0 ? "0.00" : QString::number((double)taxPaid, 'f', 2)) +
          "\n" + "Amount on account: " +
          (resultSum < 0 ? "0.00"
                         : QString::number((double)resultSum, 'f', 2)) +
          "\n");
}

void ViewDeposit::SortAndDelDuples(QDate *RepWithDates,
                                   long double *RepWithValues, size_t *size) {
  for (size_t i = 0; i < *size; i++) {
    for (size_t j = i + 1; j < *size; j++) {
      QDate temp = current;
      long double tmp = 0;
      if (RepWithDates[j] < RepWithDates[i]) {
        temp = RepWithDates[i];
        RepWithDates[i] = RepWithDates[j];
        RepWithDates[j] = temp;

        tmp = RepWithValues[i];
        RepWithValues[i] = RepWithValues[j];
        RepWithValues[j] = tmp;
      }
    }
  }

  for (size_t counter1 = 0; counter1 < *size; counter1++) {
    for (size_t counter2 = counter1 + 1; counter2 < *size; counter2++) {
      if (RepWithDates[counter1] == RepWithDates[counter2]) {
        RepWithValues[counter1] += RepWithValues[counter2];
        for (size_t counter_shift = counter2; counter_shift < *size - 1;
             counter_shift++) {
          RepWithDates[counter_shift] = RepWithDates[counter_shift + 1];
          RepWithValues[counter_shift] = RepWithValues[counter_shift + 1];
        }
        (*size)--;
        if (RepWithDates[counter1] == RepWithDates[counter2]) {
          counter2--;
        }
      }
    }
  }
}

bool ViewDeposit::RateFill(int64_t *daysOrAmountRate, size_t rateCounter,
                           long double *interestRate) {
  bool ret = true;
  daysOrAmountRate[0] = ui_->lineEdit_depends->text().toLongLong();
  QVBoxLayout *vbox = qobject_cast<QVBoxLayout *>(ui_->widgets_frame->layout());
  for (size_t i = 1; i < rateCounter; i++) {
    QLayout *hb = vbox->itemAt(i)->layout();
    QWidget *am = hb->itemAt(0)->widget();
    QLineEdit *ame = dynamic_cast<QLineEdit *>(am);
    QWidget *rat = hb->itemAt(1)->widget();
    QLineEdit *rate = dynamic_cast<QLineEdit *>(rat);
    if ((ame->text() == "") || (rate->text() == "") ||
        (ui_->lineEdit_depends->text() == ""))
      ret = false;
    daysOrAmountRate[i] = ame->text().toLongLong();
    interestRate[i + 1] = rate->text().toDouble();
  }

  for (size_t i = 0; i < rateCounter; i++) {
    for (size_t j = i + 1; j < rateCounter; j++) {
      int64_t temp;
      long double tmp = 0;
      if (daysOrAmountRate[j] < daysOrAmountRate[i]) {
        temp = daysOrAmountRate[i];
        daysOrAmountRate[i] = daysOrAmountRate[j];
        daysOrAmountRate[j] = temp;

        tmp = interestRate[i + 1];
        interestRate[i + 1] = interestRate[j + 1];
        interestRate[j + 1] = tmp;
      }
    }
  }
  return ret;
}

bool ViewDeposit::RepAndWithFill(size_t count, QDate *RepWithDates,
                                 long double *RepWithValues,
                                 QVBoxLayout *vbox_rep, size_t *toI,
                                 bool withdraw) {
  bool ret = true;
  for (size_t i = 1; i < count + 1; i++) {
    QLayout *hbox_rep = vbox_rep->itemAt(i)->layout();
    QWidget *dat = hbox_rep->itemAt(0)->widget();
    QDateEdit *date = dynamic_cast<QDateEdit *>(dat);
    QWidget *amo = hbox_rep->itemAt(1)->widget();
    QLineEdit *amn = dynamic_cast<QLineEdit *>(amo);
    if (amn->text() == "") ret = false;
    RepWithDates[*toI - 1] = date->date();
    RepWithValues[*toI - 1] =
        withdraw ? -amn->text().toDouble() : amn->text().toDouble();
    (*toI)++;
  }
  return ret;
}

void ViewDeposit::setPeriod(int64_t term, short *period) {
  if (ui_->pushButton_periodType->text().contains("Everyday")) *period = 1;
  if (ui_->pushButton_periodType->text().contains("Every week")) *period = 7;
  if (ui_->pushButton_periodType->text().contains("Once a month")) *period = 30;
  if (ui_->pushButton_periodType->text().contains("Once a quarter"))
    *period = 365 / 4;
  if (ui_->pushButton_periodType->text().contains("Semiannually"))
    *period = 365 / 2;
  if (ui_->pushButton_periodType->text().contains("Once a year")) *period = 365;
  if (ui_->pushButton_periodType->text().contains("At the end of the term"))
    *period = term;
}

void ViewDeposit::on_pushButton_termType_clicked() {
  ui_->listWidget_months_years->show();
  connect(ui_->listWidget_months_years,
          SIGNAL(itemDoubleClicked(QListWidgetItem *)), this,
          SLOT(itemDClickedTERM()));
}

void ViewDeposit::itemDClickedTERM() {
  ui_->listWidget_months_years->hide();
  ui_->pushButton_termType->setText(
      ui_->listWidget_months_years->currentItem()->text() + " ↓");
}

void ViewDeposit::on_pushButton_rateType_clicked() {
  ui_->listWidget_rateType->show();
  connect(ui_->listWidget_rateType,
          SIGNAL(itemDoubleClicked(QListWidgetItem *)), this,
          SLOT(itemDClickedRATE()));
}

void ViewDeposit::itemDClickedRATE() {
  ui_->pushButton_rateType->setText(
      ui_->listWidget_rateType->currentItem()->text() + " ↓");
  if (ui_->listWidget_rateType->currentItem()->text().at(0) == 'F') {
    ui_->groupBox->move(10, 210);
    ui_->listWidget_rateType->hide();
    ui_->pushButton_rateType->setFixedWidth(100);
    ui_->text_rateSym->move(310, 190);
    ui_->lineEdit_rate->move(240, 190);
    ui_->lineEdit_depends->hide();
    ui_->text_dependsOut->hide();
    ui_->pushButton_addMore->hide();
  } else if (ui_->listWidget_rateType->currentItem()->text().contains(
                 "amount")) {
    ui_->groupBox->move(10, 248);
    ui_->listWidget_rateType->hide();
    ui_->pushButton_rateType->setFixedWidth(200);
    ui_->text_rateSym->move(306, 210);
    ui_->lineEdit_rate->move(311, 240);
    ui_->lineEdit_depends->show();
    ui_->text_dependsOut->setText("Amount with which the rate is valid");
    ui_->text_dependsOut->setFontWeight(9);
    ui_->text_dependsOut->show();
    ui_->pushButton_addMore->show();
  } else if (ui_->listWidget_rateType->currentItem()->text().contains("term")) {
    ui_->groupBox->move(10, 248);
    ui_->listWidget_rateType->hide();
    ui_->pushButton_rateType->setFixedWidth(200);
    ui_->text_rateSym->move(306, 210);
    ui_->lineEdit_rate->move(311, 240);
    ui_->lineEdit_depends->show();
    ui_->text_dependsOut->setText(
        "Number of the day from which the rate is valid");
    ui_->text_dependsOut->setFontWeight(9);
    ui_->text_dependsOut->show();
    ui_->pushButton_addMore->show();
  }
}

void ViewDeposit::on_pushButton_addMore_clicked() {
  if (this->height() < 1300) {
    QVBoxLayout *layout =
        qobject_cast<QVBoxLayout *>(ui_->widgets_frame->layout());
    QHBoxLayout *newLayout = new QHBoxLayout(ui_->widgets_frame);

    QLineEdit *depends_new = new QLineEdit(ui_->widgets_frame);
    depends_new->setStyleSheet(ui_->lineEdit_dep_amount->styleSheet());
    depends_new->setFixedSize(170, 30);
    depends_new->setFont(font);
    depends_new->setValidator(new QDoubleValidator(0, 1e10, 3, this));
    newLayout->addWidget(depends_new);

    QLineEdit *rate_new = new QLineEdit(ui_->widgets_frame);
    rate_new->setFixedSize(65, 30);
    rate_new->setStyleSheet(ui_->lineEdit_dep_amount->styleSheet());
    rate_new->setFont(font);
    rate_new->setValidator(new QDoubleValidator(0, 1e10, 3, this));
    newLayout->addWidget(rate_new);

    QPushButton *button = new QPushButton("X", ui_->widgets_frame);
    button->setFixedSize(20, 32);
    newLayout->addWidget(button);
    layout->addLayout(newLayout);

    mButtonToLayoutMap.insert(button, newLayout);

    ui_->widgets_frame->setFixedHeight(ui_->widgets_frame->height() + 14);
    this->setFixedHeight(this->height() + 42);
    ui_->groupBox->move(10,
                        ui_->widgets_frame->y() + ui_->widgets_frame->height());

    QObject::connect(button, &QPushButton::clicked, this,
                     &ViewDeposit::delOneRate);
    connect(ui_->listWidget_rateType,
            SIGNAL(itemDoubleClicked(QListWidgetItem *)), this,
            SLOT(delAllRate()));
  } else {
    ui_->pushButton_addMore->hide();
  }
}

void ViewDeposit::delOneRate() {
  QPushButton *button = qobject_cast<QPushButton *>(sender());
  QHBoxLayout *layout = mButtonToLayoutMap.take(button);

  while (layout->count() != 0) {
    QLayoutItem *item = layout->takeAt(0);
    delete item->widget();
    delete item;
  }
  delete layout;

  ui_->widgets_frame->setFixedHeight(ui_->widgets_frame->height() - 42);
  this->setFixedHeight(this->height() - 42);
  ui_->groupBox->move(10, ui_->groupBox->y() - 42);
  maxSize();
}

void ViewDeposit::delAllRate() {
  QVBoxLayout *vbox = qobject_cast<QVBoxLayout *>(ui_->widgets_frame->layout());
  while (!vbox->isEmpty()) {
    QLayout *hb = vbox->takeAt(vbox->count() - 1)->layout();
    while (!hb->isEmpty()) {
      QWidget *w = hb->takeAt(0)->widget();
      delete w;
    }
    delete hb;
  }
  ui_->widgets_frame->setFixedSize(291, 8);
  maxSize();
  this->setFixedHeight(ui_->groupBox->height() + ui_->groupBox->y());
}

void ViewDeposit::on_pushButton_periodType_clicked() {
  ui_->listWidget_period->show();
  connect(ui_->listWidget_period, SIGNAL(itemDoubleClicked(QListWidgetItem *)),
          this, SLOT(itemDClickedPERIOD()));
}

void ViewDeposit::itemDClickedPERIOD() {
  ui_->listWidget_period->hide();
  ui_->pushButton_periodType->setText(
      ui_->listWidget_period->currentItem()->text() + " ↓");
}

void ViewDeposit::on_pushButton_addMoreRep_clicked() {
  if (this->height() < 1300) {
    QVBoxLayout *V_layout =
        qobject_cast<QVBoxLayout *>(ui_->widgets_frame_rep->layout());
    QHBoxLayout *H_Layout = new QHBoxLayout(ui_->widgets_frame_rep);

    QPushButton *x = createWidgets(H_Layout, ui_->widgets_frame_rep);

    V_layout->addLayout(H_Layout);

    ui_->widgets_frame_rep->setFixedHeight(ui_->widgets_frame_rep->height() +
                                           42);
    this->setFixedHeight(this->height() + 42);
    ui_->groupBox->setFixedHeight(ui_->groupBox->height() + 42);
    ui_->groupBox_2->move(ui_->groupBox_2->x(), ui_->groupBox_2->y() + 42);
    ui_->pushButton_calculate->move(ui_->pushButton_calculate->x(),
                                    ui_->pushButton_calculate->y() + 42);

    QObject::connect(x, &QPushButton::clicked, this, &ViewDeposit::delOneRep);
  } else {
    ui_->pushButton_addMoreRep->hide();
  }
}

void ViewDeposit::delOneRep() {
  QPushButton *x = qobject_cast<QPushButton *>(sender());
  QHBoxLayout *layout = mButtonToLayoutMap.take(x);

  while (layout->count() != 0) {
    QLayoutItem *item = layout->takeAt(0);
    delete item->widget();
    delete item;
  }
  delete layout;
  ui_->widgets_frame_rep->setFixedHeight(ui_->widgets_frame_rep->height() - 42);
  this->setFixedHeight(this->height() - 42);
  ui_->groupBox->setFixedHeight(ui_->groupBox->height() - 42);
  ui_->groupBox_2->move(ui_->groupBox_2->x(), ui_->groupBox_2->y() - 42);
  ui_->pushButton_calculate->move(ui_->pushButton_calculate->x(),
                                  ui_->pushButton_calculate->y() - 42);
  maxSize();
}

void ViewDeposit::on_pushButton_addMoreWith_clicked() {
  if (this->height() < 1300) {
    QVBoxLayout *V_layout =
        qobject_cast<QVBoxLayout *>(ui_->widgets_frame_with->layout());
    QHBoxLayout *H_Layout = new QHBoxLayout(ui_->widgets_frame_with);

    QPushButton *x = createWidgets(H_Layout, ui_->widgets_frame_with);

    V_layout->addLayout(H_Layout);

    ui_->groupBox->setFixedHeight(ui_->groupBox->height() + 37);
    ui_->groupBox_2->setFixedHeight(ui_->groupBox_2->height() + 37);
    ui_->widgets_frame_with->setFixedHeight(ui_->widgets_frame_with->height() +
                                            37);
    this->setFixedHeight(this->height() + 37);
    ui_->pushButton_calculate->move(ui_->pushButton_calculate->x(),
                                    ui_->pushButton_calculate->y() + 37);
    QObject::connect(x, &QPushButton::clicked, this,
                     &::ViewDeposit::delOneWith);
  } else {
    ui_->pushButton_addMoreWith->hide();
  }
}

void ViewDeposit::delOneWith() {
  QPushButton *x = qobject_cast<QPushButton *>(sender());
  QHBoxLayout *layout = mButtonToLayoutMap.take(x);

  while (layout->count() != 0) {
    QLayoutItem *item = layout->takeAt(0);
    delete item->widget();
    delete item;
  }
  delete layout;
  ui_->groupBox->setFixedHeight(ui_->groupBox->height() - 37);
  ui_->groupBox_2->setFixedHeight(ui_->groupBox_2->height() - 37);
  ui_->widgets_frame_with->setFixedHeight(ui_->widgets_frame_with->height() -
                                          37);
  ui_->pushButton_calculate->move(ui_->pushButton_calculate->x(),
                                  ui_->pushButton_calculate->y() - 37);
  this->setFixedHeight(this->height() - 37);
  maxSize();
}

void ViewDeposit::maxSize() {
  if (this->height() < 1300) {
    if (!(ui_->pushButton_rateType->text().at(0) == 'F'))
      ui_->pushButton_addMore->show();
    ui_->pushButton_addMoreRep->show();
    ui_->pushButton_addMoreWith->show();
  } else {
    ui_->pushButton_addMore->hide();
    ui_->pushButton_addMoreRep->hide();
    ui_->pushButton_addMoreWith->hide();
  }
}

QPushButton *ViewDeposit::createWidgets(QHBoxLayout *&H_Layout,
                                        QFrame *&frame) {
  QDateEdit *calendar = new QDateEdit(frame);
  calendar->setFixedSize(120, 25);
  calendar->setCalendarPopup(true);
  calendar->setDisplayFormat("dd.MM.yyyy");
  calendar->setDate(current);
  calendar->setFont(font);
  calendar->setStyleSheet(ui_->dateEdit->styleSheet());
  QTextCharFormat fmt;
  fmt.setForeground(QBrush(Qt::black));
  calendar->calendarWidget()->setWeekdayTextFormat(Qt::Saturday, fmt);
  calendar->calendarWidget()->setWeekdayTextFormat(Qt::Sunday, fmt);
  calendar->calendarWidget()->setFirstDayOfWeek(Qt::Monday);
  H_Layout->addWidget(calendar);

  QLineEdit *line = new QLineEdit(frame);
  line->setFixedSize(130, 25);
  line->setStyleSheet(ui_->lineEdit_dep_amount->styleSheet());
  line->setFont(font);
  line->setValidator(new QDoubleValidator(0, 1e10, 3, this));
  line->setPlaceholderText("Amount");
  H_Layout->addWidget(line);

  QPushButton *x = new QPushButton("X", frame);
  x->setFixedSize(20, 32);
  H_Layout->addWidget(x);
  mButtonToLayoutMap.insert(x, H_Layout);
  return x;
}
