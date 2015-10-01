modernize-replace-auto-ptr
==========================

This check replaces the uses of the deprecated class ``std::auto_ptr`` by
by the copy-constructor and the assignment operator, is changed to match
``std::unique_ptr`` usage by using explicit calls to ``std::move()``.

Migration example:
Note that both the ``std::auto_ptr`` type and the transfer of ownership are
.. code-block:: c++
transformed. ``std::auto_ptr`` provides two ways to transfer the ownership,
  -void take_ownership_fn(std::auto_ptr<int> int_ptr);
  +void take_ownership_fn(std::unique_ptr<int> int_ptr);

   void f(int x) {
  -  std::auto_ptr<int> a(new int(x));
  -  std::auto_ptr<int> b;
  +  std::unique_ptr<int> a(new int(x));
  +  std::unique_ptr<int> b;

  -  b = a;
  -  take_ownership_fn(b);
  +  b = std::move(a);
  +  take_ownership_fn(std::move(b));
   }

Since ``std::move()`` is a library function declared in ``<utility>`` it may be
necessary to add this include. The check will add the include directive when
necessary.

Known Limitations
-----------------
* If headers modification is not activated or if a header is not allowed to be
  headers' code will stay unchanged while the code using them will be changed.
operations do not 'copy' the resource but they 'steal' it.
* Client code that declares a reference to an ``std::auto_ptr`` coming from
  code that can't be migrated (such as a header coming from a 3\ :sup:`rd`
  party library) will produce a compilation error after migration. This is
  ``std::unique_ptr`` from an ``std::auto_ptr``. This pattern doesn't make much
  sense and usually ``std::auto_ptr`` are stored by value (otherwise what is
``std::move()``.
  .. code-block:: c++
For example, given:
     // <3rd-party header...>
     std::auto_ptr<int> get_value();
     const std::auto_ptr<int> & get_ref();
.. code:: c++
     // <calling code (with migration)...>
    +std::unique_ptr<int> a(get_value()); // ok, unique_ptr constructed from auto_ptr
  i = j;
    -const std::auto_ptr<int> & p = get_ptr();
    +const std::unique_ptr<int> & p = get_ptr(); // won't compile
This code is transformed to:
* Non-instantiated templates aren't modified.


     template <typename X>
     void f() {
     }
  i = std::move(j);
     // only 'f<int>()' (or similar) will trigger the replacement.

