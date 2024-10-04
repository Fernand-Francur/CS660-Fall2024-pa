#include <cstring>
#include <db/DbFile.hpp>
#include <fcntl.h>
#include <stdexcept>
#include <sys/stat.h>
#include <unistd.h>

using namespace db;

const TupleDesc &DbFile::getTupleDesc() const { return td; }

DbFile::DbFile(const std::string &name, const TupleDesc &td) : name(name), td(td) {
  // TODO pa2: open file and initialize numPages
  // Hint: use open, fstat
  this->fd = open(this->name.c_str(), O_RDWR | O_CREAT);
  if (this->fd < 0) {
    throw std::runtime_error("Failed to open file");
  }
  struct stat fileBuffer;
  if (fstat(this->fd, &fileBuffer) == -1) {
    throw std::runtime_error("Failed to fstat file");
  }
  this->numPages = fileBuffer.st_size / DEFAULT_PAGE_SIZE;

}

DbFile::~DbFile() {
  // TODO pa2: close file
  // Hind: use close
  if(close(this->fd) != 0) {
     printf("File failed to close");
  }
}

const std::string &DbFile::getName() const { return name; }

void DbFile::readPage(Page &page, const size_t id) const {
  reads.push_back(id);
  // TODO pa2: read page
  // Hint: use pread
  pread(this->fd, page.data(), DEFAULT_PAGE_SIZE, id * DEFAULT_PAGE_SIZE);

}

void DbFile::writePage(const Page &page, const size_t id) const {
  writes.push_back(id);
  // TODO pa2: write page
  // Hint: use pwrite
  pwrite(this->fd, page.data(), DEFAULT_PAGE_SIZE, id * DEFAULT_PAGE_SIZE);
}

const std::vector<size_t> &DbFile::getReads() const { return reads; }

const std::vector<size_t> &DbFile::getWrites() const { return writes; }

void DbFile::insertTuple(const Tuple &t) { throw std::runtime_error("Not implemented"); }

void DbFile::deleteTuple(const Iterator &it) { throw std::runtime_error("Not implemented"); }

Tuple DbFile::getTuple(const Iterator &it) const { throw std::runtime_error("Not implemented"); }

void DbFile::next(Iterator &it) const { throw std::runtime_error("Not implemented"); }

Iterator DbFile::begin() const { throw std::runtime_error("Not implemented"); }

Iterator DbFile::end() const { throw std::runtime_error("Not implemented"); }

size_t DbFile::getNumPages() const { return numPages; }
