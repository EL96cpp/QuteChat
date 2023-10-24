#ifndef LIMITEDVECTOR_H
#define LIMITEDVECTOR_H

#include <QVector>

template <typename T>
class LimitedVector
{
public:
    LimitedVector();
    void push_back(const T& value);
    auto begin();

private:
    QVector<T> vector;
    size_t limit = 500;
};

#endif // LIMITEDVECTOR_H
