#include "Model.h"

namespace s21 {
bool Model::entryPoint(std::string& exp, ld& value, const ld& x) {
  bool return_value = true;
  value = 0.0;
  exp.erase(std::remove(exp.begin(), exp.end(), ' '), exp.end());
  if (parseExpresssion_(exp, x)) {
    stackReverse_();
    shuntingYard_();
    stackReverse_();
    calcus_();
    value = expression_.top().value;
  } else {
    return_value = false;
  }
  while (!expression_.empty()) expression_.pop();
  return return_value;
}

void Model::stackReverse_() {
  bool check = false;
  std::stack<data> reversed;
  while (1) {
    reversed.push({expression_.top().value, expression_.top().priority,
                   expression_.top().type});
    expression_.pop();
    if (check) break;
    if (expression_.size() == 1) check = true;
  }
  expression_ = std::move(reversed);
}

void Model::setVals_(ld& v1, ld& v2, std::stack<data>& s) {
  v1 = s.top().value;
  s.pop();
  v2 = s.top().value;
  s.pop();
}

bool Model::parseExpresssion_(const std::string& exp, const ld& x) {
  bool err_code = true;
  size_t RBCnt = 0, LBCnt = 0, func_cnt = 0;
  if (!exp.empty()) {
    size_t iOfLB = 0, iOfRB = 0, len = exp.length();
    for (size_t i = 0; i < len; i++) {
      if (isNumber(exp[i])) {
        if (!i ||
            ((!expression_.empty() && expression_.top().type == OPEN_BRACKET) ||
             (expression_.top().type >= PLUS &&
              expression_.top().type <= POWER))) {
          getFullNumber_(exp, i, len);
        } else {
          err_code = false;
        }
      } else {
        switch (exp[i]) {
          case '(':
            LBCnt++;
            if (!expression_.empty() && (expression_.top().type == NUMBER ||
                                         expression_.top().type == X))
              expression_.push({0, 2, MULT});
            expression_.push({0, -1, OPEN_BRACKET});
            iOfLB = i;
            break;
          case ')':
            RBCnt++;
            if (!expression_.empty() && expression_.top().type == OPEN_BRACKET)
              err_code = false;
            expression_.push({0, -1, CLOSED_BRACKET});
            iOfRB = i;
            break;
          case '+':
            basicOps_(exp, i, err_code, 0);
            break;
          case '-':
            basicOps_(exp, i, err_code, 1);
            break;
          case '*':
            basicOps_(exp, i, err_code, 2);
            break;
          case '/':
            basicOps_(exp, i, err_code, 3);
            break;
          case 'x':
            easyCases_('x', exp, i, func_cnt, err_code, x);
            break;
          case 'm':
            easyCases_('m', exp, i, func_cnt, err_code, 0);
            break;
          case '^':
            easyCases_('^', exp, i, func_cnt, err_code, 0);
            break;
          case 'c':
            easyCases_('c', exp, i, func_cnt, err_code, 0);
            break;
          case 't':
            easyCases_('t', exp, i, func_cnt, err_code, 0);
            break;
          case 's':
            hardCases_('s', exp, i, func_cnt, err_code);
            break;
          case 'a':
            hardCases_('a', exp, i, func_cnt, err_code);
            break;
          case 'l':
            hardCases_('l', exp, i, func_cnt, err_code);
            break;
          default:
            err_code = false;
            break;
        }
      }
      if (iOfRB < iOfLB && (LBCnt + RBCnt) % 2 == 0 && (LBCnt == RBCnt))
        err_code = false;
      if (i == NUMBER_ERR_CODE) err_code = false;
      if (!err_code) break;
    }
    expression_.push({0, 1, PLUS}), expression_.push({0, 0, NUMBER});
  } else {
    err_code = false;
  }
  lastChecker_(LBCnt, RBCnt, func_cnt, err_code);
  return err_code;
}

void Model::getFullNumber_(const std::string& exp, size_t& i, size_t len) {
  std::string num;
  size_t dot_counter = 0;
  for (; i < len; ++i) {
    if (exp[i] == '.') dot_counter++;
    if (isNumber(exp[i]) || exp[i] == '.') {
      num += exp[i];
    } else {
      --i;
      break;
    }
  }
  if (dot_counter > 1) {
    i = NUMBER_ERR_CODE;
    return;
  }
  long double n = std::stold(num);
  expression_.push({n, 0, NUMBER});
}

void Model::basicOps_(const std::string& exp, const size_t& i, bool& err_code,
                      const short&& sign) {
  if (sign < 2) {
    if (((!expression_.empty() &&
          !(expression_.top().type >= 4 && expression_.top().type <= 9)) ||
         !i) &&
        exp[i + 1] != '\0') {
      if (!i || exp[i - 1] == '(') expression_.push({0, 0, NUMBER});

      if (sign)
        expression_.push({0, 1, MINUS});
      else
        expression_.push({0, 1, PLUS});
    } else {
      err_code = false;
    }
  } else {
    if (!expression_.empty() &&
        (!isPrevNonOp(expression_.top().type) ||
         expression_.top().type == CLOSED_BRACKET) &&
        exp[i + 1] != '\0') {
      if (sign == 2)
        expression_.push({0, 2, MULT});
      else
        expression_.push({0, 2, DIV});
    } else {
      err_code = false;
    }
  }
}

void Model::easyCases_(const char& caseType, const std::string& exp, size_t& i,
                       size_t& func_cnt, bool& err_code, const ld& x) {
  if (caseType == 'c') {
    if (exp[i + 1] == 'o' && exp[i + 2] == 's') {
      if (!expression_.empty() && expression_.top().type == NUMBER)
        expression_.push({0, 2, MULT});
      expression_.push({0, 4, COS});
      i += 2;
      func_cnt++;
    } else {
      err_code = false;
    }
  }
  if (caseType == 't') {
    if (exp[i + 1] == 'a' && exp[i + 2] == 'n') {
      if (!expression_.empty() && expression_.top().type == NUMBER)
        expression_.push({0, 2, MULT});
      expression_.push({0, 4, TAN});
      i += 2;
      func_cnt++;
    } else {
      err_code = false;
    }
  }
  if (caseType == 'x') {
    if ((!isRight(exp[i + 1]) && !isRight(exp[i - 1])) || exp[i + 1] == 'm' ||
        exp[i - 1] == 'd') {
      if (!expression_.empty() &&
          (expression_.top().type == NUMBER || expression_.top().type == X))
        expression_.push({0, 2, MULT});
      expression_.push({x, 0, X});
    } else {
      err_code = false;
    }
  }
  if (caseType == '^') {
    if (!expression_.empty() &&
        (!isPrevNonOp(expression_.top().type) ||
         expression_.top().type == CLOSED_BRACKET) &&
        exp[i + 1] != '\0')
      expression_.push({0, 3, POWER});
    else
      err_code = false;
  }
  if (caseType == 'm') {
    if (exp[i + 1] == 'o' && exp[i + 2] == 'd' && !expression_.empty() &&
        exp[i + 1] != '\0')
      expression_.push({0, 2, MOD});
    else
      err_code = false;
    i += 2;
  }
}

void Model::hardCases_(const char& caseType, const std::string& exp, size_t& i,
                       size_t& func_cnt, bool& err_code) {
  if (caseType == 'l') {
    if (exp[i + 1] == 'n') {
      if (!expression_.empty() && expression_.top().type == NUMBER)
        expression_.push({0, 2, MULT});
      expression_.push({0, 4, LN});
      i += 1;
      func_cnt++;
    } else if (exp[i + 1] == 'o' && exp[i + 2] == 'g') {
      if (!expression_.empty() && expression_.top().type == NUMBER)
        expression_.push({0, 2, MULT});
      expression_.push({0, 4, LOG});
      i += 2;
      func_cnt++;
    } else {
      err_code = false;
    }
  }
  if (caseType == 'a') {
    if (exp[i + 1] == 's' && exp[i + 2] == 'i' && exp[i + 3] == 'n') {
      if (!expression_.empty() && expression_.top().type == NUMBER)
        expression_.push({0, 2, MULT});
      expression_.push({0, 4, ASIN});
      i += 3;
      func_cnt++;
    } else if (exp[i + 1] == 'c' && exp[i + 2] == 'o' && exp[i + 3] == 's') {
      if (!expression_.empty() && expression_.top().type == NUMBER)
        expression_.push({0, 2, MULT});
      expression_.push({0, 4, ACOS});
      i += 3;
      func_cnt++;
    } else if (exp[i + 1] == 't' && exp[i + 2] == 'a' && exp[i + 3] == 'n') {
      if (!expression_.empty() && expression_.top().type == NUMBER)
        expression_.push({0, 2, MULT});
      expression_.push({0, 4, ATAN});
      i += 3;
      func_cnt++;
    } else {
      err_code = false;
    }
  }
  if (caseType == 's') {
    if (exp[i + 1] == 'i' && exp[i + 2] == 'n') {
      if (!expression_.empty() && expression_.top().type == NUMBER)
        expression_.push({0, 2, MULT});
      expression_.push({0, 4, SIN});
      i += 2;
      func_cnt++;
    } else if (exp[i + 1] == 'q' && exp[i + 2] == 'r' && exp[i + 3] == 't') {
      expression_.push({0, 4, SQRT});
      i += 3;
      func_cnt++;
    } else {
      err_code = false;
    }
  }
}

void Model::lastChecker_(size_t& LBCnt, size_t& RBCnt, size_t& func_cnt,
                         bool& err_code) {
  if (LBCnt != RBCnt ||
      (!expression_.empty() &&
       (expression_.top().type != CLOSED_BRACKET &&
        expression_.top().type != NUMBER && expression_.top().type != X)) ||
      ((func_cnt * 2) > (LBCnt + RBCnt)))
    err_code = false;
}

void Model::shuntingYard_() {
  bool check = false;
  std::stack<data> operators;
  std::stack<data> output;
  while (1) {
    if (!expression_.empty()) {
      if (expression_.top().type == CLOSED_BRACKET) {
        expression_.pop();
        while (operators.top().type != OPEN_BRACKET) {
          output.push({operators.top().value, operators.top().priority,
                       operators.top().type});
          operators.pop();
        }
        operators.pop();
        if (!operators.empty() && operators.top().priority == 4) {
          output.push({operators.top().value, operators.top().priority,
                       operators.top().type});
          operators.pop();
        }
      } else {
        if (expression_.top().type == NUMBER || expression_.top().type == X) {
          output.push({expression_.top().value, expression_.top().priority,
                       expression_.top().type});
          expression_.pop();
        } else if (expression_.top().type != NUMBER &&
                   expression_.top().type != CLOSED_BRACKET) {
          while (!operators.empty()) {
            if (expression_.top().priority != -1 &&
                ((expression_.top().priority == 3 &&
                  expression_.top().priority < operators.top().priority) ||
                 (expression_.top().priority != 3 &&
                  expression_.top().priority <= operators.top().priority))) {
              output.push({operators.top().value, operators.top().priority,
                           operators.top().type});
              operators.pop();
            } else {
              break;
            }
          }
          operators.push({expression_.top().value, expression_.top().priority,
                          expression_.top().type});
          expression_.pop();
        }
      }
    }
    if (expression_.empty() || check) break;
    if (expression_.size() == 1) check = true;
  }
  if (!operators.empty()) {
    check = false;
    while (1) {
      output.push({operators.top().value, operators.top().priority,
                   operators.top().type});
      operators.pop();
      if (operators.empty() || check) break;
      if (operators.size() == 1) check = true;
    }
  }
  expression_ = std::move(output);
}

void Model::calcus_() {
  bool check = false;
  ld val1 = 0., val2 = 0.;
  std::stack<data> calculated;
  while (1) {
    switch (expression_.top().type) {
      case PLUS:
        expression_.pop();
        setVals_(val1, val2, calculated);
        calculated.push({val2 + val1, 0, NUMBER});
        break;
      case MINUS:
        expression_.pop();
        setVals_(val1, val2, calculated);
        calculated.push({val2 - val1, 0, NUMBER});
        break;
      case MULT:
        expression_.pop();
        setVals_(val1, val2, calculated);
        calculated.push({val2 * val1, 0, NUMBER});
        break;
      case DIV:
        expression_.pop();
        setVals_(val1, val2, calculated);
        calculated.push({val2 / val1, 0, NUMBER});
        break;
      case MOD:
        expression_.pop();
        setVals_(val1, val2, calculated);
        calculated.push({std::fmod(val2, val1), 0, NUMBER});
        break;
      case POWER:
        expression_.pop();
        setVals_(val1, val2, calculated);
        calculated.push({std::pow(val2, val1), 0, NUMBER});
        break;
      case SQRT:
        expression_.pop();
        val1 = calculated.top().value;
        calculated.pop();
        calculated.push({std::sqrt(val1), 0, NUMBER});
        break;
      case COS:
        expression_.pop();
        val1 = calculated.top().value;
        calculated.pop();
        calculated.push({std::cos(val1), 0, NUMBER});
        break;
      case SIN:
        expression_.pop();
        val1 = calculated.top().value;
        calculated.pop();
        calculated.push({std::sin(val1), 0, NUMBER});
        break;
      case TAN:
        expression_.pop();
        val1 = calculated.top().value;
        calculated.pop();
        calculated.push({std::tan(val1), 0, NUMBER});
        break;
      case ACOS:
        expression_.pop();
        val1 = calculated.top().value;
        calculated.pop();
        calculated.push({std::acos(val1), 0, NUMBER});
        break;
      case ASIN:
        expression_.pop();
        val1 = calculated.top().value;
        calculated.pop();
        calculated.push({std::asin(val1), 0, NUMBER});
        break;
      case ATAN:
        expression_.pop();
        val1 = calculated.top().value;
        calculated.pop();
        calculated.push({std::atan(val1), 0, NUMBER});
        break;
      case LN:
        expression_.pop();
        val1 = calculated.top().value;
        calculated.pop();
        calculated.push({std::log(val1), 0, NUMBER});
        break;
      case LOG:
        expression_.pop();
        val1 = calculated.top().value;
        calculated.pop();
        calculated.push({std::log10(val1), 0, NUMBER});
        break;
      default:
        break;
    }
    if (!expression_.empty() &&
        (expression_.top().type == NUMBER || expression_.top().type == X)) {
      calculated.push({expression_.top().value, expression_.top().priority,
                       expression_.top().type});
      expression_.pop();
    }
    if (expression_.empty() || check) break;
    if (expression_.size() == 1) check = true;
  }
  expression_ = std::move(calculated);
}
}  // namespace s21
