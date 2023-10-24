#include "limitedvector.h"

template<typename T>
LimitedVector<T>::LimitedVector()
{

}

template<typename T>
void LimitedVector<T>::push_back(const T &value)
{
    if (vector.size() >= limit) {

        vector.erase(vector.begin());

    }
    vector.push_back(value);
}
