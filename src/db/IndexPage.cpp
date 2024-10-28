#include <cstring>
#include <db/IndexPage.hpp>
#include <stdexcept>

using namespace db;

IndexPage::IndexPage(Page &page) {
  // TODO pa2: implement
  capacity= (DEFAULT_PAGE_SIZE * 8 - (sizeof(IndexPageHeader)*8))/ ((sizeof(int)+sizeof(size_t)) * 8)-1;
  //size_t freeBytes = DEFAULT_PAGE_SIZE - capacity * td.length();
  keys = (int*)&page;
  keys += sizeof(IndexPageHeader);
  children = (size_t*)&page;
  children += (capacity / 2) + 2;
  header = (IndexPageHeader*)&page;
}

bool IndexPage::insert(int key, size_t child) {
  // TODO pa2: implement
  int i = 0;
  for(int j=0; j < this->header->size; j++) {
    if (key < this->keys[j]) break;
    i++;
  }
  for (int j = header->size - 1; j >= i; j--) {
    keys[j + 1] = keys[j];
    children[j+2] = children[j+1];
  }
  keys[i] = key;
  children[i+1] = child;
  this->header->size++;
  if(this->header->size == this->capacity) {
    return true;
  } else {
    return false;
  }
}

int IndexPage::split(IndexPage &new_page) {
  // TODO pa2: implement
  int copyAmount = (this->capacity) / 2;
  int retVal = this->keys[copyAmount];
  for(int i = 1; i < (copyAmount); i++) {
    new_page.keys[i-1] = this->keys[copyAmount + i];
    keys[copyAmount + i] = 0;
    new_page.children[i-1]=this->children[copyAmount + i];
    children[copyAmount + i] = 0;
    this->header->size--;
    new_page.header->size++;
  }
  this->keys[copyAmount] = 0;
  this->header->size--;
  new_page.children[new_page.header->size]=this->children[this->header->size-1];
  return retVal;
}
