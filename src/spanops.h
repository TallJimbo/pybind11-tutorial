#ifndef SPANOPS_H_INCLUDED
#define SPANOPS_H_INCLUDED

#include <stdexcept>
#include <vector>

namespace spanops {

class Interval {
public:

    Interval() : _min(0), _max(-1) {}

    explicit Interval(int r) : _min(r), _max(r) {}

    Interval(int min, int max);

    int min() const { return _min; }

    int max() const { return _max; }

    int length() const { return 1 + _max - _min; }

    bool empty() const { return _min == 0 && _max == -1; }

    Interval & operator&=(Interval const & other);
    Interval operator&(Interval const & other) const;

    void expand_to(int r);
    Interval expanded_to(int r) const;

    void expand_to(Interval const & other);
    Interval expanded_to(Interval const & other) const;

    bool overlaps(int r) const;
    bool overlaps(Interval const & other) const;

    bool operator==(Interval const & other) const;
    bool operator!=(Interval const & other) const { return !(*this == other); }

private:
    int _min;
    int _max;
};


class Span {
public:

    Span() : _y(0), _x() {}

    Span(Interval const & x, int y) : _y(y), _x(x) {}

    int x0() const { return _x.min(); }
    int x1() const { return _x.max(); }
    Interval const & x() const { return _x; }
    int y() const { return _y; }

    int width() const { return _x.length(); }

    bool empty() const { return _x.empty(); }

    Span & operator&=(Span const & other);
    Span operator&(Span const & other) const;

    bool overlaps(int x, int y) const;
    bool overlaps(Span const & other) const;

    bool operator==(Span const & other) const;
    bool operator!=(Span const & other) const { return !(*this == other); }

private:
    int _y;
    Interval _x;
};


class Box {
public:

    Box() : _x(), _y() {}

    Box(Span const & span) : _x(span.x()), _y(span.y()) {}

    Box(Interval const & x, Interval const & y) : _x(x), _y(y) {}

    Interval const & x() const { return _x; }

    Interval const & y() const { return _y; }

    int x0() const { return _x.min(); }
    int x1() const { return _x.max(); }

    int y0() const { return _y.min(); }
    int y1() const { return _y.max(); }

    bool empty() const { return _x.empty() || _y.empty(); }

    int width() const { return _x.length(); }

    int height() const { return _y.length(); }

    int area() const { return _x.length()*_y.length(); }

    void expand_to(int x, int y);
    Box expanded_to(int x, int y) const;

    void expand_to(Span const & span);
    Box expanded_to(Span const & span) const;

    void expand_to(Box const & span);
    Box expanded_to(Box const & span) const;

    bool overlaps(int x, int y) const;
    bool overlaps(Span const & span) const;
    bool overlaps(Box const & other) const;

    Box operator&(Box const & other) const;
    Box & operator&=(Box const & other);

    bool operator==(Box const & other) const;
    bool operator!=(Box const & other) const { return !(*this == other); }

private:
    Interval _x;
    Interval _y;
};


template <typename T>
struct ImageWrapper {
    T * ptr;
    std::ptrdiff_t stride;
    Box bbox;
};


class SpanSet {
public:

    SpanSet() : _spans() {}

    explicit SpanSet(Box const & box);

    int area() const;
    Box bbox() const;

    using iterator = std::vector<Span>::const_iterator;

    iterator begin() const { return _spans.begin(); }
    iterator end() const { return _spans.end(); }

    bool empty() const { return _spans.empty(); }
    std::size_t size() const { return _spans.size(); }

    SpanSet operator|(SpanSet const & other) const;
    SpanSet operator&(SpanSet const & other) const;

    SpanSet & operator|=(SpanSet const & other);
    SpanSet & operator&=(SpanSet const & other);

    bool operator==(SpanSet const & other) const;
    bool operator!=(SpanSet const & other) const { return !(*this == other); }

    template <typename T>
    void insert(ImageWrapper<T> const & image, T value) const;

    template <typename T>
    static SpanSet extract(ImageWrapper<T const> const & image, T value);

    std::vector<SpanSet> split() const;

private:

    explicit SpanSet(std::vector<Span> const & spans) : _spans(spans) {}

    std::vector<Span> _spans;
};


} // namespace spanops

#endif // !SPANOPS_H_INCLUDED
