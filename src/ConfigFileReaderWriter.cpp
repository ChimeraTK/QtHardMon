#include "ConfigFileReaderWriter.h"
#include <QString>
#include <QStringList>

#include <fstream>

ConfigFileReaderWriter::ConfigFileReaderWriter(std::string const &fileName)
    : _intValues(), _doubleValues(), _stringValues(), _fileName(), _badLines() {
  read(fileName);
}

ConfigFileReaderWriter::ConfigFileReaderWriter()
    : _intValues(), _doubleValues(), _stringValues(), _fileName(),
      _badLines() { /*currently empty*/
}

void ConfigFileReaderWriter::read(std::string const &fileName) {
  std::ifstream inFileStream;
  // turn on the throwing of exceptions in case of failure
  inFileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  inFileStream.open(fileName.c_str());

  // reading will fail at end of file, so only throw in case of bad
  inFileStream.exceptions(std::ifstream::badbit);

  // clear the list of bad lines
  _badLines.clear();

  while (!inFileStream.eof()) {
    std::string lineString;
    std::getline(inFileStream, lineString);

    // we use QString for the splitting, it's more convenient than plain C++
    QString qLineString(lineString.c_str());
    // split at one ore more whitespace characters as separator
    // This will also remove whitespace characters in front of the first token
    QStringList tokens =
        qLineString.split(QRegExp("\\s+"), QString::SkipEmptyParts);

    // debug output
    //    std::cout << "line is ";
    //    for (QStringList::iterator tokenIter = tokens.begin() ;
    //	 tokenIter != tokens.end(); ++tokenIter)
    //    {
    //       std::cout << "\"" << tokenIter->toStdString() << "\"  ";
    //    }
    //    std::cout << std::endl;

    if (tokens.size() == 0) {
      // it's an empty line, just continue
      continue;
    }

    // check for comment line
    if (tokens.begin()->startsWith("#")) {
      // it's a comment, just continue
      continue;
    }

    if (tokens.size() < 3) {
      // it's a bad line, add it to the list n empty line and continue parsing
      _badLines.append(qLineString);
      continue;
    }

    // read the three tokens we expect
    QStringList::const_iterator tokenIter = tokens.begin();
    QString variableName = *(tokenIter++);
    QString variableType = *(tokenIter++);
    QString variableValue = *(tokenIter);

    if (variableType == "string") {
      // value is a string, just add it
      _stringValues[variableName.toStdString()] = variableValue.toStdString();
    } else if (variableType == "int") {
      // check if conversion to int works
      bool conversionOk;

      // The 0 as last argument is the base. 0 also allows to read 0xNNN as hex
      // and 0NNN as octal.
      int intValue = variableValue.toInt(&conversionOk, 0);

      if (conversionOk) {
        _intValues[variableName.toStdString()] = intValue;
      } else {
        _badLines.append(qLineString);
      }
    } else if (variableType == "double") {
      // check if conversion to int works
      bool conversionOk;

      double doubleValue = variableValue.toDouble(&conversionOk);

      if (conversionOk) {
        _doubleValues[variableName.toStdString()] = doubleValue;
      } else {
        _badLines.append(qLineString);
      }
    } else {
      // unknown type, this is a bad line
      _badLines.append(qLineString);
    }

  } // while not eof

  // debug: print the bad lines
  //  std::cout << "BadLines: " << std::endl;
  //  for (QStringList::iterator badLinesIter = _badLines.begin() ;
  //       badLinesIter != _badLines.end(); ++badLinesIter)
  //    {
  //      std::cout << badLinesIter->toStdString() << std::endl;
  //    }

  // debug: write to debug.cfg
  // write("debug.cfg");
}

void ConfigFileReaderWriter::write(std::string const &fileName) const {
  std::ofstream outFileStream;
  // turn on the throwing of exceptions in case of failure
  outFileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  outFileStream.open(fileName.c_str());

  // let's have a nice header
  outFileStream << "#A config file for the QtHardMon user settings\n"
                << "#File format is\n"
                << "#\n"
                << "#variableName\t type\t value\n"
                << "#\n"
                << "#where type can be either int, double or string\n\n";

  // now write the data members. Loop all the maps, starting with the strings.
  for (std::map<std::string, std::string>::const_iterator stringIter =
           _stringValues.begin();
       stringIter != _stringValues.end(); ++stringIter) {
    outFileStream << stringIter->first << "\t"   // The variable name
                  << "string\t"                  // the type
                  << stringIter->second << "\n"; // the value
  }

  for (std::map<std::string, int>::const_iterator intIter = _intValues.begin();
       intIter != _intValues.end(); ++intIter) {
    outFileStream << intIter->first << "\t"   // The variable name
                  << "int\t"                  // the type
                  << intIter->second << "\n"; // the value
  }

  for (std::map<std::string, double>::const_iterator doubleIter =
           _doubleValues.begin();
       doubleIter != _doubleValues.end(); ++doubleIter) {
    outFileStream << doubleIter->first << "\t"   // The variable name
                  << "double\t"                  // the type
                  << doubleIter->second << "\n"; // the value
  }

  // Is this needed? The stream goes out of scope anyway and should close the
  // file.
  outFileStream.close();
}

QStringList const &ConfigFileReaderWriter::getBadLines() const {
  return _badLines;
}
