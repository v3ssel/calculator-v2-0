// by stevenso aka v3ssel
#include "viewcalc.h"

#include "ui_calc.h"

ViewCalc::ViewCalc(QWidget *parent) : QMainWindow(parent), ui_(new Ui::calc) {
  ui_->setupUi(this);
  auto validator = new MyValidator(parent);
  ui_->inputField->setValidator(validator);
  ui_->pushButton_less->hide();
  ui_->pushButton_def->hide();
  ui_->lineEdit_xMax->setValidator(new QDoubleValidator(this));
  ui_->lineEdit_xMin->setValidator(new QDoubleValidator(this));
  ui_->lineEdit_xValue->setValidator(new QDoubleValidator(this));
  on_pushButton_def_clicked();
  ui_->graphic->xAxis->setRange(-10, 10);
  ui_->graphic->yAxis->setRange(-10, 10);
  connect(ui_->inputField, SIGNAL(returnPressed()), this,
          SLOT(on_pushButton_start_clicked()));
}

ViewCalc::~ViewCalc() { delete ui_; }

void ViewCalc::on_pushButton_deposit_clicked() {
  dep_ = new ViewDeposit(this);
  dep_->setFixedSize(415, 510);
  dep_->setWindowTitle("Deposit Calculator");
  dep_->show();
}

void ViewCalc::on_pushButton_credit_clicked() {
  cred_ = new ViewCredit(this);
  cred_->setFixedSize(415, 600);
  cred_->setWindowTitle("Credit Calculator");
  cred_->show();
}

void ViewCalc::on_pushButton_upGraph_clicked() {
  ui_->graphic->clearGraphs();
  long double dxMax = ui_->lineEdit_xMax->text().toDouble();
  long double dxMin = ui_->lineEdit_xMin->text().toDouble();

  if (dxMax < dxMin || dxMin < -1000000)
    ui_->lineEdit_xMin->setText("Invalid axis");

  if (dxMax > 1000000) ui_->lineEdit_xMax->setText("Invalid axis");

  if (QString::compare("Invalid axis", ui_->lineEdit_xMax->text()) &&
      QString::compare("Invalid axis", ui_->lineEdit_xMin->text())) {
    ui_->graphic->setInteraction(QCP::iRangeZoom, true);
    ui_->graphic->setInteraction(QCP::iRangeDrag, true);

    QString q = ui_->inputField->text();
    q.replace(",", ".");
    q.replace("e-", "/10^");
    q.replace("e+", "*10^");
    std::string exp = q.toStdString();
    long double res = 0.0;

    for (double i = dxMin; i <= dxMax; i += 0.1) {
      x_.push_back(i);
      try {
        res = controller_.startCalculation(exp, i);
      } catch (...) {
        ui_->answerField->setText("Invalid expression");
        break;
      }
      y_.push_back(res);
    }

    ui_->graphic->addGraph();
    ui_->graphic->graph(0)->addData(x_, y_);
    ui_->graphic->graph(0)->setPen(QPen(Qt::black));
    ui_->graphic->replot();

    x_.clear();
    y_.clear();
  }
}

void ViewCalc::on_pushButton_def_clicked() {
  ui_->lineEdit_xMax->setText("100");
  ui_->lineEdit_xMin->setText("-100");
  ui_->graphic->xAxis->setRange(-10, 10);
  ui_->graphic->yAxis->setRange(-10, 10);
  ui_->graphic->replot();
}

void ViewCalc::on_pushButton_more_clicked() {
  setFixedSize(870, 575);
  ui_->pushButton_more->hide();
  ui_->pushButton_def->show();
  ui_->pushButton_less->show();
}

void ViewCalc::on_pushButton_less_clicked() {
  setFixedSize(440, 575);
  ui_->pushButton_less->hide();
  ui_->pushButton_def->hide();
  ui_->pushButton_more->show();
}

void ViewCalc::on_pushButton_start_clicked() {
  QString q = ui_->inputField->text();
  q.replace(",", ".");
  q.replace("e-", "/10^");
  q.replace("e+", "*10^");
  std::string exp = q.toStdString();
  long double value = 0.0, xValue = 0.0;
  if (ui_->lineEdit_xValue->text() != "")
    xValue = ui_->lineEdit_xValue->text().toDouble();

  try {
    value = controller_.startCalculation(exp, xValue);
    ui_->answerField->setText(QString::number((double)value, 'f', 7));
  } catch (...) {
    ui_->answerField->setText("Invalid expression");
  }
}

void ViewCalc::on_pushButton_help_clicked() {
  QMessageBox::information(this, "Calculator helper",
                           ui_->pushButton_help->toolTip());
}

void ViewCalc::on_pushButton_0_clicked() { ui_->inputField->insert("0"); }

void ViewCalc::on_pushButton_1_clicked() { ui_->inputField->insert("1"); }

void ViewCalc::on_pushButton_2_clicked() { ui_->inputField->insert("2"); }

void ViewCalc::on_pushButton_3_clicked() { ui_->inputField->insert("3"); }

void ViewCalc::on_pushButton_4_clicked() { ui_->inputField->insert("4"); }

void ViewCalc::on_pushButton_5_clicked() { ui_->inputField->insert("5"); }

void ViewCalc::on_pushButton_6_clicked() { ui_->inputField->insert("6"); }

void ViewCalc::on_pushButton_7_clicked() { ui_->inputField->insert("7"); }

void ViewCalc::on_pushButton_8_clicked() { ui_->inputField->insert("8"); }

void ViewCalc::on_pushButton_9_clicked() { ui_->inputField->insert("9"); }

void ViewCalc::on_pushButton_x_clicked() { ui_->inputField->insert("x"); }

void ViewCalc::on_pushButton_dot_clicked() { ui_->inputField->insert("."); }

void ViewCalc::on_pushButton_plus_clicked() { ui_->inputField->insert("+"); }

void ViewCalc::on_pushButton_minus_clicked() { ui_->inputField->insert("-"); }

void ViewCalc::on_pushButton_mult_clicked() { ui_->inputField->insert(" * "); }

void ViewCalc::on_pushButton_div_clicked() { ui_->inputField->insert(" / "); }

void ViewCalc::on_pushButton_mod_clicked() { ui_->inputField->insert(" mod "); }

void ViewCalc::on_pushButton_lBracket_clicked() {
  ui_->inputField->insert("(");
}

void ViewCalc::on_pushButton_rBracket_clicked() {
  ui_->inputField->insert(")");
}

void ViewCalc::on_pushButton_sin_clicked() {
  ui_->inputField->insert("sin()");
  ui_->inputField->cursorBackward(0, 1);
}

void ViewCalc::on_pushButton_cos_clicked() {
  ui_->inputField->insert("cos()");
  ui_->inputField->cursorBackward(0, 1);
}

void ViewCalc::on_pushButton_tan_clicked() {
  ui_->inputField->insert("tan()");
  ui_->inputField->cursorBackward(0, 1);
}

void ViewCalc::on_pushButton_asin_clicked() {
  ui_->inputField->insert("asin()");
  ui_->inputField->cursorBackward(0, 1);
}

void ViewCalc::on_pushButton_acos_clicked() {
  ui_->inputField->insert("acos()");
  ui_->inputField->cursorBackward(0, 1);
}

void ViewCalc::on_pushButton_atan_clicked() {
  ui_->inputField->insert("atan()");
  ui_->inputField->cursorBackward(0, 1);
}

void ViewCalc::on_pushButton_ln_clicked() {
  ui_->inputField->insert("ln()");
  ui_->inputField->cursorBackward(0, 1);
}

void ViewCalc::on_pushButton_log_clicked() {
  ui_->inputField->insert("log()");
  ui_->inputField->cursorBackward(0, 1);
}

void ViewCalc::on_pushButton_sqrt_clicked() {
  ui_->inputField->insert("sqrt()");
  ui_->inputField->cursorBackward(0, 1);
}

void ViewCalc::on_pushButton_power_clicked() { ui_->inputField->insert("^"); }

void ViewCalc::on_pushButton_clearInput_clicked() {
  ui_->inputField->setText("");
}

void ViewCalc::on_pushButton_clearAnswer_clicked() {
  ui_->answerField->setText("");
}

void ViewCalc::on_pushButton_allclear_clicked() {
  ui_->inputField->setText("");
  ui_->answerField->setText("");
}
