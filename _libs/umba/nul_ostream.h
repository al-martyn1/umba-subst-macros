#pragma once


namespace umba{


class NulOstream
{

public:

    template<typename T>
    NulOstream& operator<<( const T &t) { return *this; }

}; // class NulOstream


} // namespace umba

