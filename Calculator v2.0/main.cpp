// by stevenso aka v3ssel
#include <QApplication>

#include "View/ViewCalc/viewcalc.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  ViewCalc w;
  QFont font("Monospace");
  w.setWindowTitle("Calculator v2.0");
  w.setFixedSize(440, 575);
  w.show();
  return a.exec();
}
