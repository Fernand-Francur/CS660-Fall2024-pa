#include <cstring>
#include <db/BTreeFile.hpp>
#include <db/Database.hpp>
#include <db/IndexPage.hpp>
#include <db/LeafPage.hpp>
#include <stdexcept>

using namespace db;

BTreeFile::BTreeFile(const std::string &name, const TupleDesc &td, size_t key_index)
    : DbFile(name, td), key_index(key_index) {
  page_num = 0;
}

void BTreeFile::insertTuple(const Tuple &t) {
  Page indexPage{};
  Page leafPage{};
  int lastIndex = root_id;
  readPage(indexPage,root_id);
  IndexPage index{indexPage};
  if(this->page_num == 0) {
    index.header->index_children = true;
    this->page_num++;
    readPage(leafPage,this->page_num);
    index.children[0] = this->page_num;
    LeafPage leaf1 = LeafPage(leafPage, this->td, key_index);
    leaf1.header->size = 0;
    leaf1.header->next_leaf = this->page_num + 1;
    writePage(leafPage, page_num);
    this->page_num++;
    readPage(leafPage,this->page_num);
    LeafPage leaf2 = LeafPage(leafPage, this->td, key_index);
    leaf2.insertTuple(t);
    leaf2.header->next_leaf = -1;
    index.insert(std::get<int>(t.get_field(key_index)),this->page_num);
    writePage(leafPage, page_num);
    writePage(indexPage, root_id);
  } else {
    std::vector<size_t> page_nums;
    // page_nums.push_back(root_id);
    while (1) {
      int i = 0;
      bool found = false;
      for (int j = 0; j < index.capacity; j++) {
        if (std::get<int>(t.get_field(key_index)) < index.keys[i]) {
          found = true;
          break;
        }
        i++;
        if (std::get<int>(t.get_field(key_index)) > index.keys[i-1] &&index.keys[i] == 0) {
          found = true;
          break;
        }
      }
//      if (!found) {
//        i++;
//      }

      readPage(leafPage, index.children[i]);
      if (index.header->index_children) {
        LeafPage leaf = LeafPage(leafPage, this->td, key_index);

        if (leaf.insertTuple(t)) {
          Page newPage{};
          LeafPage newLeaf = LeafPage(newPage, this->td, key_index);

          this->page_num++;
          int splitVal = leaf.split(newLeaf);
          leaf.header->next_leaf = this->page_num;
          this->writePage(newPage, this->page_num);
          this->writePage(leafPage, index.children[i]);
          bool full = false;
          while (index.insert(splitVal, this->page_num)) {

            Page tmpPage{};
            IndexPage newIndex{tmpPage};

            splitVal = index.split(newIndex);
            newIndex.header->index_children = index.header->index_children;
            this->page_num++;
            this->writePage(tmpPage, this->page_num);

            if (!page_nums.empty()) {
              size_t new_page_num = page_nums.back();
              this->writePage(indexPage, new_page_num);
              page_nums.pop_back();
              new_page_num = page_nums.back();
              lastIndex = new_page_num;
              readPage(indexPage, new_page_num);
              index = IndexPage(indexPage);
            } else {
              full = true;
              this->page_num++;
              this->readPage(tmpPage, root_id);
              this->writePage(tmpPage, page_num);

              Page newestPage{};
              IndexPage newestIndex{newestPage};
              newestIndex.header->index_children = false;
              index.children[0] = this->page_num;
              newestIndex.insert(splitVal, this->page_num - 1);
              this->writePage(newestPage, root_id);
              break;
            }
          }
          if (!full) {
            this->writePage(indexPage, lastIndex);
          }
        } else {
          this->writePage(leafPage, index.children[i]);
        }
        break;
      } else {
        readPage(indexPage, index.children[i]);
        page_nums.push_back(index.children[i]);
        lastIndex = index.children[i];
        index = IndexPage(indexPage);
      }
    }
  }
  // TODO pa2: implement
}

void BTreeFile::deleteTuple(const Iterator &it) {
  // Do not implement
}

Tuple BTreeFile::getTuple(const Iterator &it) const {
  // TODO pa2: implement
  Page page{};
  readPage(page, it.page);
  LeafPage leaf = LeafPage(page,this->td, key_index);
  Tuple currTuple = this->td.deserialize(leaf.data + this->td.length() * it.slot);
  return currTuple;
}

void BTreeFile::next(Iterator &it) const {
  it.slot++;
  Page page{};
  readPage(page, it.page);
  LeafPage leaf = LeafPage(page,this->td, key_index);
  if(it.slot == leaf.header->size) {
    it.page = leaf.header->next_leaf;
    it.slot = 0;
  }
  // TODO pa2: implement
}

Iterator BTreeFile::begin() const {
  if(page_num == 0) {
    return end();
  } else {
    Page indexPage{};
    readPage(indexPage, root_id);
    IndexPage index{indexPage};
    int page_id;
    page_id = index.children[0];
    while(!index.header->index_children) {
      readPage(indexPage, page_id);
      index = IndexPage(indexPage);
      page_id = index.children[0];
    }
    Page page{};
    readPage(page, index.children[0]);
    LeafPage leaf = LeafPage(page,this->td, key_index);
    if(leaf.header->size != 0) {
      return Iterator(*this, page_id, 0);
    } else {
      return Iterator(*this, index.children[1], 0);
    }

  }
  // TODO pa2: implement
}

Iterator BTreeFile::end() const {
  return Iterator(*this, -1, 0);
  // TODO pa2: implement
}
