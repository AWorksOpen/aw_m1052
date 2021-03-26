try:
    import uos as os
except ImportError:
    import os

if not hasattr(os, "remove"):
    print("SKIP")
    raise SystemExit

testfile = '/sd/test.txt'

# cleanup in case testfile exists
try:
    os.remove(testfile)
except OSError:
    pass

# Should create a file
f = open(testfile, "w")
f.write("foo")
f.close()

f = open(testfile,'r')
print(f.read())
f.close()

f = open(testfile, "a")
f.write("bar")
f.close()

f = open(testfile,'r')
print(f.read())
f.close()

# cleanup
try:
    os.remove("testfile")
except OSError:
    pass