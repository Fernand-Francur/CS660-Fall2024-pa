#include <db/ColumnStats.hpp>
#include <stdexcept>
using namespace db;

ColumnStats::ColumnStats(unsigned buckets, int min, int max) : min(min), max(max), bucket_num(buckets)
{
  for(int i = 0; i < bucket_num; i++) {
    bucket_counter.push_back(0);
  }
  int modulo = (max -  min)%(bucket_num);
  divisor = (max -  min)/(bucket_num);
  if (modulo) {
    divisor++;
  }
  count = 0;
}

void ColumnStats::addValue(int v) {
  if (v < min || max < v) {
    throw std::runtime_error("value outside of hist range");
  }
  int index = (v - min) / divisor;
  if (index == bucket_num) {index--;}
  bucket_counter[index]++;
  count++;
}
//enum class PredicateOp { EQ, NE, LT, LE, GT, GE };
size_t ColumnStats::estimateCardinality(PredicateOp op, int v) const {
  int normal_v = v - min;
  int card = 0;
  int index = normal_v / divisor;
  int over = normal_v % (int )divisor;
  if (index == bucket_num) {index--;}
  int equality = (bucket_counter[index]/divisor);

  if (index > bucket_num) {
    equality = 0;
  }

  if (op == PredicateOp::EQ) {
    return equality;
  } else if (op == PredicateOp::NE) {
    return count - equality;
  } else if (op == PredicateOp::LT) {
    if (index > bucket_num) {
      return count;
    }
    card = over * bucket_counter[index] / divisor;

    for (int i = 0; i < index; i++) {
      card += bucket_counter[i];
    }
    return card;
  } else if (op == PredicateOp::LE) {
    if (index > bucket_num) {
      return count;
    }
    card = (over+1) * bucket_counter[index] / divisor;

    for (int i = 0; i < index; i++) {
      card += bucket_counter[i];
    }
    return card;

  } else if (op == PredicateOp::GT) {
    if (index > bucket_num) {
      return 0;
    }
    card = (divisor - 1 -over) * bucket_counter[index] / divisor;

    for (int i = (bucket_num-1); i > index; i--) {
      card += bucket_counter[i];
    }
    return card;

  } else if (op == PredicateOp::GE) {
    if (index > bucket_num) {
      return 0;
    }
    card = (divisor -over) * bucket_counter[index] / divisor;

    for (int i = (bucket_num-1); i > index; i--) {
      card += bucket_counter[i];
    }
    return card;
  }
}
