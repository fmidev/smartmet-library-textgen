<?php

echo "MySQLDictionary keyword validity tester\n";
echo "=======================================\n";

$link = mysql_connect("alppen.fmi.fi",
                      "textgen",
                      "w1w2w3") or die("Could not connect to mimir");
mysql_select_db("textgen2") or die("Could not select textgen2 database");

mysql_query("SET NAMES utf8");

// Establish all active languages

$languages = GetLanguages();

// Establish keywords in each language

$keywords = array();
foreach($languages as $language)
{
  $keys = GetKeywords($language);
  $keywords["$language"] = $keys;
}
mysql_close($link);

// And finally compare the sets of keys

for($i=0; $i<count($languages); $i++)
{
  for($j=0; $j<count($languages); $j++)
    {
      if($i<>$j)
	{
	  $lang1 = $languages[$i];
	  $lang2 = $languages[$j];
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
	}
    }
}
echo "\n";


function GetLanguages()
{
  $languages = array();
  $query = "SELECT translationtable FROM languages WHERE active=1 AND full=1";
  $result = mysql_query($query) or die("languages query failed");
  while($line = mysql_fetch_array($result,MYSQL_ASSOC))
    {
      extract($line);
      $languages[] = $translationtable;
    }
  mysql_free_result($result);
  return $languages;
}

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
