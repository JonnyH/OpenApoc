#pragma once

#include "library/sp.h"
#include "library/strings.h"
#include <boost/any.hpp>
#include <map>
#include <vector>

namespace OpenApoc
{

class ConfigFileImpl;
class ConfigOption;

class ConfigFile
{
  private:
	up<ConfigFileImpl> pimpl;
	ConfigFile();

  public:
	~ConfigFile();
	bool save();

	// Returns true if the settings have been read
	bool loaded() const;

	int getInt(const UString &key);
	bool getBool(const UString &key);
	UString getString(const UString &key);
	float getFloat(const UString &key);
	bool get(const UString &key); // returns true if the option was specified
	UString describe(const UString &section, const UString &name);

	void set(const UString &key, bool value);
	void set(const UString &key, int value);
	void set(const UString &key, UString value);
	void set(const UString &key, float value);

	void addOptionString(UString section, UString longName, UString shortName,
	                     UString description, UString defaultValue);
	void addOptionInt(UString section, UString longName, UString shortName,
	                  UString description, int defaultValue);
	void addOptionBool(UString section, UString longName, UString shortName,
	                   UString description, bool defaultValue);
	void addOptionFloat(UString section, UString longName, UString shortName,
	                    UString description, float defaultValue);
	void addOption(UString section, UString longName, UString shortName,
	               UString description);
	void addPositionalArgument(UString name, UString description);

	// returns 'true' if the program should exit (invalid option/'--help' specified)
	bool parseOptions(int argc, const char *const argv[]);

	// Prints out the help to stdout, used if the running program has decided some argument is
	// invalid
	void showHelp();

	std::map<UString, std::vector<ConfigOption>> getOptions();

	static ConfigFile &getInstance();
};

class ConfigOption
{
  private:
	UString section;
	UString name;
	UString description;

  public:
	ConfigOption(UString section, UString name, UString description);
	const UString &getName() const { return name; }
	const UString &getSection() const { return section; }
	const UString &getDescription() const { return description; }
	UString getKey() const;
};

class ConfigOptionString : public ConfigOption
{
  public:
	ConfigOptionString(UString section, UString name, UString description,
	                   UString defaultValue = "");
	UString get() const;
	void set(const UString &newValue);
};

class ConfigOptionInt : public ConfigOption
{
  public:
	ConfigOptionInt(UString section, UString name, UString description,
	                int defaultValue = 0);
	int get() const;
	void set(int newValue);
};

class ConfigOptionBool : public ConfigOption
{
  public:
	ConfigOptionBool(UString section, UString name, UString description,
	                 bool defaultValue = false);
	bool get() const;
	void set(bool newValue);
};

class ConfigOptionFloat : public ConfigOption
{
  public:
	ConfigOptionFloat(UString section, UString name, UString description,
	                  float defaultValue = 0.0f);
	float get() const;
};
static inline ConfigFile &config() { return ConfigFile::getInstance(); }

// validate overload required by boost::program_options for UString
// boost should find this through ADL.
// this is required for string values with spaces
void validate(boost::any &v, const std::vector<std::string> &values, UString *, int);

}; // namespace OpenApoc
