#include <gtest/gtest.h>

#include "../Calculator v2.0/Controller/Controller.h"

s21::Controller controller;

TEST(Calculator, SumMinus1) {
  std::string exp = "-1+2+3-4+(-5)+6+7-8-9";
  long double answer = controller.startCalculation(exp, 0);

  EXPECT_EQ(answer, -9);
}

TEST(Calculator, SumMinus2) {
  std::string exp = "9-(-0.30928)*3-4";
  long double answer = controller.startCalculation(exp, 0);

  EXPECT_DOUBLE_EQ(answer, 5.92784);
}

TEST(Calculator, SumFunc) {
  std::string exp = "(132+sin(asin(sqrt(ln(log(228.11)))))-4*5^6*(123))";
  long double answer = controller.startCalculation(exp, 0);

  EXPECT_DOUBLE_EQ(answer, -7687367.0737845786);
}

TEST(Calculator, MultByBracket) {
  std::string exp =
      "2cos(1) + 3sin(2) - 4tan(1) + 2acos(0.32) - 3asin(0.2) + 4atan(2) - "
      "4ln(10) + 7log(99) / 4x";
  long double answer = controller.startCalculation(exp, 10);

  EXPECT_DOUBLE_EQ(answer, 29.606796314880558);
}

TEST(Calculator, Long) {
  std::string exp =
      "15/(7-(1+1))*3-(2+(1+1))*15/(7-(200+1))*3-(2+(1+1))*(15/"
      "(7-(1+1))*3-(2+(1+1))+15/(7-(1+1))*3-(2+(1+1)))";
  long double answer = controller.startCalculation(exp, 0);

  EXPECT_DOUBLE_EQ(answer, -30.072164948453608);
}

TEST(Calculator, Functions) {
  std::string exp =
      "sin(1)+cos(1)+asin(1)+acos(1)+atan(1)+tan(1)+sqrt(16)+ln(10)+log(10)";
  long double answer = controller.startCalculation(exp, 0);

  EXPECT_DOUBLE_EQ(answer, 12.597960598517329);
}

TEST(Calculator, Power) {
  std::string exp = "2^2^3 * 4";
  long double answer = controller.startCalculation(exp, 0);

  EXPECT_EQ(answer, 1024);
}

TEST(Calculator, Mod) {
  std::string exp = "((2 * 5437 mod 521 * 224 mod (3 + 2)) * 2000 / 500) mod 3";
  long double answer = controller.startCalculation(exp, 0);

  EXPECT_EQ(answer, 1);
}

TEST(Calculator, X) {
  std::string exp = "2x(2x+1) + 4";
  long double answer = controller.startCalculation(exp, 3.15);

  EXPECT_DOUBLE_EQ(answer, 49.989999999999995);
}

TEST(Calculator, InvalidTest1) {
  std::string exp = "sin(1) * cos)(";
  EXPECT_ANY_THROW(controller.startCalculation(exp, 0));

  exp = "__trash/";
  EXPECT_ANY_THROW(controller.startCalculation(exp, 0));

  exp = "2--2**12*//4mod*31";
  EXPECT_ANY_THROW(controller.startCalculation(exp, 0));

  exp = "2-2)";
  EXPECT_ANY_THROW(controller.startCalculation(exp, 0));

  exp = "2...5132 - 1.24.12";
  EXPECT_ANY_THROW(controller.startCalculation(exp, 0));

  exp = "";
  EXPECT_ANY_THROW(controller.startCalculation(exp, 0));
}

TEST(Calculator, InvalidTest2) {
  std::string exp = "siren";
  EXPECT_ANY_THROW(controller.startCalculation(exp, 0));

  exp = "thanos";
  EXPECT_ANY_THROW(controller.startCalculation(exp, 0));

  exp = "call me";
  EXPECT_ANY_THROW(controller.startCalculation(exp, 0));

  exp = "lol";
  EXPECT_ANY_THROW(controller.startCalculation(exp, 0));

  exp = "apple";
  EXPECT_ANY_THROW(controller.startCalculation(exp, 0));

  exp = "my man";
  EXPECT_ANY_THROW(controller.startCalculation(exp, 0));

  exp = "xor is ^";
  EXPECT_ANY_THROW(controller.startCalculation(exp, 0));
}

TEST(Calculator, InvalidTest3) {
  std::string exp = "2*^3";
  EXPECT_ANY_THROW(controller.startCalculation(exp, 0));

  exp = "33^mod^33";
  EXPECT_ANY_THROW(controller.startCalculation(exp, 0));

  exp = "11^/22/^11";
  EXPECT_ANY_THROW(controller.startCalculation(exp, 0));

  exp = "sqrt(15)253";
  EXPECT_ANY_THROW(controller.startCalculation(exp, 0));

  exp = "sqrt()";
  EXPECT_ANY_THROW(controller.startCalculation(exp, 0));
}

TEST(Credit, Annuity) {
  long double sum = 1500000;
  int64_t term = 33;
  double percent = 22.3, payment[term], overpay[term], fullsum[term];
  bool type = true;
  controller.startCalculationCredit(sum, term, percent, type, payment, overpay,
                                    fullsum);

  EXPECT_FLOAT_EQ(payment[0], 61215.94);
  EXPECT_FLOAT_EQ(overpay[term - 1], 520126.02);
  EXPECT_FLOAT_EQ(fullsum[term - 1], 2020126.02);
}

TEST(Credit, Differ) {
  long double sum = 1000000;
  int64_t term = 21;
  double percent = 9.4, payment[term], overpay[term], fullsum[term];
  bool type = false;
  controller.startCalculationCredit(sum, term, percent, type, payment, overpay,
                                    fullsum);

  EXPECT_FLOAT_EQ(payment[0], 55452.38);
  EXPECT_FLOAT_EQ(payment[term - 1], 47992.06);
  EXPECT_FLOAT_EQ(overpay[term - 1], 86166.67);
  EXPECT_FLOAT_EQ(fullsum[term - 1], 1086166.67);
}

TEST(Credit, Loan) {
  long double sum = 16643;
  int64_t term = 36;
  double percent = 7.9, credit = 0., overpay = 0., fullsum = 0.;
  controller.startCalculationLoan(sum, term, percent, credit, overpay, fullsum);

  EXPECT_FLOAT_EQ(credit, 531890.86);
  EXPECT_FLOAT_EQ(overpay, 67257.14);
  EXPECT_FLOAT_EQ(fullsum, 599148.00);
}

TEST(Credit, Term) {
  long double sum = 700000;
  int64_t term = 0;
  double percent = 9.9, monthly = 33333, overpay, fullsum;
  controller.startCalculationTerm(sum, monthly, percent, term, overpay,
                                  fullsum);

  EXPECT_EQ(term, 24);
  EXPECT_FLOAT_EQ(overpay, 71884.08);
  EXPECT_FLOAT_EQ(fullsum, 771884.08);

  monthly = 3333;
  controller.startCalculationTerm(sum, monthly, percent, term, overpay,
                                  fullsum);
  EXPECT_EQ(term, 210);
  EXPECT_FLOAT_EQ(overpay, 0);
  EXPECT_FLOAT_EQ(fullsum, 700000);
}

TEST(Deposit, One) {
  long double amount = 340000, interestRate[] = {0, 14.3, 17, 25}, taxRate = 8,
              RepWithValues[] = {-3000, 6000, 5000, -10000, -20000},
              AccRate = 0, taxPaid = 0, resultSum = amount;
  int64_t term = 333, daysOrAmountRate[] = {1, 50, 150},
          DatesInDays[] = {4, 30, 50, 70, 150};
  bool rateType = true, capitalism = true;
  size_t rateCounter = 3, size = 5;
  short period = 1;
  for (size_t i = 0; capitalism && i < size; i++) amount += RepWithValues[i];
  controller.startCalculationDeposit(
      amount, term, rateType, daysOrAmountRate, rateCounter, size, interestRate,
      taxRate, period, capitalism, DatesInDays, RepWithValues, &AccRate,
      &taxPaid, &resultSum);

  EXPECT_FLOAT_EQ(AccRate, 68636.46);
  EXPECT_FLOAT_EQ(taxPaid, 0.00);
  EXPECT_FLOAT_EQ(resultSum, 386636.46);
}

TEST(Deposit, Two) {
  long double amount = 700000, interestRate[] = {0, 25, 45, 35}, taxRate = 5,
              RepWithValues[] = {10000, -6000, -5000, -10000, 20000},
              AccRate = 0, taxPaid = 0, resultSum = amount;
  int64_t term = 400, daysOrAmountRate[] = {700000, 800000, 1000000},
          DatesInDays[] = {10, 50, 100, 150, 200};
  bool rateType = false, capitalism = true;
  size_t rateCounter = 3, size = 5;
  short period = 1;
  for (size_t i = 0; capitalism && i < size; i++) amount += RepWithValues[i];
  controller.startCalculationDeposit(
      amount, term, rateType, daysOrAmountRate, rateCounter, size, interestRate,
      taxRate, period, capitalism, DatesInDays, RepWithValues, &AccRate,
      &taxPaid, &resultSum);

  EXPECT_FLOAT_EQ(AccRate, 321408.00);
  EXPECT_FLOAT_EQ(taxPaid, 35283.05);
  EXPECT_FLOAT_EQ(resultSum, 1030408.10);
}

TEST(Deposit, Three) {
  long double amount = 700000, interestRate[] = {0, 25, 45, 35}, taxRate = 5,
              RepWithValues[] = {10000, -6000, -5000, -10000, 20000},
              AccRate = 0, taxPaid = 0, resultSum = amount;
  int64_t term = 400, daysOrAmountRate[] = {700000, 800000, 1000000},
          DatesInDays[] = {10, 50, 100, 150, 200};
  bool rateType = false, capitalism = false;
  size_t rateCounter = 3, size = 5;
  short period = 1;
  for (size_t i = 0; capitalism && i < size; i++) amount += RepWithValues[i];
  controller.startCalculationDeposit(
      amount, term, rateType, daysOrAmountRate, rateCounter, size, interestRate,
      taxRate, period, capitalism, DatesInDays, RepWithValues, &AccRate,
      &taxPaid, &resultSum);

  EXPECT_FLOAT_EQ(AccRate, 193019.86);
  EXPECT_FLOAT_EQ(taxPaid, 18592.58);
  EXPECT_FLOAT_EQ(resultSum, 709000.00);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
