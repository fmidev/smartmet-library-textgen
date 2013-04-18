#include "TextGenTime.h"
#include <newbase/NFmiTime.h>

#include <iostream>     // std::cout
#include <sstream>      // std::stringstream

using namespace boost::posix_time;
using namespace boost::local_time;
using namespace boost::gregorian;
using namespace std;

TextGenTime::TimeZone* TextGenTime::pTZ = new TextGenTime::TimeZone();


TextGenTime::TextGenTime(void) : istPosixTime(second_clock::local_time())
{
}

TextGenTime::TextGenTime(const boost::posix_time::ptime& theTime) : istPosixTime(theTime)
{
}

TextGenTime::TextGenTime(time_t theTime) : istPosixTime(from_time_t(theTime))
{
}

TextGenTime::TextGenTime(const TextGenTime& theTime) : istPosixTime(theTime.istPosixTime)
{
}

TextGenTime::TextGenTime(short year, short month, short day) : istPosixTime(date(year, month, day), time_duration(0,0,0))
{
}

TextGenTime::TextGenTime(short year, short month, short day, short hour, short minute/*=0*/, short sec/*=0*/)
{
  time_duration td(hour,minute,sec,0);
  stringstream ss;
  ss << year << "-" << month << "-" << day;
  date d(from_simple_string(ss.str()));

  istPosixTime = ptime(d, td);
}


void  TextGenTime::ChangeBySeconds(long sec)
{
  istPosixTime += seconds(sec);
}

void  TextGenTime::ChangeByMinutes(long min)
{
  istPosixTime += minutes(min);
}

void  TextGenTime::ChangeByHours(long hour)
{
  istPosixTime += hours(hour);
}

void  TextGenTime::ChangeByDays(long day)
{
  istPosixTime += days(day);
}

long TextGenTime::DifferenceInMinutes(const TextGenTime& anotherTime) const
{
  time_duration td(istPosixTime - anotherTime.istPosixTime);

  return td.total_seconds() / 60;
}

long TextGenTime::DifferenceInHours(const TextGenTime& anotherTime) const
{
  time_duration td(istPosixTime - anotherTime.istPosixTime);

  return td.total_seconds() / 3600;
}

long TextGenTime::DifferenceInDays(const TextGenTime& anotherTime) const
{
  time_duration td(istPosixTime - anotherTime.istPosixTime);

  return td.total_seconds() / 86400;
}

bool TextGenTime::IsEqual(const TextGenTime& anotherTime) const
{
  return (istPosixTime == anotherTime.istPosixTime);
}

bool TextGenTime::IsLessThan(const TextGenTime& anotherTime) const
{
  return (istPosixTime < anotherTime.istPosixTime);
}

bool TextGenTime::operator <  (const TextGenTime& anotherTime) const
{
  return (istPosixTime < anotherTime.istPosixTime);
}

bool TextGenTime::operator >  (const TextGenTime& anotherTime) const
{
  return (istPosixTime > anotherTime.istPosixTime);
}

bool TextGenTime::operator >= (const TextGenTime& anotherTime) const
{
  return (istPosixTime >= anotherTime.istPosixTime);
}

bool TextGenTime::operator <= (const TextGenTime& anotherTime) const
{
  return (istPosixTime <= anotherTime.istPosixTime);
}

bool TextGenTime::operator == (const TextGenTime& anotherTime) const
{
  return (istPosixTime == anotherTime.istPosixTime);
}

bool TextGenTime::operator != (const TextGenTime& anotherTime) const
{
  return (istPosixTime != anotherTime.istPosixTime);
}

void TextGenTime::SetDate(const short year, const short month, const short day) 
{ 
  date d(year, month, day);
  time_duration td(istPosixTime.time_of_day());

  istPosixTime = ptime(d, td);
} 

void TextGenTime::SetYear(short year)
{
  date d(istPosixTime.date());
  time_duration td(istPosixTime.time_of_day());

  istPosixTime = ptime(date(year, d.month(), d.day()), td);
}

void TextGenTime::SetMonth(short month)
{
  date d(istPosixTime.date());
  time_duration td(istPosixTime.time_of_day());

  istPosixTime = ptime(date(d.year(), month, d.day()), td);
}

void TextGenTime::SetDay(short day)
{
  date d(istPosixTime.date());
  time_duration td(istPosixTime.time_of_day());

  istPosixTime = ptime(date(d.year(), d.month(), day), td);
}

void TextGenTime::SetHour(short hour)
{
  date d(istPosixTime.date());
  time_duration td(istPosixTime.time_of_day());

  istPosixTime = ptime(d, time_duration(hour, td.minutes(), td.seconds()));
}

void TextGenTime::SetMin(short minute)
{
  date d(istPosixTime.date());
  time_duration td(istPosixTime.time_of_day());

  istPosixTime = ptime(d, time_duration(td.hours(), minute, td.seconds()));
}

void TextGenTime::SetSec(short sec)
{
  date d(istPosixTime.date());
  time_duration td(istPosixTime.time_of_day());

  istPosixTime = ptime(d, time_duration(td.hours(), td.minutes(), sec));
}

short TextGenTime::GetYear(void) const
{
  return istPosixTime.date().year();
}

short TextGenTime::GetMonth(void) const
{
  return istPosixTime.date().month();
}

short TextGenTime::GetDay(void) const
{
  return istPosixTime.date().day();
}

short TextGenTime::GetHour(void) const
{
  return istPosixTime.time_of_day().hours();
}

short TextGenTime::GetMin(void) const
{
  return istPosixTime.time_of_day().minutes();
}

short TextGenTime::GetSec(void) const
{
  return istPosixTime.time_of_day().seconds();
}


std::string TextGenTime::ToStr(const unsigned long theTimeMask) const
{
  stringstream ss;
  
  if(kShortYear & theTimeMask)
    ss << std::setw(2) << (GetYear() - 2000);
  else if(kLongYear & theTimeMask)
    ss << std::setw(4) << GetYear();
  
  if(kMonth & theTimeMask)
	ss << std::setw(2) << GetMonth();
  
  if(kDay & theTimeMask)
	ss << std::setw(2) << GetDay();

  if(kHour & theTimeMask)
	ss << std::setw(2) << GetHour();

  if(kMinute & theTimeMask)
	ss << std::setw(2) << GetMin();

  if(kSecond & theTimeMask)
	ss << std::setw(2) << GetSec();

  return ss.str();
}

short TextGenTime::GetZoneDifferenceHour() const
{
  local_date_time locTime(istPosixTime.date(),
						  istPosixTime.time_of_day(),
						  pTZ->getTimeZone(),
						  local_date_time::NOT_DATE_TIME_ON_ERROR);
  //  cerr << "locTime: " << locTime << endl;
 // ei toimi
 //  local_date_time locTime(istPosixTime, pTZ->getTimeZone());

  ptime utcTime(locTime.utc_time());

  return DifferenceInHours(TextGenTime(utcTime));
}

time_t TextGenTime::EpochTime() const
{
  ptime time_t_epoch(date(1970,1,1)); 
  
  local_date_time locTime(istPosixTime.date(),
						  istPosixTime.time_of_day(),
						  pTZ->getTimeZone(),
						  local_date_time::NOT_DATE_TIME_ON_ERROR);
  //  local_date_time locTime(istPosixTime, pTZ->getTimeZone());
  //   cerr << "locTime: " << locTime << endl;
 
  return (locTime.utc_time() - time_t_epoch).total_seconds();
}

short TextGenTime::GetWeekday(void) const   // mon=1, tue=2,..., sat=6,  sun=7 
{
  short retval(istPosixTime.date().day_of_week());

  if(retval == 0)
	retval = 7;

  return retval;
}

short TextGenTime::GetJulianDay(void) const
{
  return istPosixTime.date().julian_day();
}

short TextGenTime::DaysInYear(const short aYear)
{
  return NFmiTime::DaysInYear(aYear);
}

short TextGenTime::DaysInMonth(const short aMonth, const short aYear)
{
  return NFmiTime::DaysInMonth(aMonth, aYear);
}

TextGenTime TextGenTime::UtcTime() const
{	
  local_date_time locTime(istPosixTime.date(),
						  istPosixTime.time_of_day(),
						  pTZ->getTimeZone(),
						  local_date_time::NOT_DATE_TIME_ON_ERROR);

  //  cerr << "locTime: " << locTime << endl;

  return TextGenTime(locTime.utc_time());
}

// 
TextGenTime TextGenTime::LocalTime() const
{
  short zdh(GetZoneDifferenceHour());

  boost::posix_time::ptime localptime = istPosixTime + hours(zdh);

  return TextGenTime(localptime);
}

std::ostream& operator<<(std::ostream& os, const TextGenTime& tgTime)
{
  os.width( 2 );
  os.fill( '0' );
  os << tgTime.GetDay() << ".";
  os.width( 2 );
  os.fill( '0' );
  os << tgTime.GetMonth() << "." << tgTime.GetYear() << " "; 
  os.width( 2 );
  os.fill( '0' );
  os << tgTime.GetHour() << ":";
  os.width( 2 );
  os.fill( '0' );
  os << tgTime.GetMin() << ":";
  os.width( 2 );
  os.fill( '0' );
  os << tgTime.GetSec(); 

  return os;
}

bool operator<=(const NFmiMetTime& metTime, const TextGenTime& tgTime) 
{ 
  TextGenTime met(metTime.GetYear(),
				  metTime.GetMonth(),
				  metTime.GetDay(),
				  metTime.GetHour(),
				  metTime.GetMin(),
				  metTime.GetSec());
  return met <= tgTime;
}

bool operator>=(const NFmiMetTime& metTime, const TextGenTime& tgTime)
{ 
  TextGenTime met(metTime.GetYear(),
				  metTime.GetMonth(),
				  metTime.GetDay(),
				  metTime.GetHour(),
				  metTime.GetMin(),
				  metTime.GetSec());
  return met >= tgTime;
}

bool operator==(const NFmiMetTime& metTime, const TextGenTime& tgTime)
{
  TextGenTime met(metTime.GetYear(),
				  metTime.GetMonth(),
				  metTime.GetDay(),
				  metTime.GetHour(),
				  metTime.GetMin(),
				  metTime.GetSec());
  return met == tgTime;
}

bool operator<(const NFmiMetTime& metTime, const TextGenTime& tgTime)
{
  TextGenTime met(metTime.GetYear(),
				  metTime.GetMonth(),
				  metTime.GetDay(),
				  metTime.GetHour(),
				  metTime.GetMin(),
				  metTime.GetSec());
  return met < tgTime;
}

bool operator>(const NFmiMetTime& metTime, const TextGenTime& tgTime)
{
  TextGenTime met(metTime.GetYear(),
				  metTime.GetMonth(),
				  metTime.GetDay(),
				  metTime.GetHour(),
				  metTime.GetMin(),
				  metTime.GetSec());
  return met > tgTime;
}

