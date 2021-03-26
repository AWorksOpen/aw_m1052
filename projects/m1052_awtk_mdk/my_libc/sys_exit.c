

const char __stdin_name[]    = "stdin_name";
const char __stdout_name[]   = "stdout_name";
const char __stderr_name[]   = "stderr_name";

int _sys_open(const char *name, int openmode)
{
    return 0;
}

int _sys_close(int arg)
{
    return 0;
}


int _sys_write(int fh, const unsigned char *buf, unsigned len, int mode)
{
    return 0;
}


int _sys_read(int fh, unsigned char *buf, unsigned len, int mode)
{
    return 0;
}


int _sys_istty(int fh)
{
    return 0;
}


int _sys_seek(int fh, long pos)
{
    return 0;
}


int _sys_ensure(int fh)
{
    return 0;
}

long _sys_flen(int fh)
{
    return 0;
}


void _sys_exit(int arg)
{
    arg = arg;
}

int _sys_tmpnam(char *name, int fileno,  unsigned int maxlength)
{
    return 0;
}

int remove (const char * filename)
{
   return 0;
}


unsigned int clock(void)
{
   return 0;
}


void _ttywrch(int ch)
{
    ch = ch;
}

int time ()
{
    return 0;
}

int rename()
{
    return 0;
}

int system(int arg)
{
	  arg = arg;
	  return 0;
}

void bzero(void *buf,int n)
{
  //  memset(buf,0,n);
}




