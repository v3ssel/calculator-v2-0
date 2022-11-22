#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "../Model/Model.h"

namespace s21 {
class Controller {
 public:
  ld startCalculation(std::string& exp, const ld& x);

  void startCalculationCredit(ld&, int64_t&, double&, bool&, double*, double*,
                              double*);
  void startCalculationLoan(ld&, int64_t&, double&, double&, double&, double&);
  void startCalculationTerm(ld&, double&, double&, int64_t&, double&, double&);

  void startCalculationDeposit(ld&, int64_t&, bool&, int64_t*, size_t&, size_t&,
                               ld*, ld&, short&, bool&, int64_t*, ld*, ld*, ld*,
                               ld*);

 private:
  Model model;
};
}  // namespace s21

#endif  // CONTROLLER_H
