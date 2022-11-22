#include "Controller.h"

namespace s21 {
ld Controller::startCalculation(std::string& exp, const ld& x) {
  ld value = 2.0;
  if (!model.entryPoint(exp, value, x))
    throw std::invalid_argument("Invalid argument, calculation impossible");
  return value;
}

void Controller::startCalculationCredit(ld& sum, int64_t& term, double& percent,
                                        bool& type, double* payment,
                                        double* overpay, double* fullsum) {
  model.calcMonthly(sum, term, percent, type, payment, overpay, fullsum);
}

void Controller::startCalculationLoan(ld& sum, int64_t& term, double& percent,
                                      double& credit, double& overpay,
                                      double& fullsum) {
  model.calcLoan(sum, term, percent, credit, overpay, fullsum);
}

void Controller::startCalculationTerm(ld& sum, double& monthly, double& percent,
                                      int64_t& term, double& overpay,
                                      double& fullsum) {
  model.calcTerm(sum, monthly, percent, term, overpay, fullsum);
}

void Controller::startCalculationDeposit(
    ld& amount, int64_t& termDays, bool& rateType, int64_t* daysOrAmountRate,
    size_t& rateCounter, size_t& size, ld* interestRate, ld& taxRate,
    short& period, bool& capitalism, int64_t* DatesInDays, ld* RepWithValues,
    ld* AccRate, ld* taxPaid, ld* resultSum) {
  model.depositCalc(amount, termDays, rateType, daysOrAmountRate, rateCounter,
                    size, interestRate, taxRate, period, capitalism,
                    DatesInDays, RepWithValues, AccRate, taxPaid, resultSum);
}
}  // namespace s21
