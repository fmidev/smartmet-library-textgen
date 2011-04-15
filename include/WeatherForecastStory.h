#ifndef WEATHER_FORECAST_STORY_H
#define WEATHER_FORECAST_STORY_H


class MessageLogger;

namespace TextGen
{

class Sentence;
class Paragraph;
class WeatherForecastStory;



  class WeatherForecastStoryItem
  {
  public:
	
	WeatherForecastStoryItem(WeatherForecastStory& weatherForecastStory,
							 const WeatherPeriod& period, 
							 const story_part_id& storyPartId);
	
	virtual ~WeatherForecastStoryItem() {}
	
	Sentence getSentence();
	// if periods are merged this must be overwritten
	virtual WeatherPeriod getStoryItemPeriod() const;	
	virtual Sentence getStoryItemSentence() { return Sentence(); }
	unsigned int getPeriodLength();
	int forecastPeriodLength() const;
	int storyItemPeriodLength() const;
	Sentence getTodayVectorSentence(const vector<Sentence*>& todayVector,
									const unsigned int& theBegIndex, 
									const unsigned int& theEndIndex);
	Sentence getPeriodPhrase(const bool& theFromSpecifier,
							 const WeatherPeriod* thePhrasePeriod = 0);
	Sentence checkForAamuyoAndAamuPhrase(const bool& theFromSpecifier,
										 const WeatherPeriod& thePhrasePeriod);
	Sentence checkForExtendedPeriodPhrase(const WeatherPeriod& thePhrasePeriod);

	const story_part_id& getStoryPartId() const { return theStoryPartId; }

  protected:


	WeatherForecastStory& theWeatherForecastStory;
	WeatherPeriod thePeriod;
	story_part_id theStoryPartId;
	// can be used to control if this item is included in the story
	bool theIncludeInTheStoryFlag;
	Sentence theSentence;

	friend class WeatherForecastStory;

  private:

  };
  

  class PrecipitationForecastStoryItem: public WeatherForecastStoryItem
  {
  public:

	PrecipitationForecastStoryItem(WeatherForecastStory& weatherForecastStory,
								   const WeatherPeriod& period, 
								   const story_part_id& storyPartId,
								   const float& intensity,
								   const float& extent,
								   unsigned int& form,
								   precipitation_type& type);

	bool weakPrecipitation() const;
	Sentence getStoryItemSentence();
	WeatherPeriod getStoryItemPeriod() const;

  private:

	float theIntensity;
	float theExtent;
	unsigned int theForm;
	precipitation_type theType;
	// this flag indicates if there has been a gap between precipitation periods
	bool theSadeJatkuuFlag;
	// this flag indicates whether we tell about 
	// ending precipitation period (poutaantuu)
	bool thePoutaantuuFlag;
	// if precipitation has been weak we dont report ending of it
	bool theReportPoutaantuuFlag;
	int theFullDuration; // includes precipitation period beyond the forecast period
	PrecipitationForecastStoryItem* thePeriodToMergeWith;// if periods are merged this points to the megreable period
	PrecipitationForecastStoryItem* thePeriodToMergeTo;// if periods are merged this points to the merged period

 	friend class WeatherForecastStory;
	friend class CloudinessForecastStoryItem;
 };
	

  class CloudinessForecastStoryItem: public WeatherForecastStoryItem
  {
  public:
	CloudinessForecastStoryItem(WeatherForecastStory& weatherForecastStory,
								const WeatherPeriod& period, 
								const story_part_id& storyPartId,
								const cloudiness_id& cloudinessId,
								PrecipitationForecastStoryItem* previousPrecipitationStoryItem,
								PrecipitationForecastStoryItem* nextPrecipitationStoryItem);

	Sentence getStoryItemSentence();
	Sentence cloudinessChangeSentence();

  private:

	cloudiness_id theCloudinessId;
	PrecipitationForecastStoryItem* thePreviousPrecipitationStoryItem;
 	PrecipitationForecastStoryItem* theNextPrecipitationStoryItem;
	bool theReportAboutDryWeatherFlag;
	Sentence thePoutaantuuSentence;
 	Sentence theShortFormSentence;
 	Sentence theChangeSentence;
	NFmiTime theCloudinessChangeTimestamp;

	friend class WeatherForecastStory;
  };

  class WeatherForecastStory
  {
  public:
	WeatherForecastStory(const std::string& var,
						 const WeatherAnalysis::WeatherPeriod& forecastPeriod,
						 const WeatherAnalysis::WeatherArea& weatherArea,
						 const unsigned short& forecastArea,
						 const NFmiTime& theForecastTime,
						 PrecipitationForecast& precipitationForecast,
						 const CloudinessForecast& cloudinessForecast,
						 const FogForecast& fogForecast,
						 const ThunderForecast& thunderForecast,
						 MessageLogger& logger);

	~WeatherForecastStory();

	Paragraph getWeatherForecastStory();
	void addPrecipitationStoryItems();
	void addCloudinessStoryItems();
	void mergePeriodsWhenFeasible();
	Sentence getTimePhrase();

	const vector<WeatherForecastStoryItem*>& getStoryItemVector() const { return theStoryItemVector; }
	const WeatherPeriod& getStoryPeriod() const { return theForecastPeriod; }

  private:

	const std::string theVar;
	const WeatherPeriod& theForecastPeriod;
	const WeatherArea& theWeatherArea;
	const unsigned short& theForecastArea;
	const NFmiTime& theForecastTime;
	const PrecipitationForecast& thePrecipitationForecast;
	const CloudinessForecast& theCloudinessForecast;
	const FogForecast& theFogForecast;
	const ThunderForecast& theThunderForecast;	
	MessageLogger& theLogger;
	int theStorySize;
	bool theShortTimePrecipitationReportedFlag;
	bool theReportTimePhraseFlag;
	bool theCloudinessReportedFlag;

	vector<WeatherForecastStoryItem*> theStoryItemVector;

	friend class PrecipitationForecastStoryItem;
	friend class CloudinessForecastStoryItem;
	friend class WeatherForecastStoryItem;
  };
}

#endif
