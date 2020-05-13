#include "framework/filesystem.h"
#include "library/strings.h"
#include <physfs.h>

namespace OpenApoc::fs
{
namespace sys_fs = ::OpenApoc::sys_fs;

bool create_directories(const path &p)
{
	auto ret = PHYSFS_mkdir(p.string().c_str());
	return (ret != 0);
}

struct delete_recursive_data
{
	std::string error;
	std::uintmax_t num_deleted = 0;
};

PHYSFS_EnumerateCallbackResult delete_recursive_callback(void *data, const char *origdir,
                                                         const char *fname)
{
	auto *data_struct = reinterpret_cast<delete_recursive_data *>(data);
	std::string fullpath = origdir;
	if (fname)
	{
		fullpath += '/';
		fullpath += fname;
	}
	PHYSFS_Stat stat_buffer;
	auto stat_ret = PHYSFS_stat(fullpath.c_str(), &stat_buffer);
	if (stat_ret == 0)
	{
		data_struct->error = "Failed to stat \"" + fullpath + "\" Error: \"";
		data_struct->error += PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
		data_struct->error += "\"";
		return PHYSFS_ENUM_ERROR;
	}
	if (stat_buffer.filetype == PHYSFS_FILETYPE_DIRECTORY)
	{
		auto ret = PHYSFS_enumerate(fullpath.c_str(), delete_recursive_callback, data);
		if (ret == 0)
		{
			return PHYSFS_ENUM_ERROR;
		}
		return PHYSFS_ENUM_OK;
	}

	auto ret = PHYSFS_delete(fullpath.c_str());
	if (ret == 0)
	{
		data_struct->error = "Failed to delete \"" + fullpath + "\" Error: \"";
		data_struct->error += PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
		data_struct->error += "\"";
		return PHYSFS_ENUM_ERROR;
	}

	return PHYSFS_ENUM_OK;
}

std::uintmax_t remove_all(const path &p)
{
	auto path = p.string();
	if (!exists(p))
		return 0;
	delete_recursive_data data;
	auto ret = delete_recursive_callback(&data, path.c_str(), nullptr);
	if (ret != PHYSFS_ENUM_OK)
	{
		throw filesystem_error(data.error, std::error_code(EIO, std::generic_category()));
	}

	return data.num_deleted;
}

bool is_directory(const path &p)
{
	auto fullpath = p.string();
	PHYSFS_Stat stat_buffer;
	auto stat_ret = PHYSFS_stat(fullpath.c_str(), &stat_buffer);
	if (stat_ret == 0)
	{
		std::string err = "Failed to stat \"" + fullpath + "\" Error: \"";
		err += PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
		err += "\"";
		throw filesystem_error(err, std::error_code(EIO, std::generic_category()));
	}
	return (stat_buffer.filetype == PHYSFS_FILETYPE_DIRECTORY);
}

bool exists(const path &p)
{
	auto fullpath = p.string();
	return (PHYSFS_exists(fullpath.c_str()) != 0);
}

std::uintmax_t file_size(const path &p)
{
	auto fullpath = p.string();
	PHYSFS_Stat stat_buffer;
	auto stat_ret = PHYSFS_stat(fullpath.c_str(), &stat_buffer);
	if (stat_ret == 0)
	{
		std::string err = "Failed to stat \"" + fullpath + "\" Error: \"";
		err += PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
		err += "\"";
		throw filesystem_error(err, std::error_code(EIO, std::generic_category()));
	}
	return stat_buffer.filesize;
}

void rename(const path &old_p, const path &new_p)
{
	auto old_system_path = system_path(old_p);
	auto new_system_path = system_path(new_p);
	sys_fs::rename(old_system_path, new_system_path);
}

path path::parent_path() const
{
	auto split_path = OpenApoc::split(_utf8_path, preferred_separator);
	if (split_path.size() == 0)
		return "/";

	path newpath = "/";
	for (size_t s = 0; s < split_path.size() - 1; s++)
	{
		newpath /= split_path[s];
	}
	return newpath;
}

sys_fs::path system_path(const path &p)
{
	auto fullpath = p.string();
	auto directory = PHYSFS_getRealDir(fullpath.c_str());
	if (!directory)
	{
		std::string err = "Failed to get system path for \"" + fullpath + "\" Error: \"";
		err += PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
		err += "\"";
		throw filesystem_error(err, std::error_code(EIO, std::generic_category()));
	}
	if (is_directory(p))
		return directory;
	else
		return std::string(directory) + path::preferred_separator + p.filename().string();
}

path path::filename() const
{
	if (is_directory(*this))
	{
		return "";
	}
	auto split_path = OpenApoc::split(_utf8_path, preferred_separator);
	return split_path[split_path.size() - 1];
}

path path::extension() const
{
	auto filename = this->filename();
	auto split_string = OpenApoc::split(filename._utf8_path, '.');
	if (split_string.size() == 1)
		return "";
	return (split_string[split_string.size() - 1]);
}

path operator/(const path &lhs, const path &rhs) { return path(lhs) /= rhs; }

class directory_iterator::physfs_directory_iterator
{
  private:
	friend class directory_iterator;
	path root_path;
	std::list<directory_entry> entries;
	std::list<directory_entry>::const_iterator it_position;

  public:
	static PHYSFS_EnumerateCallbackResult list_directory_entries(void *data, const char *origdir,
	                                                             const char *fname)
	{
		auto *dir_data = reinterpret_cast<physfs_directory_iterator *>(data);
		directory_entry entry;
		entry._path = path(origdir) / path(fname);
		dir_data->entries.push_back(std::move(entry));
		return PHYSFS_ENUM_OK;
	}
	physfs_directory_iterator(const path &p)
	{
		root_path = p;
		auto fullpath = p.string();
		auto ret = PHYSFS_enumerate(fullpath.c_str(), list_directory_entries, this);
		if (ret == 0)
		{
			std::string err = "Failed to enumerate directory \"" + fullpath + "\" error:\"";
			err += PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
			err += "\"";
			throw filesystem_error(err, std::error_code(EIO, std::generic_category()));
		}
		it_position = entries.begin();
	}

	bool operator==(const physfs_directory_iterator &rhs) const noexcept
	{
		return (root_path == rhs.root_path && entries == rhs.entries &&
		        it_position == rhs.it_position);
	}
};

directory_iterator::directory_iterator(const path &path) {}
directory_iterator::~directory_iterator() { delete _impl; }

directory_iterator &directory_iterator::operator++()
{
	if (!_impl)
	{
		throw filesystem_error("operator++ called on invalid directory_iterator",
		                       std::error_code(EIO, std::generic_category()));
	}
	++_impl->it_position;
	return *this;
}

bool directory_iterator::operator==(const directory_iterator &rhs) const noexcept
{
	if (_impl == nullptr && rhs._impl == nullptr)
		return true;
	if (_impl)
	{
		if (rhs._impl)
			return _impl == rhs._impl;
		if (!rhs._impl && _impl->it_position == _impl->entries.end())
			return true;
		return false;
	}
	if (rhs._impl)
	{
		if (rhs._impl->it_position == _impl->entries.end())
			return true;
	}
	return false;
}

const directory_entry &directory_iterator::operator*() const
{
	if (_impl)
		throw filesystem_error("operator* called on invalid directory_iterator",
		                       std::error_code(EIO, std::generic_category()));
	return *_impl->it_position;
}

const directory_entry *directory_iterator::operator->() const
{
	if (_impl)
		throw filesystem_error("operator-> called on invalid directory_iterator",
		                       std::error_code(EIO, std::generic_category()));
	return &(*_impl->it_position);
}

const path &directory_entry::path() const noexcept { return this->_path; }

} // namespace OpenApoc::fs
