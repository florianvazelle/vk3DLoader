#ifndef NONCOPYABLE_HPP
#define NONCOPYABLE_HPP

/**
 * @brief A class to prohibit copying.
 * The class which inherits from it will have no constructor per copy and no equal operator.
 * We prefer to pass an object by reference, which avoids the duplication of data members of the class.
 */
class NonCopyable {
public:
  NonCopyable()                   = default;
  NonCopyable(const NonCopyable&) = delete;
  NonCopyable& operator=(const NonCopyable&) = delete;
};

#endif  // NONCOPYABLE_HPP