// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextFormatterTools
 */
// ======================================================================

#ifndef TEXTGEN_TEXTFORMATTERTOOLS_H
#define TEXTGEN_TEXTFORMATTERTOOLS_H

#include <string>

namespace TextGen
{
  class TextFormatter;

  namespace TextFormatterTools
  {
	void capitalize(std::string & theString);
	void punctuate(std::string & theString);

	// ----------------------------------------------------------------------
	/*!
	 * \brief Realize the given Glyphs and join them
	 *
	 * \param theBegin The begin iterator
	 * \param theEnd The end iterator
	 * \param theSeparator The string joining the parts
	 * \return The realized string
	 */
	// ----------------------------------------------------------------------

	template<typename Iterator>
	std::string realize(Iterator it,
						Iterator end,
						const std::string & theSeparator,
						const TextFormatter & theFormatter)
	{
	  std::string ret, tmp;
	  for( ; it!=end; ++it)
		{
		  tmp = theFormatter.format(**it);	// iterator -> shared_ptr -> object
		  if(tmp.empty()) continue;
		  if(!ret.empty()) ret += (*it)->prefix();
		  ret += tmp;
		  ret += (*it)->suffix();
		}
	  return ret;
	}

  } // namespace TextFormatterTools


} // namespace TextGen

#endif // TEXTGEN_TEXTFORMATTERTOOLS_H
// =====================================================================