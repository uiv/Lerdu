
#ifdef GET_GROUPS
#endif // GET_GROUPS


#ifdef GET_PACKAGES
PACKAGE("alpha", -1, false)
PACKAGE("alpha.core", -1, true)
PACKAGE("alpha.cplusplus", -1, true)
PACKAGE("alpha.deadcode", -1, true)
PACKAGE("alpha.osx", -1, true)
PACKAGE("alpha.osx.cocoa", -1, true)
PACKAGE("alpha.security", -1, true)
PACKAGE("alpha.security.taint", -1, true)
PACKAGE("alpha.unix", -1, true)
PACKAGE("alpha.unix.cstring", -1, true)
PACKAGE("core", -1, false)
PACKAGE("core.builtin", -1, false)
PACKAGE("core.uninitialized", -1, false)
PACKAGE("cplusplus", -1, false)
PACKAGE("deadcode", -1, false)
PACKAGE("debug", -1, false)
PACKAGE("llvm", -1, false)
PACKAGE("osx", -1, false)
PACKAGE("osx.cocoa", -1, false)
PACKAGE("osx.coreFoundation", -1, false)
PACKAGE("osx.coreFoundation.containers", -1, false)
PACKAGE("security", -1, false)
PACKAGE("security.insecureAPI", -1, false)
PACKAGE("unix", -1, false)
PACKAGE("unix.cstring", -1, true)
#endif // GET_PACKAGES


#ifdef GET_CHECKERS
CHECKER("debug.Stats", AnalyzerStatsChecker, AnalyzerStatsChecker.cpp, "Emit warnings with analyzer statistics", -1, false)
CHECKER("alpha.security.ArrayBound", ArrayBoundChecker, ArrayBoundChecker.cpp, "Warn about buffer overflows (older checker)", -1, true)
CHECKER("alpha.security.ArrayBoundV2", ArrayBoundCheckerV2, ArrayBoundCheckerV2.cpp, "Warn about buffer overflows (newer checker)", -1, true)
CHECKER("core.AttributeNonNull", AttrNonNullChecker, AttrNonNullChecker.cpp, "Check for null pointers passed as arguments to a function whose arguments are marked with the 'nonnull' attribute", -1, false)
CHECKER("alpha.core.BoolAssignment", BoolAssignmentChecker, BoolAssignmentChecker.cpp, "Warn about assigning non-{0,1} values to Boolean variables", -1, true)
CHECKER("core.builtin.BuiltinFunctions", BuiltinFunctionChecker, BuiltinFunctionChecker.cpp, "Evaluate compiler builtin functions (e.g., alloca())", -1, false)
CHECKER("osx.coreFoundation.CFError", CFErrorChecker, NSErrorChecker.cpp, "Check usage of CFErrorRef* parameters", -1, false)
CHECKER("debug.DumpCFG", CFGDumper, DebugCheckers.cpp, "Display Control-Flow Graphs", -1, false)
CHECKER("debug.ViewCFG", CFGViewer, DebugCheckers.cpp, "View Control-Flow Graphs using GraphViz", -1, false)
CHECKER("osx.coreFoundation.CFNumber", CFNumberCreateChecker, BasicObjCFoundationChecks.cpp, "Check for proper uses of CFNumberCreate", -1, false)
CHECKER("osx.coreFoundation.CFRetainRelease", CFRetainReleaseChecker, BasicObjCFoundationChecks.cpp, "Check for null arguments to CFRetain/CFRelease", -1, false)
CHECKER("alpha.unix.cstring.BufferOverlap", CStringBufferOverlap, CStringChecker.cpp, "Checks for overlap in two buffer arguments", -1, true)
CHECKER("alpha.unix.cstring.NotNullTerminated", CStringNotNullTerm, CStringChecker.cpp, "Check for arguments which are not null-terminating strings", -1, true)
CHECKER("unix.cstring.NullArg", CStringNullArg, CStringChecker.cpp, "Check for null pointers being passed as arguments to C string functions", -1, true)
CHECKER("alpha.unix.cstring.OutOfBounds", CStringOutOfBounds, CStringChecker.cpp, "Check for out-of-bounds access in string functions", -1, true)
CHECKER("unix.cstring.BadSizeArg", CStringSyntaxChecker, CStringSyntaxChecker.cpp, "Check the size argument passed into C string functions for common erroneous patterns", -1, true)
CHECKER("core.CallAndMessage", CallAndMessageChecker, CallAndMessageChecker.cpp, "Check for logical errors for function calls and Objective-C message expressions (e.g., uninitialized arguments, null function pointers)", -1, false)
CHECKER("debug.DumpCalls", CallDumper, TraversalChecker.cpp, "Print calls as they are traversed by the engine", -1, false)
CHECKER("debug.DumpCallGraph", CallGraphDumper, DebugCheckers.cpp, "Display Call Graph", -1, false)
CHECKER("debug.ViewCallGraph", CallGraphViewer, DebugCheckers.cpp, "View Call Graph using GraphViz", -1, false)
CHECKER("alpha.core.CastSize", CastSizeChecker, CastSizeChecker.cpp, "Check when casting a malloc'ed type T, whether the size is a multiple of the size of T", -1, true)
CHECKER("alpha.core.CastToStruct", CastToStructChecker, CastToStructChecker.cpp, "Check for cast from non-struct pointer to struct pointer", -1, true)
CHECKER("alpha.unix.Chroot", ChrootChecker, ChrootChecker.cpp, "Check improper use of chroot", -1, true)
CHECKER("osx.cocoa.ClassRelease", ClassReleaseChecker, BasicObjCFoundationChecks.cpp, "Check for sending 'retain', 'release', or 'autorelease' directly to a Class", -1, false)
CHECKER("debug.ConfigDumper", ConfigDumper, DebugCheckers.cpp, "Dump config table", -1, false)
CHECKER("deadcode.DeadStores", DeadStoresChecker, DeadStoresChecker.cpp, "Check for values stored to variables that are never read afterwards", -1, false)
CHECKER("core.NullDereference", DereferenceChecker, DereferenceChecker.cpp, "Check for dereferences of null pointers", -1, false)
CHECKER("alpha.osx.cocoa.DirectIvarAssignment", DirectIvarAssignment, DirectIvarAssignment.cpp, "Check that the invalidatable instance variables are invalidated in the methods annotated with objc_instance_variable_invalidator", -1, true)
CHECKER("core.DivideZero", DivZeroChecker, DivZeroChecker.cpp, "Check for division by zero", -1, false)
CHECKER("debug.DumpDominators", DominatorsTreeDumper, DebugCheckers.cpp, "Print the dominance tree for a given CFG", -1, false)
CHECKER("core.DynamicTypePropagation", DynamicTypePropagation, DynamicTypePropagation.cpp, "Generate dynamic type information", -1, false)
CHECKER("debug.ExprInspection", ExprInspectionChecker, ExprInspectionChecker.cpp, "Check the analyzer's understanding of expressions", -1, false)
CHECKER("alpha.core.FixedAddr", FixedAddressChecker, FixedAddressChecker.cpp, "Check for assignment of a fixed address to a pointer", -1, true)
CHECKER("security.FloatLoopCounter", FloatLoopCounter, CheckSecuritySyntaxOnly.cpp, "Warn on using a floating point value as a loop counter (CERT: FLP30-C, FLP30-CPP)", -1, false)
CHECKER("alpha.security.taint.TaintPropagation", GenericTaintChecker, GenericTaintChecker.cpp, "Generate taint information used by other checkers", -1, true)
CHECKER("alpha.deadcode.IdempotentOperations", IdempotentOperationChecker, IdempotentOperationChecker.cpp, "Warn about idempotent operations", -1, true)
CHECKER("alpha.osx.cocoa.InstanceVariableInvalidation", IvarInvalidationChecker, IvarInvalidationChecker.cpp, "Check that the invalidatable instance variables are invalidated in the methods annotated with objc_instance_variable_invalidator", -1, true)
CHECKER("llvm.Conventions", LLVMConventionsChecker, LLVMConventionsChecker.cpp, "Check code for LLVM codebase conventions", -1, false)
CHECKER("debug.DumpLiveVars", LiveVariablesDumper, DebugCheckers.cpp, "Print results of live variable analysis", -1, false)
CHECKER("osx.SecKeychainAPI", MacOSKeychainAPIChecker, MacOSKeychainAPIChecker.cpp, "Check for proper uses of Secure Keychain APIs", -1, false)
CHECKER("osx.API", MacOSXAPIChecker, MacOSXAPIChecker.cpp, "Check for proper uses of various Mac OS X APIs", -1, false)
CHECKER("alpha.unix.MallocWithAnnotations", MallocOptimistic, MallocChecker.cpp, "Check for memory leaks, double free, and use-after-free problems. Assumes that all user-defined functions which might free a pointer are annotated.", -1, true)
CHECKER("alpha.security.MallocOverflow", MallocOverflowSecurityChecker, MallocOverflowSecurityChecker.cpp, "Check for overflows in the arguments to malloc()", -1, true)
CHECKER("unix.Malloc", MallocPessimistic, MallocChecker.cpp, "Check for memory leaks, double free, and use-after-free problems.", -1, false)
CHECKER("unix.MallocSizeof", MallocSizeofChecker, MallocSizeofChecker.cpp, "Check for dubious malloc arguments involving sizeof", -1, false)
CHECKER("osx.cocoa.NSAutoreleasePool", NSAutoreleasePoolChecker, NSAutoreleasePoolChecker.cpp, "Warn for suboptimal uses of NSAutoreleasePool in Objective-C GC mode", -1, false)
CHECKER("osx.cocoa.NSError", NSErrorChecker, NSErrorChecker.cpp, "Check usage of NSError** parameters", -1, false)
CHECKER("osx.cocoa.NilArg", NilArgChecker, BasicObjCFoundationChecks.cpp, "Check for prohibited nil arguments to ObjC method calls", -1, false)
CHECKER("core.builtin.NoReturnFunctions", NoReturnFunctionChecker, NoReturnFunctionChecker.cpp, "Evaluate \"panic\" functions that are known to not return to the caller", -1, false)
CHECKER("osx.cocoa.AtSync", ObjCAtSyncChecker, ObjCAtSyncChecker.cpp, "Check for nil pointers used as mutexes for @synchronized", -1, false)
CHECKER("osx.coreFoundation.containers.PointerSizedValues", ObjCContainersASTChecker, ObjCContainersASTChecker.cpp, "Warns if 'CFArray', 'CFDictionary', 'CFSet' are created with non-pointer-size values", -1, false)
CHECKER("osx.coreFoundation.containers.OutOfBounds", ObjCContainersChecker, ObjCContainersChecker.cpp, "Checks for index out-of-bounds when using 'CFArray' API", -1, false)
CHECKER("alpha.osx.cocoa.Dealloc", ObjCDeallocChecker, CheckObjCDealloc.cpp, "Warn about Objective-C classes that lack a correct implementation of -dealloc", -1, true)
CHECKER("osx.cocoa.Loops", ObjCLoopChecker, BasicObjCFoundationChecks.cpp, "Improved modeling of loops using Cocoa collection types", -1, false)
CHECKER("osx.cocoa.IncompatibleMethodTypes", ObjCMethSigsChecker, CheckObjCInstMethSignature.cpp, "Warn about Objective-C method signatures with type incompatibilities", -1, false)
CHECKER("osx.cocoa.NonNilReturnValue", ObjCNonNilReturnValueChecker, BasicObjCFoundationChecks.cpp, "Model the APIs that are guaranteed to return a non-nil value", -1, false)
CHECKER("osx.cocoa.SelfInit", ObjCSelfInitChecker, ObjCSelfInitChecker.cpp, "Check that 'self' is properly initialized inside an initializer method", -1, false)
CHECKER("osx.cocoa.UnusedIvars", ObjCUnusedIvarsChecker, ObjCUnusedIVarsChecker.cpp, "Warn about private ivars that are never used", -1, false)
CHECKER("alpha.core.PointerArithm", PointerArithChecker, PointerArithChecker, "Check for pointer arithmetic on locations other than array elements", -1, true)
CHECKER("alpha.core.PointerSub", PointerSubChecker, PointerSubChecker, "Check for pointer subtractions on two pointers pointing to different memory chunks", -1, true)
CHECKER("alpha.unix.PthreadLock", PthreadLockChecker, PthreadLockChecker.cpp, "Simple lock -> unlock checker", -1, true)
CHECKER("osx.cocoa.RetainCount", RetainCountChecker, RetainCountChecker.cpp, "Check for leaks and improper reference count management", -1, false)
CHECKER("alpha.security.ReturnPtrRange", ReturnPointerRangeChecker, ReturnPointerRangeChecker.cpp, "Check for an out-of-bound pointer being returned to callers", -1, true)
CHECKER("core.uninitialized.UndefReturn", ReturnUndefChecker, ReturnUndefChecker.cpp, "Check for uninitialized values being returned to the caller", -1, false)
CHECKER("alpha.core.SizeofPtr", SizeofPointerChecker, CheckSizeofPointer.cpp, "Warn about unintended use of sizeof() on pointer expressions", -1, true)
CHECKER("core.StackAddressEscape", StackAddrEscapeChecker, StackAddrEscapeChecker.cpp, "Check that addresses to stack memory do not escape the function", -1, false)
CHECKER("alpha.unix.Stream", StreamChecker, StreamChecker.cpp, "Check stream handling functions", -1, true)
CHECKER("debug.TaintTest", TaintTesterChecker, TaintTesterChecker.cpp, "Mark tainted symbols as such.", -1, false)
CHECKER("debug.DumpTraversal", TraversalDumper, TraversalChecker.cpp, "Print branch conditions as they are traversed by the engine", -1, false)
CHECKER("security.insecureAPI.UncheckedReturn", UncheckedReturn, CheckSecuritySyntaxOnly.cpp, "Warn on uses of functions whose return values must be always checked", -1, false)
CHECKER("core.uninitialized.Branch", UndefBranchChecker, UndefBranchChecker.cpp, "Check for uninitialized values used as branch conditions", -1, false)
CHECKER("core.uninitialized.CapturedBlockVariable", UndefCapturedBlockVarChecker, UndefCapturedBlockVarChecker.cpp, "Check for blocks that capture uninitialized values", -1, false)
CHECKER("core.UndefinedBinaryOperatorResult", UndefResultChecker, UndefResultChecker.cpp, "Check for undefined results of binary operators", -1, false)
CHECKER("core.uninitialized.ArraySubscript", UndefinedArraySubscriptChecker, UndefinedArraySubscriptChecker.cpp, "Check for uninitialized values used as array subscripts", -1, false)
CHECKER("core.uninitialized.Assign", UndefinedAssignmentChecker, UndefinedAssignmentChecker.cpp, "Check for assigning uninitialized values", -1, false)
CHECKER("unix.API", UnixAPIChecker, UnixAPIChecker.cpp, "Check calls to various UNIX/Posix functions", -1, false)
CHECKER("alpha.deadcode.UnreachableCode", UnreachableCodeChecker, UnreachableCodeChecker.cpp, "Check unreachable code", -1, true)
CHECKER("core.VLASize", VLASizeChecker, VLASizeChecker.cpp, "Check for declarations of VLA of undefined or zero size", -1, false)
CHECKER("osx.cocoa.VariadicMethodTypes", VariadicMethodTypeChecker, BasicObjCFoundationChecks.cpp, "Check for passing non-Objective-C types to variadic collection initialization methods that expect only Objective-C types", -1, false)
CHECKER("alpha.cplusplus.VirtualCall", VirtualCallChecker, VirtualCallChecker.cpp, "Check virtual function calls during construction or destruction", -1, true)
CHECKER("security.insecureAPI.getpw", getpw, CheckSecuritySyntaxOnly.cpp, "Warn on uses of the 'getpw' function", -1, false)
CHECKER("security.insecureAPI.gets", gets, CheckSecuritySyntaxOnly.cpp, "Warn on uses of the 'gets' function", -1, false)
CHECKER("security.insecureAPI.mkstemp", mkstemp, CheckSecuritySyntaxOnly.cpp, "Warn when 'mkstemp' is passed fewer than 6 X's in the format string", -1, false)
CHECKER("security.insecureAPI.mktemp", mktemp, CheckSecuritySyntaxOnly.cpp, "Warn on uses of the 'mktemp' function", -1, false)
CHECKER("security.insecureAPI.rand", rand, CheckSecuritySyntaxOnly.cpp, "Warn on uses of the 'rand', 'random', and related functions", -1, false)
CHECKER("security.insecureAPI.strcpy", strcpy, CheckSecuritySyntaxOnly.cpp, "Warn on uses of the 'strcpy' and 'strcat' functions", -1, false)
CHECKER("security.insecureAPI.vfork", vfork, CheckSecuritySyntaxOnly.cpp, "Warn on uses of the 'vfork' function", -1, false)
#endif // GET_CHECKERS


#ifdef GET_MEMBER_ARRAYS
static const short SubPackageArray0[] = { 1, 9, 11, 19, 26, 14, -1 };
static const short SubPackageArray1[] = { 2, 3, 4, 5, 6, 7, 8, -1 };
static const short CheckerArray2[] = { 4, -1 };
static const short CheckerArray3[] = { 20, -1 };
static const short CheckerArray4[] = { 21, -1 };
static const short CheckerArray5[] = { 32, -1 };
static const short CheckerArray6[] = { 58, -1 };
static const short CheckerArray7[] = { 59, -1 };
static const short CheckerArray8[] = { 64, -1 };
static const short SubPackageArray9[] = { 10, -1 };
static const short CheckerArray10[] = { 79, -1 };
static const short SubPackageArray11[] = { 12, 13, -1 };
static const short CheckerArray12[] = { 35, -1 };
static const short CheckerArray13[] = { 76, -1 };
static const short SubPackageArray14[] = { 15, -1 };
static const short SubPackageArray15[] = { 16, 18, 17, -1 };
static const short CheckerArray16[] = { 52, -1 };
static const short CheckerArray17[] = { 27, -1 };
static const short CheckerArray18[] = { 36, -1 };
static const short SubPackageArray19[] = { 24, 20, 21, 23, 22, -1 };
static const short CheckerArray20[] = { 1, -1 };
static const short CheckerArray21[] = { 2, -1 };
static const short CheckerArray22[] = { 42, -1 };
static const short CheckerArray23[] = { 62, -1 };
static const short SubPackageArray24[] = { 25, -1 };
static const short CheckerArray25[] = { 34, -1 };
static const short SubPackageArray26[] = { 31, 27, 28, 29, 30, -1 };
static const short CheckerArray27[] = { 22, -1 };
static const short CheckerArray28[] = { 41, -1 };
static const short CheckerArray29[] = { 60, -1 };
static const short CheckerArray30[] = { 66, -1 };
static const short SubPackageArray31[] = { 34, 32, 33, -1 };
static const short CheckerArray32[] = { 11, -1 };
static const short CheckerArray33[] = { 12, -1 };
static const short CheckerArray34[] = { 14, -1 };
static const short SubPackageArray35[] = { 44, 47, 40, 37, 36, 43, 38, 42, 41, 39, -1 };
static const short CheckerArray36[] = { 3, -1 };
static const short CheckerArray37[] = { 16, -1 };
static const short CheckerArray38[] = { 28, -1 };
static const short CheckerArray39[] = { 30, -1 };
static const short CheckerArray40[] = { 26, -1 };
static const short CheckerArray41[] = { 65, -1 };
static const short CheckerArray42[] = { 72, -1 };
static const short CheckerArray43[] = { 77, -1 };
static const short SubPackageArray44[] = { 46, 45, -1 };
static const short CheckerArray45[] = { 5, -1 };
static const short CheckerArray46[] = { 48, -1 };
static const short SubPackageArray47[] = { 48, 49, 50, 51, 52, -1 };
static const short CheckerArray48[] = { 73, -1 };
static const short CheckerArray49[] = { 74, -1 };
static const short CheckerArray50[] = { 70, -1 };
static const short CheckerArray51[] = { 71, -1 };
static const short CheckerArray52[] = { 63, -1 };
static const short SubPackageArray54[] = { 55, -1 };
static const short CheckerArray55[] = { 25, -1 };
static const short SubPackageArray56[] = { 61, 62, 67, 58, 68, 59, 57, 63, 60, 65, 66, 64, -1 };
static const short CheckerArray57[] = { 24, -1 };
static const short CheckerArray58[] = { 7, -1 };
static const short CheckerArray59[] = { 18, -1 };
static const short CheckerArray60[] = { 17, -1 };
static const short CheckerArray61[] = { 29, -1 };
static const short CheckerArray62[] = { 38, -1 };
static const short CheckerArray63[] = { 68, -1 };
static const short CheckerArray64[] = { 31, -1 };
static const short CheckerArray65[] = { 0, -1 };
static const short CheckerArray66[] = { 67, -1 };
static const short CheckerArray67[] = { 8, -1 };
static const short CheckerArray68[] = { 19, -1 };
static const short SubPackageArray69[] = { 70, -1 };
static const short CheckerArray70[] = { 37, -1 };
static const short SubPackageArray71[] = { 74, 87, 72, 73, -1 };
static const short CheckerArray72[] = { 40, -1 };
static const short CheckerArray73[] = { 39, -1 };
static const short SubPackageArray74[] = { 75, 81, 76, 86, 79, 77, 85, 84, 78, 82, 80, 83, -1 };
static const short CheckerArray75[] = { 49, -1 };
static const short CheckerArray76[] = { 23, -1 };
static const short CheckerArray77[] = { 54, -1 };
static const short CheckerArray78[] = { 53, -1 };
static const short CheckerArray79[] = { 45, -1 };
static const short CheckerArray80[] = { 46, -1 };
static const short CheckerArray81[] = { 47, -1 };
static const short CheckerArray82[] = { 55, -1 };
static const short CheckerArray83[] = { 61, -1 };
static const short CheckerArray84[] = { 56, -1 };
static const short CheckerArray85[] = { 57, -1 };
static const short CheckerArray86[] = { 78, -1 };
static const short SubPackageArray87[] = { 91, 89, 90, 88, -1 };
static const short CheckerArray88[] = { 6, -1 };
static const short CheckerArray89[] = { 9, -1 };
static const short CheckerArray90[] = { 10, -1 };
static const short SubPackageArray91[] = { 93, 92, -1 };
static const short CheckerArray92[] = { 51, -1 };
static const short CheckerArray93[] = { 50, -1 };
static const short SubPackageArray94[] = { 96, 95, -1 };
static const short CheckerArray95[] = { 33, -1 };
static const short SubPackageArray96[] = { 99, 98, 101, 100, 102, 103, 104, 97, -1 };
static const short CheckerArray97[] = { 69, -1 };
static const short CheckerArray98[] = { 80, -1 };
static const short CheckerArray99[] = { 81, -1 };
static const short CheckerArray100[] = { 82, -1 };
static const short CheckerArray101[] = { 83, -1 };
static const short CheckerArray102[] = { 84, -1 };
static const short CheckerArray103[] = { 85, -1 };
static const short CheckerArray104[] = { 86, -1 };
static const short SubPackageArray105[] = { 109, 106, 107, 108, -1 };
static const short CheckerArray106[] = { 75, -1 };
static const short CheckerArray107[] = { 43, -1 };
static const short CheckerArray108[] = { 44, -1 };
static const short SubPackageArray109[] = { 111, 110, -1 };
static const short CheckerArray110[] = { 15, -1 };
static const short CheckerArray111[] = { 13, -1 };
#endif // GET_MEMBER_ARRAYS


#ifdef GET_CHECKNAME_TABLE
  { "alpha",                                            0, SubPackageArray0, false },
  { "alpha.core",                                       0, SubPackageArray1, true },
  { "alpha.core.BoolAssignment",                        CheckerArray2, 0, false },
  { "alpha.core.CastSize",                              CheckerArray3, 0, false },
  { "alpha.core.CastToStruct",                          CheckerArray4, 0, false },
  { "alpha.core.FixedAddr",                             CheckerArray5, 0, false },
  { "alpha.core.PointerArithm",                         CheckerArray6, 0, false },
  { "alpha.core.PointerSub",                            CheckerArray7, 0, false },
  { "alpha.core.SizeofPtr",                             CheckerArray8, 0, false },
  { "alpha.cplusplus",                                  0, SubPackageArray9, true },
  { "alpha.cplusplus.VirtualCall",                      CheckerArray10, 0, false },
  { "alpha.deadcode",                                   0, SubPackageArray11, true },
  { "alpha.deadcode.IdempotentOperations",              CheckerArray12, 0, false },
  { "alpha.deadcode.UnreachableCode",                   CheckerArray13, 0, false },
  { "alpha.osx",                                        0, SubPackageArray14, true },
  { "alpha.osx.cocoa",                                  0, SubPackageArray15, true },
  { "alpha.osx.cocoa.Dealloc",                          CheckerArray16, 0, false },
  { "alpha.osx.cocoa.DirectIvarAssignment",             CheckerArray17, 0, false },
  { "alpha.osx.cocoa.InstanceVariableInvalidation",     CheckerArray18, 0, false },
  { "alpha.security",                                   0, SubPackageArray19, true },
  { "alpha.security.ArrayBound",                        CheckerArray20, 0, false },
  { "alpha.security.ArrayBoundV2",                      CheckerArray21, 0, false },
  { "alpha.security.MallocOverflow",                    CheckerArray22, 0, false },
  { "alpha.security.ReturnPtrRange",                    CheckerArray23, 0, false },
  { "alpha.security.taint",                             0, SubPackageArray24, true },
  { "alpha.security.taint.TaintPropagation",            CheckerArray25, 0, false },
  { "alpha.unix",                                       0, SubPackageArray26, true },
  { "alpha.unix.Chroot",                                CheckerArray27, 0, false },
  { "alpha.unix.MallocWithAnnotations",                 CheckerArray28, 0, false },
  { "alpha.unix.PthreadLock",                           CheckerArray29, 0, false },
  { "alpha.unix.Stream",                                CheckerArray30, 0, false },
  { "alpha.unix.cstring",                               0, SubPackageArray31, true },
  { "alpha.unix.cstring.BufferOverlap",                 CheckerArray32, 0, false },
  { "alpha.unix.cstring.NotNullTerminated",             CheckerArray33, 0, false },
  { "alpha.unix.cstring.OutOfBounds",                   CheckerArray34, 0, false },
  { "core",                                             0, SubPackageArray35, false },
  { "core.AttributeNonNull",                            CheckerArray36, 0, false },
  { "core.CallAndMessage",                              CheckerArray37, 0, false },
  { "core.DivideZero",                                  CheckerArray38, 0, false },
  { "core.DynamicTypePropagation",                      CheckerArray39, 0, false },
  { "core.NullDereference",                             CheckerArray40, 0, false },
  { "core.StackAddressEscape",                          CheckerArray41, 0, false },
  { "core.UndefinedBinaryOperatorResult",               CheckerArray42, 0, false },
  { "core.VLASize",                                     CheckerArray43, 0, false },
  { "core.builtin",                                     0, SubPackageArray44, false },
  { "core.builtin.BuiltinFunctions",                    CheckerArray45, 0, false },
  { "core.builtin.NoReturnFunctions",                   CheckerArray46, 0, false },
  { "core.uninitialized",                               0, SubPackageArray47, false },
  { "core.uninitialized.ArraySubscript",                CheckerArray48, 0, false },
  { "core.uninitialized.Assign",                        CheckerArray49, 0, false },
  { "core.uninitialized.Branch",                        CheckerArray50, 0, false },
  { "core.uninitialized.CapturedBlockVariable",         CheckerArray51, 0, false },
  { "core.uninitialized.UndefReturn",                   CheckerArray52, 0, false },
  { "cplusplus",                                        0, 0, false },
  { "deadcode",                                         0, SubPackageArray54, false },
  { "deadcode.DeadStores",                              CheckerArray55, 0, false },
  { "debug",                                            0, SubPackageArray56, false },
  { "debug.ConfigDumper",                               CheckerArray57, 0, false },
  { "debug.DumpCFG",                                    CheckerArray58, 0, false },
  { "debug.DumpCallGraph",                              CheckerArray59, 0, false },
  { "debug.DumpCalls",                                  CheckerArray60, 0, false },
  { "debug.DumpDominators",                             CheckerArray61, 0, false },
  { "debug.DumpLiveVars",                               CheckerArray62, 0, false },
  { "debug.DumpTraversal",                              CheckerArray63, 0, false },
  { "debug.ExprInspection",                             CheckerArray64, 0, false },
  { "debug.Stats",                                      CheckerArray65, 0, false },
  { "debug.TaintTest",                                  CheckerArray66, 0, false },
  { "debug.ViewCFG",                                    CheckerArray67, 0, false },
  { "debug.ViewCallGraph",                              CheckerArray68, 0, false },
  { "llvm",                                             0, SubPackageArray69, false },
  { "llvm.Conventions",                                 CheckerArray70, 0, false },
  { "osx",                                              0, SubPackageArray71, false },
  { "osx.API",                                          CheckerArray72, 0, false },
  { "osx.SecKeychainAPI",                               CheckerArray73, 0, false },
  { "osx.cocoa",                                        0, SubPackageArray74, false },
  { "osx.cocoa.AtSync",                                 CheckerArray75, 0, false },
  { "osx.cocoa.ClassRelease",                           CheckerArray76, 0, false },
  { "osx.cocoa.IncompatibleMethodTypes",                CheckerArray77, 0, false },
  { "osx.cocoa.Loops",                                  CheckerArray78, 0, false },
  { "osx.cocoa.NSAutoreleasePool",                      CheckerArray79, 0, false },
  { "osx.cocoa.NSError",                                CheckerArray80, 0, false },
  { "osx.cocoa.NilArg",                                 CheckerArray81, 0, false },
  { "osx.cocoa.NonNilReturnValue",                      CheckerArray82, 0, false },
  { "osx.cocoa.RetainCount",                            CheckerArray83, 0, false },
  { "osx.cocoa.SelfInit",                               CheckerArray84, 0, false },
  { "osx.cocoa.UnusedIvars",                            CheckerArray85, 0, false },
  { "osx.cocoa.VariadicMethodTypes",                    CheckerArray86, 0, false },
  { "osx.coreFoundation",                               0, SubPackageArray87, false },
  { "osx.coreFoundation.CFError",                       CheckerArray88, 0, false },
  { "osx.coreFoundation.CFNumber",                      CheckerArray89, 0, false },
  { "osx.coreFoundation.CFRetainRelease",               CheckerArray90, 0, false },
  { "osx.coreFoundation.containers",                    0, SubPackageArray91, false },
  { "osx.coreFoundation.containers.OutOfBounds",        CheckerArray92, 0, false },
  { "osx.coreFoundation.containers.PointerSizedValues", CheckerArray93, 0, false },
  { "security",                                         0, SubPackageArray94, false },
  { "security.FloatLoopCounter",                        CheckerArray95, 0, false },
  { "security.insecureAPI",                             0, SubPackageArray96, false },
  { "security.insecureAPI.UncheckedReturn",             CheckerArray97, 0, false },
  { "security.insecureAPI.getpw",                       CheckerArray98, 0, false },
  { "security.insecureAPI.gets",                        CheckerArray99, 0, false },
  { "security.insecureAPI.mkstemp",                     CheckerArray100, 0, false },
  { "security.insecureAPI.mktemp",                      CheckerArray101, 0, false },
  { "security.insecureAPI.rand",                        CheckerArray102, 0, false },
  { "security.insecureAPI.strcpy",                      CheckerArray103, 0, false },
  { "security.insecureAPI.vfork",                       CheckerArray104, 0, false },
  { "unix",                                             0, SubPackageArray105, false },
  { "unix.API",                                         CheckerArray106, 0, false },
  { "unix.Malloc",                                      CheckerArray107, 0, false },
  { "unix.MallocSizeof",                                CheckerArray108, 0, false },
  { "unix.cstring",                                     0, SubPackageArray109, true },
  { "unix.cstring.BadSizeArg",                          CheckerArray110, 0, false },
  { "unix.cstring.NullArg",                             CheckerArray111, 0, false },
#endif // GET_CHECKNAME_TABLE

