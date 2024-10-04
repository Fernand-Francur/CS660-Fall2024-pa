#include <db/Database.hpp>
#include <db/HeapFile.hpp>
#include <db/HeapPage.hpp>
#include <cstring>
#include <stdexcept>

using namespace db;

HeapFile::HeapFile(const std::string &name, const TupleDesc &td) : DbFile(name, td) {}

void HeapFile::insertTuple(const Tuple &t) {
  // TODO pa2: implement
  if(!this->td.compatible(t)) {
    throw std::logic_error("Incompatible Tuple\n");
  }

  bool found = false;
  for(int i=0; i < this->numPages; i++) {
    Page page{};
    this->readPage(page, i);
    HeapPage newHeapPage = HeapPage(page, this->td);
    if(newHeapPage.insertTuple(t)) {
      found = true;
      this->writePage(page,i);
      break;
    }
  }
  if (!found) {
    Page page{};
    this->numPages++;
    this->readPage(page, numPages-1);
    HeapPage newHeapPage = HeapPage(page, this->td);
    if(!newHeapPage.insertTuple(t)) {
      throw std::logic_error("Tuple could not be added to file\n");
    }
    this->writePage(page, numPages-1);
  }
}

void HeapFile::deleteTuple(const Iterator &it) {
  // TODO pa2: implement
    Page page{};
    this->readPage(page, it.page);
    HeapPage newHeapPage = HeapPage(page, this->td);
    newHeapPage.deleteTuple(it.slot);
    this->writePage(page, it.page);
}

Tuple HeapFile::getTuple(const Iterator &it) const {
  // TODO pa2: implement
  Page page{};
  this->readPage(page, it.page);
  HeapPage newHeapPage = HeapPage(page, this->td);

  return newHeapPage.getTuple(it.slot);
}

void HeapFile::next(Iterator &it) const {
  // TODO pa2: implement

  Page page{};
  this->readPage(page, it.page);
  HeapPage newHeapPage = HeapPage(page, this->td);
  newHeapPage.next(it.slot);
  if (it.slot == newHeapPage.end()) {
    it.page++;
    if(it.page == this->numPages) {
      Iterator newIt = this->end();
      memcpy((void *)&it, (void *) &newIt, sizeof(Iterator));
    } else {
      Page newPage{};
      this->readPage(newPage, it.page);
      HeapPage newHeapPage2 = HeapPage(newPage, this->td);
      it.slot = newHeapPage2.begin();
    }
  }
}

Iterator HeapFile::begin() const {
  // TODO pa2: implement
  Page page{};
  size_t slot;
  for(int i=0; i < this->numPages; i++) {
    this->readPage(page, i);
    HeapPage newHeapPage = HeapPage(page, this->td);
    slot = newHeapPage.begin();
    if (slot != newHeapPage.end()) {
      return Iterator(*this, i, slot);
    }
  }
  return Iterator(*this, this->numPages-1, HeapPage(page, this->td).end());
}

Iterator HeapFile::end() const {
  Page page{};
  this->readPage(page, this->numPages-1);
  return Iterator(*this, this->numPages-1, HeapPage(page, this->td).end());
  // TODO pa2: implement
}
