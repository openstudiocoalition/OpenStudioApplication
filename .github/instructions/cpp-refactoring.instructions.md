---
description: "Use when moving, renaming, or refactoring C++ classes in src/. Covers include order, git mv, shared_ptr vs raw pointer, and C++ covariance rules."
applyTo: "src/**/*.cpp,src/**/*.hpp,src/**/CMakeLists.txt"
---

# C++ Refactoring Guidelines

## Include Order

Within every `.cpp` and `.hpp`, includes must appear in this order, with a blank line between each group:

```cpp
// 1. Own header (in .cpp only — the header this .cpp implements)
#include "ThisClass.hpp"

// 2. Same-directory relative includes
#include "Sibling.hpp"

// 3. Cross-directory repo includes
#include "../other_module/Foo.hpp"

// 4. OpenStudio SDK includes
#include <openstudio/model/Model.hpp>

// 5. Qt, Boost, and system includes
#include <QWidget>
#include <boost/optional.hpp>
#include <memory>
```

Do not mix groups. When adding a new include, place it in the correct group in the same edit — not as a deferred cleanup.

---

## Naming

When introducing an interface method, choose its name before writing any code:

- Prefer names that describe the *concept*, not the return type (`currentDocument`, not `currentBaseDocument`).
- Check for name collisions with existing virtuals in the inheritance chain before committing.
- A rename that touches 100+ files is expensive; settle the name at design time.
