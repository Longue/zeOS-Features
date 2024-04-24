/*
 * libc.c 
*/

#include <libc.h>

#include <types.h>

#include <errno.h>

int errno=0;

void print(char *msg) {
     write(1, msg, strlen(msg));
}

void printn(char *msg, int num) {
    char buff[20];
    itoa(num, buff);
    if (msg != 0)
        write(1, msg, strlen(msg));
    write(1, buff, strlen(buff));
}

void sleep(int x) {
  int start = gettime(), now=gettime();
  while (now < x+start) {
    now = gettime();
  }
}

void itoa(int a, char *b)
{
  int i, i1;
  char c;
  
  if (a==0) { b[0]='0'; b[1]=0; return ;}
  
  i=0;
  while (a>0)
  {
    b[i]=(a%10)+'0';
    a=a/10;
    i++;
  }
  
  for (i1=0; i1<i/2; i1++)
  {
    c=b[i1];
    b[i1]=b[i-i1-1];
    b[i-i1-1]=c;
  }
  b[i]=0;
}

int strlen(char *a)
{
  int i;
  
  i=0;
  
  while (a[i]!=0) i++;
  
  return i;
}

void perror(char *msg) {
  // Identificamos el error
  char *msg_error;
  switch(errno) {
    case EPERM:
      msg_error =	 "Operation not permitted. ";
      break;
    case ENOENT:
      msg_error =	 "No such file or directory. ";
      break;
    case ESRCH:
      msg_error =	 "No such process. ";
      break;
    case EINTR:
      msg_error =	 "Interrupted system call. ";
      break;
    case EIO:
      msg_error =	 "I/O error. ";
      break;
    case ENXIO:
      msg_error =	 "No such device or address. ";
      break;
    case E2BIG:
      msg_error =	 "Arg list too long. ";
      break;
    case ENOEXEC:
      msg_error =	 "Exec format error. ";
      break;
    case EBADF:
      msg_error =	 "Bad file number. ";
      break;
    case ECHILD:
      msg_error = "No child processes. ";
      break;
    case EAGAIN:
      msg_error = "Try again. ";
      break;
    case ENOMEM:
      msg_error = "Out of memory. ";
      break;
    case EACCES:
      msg_error = "Pelarge*)rmission denied. ";
      break;
    case EFAULT:
      msg_error = "Bad address. ";
      break;
    case ENOTBLK:
      msg_error = "Block device required. ";
      break;
    case EBUSY:
      msg_error = "Device or resource busy. ";
      break;
    case EEXIST:
      msg_error = "File exists. ";
      break;
    case EXDEV:
      msg_error = "Cross-device link. ";
      break;
    case ENODEV:
      msg_error = "No such device. ";
      break;
    case ENOTDIR:
      msg_error = "Not a directory. ";
      break;
    case EISDIR:
      msg_error = "Is a directory. ";
      break;
    case EINVAL:
      msg_error = "Invalid argument. ";
      break;
    case ENFILE:
      msg_error = "File table overflow. ";
      break;
    case EMFILE:
      msg_error = "Too many open files. ";
      break;
    case ENOTTY:
      msg_error = "Not a typewriter. ";
      break;
    case ETXTBSY:
      msg_error = "Text file busy. ";
      break;
    case EFBIG:
      msg_error = "File too large. ";
      break;
    case ENOSPC:
      msg_error = "No space left on device. ";
      break;
    case ESPIPE:
      msg_error = "Illegal seek. ";
      break;
    case EROFS:
      msg_error = "Read-only file system. ";
      break;
    case EMLINK:
      msg_error = "Too many links. ";
      break;
    case EPIPE:
      msg_error = "Broken pipe. ";
      break;
    case EDOM:
      msg_error = "Math argument out of domain of func. ";
      break;
    case ERANGE:
      msg_error = "Math result not representable. ";
      break;
    case EDEADLK:
      msg_error = "Resource deadlock would occur. ";
      break;
    case ENAMETOOLONG:
      msg_error = "File name too long. ";
      break;
    case ENOLCK:
      msg_error = "No record locks available. ";
      break;
    case ENOSYS:
      msg_error = "Function not implemented. ";
      break;
    case ENOTEMPTY:
      msg_error = "Directory not empty. ";
      break;
    case ELOOP:
      msg_error = "Too many symbolic links encountered. ";
      break;
    case EWOULDBLOCK:
      msg_error ="Operation would block. ";
      break;
    case ENOMSG:
      msg_error = "No message of desired type. ";
      break;
    case EIDRM:
      msg_error = "Identifier removed. ";
      break;
    case ECHRNG:
      msg_error = "Channel number out of range. ";
      break;
    case EL2NSYNC:
      msg_error ="Level 2 not synchronized. ";
      break;
    case EL3HLT:
      msg_error = "Level 3 halted. ";
      break;
    case EL3RST:
      msg_error = "Level 3 reset. ";
      break;
    case ELNRNG:
      msg_error = "Link number out of range. ";
      break;
    case EUNATCH:
      msg_error = "Protocol driver not attached. ";
      break;
    case ENOCSI:
      msg_error = "No CSI structure available. ";
      break;
    case EL2HLT:
      msg_error = "Level 2 halted. ";
      break;
    case EBADE:
      msg_error = "Invalid exchange. ";
      break;
    case EBADR:
      msg_error = "Invalid request descriptor. ";
      break;
    case EXFULL:
      msg_error = "Exchange full. ";
      break;
    case ENOANO:
      msg_error = "No anode. ";
      break;
    case EBADRQC:
      msg_error = "Invalid request code. ";
      break;
    case EBADSLT:
      msg_error = "Invalid slot. ";
      break;
    case EDEADLOCK:
      msg_error ="File locking deadlock error. ";
      break;
    case EBFONT:
      msg_error = "Bad font file format. ";
      break;
    case ENOSTR:
      msg_error = "Device not a stream. ";
      break;
    case ENODATA:
      msg_error = "No data available. ";
      break;
    case ETIME:
      msg_error = "Timer expired. ";
      break;
    case ENOSR:
      msg_error = "Out of streams resources. ";
      break;
    case ENONET:
      msg_error = "Machine is not on the network. ";
      break;
    case ENOPKG:
      msg_error = "Package not installed. ";
      break;
    case EREMOTE:
      msg_error = "Object is remote. ";
      break;
    case ENOLINK:
      msg_error = "Link has been severed. ";
      break;
    case EADV:
      msg_error = "Advertise error. ";
      break;
    case ESRMNT:
      msg_error = "Srmount error. ";
      break;
    case ECOMM:
      msg_error = "Communication error on send. ";
      break;
    case EPROTO:
      msg_error = "Protocol error. ";
      break;
    case EMULTIHOP:
      msg_error ="Multihop attempted. ";
      break;
    case EDOTDOT:
      msg_error = "RFS specific error. ";
      break;
    case EBADMSG:
      msg_error = "Not a data message. ";
      break;
    case EOVERFLOW:
      msg_error ="Value too large for defined data type. ";
      break;
    case ENOTUNIQ:
      msg_error ="Name not unique on network. ";
      break;
    case EBADFD:
      msg_error = "File descriptor in bad state. ";
      break;
    case EREMCHG:
      msg_error = "Remote address changed. ";
      break;
    case ELIBACC:
      msg_error = "Can not access a needed shared library. ";
      break;
    case ELIBBAD:
      msg_error = "Accessing a corrupted shared library. ";
      break;
    case ELIBSCN:
      msg_error = ".lib section in a.out corrupted. ";
      break;
    case ELIBMAX:
      msg_error = "Attempting to link in too many shared libraries. ";
      break;
    case ELIBEXEC:
      msg_error ="Cannot exec a shared library directly. ";
      break;
    case EILSEQ:
      msg_error = "Illegal byte sequence. ";
      break;
    case ERESTART:
      msg_error ="Interrupted system call should be restarted. ";
      break;
    case ESTRPIPE:
      msg_error ="Streams pipe error. ";
      break;
    case EUSERS:
      msg_error = "Too many users. ";
      break;
    case ENOTSOCK:
      msg_error ="Socket operation on non-socket. ";
      break;
    case EDESTADDRREQ:
      msg_error ="Destination address required. ";
      break;
    case EMSGSIZE:
      msg_error ="Message too long. ";
      break;
    case EPROTOTYPE:
      msg_error ="Protocol wrong type for socket. ";
      break;
    case ENOPROTOOPT:
      msg_error ="Protocol not available. ";
      break;
    case EPROTONOSUPPORT:
      msg_error ="Protocol not supported. ";
      break;
    case ESOCKTNOSUPPORT:
      msg_error ="Socket type not supported. ";
      break;
    case EOPNOTSUPP:
      msg_error ="Operation not supported on transport endpoint. ";
      break;
    case EPFNOSUPPORT:
      msg_error ="Protocol family not supported. ";
      break;
    case EAFNOSUPPORT:
      msg_error ="Address family not supported by protocol. ";
      break;
    case EADDRINUSE:
      msg_error ="Address already in use. ";
      break;
    case EADDRNOTAVAIL:
      msg_error ="Cannot assign requested address. ";
      break;
    case ENETDOWN:
      msg_error ="Network is down. ";
      break;
    case ENETUNREACH:
      msg_error ="Network is unreachable. ";
      break;
    case ENETRESET:
      msg_error ="Network dropped connection because of reset. ";
      break;
    case ECONNABORTED:
      msg_error ="Software caused connection abort. ";
      break;
    case ECONNRESET:
      msg_error ="Connection reset by peer. ";
      break;
    case ENOBUFS:
      msg_error = "No buffer space available. ";
      break;
    case EISCONN:
      msg_error = "Transport endpoint is already connected. ";
      break;
    case ENOTCONN:
      msg_error ="Transport endpoint is not connected. ";
      break;
    case ESHUTDOWN:
      msg_error ="Cannot send after transport endpoint shutdown. ";
      break;
    case ETOOMANYREFS:
      msg_error ="Too many references: cannot splice. ";
      break;
    case ETIMEDOUT:
      msg_error ="Connection timed out. ";
      break;
    case ECONNREFUSED:
      msg_error ="Connection ref */used. ";
      break;
    case EHOSTDOWN:
      msg_error ="Host is down. ";
      break;
    case EHOSTUNREACH:
      msg_error ="No route to host. ";
      break;
    case EALREADY:
      msg_error ="Operation already in progress. ";
      break;
    case EINPROGRESS:
      msg_error ="Operation now in progress. ";
      break;
    case ESTALE:
      msg_error = "Stale NFS file handle. ";
      break;
    case EUCLEAN:
      msg_error = "Structure needs cleaning. ";
      break;
    case ENOTNAM:
      msg_error = "Not a XENIX named type file. ";
      break;
    case ENAVAIL:
      msg_error = "No XENIX semaphores available. ";
      break;
    case EISNAM:
      msg_error = "Is a named type file. ";
      break;
    case EREMOTEIO:
      msg_error ="Remote I/O error. ";
      break;
    default:
      msg_error = "Uknown error. ";
      break;
  }
  char error[9] = "(Codigo: ";
  char num[10];
  char space[2] = ") ";
  itoa(errno, num);
  // Imprimimos el mensaje del error especifico
  write(1, msg_error, strlen(msg_error));
  // Imprimimos el codigo de error
  write(1, error, 9);
  write(1, num, strlen(num));
  write(1, space, 2);

  // Imprimimos el mensaje del usuario
  if (msg != NULL)
    write(1, msg, strlen(msg));
  
  // limpiamos el errno
  errno = 0;
}