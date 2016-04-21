/* -*- Mode:C++; tab-width:2; indent-tabs-mode:nil; c-basic-offset:2; coding:utf-8 -*- */
/*************************************************************************
 * Kyureki Lib
 * Copyright (c) 2014 by sonokoro.jp. All Rights Reserved.
 * author : minato@sonokoro.jp
 *************************************************************************/

#ifndef SONOKORO_KYUREKI_CALC_HPP
#define SONOKORO_KYUREKI_CALC_HPP
#include <string>
#include <vector>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>

using namespace std;

class KyurekiCalc {

public:
  KyurekiCalc() {}

  const int getRokuyou(int year, int month, int day);
  const vector<int> getKyureki(int year, int month, int day);
  const double getJDByDateTime(int year, int month, int day, int hour, int min, int sec);
  const vector<int> getGregorianDateByJDTime(double jd_time);
  const vector<double> getBeforeNibun(double jd_date_time);

private:
  const double _getLongitudeSun(double jd_time);
  const double _getLongitudeMoon(double jd_time);
  const double _getNormalizationAngle(double angle);
  const vector<double> _getChuki(double jd_time);
  const double _getSaku(double jd_time);
};

#endif

