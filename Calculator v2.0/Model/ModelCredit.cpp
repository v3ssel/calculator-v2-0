#include "Model.h"

namespace s21 {
void Model::calcMonthly(ld& sum, int64_t& term, double percent, bool& type,
                        double* payment, double* overpay, double* fullsum) {
  percent = percent / 100.0 / 12.0;
  if (type) {
    double annuity =
        sum * (percent / (1 - std::pow((ld)(1 + percent), (ld)(-term))));
    annuity = std::round((ld)(annuity * 100)) / 100.0;
    for (int64_t i = 0; i < term; i++) {
      payment[i] = annuity;
      fullsum[i] = payment[i] * (i + 1);
      overpay[i] = fullsum[i] - sum / term * (i + 1);
    }
  } else {
    double clearMonthly = sum / (double)term;
    double save_sum = sum;
    for (int64_t i = 0; i < term; i++) {
      payment[i] = clearMonthly + sum * percent;
      if (!i)
        fullsum[i] = payment[i];
      else
        fullsum[i] = fullsum[i - 1] + payment[i];
      overpay[i] = fullsum[i] - save_sum / term * (i + 1);
      sum -= clearMonthly;
    }
  }
}

void Model::calcLoan(ld& sum, int64_t& term, double& percent, double& credit,
                     double& overpay, double& fullsum) {
  credit = sum * ((1 - (std::pow((ld)(1 + (percent / 1200)), (ld)(-term)))) /
                  (ld)(percent / 1200));
  fullsum = term * sum;
  overpay = fullsum - credit;
}

void Model::calcTerm(ld sum, double& monthly, double& percent, int64_t& term,
                     double& overpay, double& fullsum) {
  term = 0;
  overpay = 0;
  fullsum = sum;
  if (percent && ((sum * (100 + percent / 12) / 100)) - sum < monthly) {
    while (sum >= 0) {
      overpay += monthly - (sum - (sum * (100 + percent / 12) / 100 - monthly));
      sum = sum * (100 + percent / 12) / 100 - monthly;
      term++;
    }
    fullsum += overpay;
  } else {
    term = sum / monthly;
  }
}
}  // namespace s21
