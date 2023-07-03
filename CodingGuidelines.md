# Coding Guidelines

## 1. General
The objectives of these **Coding Guidelines** are to:
- Enforce a coherent code style across files
- Avoid errors and bugs, especially those caused by C++ quirks

## 2. Naming Conventions

### 2.1 Classes & Structs
- Use *CamelCase*
- Start with a capital letter
- Do not use abbreviations

```C++
class ExampleClassName
{};
```

### 2.2 Namespaces
- Do not use capital letters
- If the name is composed by more than one word, simply concatenate them.

```C++
namespace vulture{}

namespace gamename{}
```

### 2.3 Local variables, Parameters & Methods
- Use *CamelCase*
- Start with a lower case letter
- Avoid abbreviations (except for trivial case)

```C++
void exampleMethod(int startPosition)
{
	int targetPosition = 2 * startPosition;
}
```

### 2.4 Private class variables
- Use *CamelCase*
- Use type prefixes
- Start with a capital letter
- Do not use abbreviations

Type prefixes:
1) `m_` for private member variables
2) `s_` for static variables
3) `c_` for constant variables

Exception:
- For public member variables, don't use any prefix and start with a lower case letter.
- For variables that are static and constant, use *UPPER_CASE*.

```C++
class ExampleClassName
{
public:
int publicMemberVariable;
private
int m_MemberVariable;
static int s_StaticVariable;
const int c_ConstVariable;
static const int STATIC_CONST_VARIABLE;
};
```

## 3. Namespaces & Fully Static Classes
Avoid nested namespace.
Use namespaces to encapsulate an entire subproject. Use Fully Static Classes to encapsulate functionality (eg. Logger).

**DO NOT USE** `using namespace` in the global scope within header files.

## 4. Enums
Use `enum class`(C++) instead of `enum`(C) unless you need to directly manipulate the data represented by the enumerators.
