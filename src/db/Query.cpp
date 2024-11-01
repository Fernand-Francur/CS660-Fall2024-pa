#include <db/Query.hpp>

using namespace db;

void db::projection(const DbFile &in, DbFile &out, const std::vector<std::string> &field_names) {
  auto it1 = in.begin();
  TupleDesc td1 = in.getTupleDesc();
  std::vector<size_t> indices;
  auto it3 = field_names.begin();
  while (it3 != field_names.end()) {
    indices.push_back(td1.index_of(*it3));
    ++it3;
  }
  while (it1 != in.end()) {
    std::vector<field_t> fields;
    auto t1 = *it1;

    auto it2 = indices.begin();
    while (it2 != indices.end()) {
      fields.push_back(t1.get_field(*it2));
      ++it2;
    }

    out.insertTuple({fields});
    ++it1;
  }
}

void db::filter(const DbFile &in, DbFile &out, const std::vector<FilterPredicate> &pred) {
  auto it1 = in.begin();
  TupleDesc td1 = in.getTupleDesc();
  int total = pred.size();
  while (it1 != in.end()) {
    auto t1 = *it1;
    auto it2 = pred.begin();
    double count = 0;

    while (it2 != pred.end()) {
      switch (it2->op) {
      case PredicateOp::EQ:
        if (t1.get_field(td1.index_of(it2->field_name)) == it2->value) {
          count++;
        };
        break;
      case PredicateOp::NE:
        if (t1.get_field(td1.index_of(it2->field_name)) != it2->value) {
          count++;
        };
        break;
      case PredicateOp::LT:
        if (t1.get_field(td1.index_of(it2->field_name)) < it2->value) {
          count++;
        };
        break;
      case PredicateOp::LE:
        if (t1.get_field(td1.index_of(it2->field_name)) <= it2->value) {
          count++;
        };
        break;
      case PredicateOp::GT:
        if (t1.get_field(td1.index_of(it2->field_name)) > it2->value) {
          count++;
        };
        break;
      case PredicateOp::GE:
        if (t1.get_field(td1.index_of(it2->field_name)) >= it2->value) {
          count++;
        };
        break;
      }
      ++it2;

    }
    if (count == total) {
      out.insertTuple(t1);
    }

    ++it1;
  }
}

void db::aggregate(const DbFile &in, DbFile &out, const Aggregate &agg) {
  auto it1 = in.begin();
  std::unordered_map<field_t, double> groups;
  std::unordered_map<field_t, double> groupCount;
  TupleDesc td1 = in.getTupleDesc();
  double noGroupValue = 0;
  int count = 0;
  bool set = false;
  auto first = *it1;
  size_t index = td1.index_of(agg.field);
  size_t groupIndex = (agg.group == std::nullopt) ? 0 : td1.index_of(agg.group.value());
  bool integer = std::holds_alternative<int>(first.get_field(td1.index_of(agg.field)));
  if (agg.group == std::nullopt) {
    while (it1 != in.end()) {
      auto t1 = *it1;

      double value = (integer) ? (double)std::get<int>(t1.get_field(index)) : std::get<double>(t1.get_field(index));
      switch (agg.op) {

      case AggregateOp::SUM:
        noGroupValue += value;
        break;
      case AggregateOp::AVG:
        noGroupValue += value;
        count++;
        break;
      case AggregateOp::MIN:
        if (!set) {
          noGroupValue = value;
          set = true;
        } else {
          noGroupValue = (value < noGroupValue) ? value : noGroupValue;
        }
        break;
      case AggregateOp::MAX:
        if (!set) {
          noGroupValue = value;
          set = true;
        } else {
          noGroupValue = (value > noGroupValue) ? value : noGroupValue;
        }
        break;
      case AggregateOp::COUNT:
        count++;
        break;
      }

      ++it1;
    }
    int new_val;
    switch (agg.op) {
    case AggregateOp::SUM:
      new_val = (int) noGroupValue;
      out.insertTuple({{new_val}});
      break;
    case AggregateOp::AVG:
      out.insertTuple({{noGroupValue/count}});
      break;
    case AggregateOp::MIN:
      if (integer) {
        new_val = (int) noGroupValue;
        out.insertTuple({{new_val}});
      } else {
        out.insertTuple({{noGroupValue}});
      }
      break;
    case AggregateOp::MAX:
      if (integer) {
        new_val = (int) noGroupValue;
        out.insertTuple({{new_val}});
      } else {
        out.insertTuple({{noGroupValue}});
      }
      break;
    case AggregateOp::COUNT:
      out.insertTuple({{count}});
      break;
    }
  } else {
    while (it1 != in.end()) {
      auto t1 = *it1;
      double value = (integer) ? (double)std::get<int>(t1.get_field(index)) : std::get<double>(t1.get_field(index));
      if (groups.count(t1.get_field(groupIndex)) == 0) {
        if (agg.op != AggregateOp::COUNT) {
          groups.insert({t1.get_field(groupIndex),value});
          groupCount.insert({t1.get_field(groupIndex),1});
        } else {
          groups.insert({t1.get_field(groupIndex),1});
        }
      } else {
        switch (agg.op) {
        case AggregateOp::SUM:
          groups.at(t1.get_field(groupIndex)) += value;
          break;
        case AggregateOp::AVG:
          groups.at(t1.get_field(groupIndex)) += value;
          groupCount.at(t1.get_field(groupIndex)) += 1;
          break;
        case AggregateOp::MIN:
          groups.at(t1.get_field(groupIndex)) = (value < groups.at(t1.get_field(groupIndex))) ? value : groups.at(t1.get_field(groupIndex));
          break;
        case AggregateOp::MAX:
          groups.at(t1.get_field(groupIndex)) = (value > groups.at(t1.get_field(groupIndex))) ? value : groups.at(t1.get_field(groupIndex));
          break;
        case AggregateOp::COUNT:
          groups.at(t1.get_field(groupIndex))++;
          break;
        }
      }
      ++it1;
    }
    for (auto it = groups.begin(); it != groups.end(); ++it) {
      switch (agg.op) {
      case AggregateOp::SUM:
        out.insertTuple({{it->first, it->second}});
        break;
      case AggregateOp::AVG:
        out.insertTuple({{it->first, it->second/groupCount.at(it->first)}});
        break;
      case AggregateOp::MIN:
        out.insertTuple({{it->first, it->second}});
        break;
      case AggregateOp::MAX:
        out.insertTuple({{it->first, it->second}});
        break;
      case AggregateOp::COUNT:
        out.insertTuple({{it->first, it->second}});
        break;
      }
    }
  }
}

void db::join(const DbFile &left, const DbFile &right, DbFile &out, const JoinPredicate &pred) {


  TupleDesc td1 = left.getTupleDesc();
  TupleDesc td2 = right.getTupleDesc();
  size_t indexLeft = td1.index_of(pred.left);
  size_t indexRight = td2.index_of(pred.right);

  auto left1 = left.begin();
  while (left1 != left.end()) {
    auto t1 = *left1;

    auto right1 = right.begin();
    while (right1 != right.end()) {
      auto t2 = *right1;
      switch (pred.op) {
      case PredicateOp::EQ:
        if (t1.get_field(indexLeft) == t2.get_field(indexRight)) {
          std::vector<field_t> fields;
          for (int i = 0; i < t1.size(); i++) {
            fields.push_back(t1.get_field(i));
          }
          for (int j = 0; j < t2.size(); j++) {
            if (j != td2.index_of(pred.right)) {
              fields.push_back(t2.get_field(j));
            }
          }
          out.insertTuple(fields);
        };
        break;
      case PredicateOp::NE:
        if (t1.get_field(indexLeft) != t2.get_field(indexRight)) {
          std::vector<field_t> fields;
          for (int i = 0; i < t1.size(); i++) {
            fields.push_back(t1.get_field(i));
          }
          for (int j = 0; j < t2.size(); j++) {
            fields.push_back(t2.get_field(j));
          }
          out.insertTuple(fields);

        };
        break;
      case PredicateOp::LT:
        if (t1.get_field(indexLeft) < t2.get_field(indexRight)) {
          std::vector<field_t> fields;
          for (int i = 0; i < t1.size(); i++) {
            fields.push_back(t1.get_field(i));
          }
          for (int j = 0; j < t2.size(); j++) {
            fields.push_back(t2.get_field(j));
          }
          out.insertTuple(fields);
        };
        break;
      case PredicateOp::LE:
        if (t1.get_field(indexLeft) <= t2.get_field(indexRight)) {
          std::vector<field_t> fields;
          for (int i = 0; i < t1.size(); i++) {
            fields.push_back(t1.get_field(i));
          }
          for (int j = 0; j < t2.size(); j++) {
            fields.push_back(t2.get_field(j));
          }
          out.insertTuple(fields);
        };
        break;
      case PredicateOp::GT:
        if (t1.get_field(indexLeft) > t2.get_field(indexRight)) {
          std::vector<field_t> fields;
          for (int i = 0; i < t1.size(); i++) {
            fields.push_back(t1.get_field(i));
          }
          for (int j = 0; j < t2.size(); j++) {
            fields.push_back(t2.get_field(j));
          }
          out.insertTuple(fields);
        };
        break;
      case PredicateOp::GE:
        if (t1.get_field(indexLeft) >= t2.get_field(indexRight)) {
          std::vector<field_t> fields;
          for (int i = 0; i < t1.size(); i++) {
            fields.push_back(t1.get_field(i));
          }
          for (int j = 0; j < t2.size(); j++) {
            fields.push_back(t2.get_field(j));
          }
          out.insertTuple(fields);
        };
        break;
      }
      ++right1;
    }

    ++left1;
  }
  // TODO: Implement this function
}
/*
 * void db::aggregate(const DbFile &in, DbFile &out, const Aggregate &agg) {
auto it1 = in.begin();
TupleDesc td1 = in.getTupleDesc();

size_t index = td1.index_of(agg.field);
double sum = 0;
double best;
auto first = *it1;
bool integer = std::holds_alternative<int>(first.get_field(td1.index_of(agg.field)));

int count = 0;
bool set = false;

while (it1 != in.end()) {
std::vector<field_t> fields;
auto t1 = *it1;

if (agg.op == AggregateOp::SUM || agg.op == AggregateOp::AVG || agg.op == AggregateOp::COUNT) {

  sum +=  integer ? std::get<int>(t1.get_field(index)):std::get<double>(t1.get_field(index));
  count++;
} else {
  if (agg.op == AggregateOp::MAX) {
    if (!set) {
      best = integer ? std::get<int>(t1.get_field(index)) : std::get<double>(t1.get_field(index));
      set = true;
    } else {
      best = (integer ? std::get<int>(t1.get_field(index)) : std::get<double>(t1.get_field(index))) > best ? (integer ? std::get<int>(t1.get_field(index)) : std::get<double>(t1.get_field(index))) : best;
    }
  } else if (agg.op == AggregateOp::MIN) {
    if (!set) {
      best = integer ? std::get<int>(t1.get_field(index)) : std::get<double>(t1.get_field(index));
      set = true;
    } else {
      best = (integer ? std::get<int>(t1.get_field(index)) : std::get<double>(t1.get_field(index))) < best ? (integer ? std::get<int>(t1.get_field(index)) : std::get<double>(t1.get_field(index))) : best;
    }
  }
}
++it1;
}

if (agg.group == std::nullopt) {
int new_num;

switch (agg.op) {

case AggregateOp::SUM:
  if (integer) {
    new_num = (int) sum;
    out.insertTuple({{new_num}});
  } else {
    out.insertTuple({{sum}});
  }
  break;
case AggregateOp::AVG:
  out.insertTuple({{sum / count}});
  break;
case AggregateOp::MIN:
  if (integer) {
    new_num = (int) best;
    out.insertTuple({{new_num}});
  } else {
    out.insertTuple({{best}});
  }
  break;
case AggregateOp::MAX:
  if (integer) {
    new_num = (int) best;
    out.insertTuple({{new_num}});
  } else {
    out.insertTuple({{best}});
  }
  break;
case AggregateOp::COUNT:
  new_num = (int) count;
  out.insertTuple({{new_num}});
  break;
}
}
}
 */