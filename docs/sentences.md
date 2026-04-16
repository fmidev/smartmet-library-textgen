# Building sentences

## Basic usage

`TextGen::Document` is the object from which the final text is composed. `TextGen::Document` in turn consists of `TextGen::Header` and `TextGen::Paragraph` objects. A `Paragraph` further consists of `TextGen::Sentence` objects. Both `TextGen::Header` and `TextGen::Sentence` are built from the lowest-level objects `TextGen::Phrase` and `TextGen::Number`.

Sentences can be built like this:

```cpp
shared_ptr<Dictionary> dict = TextGen::DictionaryFactory::create("mysql");
dict->init("en");

TextGen::Sentence s;
s << TextGen::Phrase("kaakko");
s << TextGen::Number<int>(12);
cout << s.realize(dict);
```

which prints "South east 12.". The example uses the `TextGen` namespace explicitly only for the sake of Doxygen; in normal code there would be a

```cpp
using namespace TextGen;
```

somewhere. The code

```cpp
TextGen::Sentence s;
s << TextGen::Phrase("luode") << TextGen::Number<float>(12.33333333333);
cout << s.realize(dict);
```

prints "South east 12.3.", i.e. numbers have some default display precision. The precision can be changed using the second template parameter of `TextGen::Number` in traits style.

For strings and integers there are default conversions to `TextGen::Phrase` and `TextGen::Number<int>`, so the following also works:

```cpp
TextGen::Sentence s;
s << "kaakko" << 12;
cout << s.realize(dict);
```

City names have their own type `TextGen::LocationPhrase`. The phrase returns itself capitalized, unless a proper translation is found in the dictionary. Each separate word in the phrase is capitalized, as well as every hyphen-separated part.

Full paragraphs are created as follows:

```cpp
TextGen::Paragraph p;
TextGen::Sentence s1;
s1 << "ke" << "kaakko" << 12;
TextGen::Sentence s2;
s2 << "to" << "luode" << 13;
p << s1 << s2;
cout << p.realize(dict);
```

which would print "Wed south east 12. Thu north west 13.".

## Delimiters

The `TextGen::Delimiter` class represents a delimiter. The delimiter is given in the constructor. The class's `realize` method returns the string given in the constructor. A key feature is that the `prefix` method returns an empty string, which allows the `TextGen::Sentence` class to attach the delimiter directly to the previous word.

For example:

```cpp
Sentence s;
s << "kaakko" << 12 << Delimiter(",") << "etelä" << 13;
cout << s.realize(dict);
```

would return the string "Kaakko 12, etelä 13."

## Modifying numbers

`TextGen::Number` is a template that takes both the displayed number's type and a policy class with a static `tostring` method for conversion.

For example, integers are essentially the class

```cpp
Number<int, DefaultNumberTraits<int> >
```

where the default `DefaultNumberTraits` is

```cpp
template <typename Value>
struct DefaultNumberTraits
{
      static std::string tostring(const Value & theValue)
      {
        std::ostringstream os;
        os << theValue;
        return os.str();
      }
};
```

Integers are thus printed normally, as are most other types.

`float` and `double` types are specialized templates, for example:

```cpp
template <>
struct DefaultNumberTraits<float>
{
      static std::string tostring(float theValue)
      {
        std::ostringstream os;
        os << std::fixed << std::setprecision(1) << theValue;
        std::string ret = os.str();
        if(ret.size() > 2)
              if(ret.substr(ret.size()-2,2) == ".0")
                return ret.substr(0,ret.size()-2);
        return ret;
      }
};
```

Floats are thus printed with one decimal of precision, but a possible ".0" ending is stripped off.

Defining your own number types is possible by defining a suitable traits class and a suitable typedef for convenience. For example, below is a traits class suitable for rounding wind direction to the nearest 45 degrees:

```cpp
template <typename Value, int Mult>
struct WindDirectionTraits
{
      static std::string tostring(const Value & theValue)
      {
        std::ostringstream os;
        int value = static_cast<int>(std::floor(theValue/Mult+0.5)*Mult);
        value %= 360;
        os << value;
        return os.str();
      }
};
```

For example, the following code prints the values 45 and 0:

```cpp
typedef TextGen::Number<float, WindDirectionTraits<float,45> > PhraseWDir45

cout << PhraseWDir45(30).realize(dict) << endl
     << PhraseWDir45(359).realize(dict) << endl;
```
