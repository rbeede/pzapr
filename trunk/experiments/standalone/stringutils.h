#ifndef _STRINGUTILS_H
#define _STRINGUTILS_H


#include <iostream>
#include <sstream>


/**
 * Useful for inline conversion of array to ostream
 *
 * Example:  cout << JoinArray(myDoubleArray, myDoubleArray_len) << endl;
 *
 *
*/
class JoinArray {
public:
	JoinArray(const double * array, const int size) : array(array), size(size) {}
	
	friend std::ostream& operator<<(std::ostream& os, const JoinArray& mp) {
        if(0 == mp.size) {
			return os;
		}
		
		for(int i = 0; i < mp.size - 1; i++) {
			os << mp.array[i];
			os << ", ";
		}
		
		os << mp.array[mp.size - 1];
		
        return os;
    }

	private:
		const double * array;
		const int size;
};


/**
 * Very useful for converting int, etc. to strings
 *
 * string("Testing" + to_string(123));
 *
*/
template <class T>
	inline std::string to_string (const T& t) {
		std::stringstream ss;
		ss << t;
		
		return ss.str();
	}


#endif /* _STRINGUTILS_H */
