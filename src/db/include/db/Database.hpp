#pragma once

#include <db/BufferPool.hpp>
#include <db/DbFile.hpp>
#include <memory>

/**
 * @brief A database is a collection of files and a BufferPool.
 * @details The Database class is responsible for managing the database files.
 * It provides functions to add new database files, get the internal id of a file, and retrieve database files.
 * The class also supports removing all files from the catalog.
 * @note A Database owns the DbFile objects that are added to it.
 */

/*
 * For my implementation of the Database, I am assuming that we would actually want to have an SQL or
 * other database structure backend, but since we are essentially making a wrapper for the Database
 * the main inclusion is a hashmap of file names and the associated DbFiles. This allows for amortized
 * O(1) access queries which is advantageous since most Bufferpool and Database functions require
 * locating a file on the database. Even adding a file, we must first check to see if it exists. For the
 * map, you can find it in the class declaration written simply under the variable 'data'.
 *
 * A few notes:
 * 1) Since we use unique_pointers, I specifically transfer ownership through std::move.
 * Given that it was not specified exactly how ownership is supposed to be transferred,
 * this was what I assumed should be done, moving the original pointer caller to be the Database.
 *
 * 2) For remove, I flush the pages of the file if they exist, but I realized if I did this then
 * this leaves an opening where a user can mistakenly write to a file in memory that is no longer
 * in the database, making it dirty and causing a problem if it is evicted. I am not sure if we
 * are assuming perfect usage by user here, so I added a helper function in Bufferpool that is called
 * in one place and only after the file has been flushed which discards all the pages related to a file.
 * This allows us to avoid that issue again of dirty pages floating in the bufferpool with the file
 * not in the database at the cost of some performance on remove. I did this explicitly as this was
 * the quickest method to do this. Realistically, the best way to do this is just to add a flag to
 * the files' pages that they are just read-only in memory and let them be discarded organically,
 * but since this was not a prerequisite of the assignment nor are we asked for complete efficiency,
 * I chose this quick and dirty method for making remove work as intended while being user proof.
 *
 * 3) I left in the TODOs for each function in the cpp file in order to be good checkpoints where
 * code was added. I will have some minor comments here and there on important parts of the code
 * but I will put them only in hpp files as a form of good style practices.
 */
namespace db {
class Database {
  std::unordered_map<std::string,std::unique_ptr<DbFile>, std::hash<std::string>> data;
  BufferPool bufferPool;

  Database() = default;

public:
  friend Database &getDatabase();

  Database(Database const &) = delete;
  void operator=(Database const &) = delete;
  Database(Database &&) = delete;
  void operator=(Database &&) = delete;

  /**
   * @brief Provides access to the singleton instance of the BufferPool.
   * @return The buffer pool
   */
  BufferPool &getBufferPool();

  /**
   * @brief Adds a new file to the Database.
   * @param file The file to add.
   * @throws std::logic_error if the file name already exists.
   * @note This method takes ownership of the DbFile.
   */
  void add(std::unique_ptr<DbFile> file);

  /**
   * @brief Removes a file.
   * @param name The name of the file to remove.
   * @return The removed file.
   * @throws std::logic_error if the name does not exist.
   * @note This method should call BufferPool::flushFile(name)
   * @note This method moves the DbFile ownership to the caller.
   */
  std::unique_ptr<DbFile> remove(const std::string &name);

  /**
   * @brief Returns the DbFile of the specified id.
   * @param name The name of the file.
   * @return The DbFile object.
   * @throws std::logic_error if the name does not exist.
   */
  DbFile &get(const std::string &name) const;
};

/**
 * @brief Returns the singleton instance of the Database.
 * @return The Database object.
 */
Database &getDatabase();
} // namespace db
