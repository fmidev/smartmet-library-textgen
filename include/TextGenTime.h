#ifndef TEXTGENTIME_H
#define TEXTGENTIME_H

#include <newbase/NFmiMetTime.h>

#include <string>       // std::string
#include <boost/date_time/posix_time/posix_time.hpp> //include all types plus i/o
#include <boost/date_time/time_zone_base.hpp>
#include <boost/date_time/local_time/local_time.hpp>

  class TextGenTime
  {
  public:

	TextGenTime(void);
	TextGenTime(time_t theTime); 
	TextGenTime(const boost::posix_time::ptime& theTime);
	TextGenTime(const TextGenTime& theTime);
	//  TextGenTime(long datePart, long timePart); 

	TextGenTime(short year, short month, short day) ;
	TextGenTime(short year, short month, short day, short hour, short minute=0, short sec=0);
  
	void  ChangeBySeconds(long sec);
	void  ChangeByMinutes(long min);
	void  ChangeByHours(long hour); 
	void  ChangeByDays(long day); 
 
	long DifferenceInMinutes(const TextGenTime& anotherTime) const; 
	long DifferenceInHours(const TextGenTime& anotherTime) const; 
	long DifferenceInDays(const TextGenTime& anotherTime) const; 

	bool IsEqual(const TextGenTime& anotherTime) const;
	bool IsLessThan(const TextGenTime& anotherTime) const;  
	bool operator <  (const TextGenTime& anotherTime) const;
	bool operator >  (const TextGenTime& anotherTime) const;
	bool operator >= (const TextGenTime& anotherTime) const;
	bool operator <= (const TextGenTime& anotherTime) const;
	bool operator == (const TextGenTime& anotherTime) const;
	bool operator != (const TextGenTime& anotherTime) const;

	short GetYear(void) const;
	short GetMonth(void) const;
	short GetDay(void) const;
	short GetHour(void) const;
	short GetMin(void) const;
	short GetSec(void) const;

	void SetDate(const short year, const short month, const short day);
	void SetYear(short year);
	void SetMonth(short month);
	void SetDay(short day);
	void SetHour(short hour);
	void SetMin(short minute);
	void SetSec(short sec);

	std::string ToStr(const unsigned long theTimeMask) const;
	short GetZoneDifferenceHour() const;
	time_t EpochTime() const;
	short GetJulianDay(void) const;
	short GetWeekday(void) const;   // mon=1, tue=2,..., sat=6,  sun=7 
	TextGenTime UtcTime() const; 
	TextGenTime LocalTime() const;

	static short DaysInYear(short aYear);
	static short DaysInMonth(short aMonth, short aYear);

  private:
class TimeZone
{
public:
  TimeZone()
  {
	tz_db.load_from_file("/home/reponen/work/brainstorm/plugins/textgenplugin/cnf/date_time_zonespec.csv");
	tz=tz_db.time_zone_from_region("Europe/Helsinki");
  }
  void setTimeZone(std::string zoneString) { tz = tz_db.time_zone_from_region(zoneString.c_str()); }
  boost::local_time::time_zone_ptr getTimeZone() { return tz; }
private:
  boost::local_time::tz_database tz_db;
  boost::local_time::time_zone_ptr tz;
};
	static TimeZone* pTZ;

	boost::posix_time::ptime istPosixTime;

  }; // class TextGenTime

  std::ostream& operator<<(std::ostream & os, const TextGenTime& tgTime);
  bool operator<=(const NFmiMetTime& metTime, const TextGenTime& tgTime);
  bool operator>=(const NFmiMetTime& metTime, const TextGenTime& tgTime);
  bool operator==(const NFmiMetTime& metTime, const TextGenTime& tgTime);
  bool operator<(const NFmiMetTime& metTime, const TextGenTime& tgTime);
  bool operator>(const NFmiMetTime& metTime, const TextGenTime& tgTime);


#endif // TEXTGENTIME_H
