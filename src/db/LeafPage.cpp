#include <db/LeafPage.hpp>
#include <stdexcept>

using namespace db;

LeafPage::LeafPage(Page &page, const TupleDesc &td, size_t key_index) : td(td), key_index(key_index) {
  capacity= (DEFAULT_PAGE_SIZE * 8 - sizeof(LeafPageHeader))/ (td.length() * 8);
  //size_t freeBytes = DEFAULT_PAGE_SIZE - capacity * td.length();
  data = (uint8_t*)&page;
  data += sizeof(LeafPageHeader);
  header = (LeafPageHeader*)&page;
}

bool LeafPage::insertTuple(const Tuple &t) {
  bool tupleFound = false;
  for(int i = 0; i < header->size; i++) {
    Tuple currTuple = this->td.deserialize(data + this->td.length() * i);
    if(currTuple.get_field(key_index) >= t.get_field(key_index)) {
      tupleFound = true;
      if (currTuple.get_field(key_index) == t.get_field(key_index)) {
        this->td.serialize(data+this->td.length()*i,t);
        break;
      } else {
        for (int j = header->size - 1; j >= i; j--) {
          Tuple oldTuple = this->td.deserialize(data + this->td.length() * j);
          this->td.serialize(data+this->td.length()*(j+1),oldTuple);
        }
        this->td.serialize(data+this->td.length()*i,t);
        header->size++;
        break;
      }
    }
  }
  if(!tupleFound) {
    this->td.serialize(data+this->td.length()*header->size,t);
    header->size++;
  }
  // TODO: Write header back to page
  if(header->size == this->capacity) {
    return true;
  } else {
    return false;
  }

}

int LeafPage::split(LeafPage &new_page) {
  int copyAmount = this->capacity / 2;
  for(int i = 0; i < (copyAmount + this->capacity % 2); i++) {
    Tuple currTuple = this->td.deserialize(data + this->td.length() * (copyAmount + i));
    new_page.td.serialize(new_page.data+this->td.length()*(i),currTuple);
    new_page.header->size++;
    this->header->size--;
  }
  new_page.header->next_leaf = this->header->next_leaf;
  return std::get<int>(new_page.td.deserialize(new_page.data).get_field(key_index));

}

Tuple LeafPage::getTuple(size_t slot) const {
  return this->td.deserialize(data + this->td.length() * slot);
}
