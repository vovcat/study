// #include <experimental/scope>
template <class Lambda> struct AtScopeExit {
    Lambda& m_lambda;
    AtScopeExit(Lambda& action) : m_lambda(action) {}
    ~AtScopeExit() { m_lambda(); }
};
#define Auto_PASTEx(x, y) x ## y
#define Auto_PASTE(x, y) Auto_PASTEx(x, y)
#define Auto_INTERNAL1(lname, aname, args...) \
    auto lname = [&]() { args; }; \
    AtScopeExit<decltype(lname)> aname(lname);
#define Auto_INTERNAL2(ctr, args...) \
    Auto_INTERNAL1(Auto_PASTE(Auto_func_, ctr), \
    Auto_PASTE(Auto_instance_, ctr), args)
#define Auto(args...) \
    Auto_INTERNAL2(__COUNTER__, args)
