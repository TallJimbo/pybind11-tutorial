#include <algorithm>
#include <map>

#include "spanops.h"

namespace spanops {

Interval::Interval(int min, int max) : _min(min), _max(max) {
    if (_min > _max) {
        *this = Interval();
    }
}

Interval & Interval::operator&=(Interval const & other) {
    *this = *this & other;
    return *this;
}

Interval Interval::operator&(Interval const & other) const {
    if (empty() || other.empty()) {
        return Interval();
    }
    return Interval(std::max(min(), other.min()),
                    std::min(max(), other.max()));
}

void Interval::expand_to(int r) {
    if (empty()) {
        _min = r;
        _max = r;
    } else if (r < min()) {
        _min = r;
    } else if (r > max()) {
        _max = r;
    }
}

void Interval::expand_to(Interval const & other) {
    if (empty()) {
        *this = other;
    } else if (!other.empty()) {
        if (other.min() < min()) {
            _min = other.min();
        }
        if (other.max() > max()) {
            _max = other.max();
        }
    }
}

Interval Interval::expanded_to(Interval const & other) const {
    Interval copy(*this);
    copy.expand_to(other);
    return copy;
}

Interval Interval::expanded_to(int r) const {
    Interval copy(*this);
    copy.expand_to(r);
    return copy;
}

bool Interval::overlaps(int r) const {
    return min() <= r && r <= max();
}

bool Interval::overlaps(Interval const & other) const {
    return max() >= other.min() && min() <= other.max();
}

bool Interval::operator==(Interval const & other) const {
    return min() == other.min() && max() == other.max();
}

Span Span::operator&(Span const & other) const {
    if (y() == other.y()) {
        return Span(x() & other.x(), y());
    }
    return Span();
}

Span & Span::operator&=(Span const & other) {
    if (y() == other.y()) {
        _x &= other.x();
    } else {
        _x = Interval();
    }
    return *this;
}

bool Span::overlaps(int x, int y) const {
    return _y == y && _x.overlaps(x);
}

bool Span::overlaps(Span const & other) const {
    return y() == other.y() && x().overlaps(other.x());
}

bool Span::operator==(Span const & other) const {
    return y() == other.y() && x() == other.x();
}

Box Box::operator&(Box const & other) const {
    return Box(x() & other.x(), y() & other.y());
}

Box & Box::operator&=(Box const & other) {
    _x &= other.x();
    _y &= other.y();
    return *this;
}

bool Box::operator==(Box const & other) const {
    return x() == other.x() && y() == other.y();
}

void Box::expand_to(int x, int y) {
    _x.expand_to(x);
    _y.expand_to(y);
}

Box Box::expanded_to(int x, int y) const {
    Box copy(*this);
    copy.expand_to(x, y);
    return copy;
}

void Box::expand_to(Span const & span) {
    _x.expand_to(span.x());
    _y.expand_to(span.y());
}

Box Box::expanded_to(Span const & span) const {
    Box copy(*this);
    copy.expand_to(span);
    return copy;
}

void Box::expand_to(Box const & other) {
    _x.expand_to(other.x());
    _y.expand_to(other.y());
}

Box Box::expanded_to(Box const & other) const {
    Box copy(*this);
    copy.expand_to(other);
    return copy;
}

bool Box::overlaps(int x, int y) const {
    return _x.overlaps(x) && _y.overlaps(y);
}

bool Box::overlaps(Span const & span) const {
    return _x.overlaps(span.x()) && _y.overlaps(span.y());
}

bool Box::overlaps(Box const & other) const {
    return _x.overlaps(other.x()) && _y.overlaps(other.y());
}

SpanSet::SpanSet(Box const & box) : _spans() {
    for (int y = box.y0(); y <= box.y1(); ++y) {
        _spans.emplace_back(box.x(), y);
    }
}

int SpanSet::area() const {
    int a = 0;
    for (auto const & span : _spans) {
        a += span.width();
    }
    return a;
}

Box SpanSet::bbox() const {
    Box box;
    for (auto const & span : _spans) {
        box.expand_to(span);
    }
    return box;
}

namespace {

struct SpanAnyLess {
    bool operator()(Span const & a, Span const & b) {
        if (a.y() == b.y()) {
            if (a.x0() == b.x0()) {
                return a.x1() < b.x1();
            }
            return a.x0() < b.x0();
        }
        return a.y() < b.y();
    }
};

struct SpanStrictLess {
    bool operator()(Span const & a, Span const & b) {
        if (a.y() == b.y()) {
            return a.x1() < b.x0();
        }
        return a.y() < b.y();
    }
};

template <typename Iter>
std::vector<Span> intersect(
    Iter i1, Iter const end1,
    Iter i2, Iter const end2
) {
    std::vector<Span> intersection;
    auto strictly_less = SpanStrictLess();
    if (i1 == end1 || i2 == end2) return intersection;
    while (true) {
        if (strictly_less(*i1, *i2)) {
            if (++i1 == end1) break;
        } else if (strictly_less(*i2, *i1)) {
            if (++i2 == end2) break;
        } else { // they must overlap
            intersection.push_back((*i1) & (*i2));
            if (i1->x1() <= i2->x1()) {
                if (++i1 == end1) break;
            }
            if (i1->x1() >= i2->x1()) {
                if (++i2 == end2) break;
            }
        }
    }
    return intersection;
}

} // anonymous

SpanSet SpanSet::operator|(SpanSet const & other) const {
    std::vector<Span> together;
    together.reserve(size() + other.size());
    std::merge(begin(), end(), other.begin(), other.end(),
               std::back_inserter(together), SpanAnyLess());
    std::vector<Span> updated;
    auto first = together.begin();
    for (auto first = together.begin(); first != together.end(); ++first) {
        auto last = std::adjacent_find(
            first, together.end(),
            [](Span const & a, Span const & b) {
                return !a.overlaps(b);
            }
        );
        if (last == together.end()) {
            --last;
        }
        updated.emplace_back(Interval(first->x0(), last->x1()), first->y());
        first = last;
    }
    return SpanSet(updated);
}

SpanSet SpanSet::operator&(SpanSet const & other) const {
    return SpanSet(intersect(begin(), end(), other.begin(), other.end()));
}

SpanSet & SpanSet::operator|=(SpanSet const & other) {
    return *this = *this | other;
}

SpanSet & SpanSet::operator&=(SpanSet const & other) {
    return *this = *this & other;
}

bool SpanSet::operator==(SpanSet const & other) const {
    return std::equal(begin(), end(), other.begin(), other.end());
}

template <typename T>
SpanSet SpanSet::extract(ImageWrapper<T const> const & image, T value) {
    std::vector<Span> spans;
    if (image.bbox.empty()) {
        return SpanSet(spans);
    }
    T const * py = image.ptr;
    for (int y = image.bbox.y0(); y <= image.bbox.y1(); ++y) {
        T const * p = py;
        int x = image.bbox.x0();
        while (x <= image.bbox.x1()) {
            if (*p == value) {
                int x0 = x;
                while (true) {
                    ++x;
                    ++p;
                    if (x > image.bbox.x1() || *p != value) {
                        spans.emplace_back(Interval(x0, x - 1), y);
                        break;
                    }
                }
            }
            ++x;
            ++p;
        }
        py += image.stride;
    }
    return SpanSet(spans);
}

template <typename T>
void SpanSet::insert(ImageWrapper<T> const & image, T value) const {
    for (auto const & span : *this) {
        if (!image.bbox.y().overlaps(span.y())) {
            continue;
        }
        Interval x_intersection = span.x() & image.bbox.x();
        if (x_intersection.empty()) {
            continue;
        }
        T * p = image.ptr + image.stride*(span.y() - image.bbox.y0());
        T * first = p + x_intersection.min() - image.bbox.x0();
        T * last = first + x_intersection.length();
        std::transform(first, last, first, [value](T z) { return z + value; });
    }
}

class LabeledSpansByRow {
public:

    static constexpr int const NOT_LABELED = 0;
    static constexpr int const FIRST_LABEL = 1;

    // A range of SpanSet iterators.
    class Range {
    public:

        using iterator = SpanSet::iterator;

        explicit Range(iterator first) : _first(first), _count(1) {}

        iterator begin() const { return _first; }
        iterator end() const { return _first + _count; }

        void expand() { ++_count; }

    private:
        iterator _first;
        std::size_t _count;
    };

    LabeledSpansByRow(Range::iterator first, Range::iterator const last) :
        _first(first),
        _map(),
        _labels(last - first, NOT_LABELED)
    {
        auto row = _map.begin();
        for (auto iter = first; iter != last; ++iter) {
            if (row != _map.end() && row->first == iter->y()) {
                row->second.expand();
            } else {
                row = _map.emplace_hint(_map.end(), iter->y(), Range(iter));
            }
        }
    }

    std::pair<Range const *, Range const *> neighboring_rows(int y) const {
        auto found = _map.find(y);
        std::pair<Range const *, Range const *> result(nullptr, nullptr);
        if (found != _map.begin()) {
            auto previous = found;
            --previous;
            result.first = &previous->second;
        }
        auto next = found;
        if (++next != _map.end()) {
            result.second = &next->second;
        }
        return result;
    }

    int & get_label(Range::iterator iter) { return _labels[iter - _first]; }

private:
    Range::iterator _first;
    std::map<int, Range> _map;
    std::vector<int> _labels;
};


int const LabeledSpansByRow::NOT_LABELED;
int const LabeledSpansByRow::FIRST_LABEL;


void label_adjacent(
    SpanSet::iterator span,
    LabeledSpansByRow & by_row,
    int current_label
) {
    auto process_row = [span, &by_row, current_label](
        LabeledSpansByRow::Range const * range
    ) {
        if (range == nullptr) return;
        for (auto i = range->begin(); i != range->end(); ++i) {
            int & label_i = by_row.get_label(i);
            if (label_i == 0 && i->x().overlaps(span->x())) {
                label_i = current_label;
                label_adjacent(i, by_row, current_label);
            }
        }
    };
    auto neighbors = by_row.neighboring_rows(span->y());
    process_row(neighbors.first);  // previous row
    process_row(neighbors.second); // next row
}

std::vector<SpanSet> SpanSet::split() const {
    LabeledSpansByRow by_row(begin(), end());
    int current_label = LabeledSpansByRow::FIRST_LABEL;
    for (auto i = begin(); i != end(); ++i) {
        int & label = by_row.get_label(i);
        if (label == LabeledSpansByRow::NOT_LABELED) {
            label = current_label;
            label_adjacent(i, by_row, current_label);
            ++current_label;
        }
    }
    // Vector-of-vectors-of-Span, one inner vector for each label.
    std::vector<std::vector<Span>> groups(current_label - 1);
    for (auto i = begin(); i != end(); ++i) {
        groups[by_row.get_label(i) - 1].push_back(*i);
    }
    std::vector<SpanSet> result;
    result.reserve(groups.size());
    for (auto & spans : groups) {
        result.push_back(SpanSet(std::move(spans)));
    }
    return result;
};


#define INSTANTIATE(T)                                                  \
    template SpanSet SpanSet::extract(ImageWrapper<T const> const &, T); \
    template void SpanSet::insert(ImageWrapper<T> const &, T) const

INSTANTIATE(bool);
INSTANTIATE(std::uint8_t);
INSTANTIATE(std::int8_t);
INSTANTIATE(std::uint16_t);
INSTANTIATE(std::int16_t);
INSTANTIATE(std::uint32_t);
INSTANTIATE(std::int32_t);
INSTANTIATE(std::uint64_t);
INSTANTIATE(std::int64_t);
INSTANTIATE(float);
INSTANTIATE(double);

} // namespace spanops
