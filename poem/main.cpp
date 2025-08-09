#include <iostream>
#include <iomanip>

using namespace std;


int main() {
  string name[] { "Student 1", "Student 2", "Student 3", "Student 4", "Student 5" };
  float midtem[] { 1.0, 1.3, 3.0, 1.4, 1.9 };
  float final[] { 1.2, 1.4, 4.0, 1.2, 2.0 };
  float final_rating[] { 1.5, 1.35, 3.5, 1.3, 1.95 };
  string remark[] { "PASSED", "PASSED", "FAILED", "PASSED", "PASSED"};

  string heading[] { "Name", "Midterm Grade", "Final Grade", "Final Rating", "Remarks"};

  cout << std::left << setw(20) << "Name";
  cout << std::right << setw(20) << "Midterm Grade";
  cout << std::right << setw(20) << "Final Grade";
  cout << std::right << setw(20) << "Final Rating";
  cout << std::right << setw(20) << "Remark";
  cout << endl;

  for (int i = 0; i < 5; i++) {
      cout << std::left  << setw(20) << std::fixed << std::setprecision(1) << name[i];
      cout << std::right << setw(20) << std::fixed << std::setprecision(1) << midtem[i];
      cout << std::right << setw(20) << std::fixed << std::setprecision(1) << final[i];
      cout << std::right << setw(20) << std::fixed << std::setprecision(2) << final_rating[i];
      cout << std::right << setw(20) << remark[i];
      cout << endl;
  }
  return 0;
}
