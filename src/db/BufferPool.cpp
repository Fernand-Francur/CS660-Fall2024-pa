#include <db/BufferPool.hpp>
#include <db/Database.hpp>
#include <numeric>

using namespace db;

BufferPool::BufferPool()
// TODO pa1: add initializations if needed
{
  freePages = DEFAULT_NUM_PAGES;
  first = nullptr;
  current = nullptr;
  last = nullptr;
  // TODO pa1: additional initialization if needed
}

BufferPool::~BufferPool() {
  // TODO pa1: flush any remaining dirty pages
  if (freePages == DEFAULT_NUM_PAGES) {
  } else {
    current = first;
    Database &db = getDatabase();
    while (current != nullptr) {
      if (current->isDirty) {
        DbFile *currFile = &db.get(current->pageId.file);
        currFile->writePage(current->page, current->pageId.page);
        current->isDirty = false;
      }
      current = current->next;
    }
  }
}

Page &BufferPool::getPage(const PageId &pid) {
  // TODO pa1: If already in buffer pool, make it the most recent page and return it

  // TODO pa1: If there are no available pages, evict the least recently used page. If it is dirty, flush it to disk

  // TODO pa1: Read the page from disk to one of the available slots, make it the most recent page

  if (freePages != DEFAULT_NUM_PAGES) {
    searchPid(pid);
  } else {
    current = nullptr;
  }
  if (current != nullptr) {
    if (current != first) {
      if (current->next == nullptr) {
        current->prev->next = nullptr;
        last = current->prev;
        first->prev = current;
        current->prev = nullptr;
        current->next = first;
        first = current;
        return first->page;
      } else {
        current->next->prev = current->prev;
        current->prev->next = current->next;
        current->prev = nullptr;
        current->next = first;
        first->prev = current;
        first = current;
        return first->page;
      }
    } else {
      return first->page;
    }
  } else {
    Database &db = db::getDatabase();
    if (freePages == DEFAULT_NUM_PAGES) {
      first = new PCB;
      first->isDirty = false;
      first->next = nullptr;
      first->prev = nullptr;
      first->pageId = pid;
      last = first;

      Page *newPage = new Page;
      DbFile *currFile = &db.get(pid.file);
      currFile->readPage(*newPage, pid.page);
      first->page = *newPage;

      freePages--;
      return first->page;
    }
    if (freePages == 0) {
      if (last->isDirty) {
        flushPage(last->pageId);
      }
      discardPage(last->pageId);
    }
    if (freePages > 0) {
      current = new PCB;
      current->isDirty = false;
      current->next = first;
      current->next->prev = current;
      current->prev = nullptr;
      current->pageId = pid;
      first = current;

      Page *newPage = new Page;
      DbFile *currFile = &db.get(pid.file);
      currFile->readPage(*newPage, pid.page);
      first->page = *newPage;

      freePages--;
      return first->page;
    } else {
      throw std::logic_error("Something went wrong in get page");
    }
  }
}

void BufferPool::markDirty(const PageId &pid) {
  // TODO pa1: Mark the page as dirty. Note that the page must already be in the buffer pool
  if (freePages == DEFAULT_NUM_PAGES) {
    throw std::logic_error("No such page in bufferpool");
  } else {
    searchPid(pid);
    if (current == nullptr) {
      throw std::logic_error("No such page in bufferpool");
    } else {
      current->isDirty = true;
    }
  }
}

bool BufferPool::isDirty(const PageId &pid) const {
  // TODO pa1: Return whether the page is dirty. Note that the page must already be in the buffer pool
  if (freePages == DEFAULT_NUM_PAGES) {
    throw std::logic_error("No such page in bufferpool");
  } else {
    searchPid(pid);
    if (current == nullptr) {
      throw std::logic_error("No such page in bufferpool");
    } else {
      return current->isDirty;
    }
  }
}

bool BufferPool::contains(const PageId &pid) const {
  // TODO pa1: Return whether the page is in the buffer pool
  if (freePages == DEFAULT_NUM_PAGES) {
    return false;
  } else {
    searchPid(pid);
    if (current == nullptr) {
      return false;
    } else {
      return true;
    }
  }
}

void BufferPool::discardPage(const PageId &pid) {
  // TODO pa1: Discard the page from the buffer pool. Note that the page must already be in the buffer pool
  if (freePages == DEFAULT_NUM_PAGES) {
    throw std::logic_error("No such page in bufferpool");
  } else {
    searchPid(pid);
    if (current == nullptr) {
      throw std::logic_error("No such page in bufferpool");
    } else {
      if (current == first) {
        if (first->next == nullptr) {
          free(first);
          freePages++;
          first = nullptr;
          last = nullptr;
        } else {
          first = first->next;
          free(current);
          first->prev = nullptr;
          freePages++;
        }
      } else {
        if (current->next == nullptr) {
          current->prev->next = nullptr;
          last = current->prev;
          free(current);
          current = first;
          freePages++;
        } else {
          current->next->prev = current->prev;
          current->prev->next = current->next;
          free(current);
          current = first;
          freePages++;
        }
      }
    }
  }
}

void BufferPool::flushPage(const PageId &pid) {
  // TODO pa1: Flush the page to disk. Note that the page must already be in the buffer pool
  if (freePages == DEFAULT_NUM_PAGES) {
    throw std::logic_error("No such page in bufferpool");
  } else {
    searchPid(pid);
    if (current != nullptr) {
      if (current->isDirty) {
        Database &db = getDatabase();
        DbFile *currFile = &db.get(current->pageId.file);
        currFile->writePage(current->page,current->pageId.page);
        current->isDirty = false;
      }
    } else {
      throw std::logic_error("No such page in bufferpool");
    }
  }
}

void BufferPool::flushFile(const std::string &file) {
  // TODO pa1: Flush all pages of the file to disk
  if (freePages == DEFAULT_NUM_PAGES) {
    throw std::logic_error("No such file in bufferpool");
  } else {
    current = first;
    Database &db = getDatabase();
    DbFile *currFile = &db.get(file);
    while (current != nullptr) {
      if (current->pageId.file == file && current->isDirty) {
        currFile->writePage(current->page, current->pageId.page);
        current->isDirty = false;
      }
      current = current->next;
    }
  }
}

void BufferPool::searchPid(const PageId &pid) const {
  current = first;
  do {
    if (current->pageId == pid) {
      break;
    }
    current = current->next;
  } while (current != nullptr);
}

bool BufferPool::searchFile(const std::string &name) const {
  current = first;
  if (first == nullptr) {
    return false;
  }
  do {
    if (current->pageId.file == name) {
      return true;
    }
    current = current->next;
  } while (current != nullptr);
  return false;
}

void BufferPool::discardFile(const std::string &file) {
  // TODO pa1: Flush all pages of the file to disk
    current = first;
    while (current != nullptr) {
      if (current->pageId.file == file) {
        discardPage(current->pageId);
      }
      current = current->next;
    }
}