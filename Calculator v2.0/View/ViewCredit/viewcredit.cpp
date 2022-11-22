// by stevenso aka v3ssel
#include "../ViewCalc/viewcalc.h"
#include "ui_credit.h"

ViewCredit::ViewCredit(QWidget* parent) : QDialog(parent), ui_(new Ui::credit) {
  ui_->setupUi(this);
  ui_->listWidget_type->addItem("Monthly payment");
  ui_->listWidget_type->addItem("Credit term");
  ui_->listWidget_type->addItem("Maximum loan amount");
  ui_->listWidget_months_years->addItem("Months");
  ui_->listWidget_months_years->addItem("Years");
  ui_->lineEdit_amount->setValidator(new QDoubleValidator(0, 1e10, 3, this));
  ui_->lineEdit_credit->setValidator(new QDoubleValidator(0, 1e10, 3, this));
  ui_->lineEdit_rate->setValidator(new QDoubleValidator(1, 1e10, 3, this));
  ui_->lineEdit_monthly_pay->setValidator(
      new QDoubleValidator(0, 1e10, 3, this));
  ui_->lineEdit_monthly_pay_loan->setValidator(
      new QDoubleValidator(0, 1e10, 3, this));
  hideall();
}

ViewCredit::~ViewCredit() { delete ui_; }

void ViewCredit::hideall() {
  ui_->listWidget_payment_output->hide();
  ui_->pushButton_months->setText("Months ↓");
  ui_->radioButton_annuity->setChecked(true);
  ui_->listWidget_months_years->hide();
  ui_->listWidget_type->hide();
  ui_->pushButton_months->hide();
  ui_->typeOfMonthly->hide();
  ui_->amount->hide();
  ui_->interest_rate->hide();
  ui_->credit_term->hide();
  ui_->radioButton_annuity->hide();
  ui_->radioButton_differ->hide();
  ui_->lineEdit_amount->hide();
  ui_->lineEdit_credit->hide();
  ui_->lineEdit_rate->hide();
  ui_->rub->hide();
  ui_->percent->hide();
  ui_->pushButton_calculate_monthly->hide();
  ui_->pushButton_calculate_credit->hide();
  ui_->pushButton_calculate_loan->hide();
  ui_->rub_2->hide();
  ui_->credit_monthly_pay->hide();
  ui_->lineEdit_monthly_pay->hide();
  ui_->credit_monthly_pay_loan->hide();
  ui_->lineEdit_monthly_pay_loan->hide();
  ui_->line->hide();
  ui_->text_monthlyOut->hide();
  ui_->text_total_over->hide();
  ui_->text_total_payout->hide();
  ui_->lineEdit_overpay->hide();
  ui_->lineEdit_payout->hide();
  ui_->text_total_pay_term->hide();
  ui_->text_total_over_term->hide();
  ui_->text_total_term->hide();
  ui_->lineEdit_overpay_term->hide();
  ui_->lineEdit_fullsum_term->hide();
  ui_->lineEdit_term->hide();
  ui_->text_total_credit->hide();
  ui_->line->hide();
  ui_->line_2->hide();
}

void ViewCredit::on_pushButton_back_clicked() { this->close(); }

void ViewCredit::on_pushButton_choose_type_clicked() {
  ui_->pushButton_choose_type->hide();
  ui_->listWidget_type->show();
  connect(ui_->listWidget_type, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
          this, SLOT(itemClicked(QListWidgetItem*)));
}

void ViewCredit::itemClicked(QListWidgetItem* listWidgetItem) {
  ui_->pushButton_choose_type->setText(listWidgetItem->text() + " ↓");
  ui_->listWidget_type->hide();
  ui_->pushButton_choose_type->show();
  if (!QString::compare("Monthly payment",
                        ui_->listWidget_type->currentItem()->text())) {
    show_monthly();
  } else if (!QString::compare("Credit term",
                               ui_->listWidget_type->currentItem()->text())) {
    show_credit();
  } else {
    show_loan();
  }
}

void ViewCredit::show_monthly() {
  hideall();
  ui_->pushButton_months->show();
  ui_->typeOfMonthly->show();
  ui_->amount->show();
  ui_->interest_rate->show();
  ui_->credit_term->show();
  ui_->radioButton_annuity->show();
  ui_->radioButton_differ->show();
  ui_->lineEdit_amount->show();
  ui_->lineEdit_credit->show();
  ui_->lineEdit_rate->show();
  ui_->rub->show();
  ui_->percent->show();
  ui_->pushButton_calculate_monthly->show();
  ui_->line->show();
  ui_->lineEdit_amount->setText("");
  ui_->lineEdit_credit->setText("");
  ui_->lineEdit_rate->setText("");
  setFixedSize(415, 600);
}

void ViewCredit::on_pushButton_calculate_monthly_clicked() {
  ui_->listWidget_payment_output->hide();
  ui_->listWidget_payment_output->clear();
  if (QString::compare(ui_->lineEdit_amount->text(), "") &&
      QString::compare(ui_->lineEdit_credit->text(), "") &&
      QString::compare(ui_->lineEdit_rate->text(), "")) {
    long double sum = ui_->lineEdit_amount->text().toDouble();
    int64_t term = ui_->lineEdit_credit->text().toLongLong();

    if (ui_->pushButton_months->text().at(0) == 'Y') term *= 12;
    double rate = ui_->lineEdit_rate->text().toDouble();
    bool type = true;
    if (ui_->radioButton_differ->isChecked()) type = false;
    double payment[term], overpay[term], fullSum[term];

    controller_.startCalculationCredit(sum, term, rate, type, payment, overpay,
                                       fullSum);

    for (size_t i = 0; i < (size_t)term; i++) {
      ui_->listWidget_payment_output->addItem(
          QString::number(i + 1) + ". " + QString::number(payment[i], 'f', 2));
    }

    ui_->lineEdit_overpay->setText(QString::number(overpay[term - 1], 'f', 2));
    ui_->lineEdit_payout->setText(QString::number(fullSum[term - 1], 'f', 2));
    ui_->text_total_over->show();
    ui_->text_total_payout->show();
    ui_->lineEdit_overpay->show();
    ui_->lineEdit_payout->show();
    ui_->text_monthlyOut->show();

    ui_->listWidget_payment_output->show();
  }
}

void ViewCredit::on_pushButton_months_clicked() {
  ui_->pushButton_months->hide();
  ui_->listWidget_months_years->show();
  connect(ui_->listWidget_months_years,
          SIGNAL(itemDoubleClicked(QListWidgetItem*)), this,
          SLOT(itemClickedTwo(QListWidgetItem*)));
}

void ViewCredit::itemClickedTwo(QListWidgetItem* listWidgetItem) {
  ui_->pushButton_months->setText(listWidgetItem->text() + " ↓");
  ui_->listWidget_months_years->hide();
  ui_->pushButton_months->show();
}

void ViewCredit::show_credit() {
  hideall();
  ui_->amount->show();
  ui_->interest_rate->show();
  ui_->lineEdit_amount->show();
  ui_->lineEdit_monthly_pay->show();
  ui_->lineEdit_rate->show();
  ui_->rub->show();
  ui_->rub_2->show();
  ui_->credit_monthly_pay->show();
  ui_->percent->show();
  ui_->pushButton_calculate_credit->show();
  ui_->line_2->show();
  ui_->lineEdit_amount->setText("");
  ui_->lineEdit_monthly_pay->setText("");
  ui_->lineEdit_rate->setText("");
  setFixedSize(415, 500);
}

void ViewCredit::on_pushButton_calculate_credit_clicked() {
  if (QString::compare(ui_->lineEdit_amount->text(), "") &&
      QString::compare(ui_->lineEdit_monthly_pay->text(), "") &&
      QString::compare(ui_->lineEdit_rate->text(), "")) {
    ui_->lineEdit_term->setText("");

    long double sum = ui_->lineEdit_amount->text().toDouble();
    double pay = ui_->lineEdit_monthly_pay->text().toDouble();
    double rate = ui_->lineEdit_rate->text().toDouble();
    int64_t term = 0;
    double overpay = 0, payout = 0;

    controller_.startCalculationTerm(sum, pay, rate, term, overpay, payout);

    int64_t years = term / 12;
    if (years > 0) {
      ui_->lineEdit_term->insert(QString::number(years) + " year");
      if (years > 1) ui_->lineEdit_term->insert("s");
      ui_->lineEdit_term->insert(" ");
    }
    if (term % 12 != 0)
      ui_->lineEdit_term->insert(QString::number(term % 12) + " month");
    if (term % 12 > 1) ui_->lineEdit_term->insert("s");

    ui_->lineEdit_overpay_term->setText(QString::number(overpay, 'f', 2));
    ui_->lineEdit_fullsum_term->setText(QString::number(payout, 'f', 2));

    double first = (sum * (100 + rate / 12) / 100) - sum;
    if (first >= pay)
      QMessageBox::warning(this, "Error",
                           "Monthly payment (" + QString::number(pay) +
                               ") less than accrued interest (" +
                               QString::number(first) + ")");

    ui_->text_total_pay_term->show();
    ui_->text_total_over_term->show();
    ui_->text_total_term->show();
    ui_->lineEdit_overpay_term->show();
    ui_->lineEdit_fullsum_term->show();
    ui_->lineEdit_term->show();
  }
}

void ViewCredit::show_loan() {
  hideall();
  ui_->credit_monthly_pay_loan->show();
  ui_->lineEdit_monthly_pay_loan->show();
  ui_->credit_term->show();
  ui_->lineEdit_monthly_pay->show();
  ui_->pushButton_months->show();
  ui_->interest_rate->show();
  ui_->lineEdit_rate->show();
  ui_->percent->show();
  ui_->rub->show();
  ui_->pushButton_calculate_loan->show();
  ui_->line_2->show();
  ui_->lineEdit_monthly_pay_loan->setText("");
  ui_->lineEdit_monthly_pay->setText("");
  ui_->lineEdit_rate->setText("");
  ui_->lineEdit_overpay_term->setText("");
  ui_->lineEdit_fullsum_term->setText("");
  ui_->lineEdit_term->setText("");
  setFixedSize(415, 500);
}

void ViewCredit::on_pushButton_calculate_loan_clicked() {
  if (QString::compare(ui_->lineEdit_monthly_pay_loan->text(), "") &&
      QString::compare(ui_->lineEdit_monthly_pay->text(), "") &&
      QString::compare(ui_->lineEdit_rate->text(), "")) {
    long double sum = ui_->lineEdit_monthly_pay_loan->text().toDouble();
    int64_t term = ui_->lineEdit_monthly_pay->text().toLongLong();
    if (ui_->pushButton_months->text().at(0) == 'Y') term *= 12;
    double rate = ui_->lineEdit_rate->text().toDouble();
    double credit = 0, overpay = 0, payout = 0;

    controller_.startCalculationLoan(sum, term, rate, credit, overpay, payout);

    ui_->lineEdit_term->setText(QString::number(credit, 'f', 2));
    ui_->lineEdit_overpay_term->setText(QString::number(overpay, 'f', 2));
    ui_->lineEdit_fullsum_term->setText(QString::number(payout, 'f', 2));

    ui_->text_total_pay_term->show();
    ui_->text_total_over_term->show();
    ui_->text_total_credit->show();
    ui_->lineEdit_overpay_term->show();
    ui_->lineEdit_fullsum_term->show();
    ui_->lineEdit_term->show();
  }
}
