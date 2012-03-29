// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::SpecialStory::text
 */
// ======================================================================

#include "SpecialStory.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "Sentence.h"
#include "Settings.h"
#include "Text.h"

#include <boost/filesystem.hpp>

#include <cstdio>
#include <unistd.h>

using namespace WeatherAnalysis;
using namespace std;

namespace TextGen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Test if a file is executable
   */
  // ----------------------------------------------------------------------

  bool is_executable(const std::string & filename)
  {
	return !access(filename.c_str(), X_OK);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on text
   *
   * Variables:
   *
   *     value = "Text to be inserted into paragraph as is."
   *     value = @filename
   *     value = @filename.php
   *
   * Product specific variables are possible:
   *
   *     value::en_html = "<underline>Text to be underlined.</underline>"
   *
   * \return The story
   */
  // ----------------------------------------------------------------------

  Paragraph SpecialStory::text() const
  {
	MessageLogger log("SpecialStory::text");

	// Get the options

	using namespace Settings;

	const string default_text = Settings::optional_string(itsVar+"::value","");

	Paragraph paragraph;
	Sentence sentence;

	if(default_text.empty())
	  {
	  }
	else if(default_text[0] != '@')
	  {
		sentence << Text(default_text);
		paragraph << sentence;
	  }
	else
	  {
		string filename = default_text.substr(1,string::npos);
		log << "File to be included: " << filename << endl;
		if(!boost::filesystem::exists(filename))
		  {
			log << "The file does not exist!" << endl;
			throw runtime_error("File '"+filename+"' is not readable");
		  }

		// test here if file is executable

		if(false)
		  {
			string txt;
			// txt = read_file_contents(filename);
			sentence << Text(txt);
			paragraph << sentence;
		  }
		else
		  {
			string txt;
			// txt = execute_file(filename);
			sentence << Text(txt);
			paragraph << sentence;
		  }
	  }

	return paragraph;
  }

} // namespace TextGen
  
// ======================================================================
