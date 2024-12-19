# CIRC : A raylib-compatible configuration framework.
**v0.0.1-alpha**
## Why?
 &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Programming advanced graphical applications can be very messy. Let's clean that up a little. With Circ, you can keep all your environment variables (objects, int, string, etc..) in a separate file and just look up these constants using the Circ config loader API!

### Setting up a Circ file
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;At this moment (12/18/2024) circ files are just plain text files.
## Usage
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Lets begin with the configuration language. Don't worry this is going to be an easy learn.
### Entry :
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Each Circ file has an entry point. To denote this entry point, type ```::``` before the file's variables. Following the entry point, create the entry object : ```{ }``` 
### Declaring a configuration variable :
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;To assign a variable all you have to do is prefix the identifier with a ```$``` sign and assign it ```:``` to a value.
A very simple file will look something like this.
```
//comments work too!
/*
so do these...
*/

//here is the entry token
::
{
$some_var : 5,
}
```
### Different types :
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Circ currently supports three types of variables, double, string, and objects.
```
::
{
$double : 5,
$str : "string",
$obj : { $x : 1, $y : 2, }
}
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
The CFGAttr function takes the type of the config variable as its template argument and the name of that variable as its function argument, this searches the Circ env for this variable and throws an exception if one isn't found. Ok cool, now how do we retrieve objects?
Use CFGObj to retrieve an object. CFGObj returns a map of the objects members key-value pairs where the key is a string and the value can be any type.
```c++
std::map<std::string, std::any> CFGObj(std::initializer_list<std::string> obj_path);
```
This function takes a list of strings starting with the outer-most key to the inner most key of the object.
For example given this Circ file :
```
::
$obj : {
  $x : 1,  
  $y : 2,
  $inner : {
    $ix : 3,
    $iy : 4,
  },
}
```

To retrieve the ```$ix``` object-key you can do so :
```c++
double d = cfgl.CFGAttr<double>({"obj", "inner", "ix"});
//retrieve $ix
std::cout << d;

```
That's it! Well... currently, I am still working hard every day to better this project.
### IN THE WORKS 
 1. Easier object property retrieval.
 2. Using variable substitution for values.
 3. Multi-file modules
 4. Keywords and API functions that incorporate Raylib.

## Building
This project is built using CMake on MSVS2022, ISO C++20 Standard, with the latest version of Raylib. 
