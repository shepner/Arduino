// http://www.learncpp.com/cpp-tutorial/89-class-code-and-header-files/
// http://www.cplusplus.com/forum/articles/10627/
// https://stackoverflow.com/questions/9579930/separating-class-code-into-a-header-and-cpp-file#9579974

//=================================

#ifndef MYCLASS_H
#define MYCLASS_H

class Date {

  private:
    int m_year;
    int m_month;
    int m_day;
 
  public:
    Date(int year, int month, int day);
 
    void SetDate(int year, int month, int day);
 
    int getYear() { return m_year; }
    int getMonth() { return m_month; }
    int getDay()  { return m_day; }

};

#endif
