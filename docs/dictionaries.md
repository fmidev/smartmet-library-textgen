# Dictionaries

## API

Dictionary objects inherit from the `Dictionary` class, which defines the API for dictionary objects.

```cpp
void init(const std::string & theLanguage);
bool contains(const std::string & theKey) const;
const std::string & find(const std::string & theKey) const;
void insert(const std::string & theKey, const std::string & thePhrase);

size_type size(void) const;
bool empty(void) const;
```

Dictionary objects are typically used as follows:

```cpp
MySQLDictionary dict;
dict.init("fi");
cout << dict.find("sää");
```

Factory-style usage is similar:

```cpp
std::unique_ptr<Dictionary> dict = DictionaryFactory::create("mysql");
dict->init("fi");
cout << dict->find("sää");
```

## Global dictionary

The dictionary is used almost everywhere in the text generator. Passing dictionary objects around the program as parameters is not practical. A better solution is to use a singleton class to represent the dictionary. In the text generator, the `TheDictionary` class acts as the singleton.

`TheDictionary` otherwise has the same interface as the `Dictionary` class, but in addition the singleton is initialized with the `dictionary` method to use a dictionary of the desired type. For example:

```cpp
TheDictionary::instance().dictionary(DictionaryFactory::create("mysql"));
TheDictionary::instance().init("fi");
cout << TheDictionary::instance().find("sää");
```
