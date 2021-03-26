import os
import io


def main():
    target_begin = "<div class=\"toctree-wrapper compound\">"
    target_end = "</div>"
    begin_index = 0
    end_index = 0
    
    fd = open(".axioenvs/rstdoc.imxrt1050-quick/build/html/index.html",'r')
        
    list = fd.readlines()
    for i in range(0,len(list)):
        if target_begin in list[i]:
            begin_index = i
            print("find[%s] ",begin_index,target_begin)
        if begin_index != 0 and target_end in list[i]:
            end_index = i
            print("find ",target_end)
            break
    print(begin_index,end_index)
    print(len(list))
    
    del list[begin_index:end_index]
    
    top_fd = open("topindex.html",'r')
    
    for line in top_fd.readlines():
        list.insert(begin_index,line)
        begin_index += 1

    fd.close()
    top_fd.close()
    
    
    fd = open(".axioenvs/rstdoc.imxrt1050-quick/build/html/index.html",'w')
    fd.writelines(list)
    fd.close()
    
if __name__ == "__main__":
    main()