#include <db/Database.hpp>

using namespace db;

BufferPool &Database::getBufferPool() { return bufferPool; }

Database &db::getDatabase() {
  static Database instance;
  return instance;
}

void Database::add(std::unique_ptr<DbFile> file) {
  // TODO pa1: add the file to the catalog. Note that the file must not exist.
  if (!(data.find(file->getName()) == data.end())) {
    throw std::logic_error("File name already exists");
  } else {
    if (file) {
      data[file->getName()] = std::move(file);
    } else {
      throw std::logic_error("No file in unique_ptr");
    }
  }
}

std::unique_ptr<DbFile> Database::remove(const std::string &name) {
  // TODO pa1: remove the file from the catalog. Note that the file must exist.
  if (auto search = data.find(name); search != data.end()) {
    std::unique_ptr<DbFile> tmp = std::move(search->second);
    data.erase(search); //FLUSHFILE HERE INSTEAD
    return tmp;
  } else {
    throw std::logic_error("No such file name in Database");
  }
}

DbFile &Database::get(const std::string &name) const {
  // TODO pa1: get the file from the catalog. Note that the file must exist.
  if (auto search = data.find(name); search != data.end()) {
    DbFile *tmp = search->second.get();
    return *tmp;
  } else {
    throw std::logic_error("No such file name in Database");
  }
}