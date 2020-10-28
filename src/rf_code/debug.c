#define Assert(exp) if(!(exp)) { _AssertionFailure(#exp, __FILE__, __LINE__, 1); }
#define SoftAssert(exp) if(!(exp)) { _AssertionFailure(#exp, __FILE__, __LINE__, 0); }

internal void
_AssertionFailure(const char *expression, const char *file, int line, int crash)
{
    fprintf(stderr, "ASSERTION FAILURE: %s at %s:%i\n", expression, file, line);
    if(crash)
    {
        *(int *)0 = 0;
    }
}

#define INVALID_CODE_PATH Assert("Invalid code path!" == 0)