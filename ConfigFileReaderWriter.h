#ifndef CONFIG_FILE_READER_WRITER_H
#define CONFIG_FILE_READER_WRITER_H

#include <map>
#include <string>
#include <iostream>
#include <QStringList>

/** Reader and writer class for config files. This class does the actual parsing and syntax checking. */
class ConfigFileReaderWriter
{
 public:
  /** When using the ReaderWriter as a reader you can directly read the file in the constructor.
   * \attention The constructor will throw an std::ifstream::failure exception when the file cannot be read!
   */
  ConfigFileReaderWriter(std::string const & fileName );
  
  /** Initialise the ReaderWriter without reading from a file.
   *  This constructor will allways succeed and not throw an exception.
   */
  ConfigFileReaderWriter();

  /** Read the config from a file. Will throw std::ifstream::failure if not successful.
   *  With an empty string the reader will try to read the previously read or written file.
   */
  void read( std::string const & fileName = std::string() ); 

  /** Write the config to a file. Will throw std::ifstream::failure if not successful. */
  void write( std::string const & fileName ) const;   

  /** Get a value by variabe name. The template function works for int, double and string.
   *  In case the variable name is not in the current config (usually read from a file) the
   *  default value is returned.
   */
  template <class T> T getValue(std::string const & variableName, T const & defaultValue) const;

  /** Set a value by variabe name. The template function works for int, double and string.
   *  This adds (or replaces) the variable in the internal maps, so the variable will be 
   *  written to file or can be read back by getValue().
   */
  template <class T> void setValue(std::string const & variableName, T const & value);

  /** In case there are bad line during parsing, these can be accessed from a list after parsing. */
  QStringList const & getBadLines() const;

  /** Return a map with all the integer variables (name and value) */
  std::map< std::string, int> const & getIntValues() const {return _intValues;}

  /** Return a map with all the double variables (name and value) */
  std::map< std::string, double>  const & getDoubleValues() const {return _doubleValues;}

  /** Return a map with all the string variables (name and value) */
  std::map< std::string, std::string> const & getStringValues() const {return _stringValues;}


 private:
  std::map< std::string, int> _intValues;
  std::map< std::string, double> _doubleValues;
  std::map< std::string, std::string> _stringValues;

  // Functions to access the local variables for the different types of maps from the template function.
  // The trick is that the functions have the same name for all types and are overloaded.
  // The * & is call by reference for a pointer, i.e the pointer is modifies by the function.
  void getMap( std::map< std::string, int> * & map) { map =  &_intValues; }
  void getMap( std::map< std::string, double> * & map) { map = &_doubleValues; };
  void getMap( std::map< std::string, std::string> * & map) { map =  &_stringValues; }

  // there also is a const version for use in const getter functions
  void getMap( std::map< std::string, int> const * & map) const { map =  &_intValues; }
  void getMap( std::map< std::string, double> const * & map) const { map = &_doubleValues; };
  void getMap( std::map< std::string, std::string> const * & map) const { map =  &_stringValues; }
  
  std::string _fileName;
  QStringList _badLines;
};

// the template code shold be in the header
template <class T> T ConfigFileReaderWriter::getValue(std::string const & variableName, T const & defaultValue) const
{
  // get the map using the getMap functions, which are overloaded by type
  std::map< std::string, T> const *map;
  getMap(map);
  
  // Try to find the value. When it's not found the iterator is map->end()
  typename std::map< std::string, T>::const_iterator valueIter = map->find(variableName);
  //std::map< std::string, T>::iterator valueIter = map->find(variableName);
  if ( valueIter == map->end() )
  {
    // not found, return the default
    return defaultValue;
  }
  else
  {
    // the map iter points to a pair. The 'second' entry is the value.
    return valueIter->second;
  }
}

template <class T> void ConfigFileReaderWriter::setValue(std::string const & variableName, T const & value)
{
    // get the map using the getMap functions, which are overloaded by type
    std::map< std::string, T> *map;
    getMap(map);

    (*map)[variableName] = value;
}

#endif // CONFIG_FILE_READER_WRITER_H
