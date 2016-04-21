/* -*- Mode:C++; tab-width:2; indent-tabs-mode:nil; c-basic-offset:2; coding:utf-8 -*- */
/*************************************************************************
 * Kyureki Lib
 * Copyright (c) 2014 by sonokoro.jp. All Rights Reserved.
 * author : minato@sonokoro.jp
 *************************************************************************/

#include <iostream>
#include "KyurekiCalc.hpp"

const int
KyurekiCalc::getRokuyou(int year, int month, int day)
{
  const vector<int> kyureki = this->getKyureki(year, month, day);
  int rokuyou = ((kyureki[2] + kyureki[3]) % 6);
  return rokuyou;
}

const vector<int>
KyurekiCalc::getKyureki(int year, int month, int day)
{
  // ユリウス暦の時間を求める
  double jd_date_time = this->getJDByDateTime(year, month, day, 0, 0, 0);
  // printf("jd=%lf\n", jd_date_time);

  // 計算対象の直前にあたる二分二至の時刻を求める
  // chu[0,0]:二分二至の時刻  chu[0,1]:その時の太陽黄経
  vector<double> before_nibun = this->getBeforeNibun(jd_date_time);
  // printf("0=%.11lf, 1=%.11lf", before_nibun[0], before_nibun[1]);

  // 中気の時刻を計算（４回計算する）
  // chu[i,0]:中気の時刻  chu[i,1]:太陽黄経
  vector<vector<double> > chukis;
  vector<double> chu;
  chu.push_back(before_nibun[0]);
  chu.push_back(before_nibun[1]);
  chukis.push_back(chu);

  for (int i=1; i<4; ++i) {
    vector<double> chuki = this->_getChuki(chukis[i-1][0] + 32.0);
    chukis.push_back(chuki);
    //    printf("c1=%f,c2=%f\n", chuki[0], chuki[1]);
  }

  //-----------------------------------------------------------------------
  //  計算対象の直前にあたる二分二至の直前の朔の時刻を求める
  //-----------------------------------------------------------------------
  double saku = this->_getSaku(chukis[0][0]);
  vector<double> sakus;
  sakus.push_back(saku);
  // printf("%.7f\n", saku);

  //-----------------------------------------------------------------------
  // 朔の時刻を求める
  //-----------------------------------------------------------------------
  for (int i=1; i<5; ++i) {
    double time = sakus[i-1];
    time += 30.0;
    double saku = this->_getSaku(time);
    sakus.push_back(saku);

    // 前と同じ時刻を計算した場合（両者の差が26日以内）には、初期値を
    // +33日にして再実行させる。
    if (fabs((int)(sakus[i-1]) - (int)(sakus[i])) <= 26.0) {
      sakus.pop_back();
      saku = this->_getSaku(time + 35.0);
      sakus.push_back(saku);
    }
    //    printf("%.7f\n", sakus[i]);
  }

  //-----------------------------------------------------------------------
  // saku[1]が二分二至の時刻以前になってしまった場合には、朔をさかのぼり過ぎ
  // たと考えて、朔の時刻を繰り下げて修正する。
  // その際、計算もれ（saku[4]）になっている部分を補うため、朔の時刻を計算
  // する。（近日点通過の近辺で朔があると起こる事があるようだ...？）
  //-----------------------------------------------------------------------
  vector<double> saku_tmp;
  if ((int)sakus[1] <= (int)chukis[0][0]) {
    for (int i=0; i<5; ++i) {
      saku_tmp.push_back(sakus[i+1]);
    }
    saku_tmp[4] = this->_getSaku(saku_tmp[3] + 35.0);
  }
  else if ((int)sakus[0] > (int)chukis[0][0]) {
    for (int i=4; i>0; --i) {
      saku_tmp.push_back(sakus[i-1]);
    }
    saku_tmp.push_back(sakus[0] - 27.0);
    reverse(saku_tmp.begin(), saku_tmp.end());
  }
  if (saku_tmp.size()) {
    sakus = saku_tmp;
  }

  //-----------------------------------------------------------------------
  // 閏月検索Ｆｌａｇセット
  // （節月で４ヶ月の間に朔が５回あると、閏月がある可能性がある。）
  // lap=0:平月  lap=1:閏月
  //-----------------------------------------------------------------------
  // printf("%d, %d\n", (int)sakus[4], (int)chukis[3][0]);
  int lap = 0;
  if ((int)sakus[4] <= (int)chukis[3][0]) {
    lap = 1;
  }
  // printf("%d\n", lap);
  // vector<double> sakus;
  // double saku = this->_getSaku(chukis[0][0]);
  //  $saku[0] = calc_saku($chu[0][0]);

  //-----------------------------------------------------------------------
  // 朔日行列の作成
  // m[i,0] ... 月名（1:正月 2:２月 3:３月 ....）
  // m[i,1] ... 閏フラグ（0:平月 1:閏月）
  // m[i,2] ... 朔日のjd
  //-----------------------------------------------------------------------
  vector<vector<int> > monthes;
  vector<int> month_data;
  int month_number = (int)(chukis[0][1] / 30.0) + 2;
  month_data.push_back(month_number);
  month_data.push_back(0);
  int jd = (int)sakus[0];
  month_data.push_back(jd);
  monthes.push_back(month_data);

  for (int i=1; i<5; ++i) {
    if (lap && 1 != i) {
      if ((int)chukis[i-1][0] <= (int)sakus[i-1] || (int)chukis[i-1][0] >= (int)sakus[i]) {
        vector<int>* month_data = &monthes.at(i-1);
        (*month_data).clear();

        int month_number = monthes[i-2][0];
        int jd = (int)sakus[i-1];
        (*month_data).push_back(month_number);
        (*month_data).push_back(1);
        (*month_data).push_back(jd);
        lap = 0;
      }
    }

    vector<int> month_data;
    int month_number = monthes[i-1][0] + 1;
    if (12 < month_number) {
      month_number -= 12;
    }
    month_data.push_back(month_number);
    month_data.push_back(0);
    int jd = (int)sakus[i];
    month_data.push_back(jd);
    monthes.push_back(month_data);
  }

  //-----------------------------------------------------------------------
  // 朔日行列から旧暦を求める。
  //-----------------------------------------------------------------------
  int i;
  int state = 0;
  for (i=0; i<5; ++i) {
    if ((int)jd_date_time < (int)(monthes[i][2])) {
      state = 1;
      break;
    } else if ((int)jd_date_time == (int)monthes[i][2]) {
      state = 2;
      break;
    }
  }
  if (0 == state || 1 == state) {
    i--;
  }

  vector<int> result;
  //  result.push_back(monthes[i][1]);
  //  result.push_back(monthes[i][0]);

  int kyureki_lap   = monthes[i][1];
  int kyureki_month = monthes[i][0];
  int kyureki_day   = (int)jd_date_time - (int)monthes[i][2] + 1;

  // $kyureki[1]=$m[$i][1];
  // $kyureki[2]=$m[$i][0];
  // $kyureki[3]=int($tm0)-int($m[$i][2])+1;

  //-----------------------------------------------------------------------
  // 旧暦年の計算
  // （旧暦月が10以上でかつ新暦月より大きい場合には、
  //   まだ年を越していないはず...）
  //-----------------------------------------------------------------------
  vector<int> g_date = this->getGregorianDateByJDTime(jd_date_time);
  int kyureki_year = g_date[0];

  if (9 < kyureki_month && g_date[1] < kyureki_month) {
    kyureki_year -= 1;
  }

  result.push_back(kyureki_year);
  result.push_back(kyureki_lap);
  result.push_back(kyureki_month);
  result.push_back(kyureki_day);

  return result;

  // $a = JD2YMDT($tm0);
  // $kyureki[0] = $a[0];
  // if ($kyureki[2] > 9 && $kyureki[2] > $a[1]) {
  //   $kyureki[0]--;
  // }

  // return array($kyureki[0],$kyureki[1],$kyureki[2],$kyureki[3]);

  // vector<int> kyureki;
  // kyureki.push_back(year);
  // kyureki.push_back(month);
  // kyureki.push_back(day);
  // kyureki.push_back(jd_date_time);
  // kyureki.push_back(before_nibun[0]);
  // return kyureki;
}

/**
 * ユリウス日（JD）から年月日、時分秒（世界時）を計算する
 * この関数で求めた年月日は、グレゴリオ暦法によって表されている。
 * 戻り値の配列TIME[]の内訳
 * result[0] ... 年  result[1] ... 月  result[2] ... 日
 * result[3] ... 時  result[4] ... 分  result[5] ... 秒
 */
const vector<int>
KyurekiCalc::getGregorianDateByJDTime(double jd_time)
{
  int data0 = (int)jd_time + 68570.0;
  int data1 = (int)data0 / 36524.25;
  int data2 = data0 - (int)(36524.25 * data1 + 0.75);
  int data3 = (int)((data2 + 1) / 365.2425);
  int data4 = data2 - (int)(365.25 * data3) + 31.0;
  int data5 = (int)((int)data4 / 30.59);
  int data6 = (int)((int)data5 / 11.0);

  int day   = data4 - int(30.59*data5);
  int month = data5 - 12 * data6 + 2;
  int year  = 100 * (data1 - 49) + data3 + data6;

  // 2月30日の補正
  if (2 == month && 28 < day) {
    if ((year % 100) == 0 && (year % 400) == 0) {
      day = 29;
    } else if ((year % 4) == 0) {
      day = 29;
    } else {
      day = 28;
    }
  }

  int time =86400.0 * (jd_time - int(jd_time));
  int hour = (int)(time / 3600.0);
  int min  = (int)((time - 3600.0 * hour) / 60.0);
  int sec  = (int)(time - 3600.0 * hour - 60 * min);

  vector<int> result;
  result.push_back(year);
  result.push_back(month);
  result.push_back(day);
  result.push_back(hour);
  result.push_back(min);
  result.push_back(sec);
  return result;

  //  return array($TIME[0],$TIME[1],$TIME[2],$TIME[3],$TIME[4],$TIME[5]);
}

const vector<double>
KyurekiCalc::getBeforeNibun(double jd_date_time)
{
  // 時刻引数を分解する
  double time1 = jd_date_time;
  double time2 = jd_date_time - time1;

  // JST ==> DT （補正時刻=0.0sec と仮定して計算）
  time2 -= 9.0 / 24.0;

  // 直前の二分二至の黄経 λsun0 を求める
  double time = (time2 + 0.5) / 36525.0;
  time = time + (time1 - 2451545.0) / 36525.0;
  double rm_sun  = this->_getLongitudeSun(time);
  double rm_sun0 = 90 * (int)(rm_sun / 90.0);

  // 繰り返し計算によって直前の二分二至の時刻を計算する
  // （誤差が±1.0 sec以内になったら打ち切る。）
  double delta_t1 = 0;
  //  double delta_t2 = 0;

  for (double delta_t2=1.0; fabs(delta_t1 + delta_t2) > (1.0 / 86400.0);) {
    // λsun を計算
    time = (time2 + 0.5) / 36525.0;
    time = time + (time1 - 2451545.0) / 36525.0;
    double rm_sun = this->_getLongitudeSun(time);

    // 黄経差 Δλ＝λsun −λsun0
    double delta_rm = rm_sun - rm_sun0 ;

    // Δλの引き込み範囲（±180°）を逸脱した場合には、補正を行う
    if (delta_rm > 180.0) {
      delta_rm -= 360.0;
    } else if (delta_rm < -180.0) {
      delta_rm += 360.0;
    }

    // 時刻引数の補正値 Δt
    // delta_t = delta_rm * 365.2 / 360.0;
    delta_t1 = (int)(delta_rm * 365.2 / 360.0);
    delta_t2 = delta_rm * 365.2 / 360.0;
    delta_t2 -= delta_t1;

    // 時刻引数の補正
    time1 = time1 - delta_t1;
    time2 = time2 - delta_t2;
    if (time2 < 0) {
      time2 += 1.0;
      time1 -= 1.0;
    }
    // printf("d1=%f\nd2=%f\nabs=%f\n", delta_t1, delta_t2, fabs(delta_t1 + delta_t2));
    // printf("time1=%.11f\ntime2=%.11f\ndelt=%f\n", time1, time2, abs(delta_t1 + delta_t2));
  }

  // 戻り値の作成
  // nibun[0,0]:時刻引数を合成するのと、DT ==> JST 変換を行い、戻り値とする
  // （補正時刻=0.0sec と仮定して計算）
  // nibun[0,1]:黄経
  double nibun0 = time2 + 9.0 / 24.0;
  nibun0 += time1;

  double nibun1 = rm_sun0;

  //  return array($nibun[0],$nibun[1]);
  //  printf("rm_sun=%.11lf\n", rm_sun);
  vector<double> result;
  result.push_back(nibun0);
  result.push_back(nibun1);
  return result;
}

const double
KyurekiCalc::getJDByDateTime(int year, int month, int day, int hour, int min, int sec)
{
  if (month < 3) {
    year -= 1;
    month += 12;
  }

  double jd  = (int)(365.25 * year);
  jd += (int)(year / 400.0);
  jd -= (int)(year / 100.0);
  jd += (int)(30.59 * (month-2.0));
  jd += 1721088;
  jd += day;

  double time  = sec / 3600;
  time += min / 60;
  time += hour;
  time  = time / 24.0;

  jd += time;

  //  printf("jd=%lf\n", jd);

  return jd;
}

// 太陽の黄経 λsun を計算する
const double
KyurekiCalc::_getLongitudeSun(double jd_time)
{
  //  printf("jd_time=%lf\n", jd_time);

  // 円周率の定義と（角度の）度からラジアンに変換する係数の定義
  const double PI = 3.141592653589793238462;
  double k = PI / 180.0;

  // 摂動項の計算
  double angle;
  double result;

  angle  = this->_getNormalizationAngle( 31557.0 * jd_time + 161.0);
  result = 0.0004 * cos(k * angle);
  angle  = this->_getNormalizationAngle( 29930.0 * jd_time +  48.0);
  result = result + 0.0004 * cos(k * angle);
  angle  = this->_getNormalizationAngle(  2281.0 * jd_time + 221.0);
  result = result + 0.0005 * cos(k * angle);
  angle  = this->_getNormalizationAngle(   155.0 * jd_time + 118.0);
  result = result + 0.0005 * cos(k * angle);
  angle  = this->_getNormalizationAngle( 33718.0 * jd_time + 316.0);
  result = result + 0.0006 * cos(k * angle);
  angle  = this->_getNormalizationAngle(  9038.0 * jd_time +  64.0);
  result = result + 0.0007 * cos(k * angle);
  angle  = this->_getNormalizationAngle(  3035.0 * jd_time + 110.0);
  result = result + 0.0007 * cos(k * angle);
  angle  = this->_getNormalizationAngle( 65929.0 * jd_time +  45.0);
  result = result + 0.0007 * cos(k * angle);
  angle  = this->_getNormalizationAngle( 22519.0 * jd_time + 352.0);
  result = result + 0.0013 * cos(k * angle);
  angle  = this->_getNormalizationAngle( 45038.0 * jd_time + 254.0);
  result = result + 0.0015 * cos(k * angle);
  angle  = this->_getNormalizationAngle(445267.0 * jd_time + 208.0);
  result = result + 0.0018 * cos(k * angle);
  angle  = this->_getNormalizationAngle(    19.0 * jd_time + 159.0);
  result = result + 0.0018 * cos(k * angle);
  angle  = this->_getNormalizationAngle( 32964.0 * jd_time + 158.0);
  result = result + 0.0020 * cos(k * angle);
  angle  = this->_getNormalizationAngle( 71998.1 * jd_time + 265.1);
  result = result + 0.0200 * cos(k * angle);
  angle  = this->_getNormalizationAngle( 35999.05 * jd_time + 267.52);
  result = result - 0.0048 * jd_time * cos(k * angle) ;
  result = result + 1.9147 * cos(k * angle) ;

  //-----------------------------------------------------------------------
  // 比例項の計算
  //-----------------------------------------------------------------------
  angle = this->_getNormalizationAngle(36000.7695 * jd_time);
  angle = this->_getNormalizationAngle(angle + 280.4659);
  result = this->_getNormalizationAngle(result + angle);

  //  printf("result=%lf\n", result);

  return result;
}

const double
KyurekiCalc::_getNormalizationAngle(double angle)
{
  double result;
  double angle_tmp;

  if (angle < 0.0) {
    result    = - angle;
    angle_tmp = (int)(result / 360.0);
    result   -= 360.0 * angle_tmp;
    result    = 360.0 - result;
  } else {
    result = (int)(angle / 360.0);
    result = angle - 360.0 * result;
  }

  return result;
}

// 中気の時刻を求める
const vector<double>
KyurekiCalc::_getChuki(double jd_time)
{
  // 時刻引数を分解する
  double time1 = (int)jd_time;
  double time2 = jd_time - time1;

  // JST ==> DT （補正時刻=0.0sec と仮定して計算）
  time2 -= 9.0 / 24.0;

  // 中気の黄経 λsun0 を求める
  double time = (time2 + 0.5) / 36525.0;
  time = time + (time1 - 2451545.0) / 36525.0;
  double rm_sun = this->_getLongitudeSun(time);
  double rm_sun0 = 30.0 * (int)(rm_sun / 30.0);

  // 繰り返し計算によって中気の時刻を計算する
  // （誤差が±1.0 sec以内になったら打ち切る。）
  double delta_t1 = 0;
  for (double delta_t2=1.0; fabs(delta_t1 + delta_t2) > (1.0 / 86400.0);) {
    // λsun を計算
    time = (time2 + 0.5) / 36525.0;
    time = time + (time1 - 2451545.0) / 36525.0;
    double rm_sun = this->_getLongitudeSun(time);

    // 黄経差 Δλ＝λsun −λsun0
    double delta_rm = rm_sun - rm_sun0;

    // Δλの引き込み範囲（±180°）を逸脱した場合には、補正を行う
    if (delta_rm > 180.0) {
      delta_rm -= 360.0;
    } else if (delta_rm < -180.0) {
      delta_rm += 360.0;
    }

    // 時刻引数の補正値 Δt
    delta_t1 = (int)(delta_rm * 365.2 / 360.0);
    delta_t2 = delta_rm * 365.2 / 360.0;
    delta_t2 -= delta_t1;

    // 時刻引数の補正
    time1 = time1 - delta_t1;
    time2 = time2 - delta_t2;

    if (time2 < 0) {
      time2+=1.0;
      time1-=1.0;
    }
  }

  // 戻り値の作成
  // chu[i,0]:時刻引数を合成するのと、DT ==> JST 変換を行い、戻り値とする
  // （補正時刻=0.0sec と仮定して計算）
  // chu[i,1]:黄経
  double chuki0 = time2 + 9.0 / 24.0;
  chuki0 += time1;
  double chuki1 = rm_sun0;

  vector<double> result;
  result.push_back(chuki0);
  result.push_back(chuki1);
  return result;
}

// 朔の計算
const double
KyurekiCalc::_getSaku(double jd_time)
{
  // ループカウンタのセット
  int lc = 1;

  //-----------------------------------------------------------------------
  //時刻引数を分解する
  //-----------------------------------------------------------------------
  double time1 = int(jd_time);
  double time2 = jd_time - time1;

  //-----------------------------------------------------------------------
  // JST ==> DT （補正時刻=0.0sec と仮定して計算）
  //-----------------------------------------------------------------------
  time2 -= 9.0 / 24.0;

  //-----------------------------------------------------------------------
  // 繰り返し計算によって朔の時刻を計算する
  // （誤差が±1.0 sec以内になったら打ち切る。）
  //-----------------------------------------------------------------------
  double delta_t1 = 0;
  for (double delta_t2=1.0 ; fabs(delta_t1 + delta_t2) > (1.0 / 86400.0); ++lc) {

    //-----------------------------------------------------------------------
    // 太陽の黄経λsun ,月の黄経λmoon を計算
    // t = (tm - 2451548.0 + 0.5)/36525.0;
    //-----------------------------------------------------------------------
    double time = (time2 + 0.5) / 36525.0;
    time = time + (time1 - 2451545.0) / 36525.0;
    double rm_sun = this->_getLongitudeSun(time);
    double rm_moon = this->_getLongitudeMoon(time);

    //-----------------------------------------------------------------------
    // 月と太陽の黄経差Δλ
    // Δλ＝λmoon−λsun
    //-----------------------------------------------------------------------
    double delta_rm = rm_moon - rm_sun ;

    //-----------------------------------------------------------------------
    // ループの１回目（lc=1）で delta_rm < 0.0 の場合には引き込み範囲に
    // 入るように補正する
    //-----------------------------------------------------------------------
    if (lc==1 && delta_rm < 0.0) {
      delta_rm = this->_getNormalizationAngle(delta_rm);
    }
    //-----------------------------------------------------------------------
    //   春分の近くで朔がある場合（0 ≦λsun≦ 20）で、月の黄経λmoon≧300 の
    //   場合には、Δλ＝ 360.0 − Δλ と計算して補正する
    //-----------------------------------------------------------------------
    else if (rm_sun >= 0 && rm_sun <= 20 && rm_moon >= 300) {
      delta_rm = this->_getNormalizationAngle(delta_rm);
      delta_rm = 360.0 - delta_rm;
    }
    //-----------------------------------------------------------------------
    // Δλの引き込み範囲（±40°）を逸脱した場合には、補正を行う
    //-----------------------------------------------------------------------
    else if (fabs(delta_rm) > 40.0) {
      delta_rm = this->_getNormalizationAngle(delta_rm);
    }

    //-----------------------------------------------------------------------
    // 時刻引数の補正値 Δt
    // delta_t = delta_rm * 29.530589 / 360.0;
    //-----------------------------------------------------------------------
    delta_t1 = (int)(delta_rm * 29.530589 / 360.0);
    delta_t2 = delta_rm * 29.530589 / 360.0;
    delta_t2 -= delta_t1;

    //-----------------------------------------------------------------------
    // 時刻引数の補正
    // tm -= delta_t;
    //-----------------------------------------------------------------------
    time1 = time1 - delta_t1;
    time2 = time2 - delta_t2;
    if (time2 < 0.0) {
      time2+=1.0;time1-=1.0;
    }

    //-----------------------------------------------------------------------
    // ループ回数が15回になったら、初期値 tm を tm-26 とする。
    //-----------------------------------------------------------------------
    if (lc == 15 && fabs(delta_t1 + delta_t2) > (1.0 / 86400.0)) {
      time1 = (int)(jd_time - 26);
      time2 = 0;
    }

    //-----------------------------------------------------------------------
    // 初期値を補正したにも関わらず、振動を続ける場合には初期値を答えとして
    // 返して強制的にループを抜け出して異常終了させる。
    //-----------------------------------------------------------------------
    else if (lc > 30 && fabs(delta_t1 + delta_t2) > (1.0 / 86400.0)) {
      time1 = jd_time;
      time2 = 0;
      break;
    }
  }

  //-----------------------------------------------------------------------
  // 時刻引数を合成するのと、DT ==> JST 変換を行い、戻り値とする
  // （補正時刻=0.0sec と仮定して計算）
  //-----------------------------------------------------------------------
  return (time2 + time1 + 9.0 / 24.0);
}

// 月の黄経 λmoon を計算する
const double
KyurekiCalc::_getLongitudeMoon(double jd_time)
{
  //-----------------------------------------------------------------------
  // 円周率の定義と（角度の）度からラジアンに変換する係数の定義
  //-----------------------------------------------------------------------
  const double PI = 3.141592653589793238462;
  double k = PI / 180.0;

  //-----------------------------------------------------------------------
  // 摂動項の計算
  //-----------------------------------------------------------------------
  double angle;
  double result;
  angle = this->_getNormalizationAngle(2322131.0  * jd_time + 191.0 );
  result = 0.0003 * cos(k * angle);
  angle = this->_getNormalizationAngle(   4067.0  * jd_time +  70.0 );
  result = result + 0.0003 * cos(k * angle);
  angle = this->_getNormalizationAngle( 549197.0  * jd_time + 220.0 );
  result = result + 0.0003 * cos(k * angle);
  angle = this->_getNormalizationAngle(1808933.0  * jd_time +  58.0 );
  result = result + 0.0003 * cos(k * angle);
  angle = this->_getNormalizationAngle( 349472.0  * jd_time + 337.0 );
  result = result + 0.0003 * cos(k * angle);
  angle = this->_getNormalizationAngle( 381404.0  * jd_time + 354.0 );
  result = result + 0.0003 * cos(k * angle);
  angle = this->_getNormalizationAngle( 958465.0  * jd_time + 340.0 );
  result = result + 0.0003 * cos(k * angle);
  angle = this->_getNormalizationAngle(  12006.0  * jd_time + 187.0 );
  result = result + 0.0004 * cos(k * angle);
  angle = this->_getNormalizationAngle(  39871.0  * jd_time + 223.0 );
  result = result + 0.0004 * cos(k * angle);
  angle = this->_getNormalizationAngle( 509131.0  * jd_time + 242.0 );
  result = result + 0.0005 * cos(k * angle);
  angle = this->_getNormalizationAngle(1745069.0  * jd_time +  24.0 );
  result = result + 0.0005 * cos(k * angle);
  angle = this->_getNormalizationAngle(1908795.0  * jd_time +  90.0 );
  result = result + 0.0005 * cos(k * angle);
  angle = this->_getNormalizationAngle(2258267.0  * jd_time + 156.0 );
  result = result + 0.0006 * cos(k * angle);
  angle = this->_getNormalizationAngle( 111869.0  * jd_time +  38.0 );
  result = result + 0.0006 * cos(k * angle);
  angle = this->_getNormalizationAngle(  27864.0  * jd_time + 127.0 );
  result = result + 0.0007 * cos(k * angle);
  angle = this->_getNormalizationAngle( 485333.0  * jd_time + 186.0 );
  result = result + 0.0007 * cos(k * angle);
  angle = this->_getNormalizationAngle( 405201.0  * jd_time +  50.0 );
  result = result + 0.0007 * cos(k * angle);
  angle = this->_getNormalizationAngle( 790672.0  * jd_time + 114.0 );
  result = result + 0.0007 * cos(k * angle);
  angle = this->_getNormalizationAngle(1403732.0  * jd_time +  98.0 );
  result = result + 0.0008 * cos(k * angle);
  angle = this->_getNormalizationAngle( 858602.0  * jd_time + 129.0 );
  result = result + 0.0009 * cos(k * angle);
  angle = this->_getNormalizationAngle(1920802.0  * jd_time + 186.0 );
  result = result + 0.0011 * cos(k * angle);
  angle = this->_getNormalizationAngle(1267871.0  * jd_time + 249.0 );
  result = result + 0.0012 * cos(k * angle);
  angle = this->_getNormalizationAngle(1856938.0  * jd_time + 152.0 );
  result = result + 0.0016 * cos(k * angle);
  angle = this->_getNormalizationAngle( 401329.0  * jd_time + 274.0 );
  result = result + 0.0018 * cos(k * angle);
  angle = this->_getNormalizationAngle( 341337.0  * jd_time +  16.0 );
  result = result + 0.0021 * cos(k * angle);
  angle = this->_getNormalizationAngle(  71998.0  * jd_time +  85.0 );
  result = result + 0.0021 * cos(k * angle);
  angle = this->_getNormalizationAngle( 990397.0  * jd_time + 357.0 );
  result = result + 0.0021 * cos(k * angle);
  angle = this->_getNormalizationAngle( 818536.0  * jd_time + 151.0 );
  result = result + 0.0022 * cos(k * angle);
  angle = this->_getNormalizationAngle( 922466.0  * jd_time + 163.0 );
  result = result + 0.0023 * cos(k * angle);
  angle = this->_getNormalizationAngle(  99863.0  * jd_time + 122.0 );
  result = result + 0.0024 * cos(k * angle);
  angle = this->_getNormalizationAngle(1379739.0  * jd_time +  17.0 );
  result = result + 0.0026 * cos(k * angle);
  angle = this->_getNormalizationAngle( 918399.0  * jd_time + 182.0 );
  result = result + 0.0027 * cos(k * angle);
  angle = this->_getNormalizationAngle(   1934.0  * jd_time + 145.0 );
  result = result + 0.0028 * cos(k * angle);
  angle = this->_getNormalizationAngle( 541062.0  * jd_time + 259.0 );
  result = result + 0.0037 * cos(k * angle);
  angle = this->_getNormalizationAngle(1781068.0  * jd_time +  21.0 );
  result = result + 0.0038 * cos(k * angle);
  angle = this->_getNormalizationAngle(    133.0  * jd_time +  29.0 );
  result = result + 0.0040 * cos(k * angle);
  angle = this->_getNormalizationAngle(1844932.0  * jd_time +  56.0 );
  result = result + 0.0040 * cos(k * angle);
  angle = this->_getNormalizationAngle(1331734.0  * jd_time + 283.0 );
  result = result + 0.0040 * cos(k * angle);
  angle = this->_getNormalizationAngle( 481266.0  * jd_time + 205.0 );
  result = result + 0.0050 * cos(k * angle);
  angle = this->_getNormalizationAngle(  31932.0  * jd_time + 107.0 );
  result = result + 0.0052 * cos(k * angle);
  angle = this->_getNormalizationAngle( 926533.0  * jd_time + 323.0 );
  result = result + 0.0068 * cos(k * angle);
  angle = this->_getNormalizationAngle( 449334.0  * jd_time + 188.0 );
  result = result + 0.0079 * cos(k * angle);
  angle = this->_getNormalizationAngle( 826671.0  * jd_time + 111.0 );
  result = result + 0.0085 * cos(k * angle);
  angle = this->_getNormalizationAngle(1431597.0  * jd_time + 315.0 );
  result = result + 0.0100 * cos(k * angle);
  angle = this->_getNormalizationAngle(1303870.0  * jd_time + 246.0 );
  result = result + 0.0107 * cos(k * angle);
  angle = this->_getNormalizationAngle( 489205.0  * jd_time + 142.0 );
  result = result + 0.0110 * cos(k * angle);
  angle = this->_getNormalizationAngle(1443603.0  * jd_time +  52.0 );
  result = result + 0.0125 * cos(k * angle);
  angle = this->_getNormalizationAngle(  75870.0  * jd_time +  41.0 );
  result = result + 0.0154 * cos(k * angle);
  angle = this->_getNormalizationAngle( 513197.9  * jd_time + 222.5 );
  result = result + 0.0304 * cos(k * angle);
  angle = this->_getNormalizationAngle( 445267.1  * jd_time +  27.9 );
  result = result + 0.0347 * cos(k * angle);
  angle = this->_getNormalizationAngle( 441199.8  * jd_time +  47.4 );
  result = result + 0.0409 * cos(k * angle);
  angle = this->_getNormalizationAngle( 854535.2  * jd_time + 148.2 );
  result = result + 0.0458 * cos(k * angle);
  angle = this->_getNormalizationAngle(1367733.1  * jd_time + 280.7 );
  result = result + 0.0533 * cos(k * angle);
  angle = this->_getNormalizationAngle( 377336.3  * jd_time +  13.2 );
  result = result + 0.0571 * cos(k * angle);
  angle = this->_getNormalizationAngle(  63863.5  * jd_time + 124.2 );
  result = result + 0.0588 * cos(k * angle);
  angle = this->_getNormalizationAngle( 966404.0  * jd_time + 276.5 );
  result = result + 0.1144 * cos(k * angle);
  angle = this->_getNormalizationAngle(  35999.05 * jd_time +  87.53);
  result = result + 0.1851 * cos(k * angle);
  angle = this->_getNormalizationAngle( 954397.74 * jd_time + 179.93);
  result = result + 0.2136 * cos(k * angle);
  angle = this->_getNormalizationAngle( 890534.22 * jd_time + 145.7 );
  result = result + 0.6583 * cos(k * angle);
  angle = this->_getNormalizationAngle( 413335.35 * jd_time +  10.74);
  result = result + 1.2740 * cos(k * angle);
  angle = this->_getNormalizationAngle(477198.868 * jd_time + 44.963);
  result = result + 6.2888 * cos(k * angle);

  //-----------------------------------------------------------------------
  // 比例項の計算
  //-----------------------------------------------------------------------
  angle  = this->_getNormalizationAngle(481267.8809 * jd_time);
  angle  = this->_getNormalizationAngle(angle + 218.3162);
  result = this->_getNormalizationAngle(result + angle);

  return result;
}
