// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::TextFormatterTools
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
	 * \param it The begin iterator
	 * \param end The end iterator
	 * \param theFormatter The text formatter
	 * \param thePrefix The string joining prefix
	 * \param theSuffix The string joining prefix
	 * \return The realized string
	 */
	// ----------------------------------------------------------------------

	template<typename Iterator>
	std::string realize(Iterator it,
						Iterator end,
						const TextFormatter & theFormatter,
						const std::string & thePrefix,
						const std::string & theSuffix)
	{
	  std::string ret, tmp;
	  for( ; it!=end; ++it)
		{
		  bool isdelim = (*it)->isDelimiter();
		  
		  tmp = theFormatter.format(**it);	// iterator -> shared_ptr -> object
		  if(!tmp.empty())
			{
			  if(!ret.empty() && !isdelim)
				ret += thePrefix;
			  ret += tmp;
			  if(!isdelim)
				ret += theSuffix;
			}
		}
	  return ret;
	}

  } // namespace TextFormatterTools


} // namespace TextGen

#endif // TEXTGEN_TEXTFORMATTERTOOLS_H
// =====================================================================
