#include "Model.h"

namespace s21 {
void Model::depositCalc(ld& amount, int64_t& termDays, bool& rateType,
                        int64_t* daysOrAmountRate, size_t& rateCounter,
                        size_t& size, ld* interestRate, ld& taxRate,
                        short& period, bool& capitalism, int64_t* DatesInDays,
                        ld* RepWithValues, ld* AccRate, ld* taxPaid,
                        ld* resultSum) {
  ld noTaxes = 1e6 * taxRate / 100.0;
  int64_t lastAmount = daysOrAmountRate[rateCounter - 1];
  size_t cnt = 0;
  ld lastRate = interestRate[rateCounter];
  termDays = rateType ? termDays - 1 : termDays;
  for (int64_t i = 1; i <= termDays; i++) {
    if (RepWithValues && DatesInDays && cnt < size &&
        (i == *DatesInDays || *DatesInDays == 0)) {
      *resultSum += *RepWithValues;
      DatesInDays++, RepWithValues++;
      cnt++;
    }
    if ((i == *daysOrAmountRate && rateType) ||
        (*resultSum >= *daysOrAmountRate && *interestRate != lastRate &&
         !rateType)) {
      if (*daysOrAmountRate == lastAmount) {
        interestRate++;
      } else {
        daysOrAmountRate++, interestRate++;
      }
    }
    if ((i >=
             (rateCounter == 1 ? *daysOrAmountRate : *(daysOrAmountRate - 1)) &&
         rateType) ||
        (((*resultSum <= *daysOrAmountRate) ||
          (*resultSum >= *daysOrAmountRate &&
           *daysOrAmountRate == lastAmount)) &&
         !rateType)) {
      if (capitalism) {
        *AccRate += *resultSum / 100.0 * *interestRate / 365.0;
        if (i % period == 0) {
          *resultSum += *AccRate;
          *AccRate = 0;
        }
        if (i == termDays) {
          *resultSum +=
              *resultSum / 100.0 * *interestRate / 365.0 * (termDays % period);
        }
      } else {
        *AccRate += *resultSum / 100.0 * *interestRate / 365.0;
      }
    }
  }
  if (capitalism) *AccRate = *resultSum - amount;
  if (taxRate != 0 && *AccRate > noTaxes)
    *taxPaid = (*AccRate - noTaxes) * 13.0 / 100.0;
}
}  // namespace s21
