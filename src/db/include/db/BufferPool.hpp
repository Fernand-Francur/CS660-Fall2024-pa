#pragma once

#include <db/types.hpp>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <vector>

/*
 * The bufferpool is implemented in the most logical way, using a doubly-linked list
 * as the backend data structure for LRU. For this purpose, I use the pageControlBlock (PCB)
 * As the structure for each linked-list block, containing the pageId, page, isDirty, and next
 * and prev pointers for the linked list. The list is managed and scaled dynamically with
 * the DEFAULT_NUM_PAGES as it checks at every function if the limit has been used or reached
 * respective of the function's purpose. The description of what each function does are labeled
 * above each respective function as a brief and note, but most information will be here at the top
 * of the hpp file. No additional notes are made in the .ccp file for the sake of good code styling.
 * The rest of specific design choices and notes I will list here:
 *
 * Notes:
 * 1) Each function behaves exactly as the TODOs listed and I did not remove the TODOs as
 * they are good descriptors of what each function does.
 *
 * 2) I use three pointers to manage the linked list which are first and last, which simply
 * point to the first and last PCB in the list respectively, and current, which is changed
 * according to which page needs to be looked at at a present moment.
 *
 * 3) Every function that innately assumes that a pid is in the Database or the bufferpool
 * will throw a logic error 'No such page in bufferpool' if that page does not exist in the
 * bufferpool.
 *
 * 4) For the sake of reducing code repetitiveness, I added the function searchPid, which
 * simply searches and sets the current pointer to the specified pid.
 *
 * 5) There are two more helper functions: searchFile and discardFile, which respectively
 * do as the name entails. The first checks if a page from a particular file exists in the
 * bufferpool, and the other discards all pages associated with a file. These have other
 * uses, but currently are generally just to make the job of the Database::remove function
 * easier by being able to flush and discard all the pages related to a file that is to be
 * deleted from the underlying database.
 *
 * 6) Since the database owns the bufferpool, I did not add a pointer to the database as an
 * internal component to the bufferpool and instead invoke a getDatabase call whenever I need
 * to see a file from the database. This is probably very inefficient, but as the problem
 * states, we are only to implement functions and not care about the efficiency too much.
 * However if you have any recommendations on better ways of accessing the Database in these
 * functions they would be greatly appreciated.
 */

typedef struct pageControlBlock {
  db::PageId pageId;
  db::Page page;
  bool isDirty;
  struct pageControlBlock *next;
  struct pageControlBlock *prev;
} PCB;

static PCB * first;
static PCB * current;
static PCB * last;

namespace db {
constexpr size_t DEFAULT_NUM_PAGES = 50;
/**
 * @brief Represents a buffer pool for database pages.
 * @details The BufferPool class is responsible for managing the database pages in memory.
 * It provides functions to get a page, mark a page as dirty, and check the status of pages.
 * The class also supports flushing pages to disk and discarding pages from the buffer pool.
 * @note A BufferPool owns the Page objects that are stored in it.
 */


class BufferPool {
  // TODO pa1: add private members
private:


  int freePages;
public:
  /**
   * @brief: Constructs a BufferPool object with the default number of pages.
   */
  explicit BufferPool();

  /**
   * @brief: Destructs a BufferPool object after flushing all dirty pages to disk.
   */
  ~BufferPool();

  BufferPool(const BufferPool &) = delete;

  BufferPool(BufferPool &&) = delete;

  BufferPool &operator=(const BufferPool &) = delete;

  BufferPool &operator=(BufferPool &&) = delete;

  /**
   * @brief: Returns the page with the specified page id.
   * @param pid: The page id of the page to return.
   * @return: The page with the specified page id.
   * @note This method should make this page the most recently used page.
   */
  Page &getPage(const PageId &pid);

  /**
   * @brief: Marks the page with the specified page id as dirty.
   * @param pid: The page id of the page to mark as dirty.
   */
  void markDirty(const PageId &pid);

  /**
   * @brief: Returns whether the page with the specified page id is dirty.
   * @param pid: The page id of the page to check.
   * @return: True if the page is dirty, false otherwise.
   */
  bool isDirty(const PageId &pid) const;

  /**
   * @brief: Returns whether the buffer pool contains the page with the specified page id.
   * @param pid: The page id of the page to check.
   * @return: True if the buffer pool contains the page, false otherwise.
   */
  bool contains(const PageId &pid) const;

  /**
   * @brief: Discards the page with the specified page id from the buffer pool.
   * @param pid: The page id of the page to discard.
   * @note This method does NOT flush the page to disk.
   * @note This method also updates the LRU and dirty pages to exclude tracking this page.
   */
  void discardPage(const PageId &pid);

  /**
   * @brief: Flushes the page with the specified page id to disk.
   * @param pid: The page id of the page to flush.
   * @note This method should remove the page from dirty pages.
   */
  void flushPage(const PageId &pid);
  /**
   * @brief: Flushes all dirty pages in the specified file to disk.
   * @param file: The name of the associated file.
   * @note This method should call BufferPool::flushPage(pid).
   */
  void flushFile(const std::string &file);

  /**
   * @brief: Helper function which sets the current pointer to the page with
   * the input PageId.
   * @note Does not check for existence of the page. This check is
   * performed by each function individually based on what each function is looking
   * for.
   */
  void searchPid(const PageId &pid) const;

  /**
   * @brief: Helper function which simply returns if a page from specified file
   * name exists in the bufferpool.
   */
  bool searchFile(const std::string &name) const;

  /**
   * @brief: Helper function which discards all pages related to a given file.
   * @note  Does not flush the file and assumes a flushFile has already been performed.
   * Used solely for a database remove function in order to erase any pages in bufferpool
   * from a file that has been deleted from the database.
   */
  void discardFile(const std::string &file);

};
} // namespace db
