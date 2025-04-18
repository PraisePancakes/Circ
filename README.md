# CIRC : A text-based serialization framework.
**v0.0.1-alpha**
## Why?
 &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Programming advanced graphical applications can be very messy. Let's clean that up a little. With Circ, you can keep all your environment variables (objects, int, string, etc..) in a separate file and just look up these constants using the Circ config loader API!

### Setting up a Circ file
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Create a file with the .Circ extension ``` file.Circ ```.
## Usage
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Lets begin with the configuration language. Don't worry this is going to be an easy learn.
### Declaring a configuration variable :
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; To assign a variable all you have to do is prefix the identifier with a ```$``` sign and assign it ```:``` to a value.
A very simple file will look something like this.
```
//comments work too!
/*
so do these...
*/
//here is the entry
$some_var : 5

```
### Different types :
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; Circ currently supports four types of variables, double, string, object literals, and arrays.
```
$double : 5,
$str : "string",
$obj : { $x : 1, $y : 2 },
$arr : [0, 1, 2]

```
Ok sweet! now we have some config variables, let's use them in some code!
Starting off, let's create an instance of the API.
```c++
const std::string path = "/path_to_circ_file"
Circ::CFGLoader cfgl(path);
```

pass the path string of the Circ file to the Config loader. The config loader API is now responsible for parsing and 'interpreting' the file and places all key-value pairs of the Circ file into a map.
Accessing a variable depends on the type of variable you want to access.
For Primary Data Types (PDTs) you can simply access a variable like so.
```c++
double dbl_var = cfgl.CFGAttr<double>("double_variable");
```
The config attribute (CFGAttr) function takes the type of the config variable as its template argument and the name of that variable as its function argument, this searches the Circ env for this variable and throws an exception if one isn't found. Ok cool, now how do we retrieve objects?
```c++
        template<typename WrapperType>
        WrapperType CFGAttr(std::initializer_list<std::string> key_path)
```
This function takes a list of strings starting with the outer-most key to the innermost key of the object.
For example given this Circ file :
```
 $obj : {
   $x : 1,  
   $y : 2,
   $inner : {
     $ix : 3,
     $iy : 4
   }
 }
```

To retrieve the ```$ix``` object-key you can do so :
```c++
double d = cfgl.CFGAttr<double>({"obj", "inner", "ix"});
//retrieve $ix
std::cout << d;

```
Circ provides a CircObject type to retrieve the objects themselves.
```c++
CircObject object = cfgl.CFGAttr<CircObject>({"obj", "inner"});
std::map<std::string, std::any> members = object.members;
for(auto& m : members) {
 std::cout << m.first << std::endl;
}
```

## Arrays
std::any is a type-safe void* that allows for safe casts on an unknown type at runtime. 
In the current implementation (due to change), you may access arrays in a simple two-step (working on one-step) procedure.
First, all Circ arrays will return a std::vector<std::any>> type.
```c++
auto v = cfgl.CFGAttr<std::vector<std::any>>({ "arr" });
``` 
Second, access the element in the array via std::any_cast<T> where T is the type of the array, Mismatch types are an exception and will raise a runtime error.
```c++
auto el = std::any_cast<double>(v[0]);
std::cout << el;
```

# Serialization
DISCLAIMER : In the current version of Circ the following types are NOT serializable
-char*


## Setting members.
You can set the values of members like so :
```c++
 std::initializer_list<std::string> key_path = {"some", "member", "test"};
 Circ::CFGLoader cfgl(cfg_path);
 cfgl.CFGAttrSet<double>(key_path, 5.00);
 //the value at key_path is now 5.00
```
## Serializing members.
In Circ Serialization is an explicit call, to serialize members that change or remain constant, do so.
```c++
std::initializer_list<std::string> key_path = {"re", "a", "test"};
Circ::CFGLoader cfgl(cfg_path);
cfgl.CFGAttrSet<double>(key_path, 5.00);
// the value at key_path is now 5.00
cfgl.Serialize();
//the value at key_path is now serialized to the Circ config file.
```

That's it! Well... for now, I am still working hard every day to better this project.
### IN THE WORKS 
 1. Using variable substitution for values.
 2. Multi-file modules
 4. One-step array retrieval.
 5. Serialization formatting.
 6. More types!

## Building
This project is built using CMake on MSVS2022, ISO C++20 Standard
