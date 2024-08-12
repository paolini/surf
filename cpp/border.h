#include <map>
#include <string>

#include "surf.h"

class Border: public surf {
 public:
  
  typedef Border *(*factory_function_ptr)();
  
  typedef std::map<std::string, factory_function_ptr> Registry;
  static Registry factory;
};

template<class T> Border *factory_function() {
  return new T();
};

template<class T> bool registry_function(const char *border_name) {
  Border::factory[border_name] = factory_function<T>;
  return true;
}
