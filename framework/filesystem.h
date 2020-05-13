#pragma once

#include <iterator>
#include <locale>
#include <string>
#include <string_view>
#include <system_error>

#if defined(USE_STD_FILESYSTEM)

#include <filesystem>
namespace OpenApoc
{
namespace sys_fs = std::filesystem;
}
#elif defined(USE_BOOST_FILESYSTEM)

#include <boost/filesystem.hpp>
namespace OpenApoc
{
using sys_fs = boost : filesystem;
}
#else
#error No filesystem implementation
#endif

namespace OpenApoc::fs
{

// Note: All paths are utf8

class filesystem_error : public std::system_error
{
  public:
	filesystem_error(const std::string &what_arg, std::error_code ec)
	    : std::system_error(ec, what_arg)
	{
	}
};
class path
{
  private:
	std::string _utf8_path;

  public:
	enum format
	{
		// native_format,
		// generic_format,
		auto_format
	};
	using value_type = char;
	using string_type = std::basic_string<value_type>;

	static constexpr value_type preferred_separator = '/';

	path() = default;
	path(const path &p) { _utf8_path = p._utf8_path; }
	path(path &&p) noexcept { _utf8_path = std::move(p._utf8_path); };
	path(string_type &&source, format fmt = auto_format) { _utf8_path = std::move(source); };
	template <class Source> path(const Source &source, format fmt = auto_format)
	{
		_utf8_path = source;
	}
	// template <class InputIt> path(InputIt first, InputIt last, format fmt = auto_format);
	// template <class Source>
	// path(const Source &source, const std::locale &loc, format fmt = auto_format);
	// template <class InputIt>
	// path(InputIt first, InputIt last, const std::locale &loc, format fmt = auto_format);

	path parent_path() const;
	path filename() const;
	std::string string() const { return _utf8_path; }
	path extension() const;
	bool empty() const noexcept { return _utf8_path.empty(); }
	path &operator/=(const path &p)
	{
		_utf8_path += preferred_separator + p.string();
		return *this;
	}

	path &operator=(const path &p)
	{
		_utf8_path = p._utf8_path;
		return *this;
	}
	path &operator=(path &&p) noexcept
	{
		_utf8_path = std::move(p._utf8_path);
		return *this;
	}
	path &operator=(string_type &&source)
	{
		_utf8_path = std::move(source);
		return *this;
	}
};

class directory_entry
{
  private:
	path _path;

	friend class directory_iterator;

  public:
	directory_entry() noexcept = default;
	directory_entry(const directory_entry &) = default;
	directory_entry(directory_entry &&) noexcept = default;
	explicit directory_entry(const path &p);
	const path &path() const noexcept;
	operator const class path &() const noexcept;

	bool operator==(const directory_entry &rhs) const noexcept;
	bool operator!=(const directory_entry &rhs) const noexcept;
};
class directory_iterator
{
  private:
	class physfs_directory_iterator;
	physfs_directory_iterator *_impl = nullptr;

  public:
	using value_type = directory_entry;
	using difference_type = std::ptrdiff_t;
	using pointer = const directory_entry *;
	using reference = const directory_entry &;
	using iterator_category = std::input_iterator_tag;

	directory_iterator() noexcept = default;
	explicit directory_iterator(const path &p);
	~directory_iterator();

	directory_iterator(const directory_iterator &) = default;
	directory_iterator(directory_iterator &&) = default;

	const directory_entry &operator*() const;
	const directory_entry *operator->() const;

	directory_iterator &operator++();
	directory_iterator operator++(int);

	bool operator==(const directory_iterator &rhs) const noexcept;
	bool operator!=(const directory_iterator &rhs) const noexcept { return !(*this == rhs); }
};

directory_iterator begin(directory_iterator iter) noexcept;
directory_iterator end(const directory_iterator &) noexcept;

bool operator==(const path &lhs, const path &rhs) noexcept;
path operator/(const path &lhs, const path &rhs);
bool exists(const path &p);

template <class CharT, class Traits>
std::basic_ostream<CharT, Traits> &operator<<(std::basic_ostream<CharT, Traits> &os, const path &p)
{
	return os << p.string();
}

path canonical(const path &p);

bool is_directory(const path &p);

bool create_directories(const path &p);
bool remove(const path &p);
std::uintmax_t remove_all(const path &p);
std::uintmax_t file_size(const path &p);
void rename(const path &old_p, const path &new_p);

path relative(const path &p, const path &base = {});

// New functions to support physfs init/mounting/etc
// All paths are system paths
enum class portable_mode
{
	portable,
	non_portable,
	automatic
};
sys_fs::path system_path(const path &p);
sys_fs::path get_write_dir();
void fs_init(const std::string program_name, const std::string organisation,
             portable_mode mode = portable_mode::automatic);
void fs_deinit();
void fs_mount(std::string new_dir, std::string mount_point, bool append);
void fs_unmount(std::string old_dir);
void fs_set_write_dir(std::string path);

}; // namespace OpenApoc::fs
