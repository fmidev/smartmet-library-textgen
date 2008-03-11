<?php

# This script tests that all Sonera keywords have equivalents
# in translation_fi

echo "Sonera keyword validity tester\n";
echo "==============================\n";

$link = mysql_connect("base.weatherproof.fi",
		      "textgen",
		      "w1w2w3") or die("Could not connect to mimir");
mysql_select_db("textgen") or die("Could not select textgen database");

// Establish all active languages

$languages = array("translation_sonera","translation_fi");

// Establish keywords in each language

$keywords = array();
foreach($languages as $language)
{
  $keys = GetKeywords($language);
  $keywords["$language"] = $keys;
}
mysql_close($link);

// And finally compare the sets of keys

$lang1 = $languages[0];
$lang2 = $languages[1];
echo "$lang2 includes $lang1............";
	  
$keys1 = $keywords["$lang1"];
$keys2 = $keywords["$lang2"];
$diff = array_diff($keys1,$keys2);
if(count($diff)==0)
     echo "passed\n";
else
{
  echo " FAILED\n";
  echo "\tWords missing from $lang2 are:\n";
  foreach($diff as $word)
    echo "\t\t$word\n";
}
echo "\n";

# Return has of keywords in table
function GetKeywords($table)
{
  $keywords = array();
  $query = "SELECT keyword FROM $table";
  $result = mysql_query($query) or die("$table query failed");
  while($line = mysql_fetch_array($result,MYSQL_ASSOC))
    {
      extract($line);
      $keywords[] = $keyword;
    }
  mysql_free_result($result);
  return $keywords;
}

?>