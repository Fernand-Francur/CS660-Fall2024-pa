#include <cstring>
#include <db/Database.hpp>
#include <db/HeapPage.hpp>
#include <stdexcept>

using namespace db;

HeapPage::HeapPage(Page &page, const TupleDesc &td) : td(td) {
  // NOTE: header and data should point to locations inside the page buffer. Do not allocate extra memory.
  capacity = DEFAULT_PAGE_SIZE * 8 / (td.length() * 8 + 1);
  size_t freeBytes = DEFAULT_PAGE_SIZE - capacity * td.length();
  data = (uint8_t*)&page;
  data += freeBytes;
  header = (uint8_t*)&page;
}

int bit_ext(uint8_t num, int k, int p) {
  return (((1 << k) - 1) & (num >> (p - 1)));
}

uint8_t modifyBit(uint8_t n, int p, int b)
{
  int mask = 1 << p;
  return ((n & ~mask) | (b << p));
}

size_t HeapPage::begin() const {
  uint8_t *start = this->header;

  for(int i = 0; i < this->capacity; i++) {
    if ((!(i%8)) && i != 0) {
      start += 1;
    }
    if (bit_ext(*start,1,8-(i%8))) {
      return i;
    }
  }
  return this->capacity;
}

size_t HeapPage::end() const {
  return this->capacity;
}

bool HeapPage::insertTuple(const Tuple &t) {
  uint8_t *start = this->header;
  int freeTuple = (int)this->capacity;
  int increase = 0;
  for(int i = 0; i < this->capacity; i++) {
    if ((!(i%8)) && i != 0) {
      start += 1;
      increase++;
    }
    if (!bit_ext(*start,1,8-(i%8)  )) {
      freeTuple = i;
      break;
    }
  }
  if (freeTuple == this->capacity) {
    return false;
  } else {
    start = data + freeTuple * this->td.length();
    this->td.serialize(start, t);
    this->header[increase] = modifyBit(this->header[increase], 8 - (freeTuple % 8 + 1), 1);
    return true;
  }
}

void HeapPage::deleteTuple(size_t slot) {
  int offset = (int) slot / 8;
  int bitOffset = (int) slot % 8;
  if (!bit_ext(*(this->header + offset),1,(int) (8 - bitOffset%8))) {
    throw std::logic_error("Slot is already deleted");
  }
  this->header[offset] = modifyBit(this->header[ offset], 8-(bitOffset%8+1), 0);
}

Tuple HeapPage::getTuple(size_t slot) const {
  if (this->empty(slot)) {
    throw std::logic_error("Empty Tuple slot\n");
  }
  return this->td.deserialize(data + this->td.length() * slot);
}

void HeapPage::next(size_t &slot) const {
  size_t currSlot = slot + 1;
  uint8_t *start = this->header + currSlot / 8;
  bool found = false;
  for(size_t i = currSlot; i <= this->capacity; i++) {
    if (!(i%8) && (i != currSlot)) {
      start += 1;
    }
    if (bit_ext(*start,1,(int) (8 - i%8))) {
      memcpy(&slot,&i, sizeof(size_t));
      found = true;
      break;
    }
  }
  if (!found) {
    memcpy(&slot,&this->capacity, sizeof(size_t));
  }

}

bool HeapPage::empty(size_t slot) const {
  int offset = (int) slot / 8;
  int bitOffset = (int) slot % 8;
  if (!bit_ext(*(this->header + offset),1,(int) (8 - bitOffset%8))) {
    return true;
  }
  return false;
}
