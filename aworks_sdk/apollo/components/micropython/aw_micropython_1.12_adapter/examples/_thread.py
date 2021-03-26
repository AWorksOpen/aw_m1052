import _thread
import utime

l = 0
lock = _thread.allocate_lock()

def entry1():
    global l
    while lock.acquire():
        l = 1
        print("entry1",l)
        lock.release()
        utime.sleep_ms(1000)

def entry2():
    global l
    while lock.acquire():
        l = 2
        print("entry2",l)
        lock.release()
        utime.sleep_ms(1000)
        
def main():
    _thread.start_new_thread(entry1,())
    utime.sleep_ms(100)
    _thread.start_new_thread(entry2,())
    
main()