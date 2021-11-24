/******************************************************************************
* COPYRIGHT @ Reserved
*******************************************************************************
* File         : 
* Function     : 
* Statement    : 
* Creation Info：
******************************************************************************/
/******************************************************************************
* Modification: 
* Time        ：
* Modifier    ：
* Statement   ：
******************************************************************************/

#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/times.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>

//#include "vxWorks.h"
#include "cpss_public.h"
#include "cpss_fs_tftpLib.h"

extern INT32 g_lCpsFsFtError;

#define KHEAP_ALLOC malloc
#define KHEAP_FREE free
#define SOCKADDR struct sockaddr

//#define TFTPC_DEBUG
#define printErr printf

BOOL 	tftpVerbose	= FALSE; 		/* verbose mode		*/
BOOL	tftpTrace	= FALSE;		/* packet tracing 	*/
int	tftpTimeout	= TFTP_TIMEOUT * 5;	/* total timeout (sec.)	*/
int	tftpReXmit	= TFTP_TIMEOUT;		/* rexmit value  (sec.) */

STATUS		tftpCopy ();

TFTP_DESC *	tftpInit ();			/* Lower Level Routines	*/
STATUS		tftpModeSet ();
STATUS  	tftpPeerSet ();
STATUS  	tftpPut ();
STATUS  	tftpGet ();
STATUS  	tftpQuit ();

/* locals */

LOCAL int	fileToAscii();
LOCAL STATUS	asciiToFile ();
LOCAL int	tftpRequestCreate ();
LOCAL void 	tftpPacketTrace ();

char* perr;

int fioRead
(
	int		fd,		/* file descriptor of file to read */
	char *	buffer,		/* buffer to receive input */
	int		maxbytes	/* maximum number of bytes to read */
)
{
	int original_maxbytes = maxbytes;
	int nbytes;

	while (maxbytes > 0)
	{
		nbytes = read (fd, buffer, maxbytes);

		if (nbytes < 0)
			return (ERROR);

		if (nbytes == 0)
			return (original_maxbytes - maxbytes);

		maxbytes -= nbytes;
		buffer   += nbytes;
	}

	return (original_maxbytes);
}

/*******************************************************************************
*
* tftpCopy - transfer a file via TFTP
*
* This routine transfers a file using the TFTP protocol to or from a remote
* system.
* <pHost> is the remote server name or Internet address.  A non-zero value
* for <port> specifies an alternate TFTP server port (zero means use default
* TFTP port number (69)).  <pFilename> is the remote filename. <pCommand>
* specifies the TFTP command, which can be either "put" or "get".
* <pMode> specifies the mode of transfer, which can be "ascii",
* "netascii",  "binary", "image", or "octet".
*
* <fd> is a file descriptor from which to read/write the data from or to
* the remote system.  For example, if the command is "get", the remote data
* will be written to <fd>.  If the command is "put", the data to be sent is
* read from <fd>.  The caller is responsible for managing <fd>.  That is, <fd>
* must be opened prior to calling tftpCopy() and closed up on completion.
*
* EXAMPLE
* The following sequence gets an ASCII file "/folk/vw/xx.yy" on host
* "congo" and stores it to a local file called "localfile":
*
* .CS
*     -> fd = open ("localfile", 0x201, 0644)
*     -> tftpCopy ("congo", 0, "/folk/vw/xx.yy", "get", "ascii", fd)
*     -> close (fd)
* .CE
*
* RETURNS: OK, or ERROR if unsuccessful.
*
* ERRNO
* S_tftpLib_INVALID_COMMAND
*
* SEE ALSO: ftpLib
*/

STATUS tftpCopy
(
	char *		pHost,			/* host name or address	*/
	int			port,			/* optional port number	*/
	char *		pFilename,		/* remote filename 	*/
	char *		pCommand,		/* TFTP command 	*/
	char *		pMode,			/* TFTP transfer mode 	*/
	int	 		fd 			/* fd to put/get data   */
)
{
	TFTP_DESC *		pTftpDesc;		/* TFTP descriptor	*/
	int			status;			/* return status 	*/

	if ((pTftpDesc = tftpInit ()) == NULL)	/* get tftp descriptor  */
		return (ERROR);
	/* set peer and mode */

	if ((tftpPeerSet (pTftpDesc, pHost, port) == ERROR) ||
			((pMode != NULL) && (tftpModeSet (pTftpDesc, pMode) == ERROR)))
	{
		(void) tftpQuit (pTftpDesc);
		return (ERROR);
	}

	if (strcmp (pCommand, "get") == 0)		/* get or put the file */
	{
		status = tftpGet (pTftpDesc, pFilename, fd, TFTP_CLIENT);
	}
	else if (strcmp (pCommand, "put") == 0)
	{
		status =  tftpPut (pTftpDesc, pFilename, fd, TFTP_CLIENT);
	}
	else
	{
		perr = "S_tftpLib_INVALID_COMMAND";
		status = ERROR;
	}

	(void) tftpQuit (pTftpDesc);		/* close tftp session */
	return (status);
}

/*******************************************************************************
*
* tftpInit - initialize a TFTP session
*
* This routine initializes a TFTP session by allocating and initializing a TFTP
* descriptor.  It sets the default transfer mode to "netascii".
*
* RETURNS: A pointer to a TFTP descriptor if successful, otherwise NULL.
*/

TFTP_DESC * tftpInit (void)
{
	TFTP_DESC *		pTftpDesc;		/* TFTP descriptor     */
	struct sockaddr_in	localAddr;   		/* local address       */

	if ((pTftpDesc = (TFTP_DESC *) calloc (1, sizeof (TFTP_DESC))) == NULL)
		return (NULL);
	/* default mode is netascii */

	strcpy (pTftpDesc->mode, "netascii");
	pTftpDesc->connected  = FALSE;
	/* set up a datagram socket */

	if ((pTftpDesc->sock = socket (AF_INET, SOCK_DGRAM, 0)) == ERROR)
	{
		free ((char *) pTftpDesc);
		return (NULL);
	}

	bzero ((char *) &localAddr, sizeof (struct sockaddr_in));
	localAddr.sin_family = AF_INET;
	localAddr.sin_port = htons ((u_short) 0);

	if (bind (pTftpDesc->sock, (SOCKADDR *) &localAddr, sizeof (struct sockaddr_in)) == ERROR)
	{
		free ((char *) pTftpDesc);
		close (pTftpDesc->sock);
		return (NULL);
	}

	return (pTftpDesc);				/* return descriptor */
}

/*******************************************************************************
*
* tftpModeSet - set the TFTP transfer mode
*
* This routine sets the transfer mode associated with the TFTP descriptor
* <pTftpDesc>.  <pMode> specifies the transfer mode, which can be
* "netascii", "binary", "image", or "octet".  Although recognized, these
* modes actually translate into either octet or netascii.
*
* RETURNS: OK, or ERROR if unsuccessful.
*
* ERRNO
*  S_tftpLib_INVALID_DESCRIPTOR
*  S_tftpLib_INVALID_ARGUMENT
*  S_tftpLib_INVALID_MODE
*/

STATUS tftpModeSet
(
	TFTP_DESC *		pTftpDesc,		/* TFTP descriptor 	*/
	char *		pMode 			/* TFTP transfer mode  	*/
)

{
#ifdef TFTPC_DEBUG
	char *		sep = " ";		/* separator character	*/
#endif

	TFTP_MODE *		pTftpModes;		/* pointer to modes	*/
	LOCAL TFTP_MODE 	tftpModes [] = 		/* available modes	*/
	{
		{ "ascii",		"netascii" },
		{ "netascii",	"netascii" },
		{ "binary",		"octet"	   },
		{ "image",		"octet"	   },
		{ "octet",		"octet"	   },
		{ NULL,		NULL	   }
	};

	if (pTftpDesc == NULL)			/* validate arguments */
	{
		perr = "S_tftpLib_INVALID_DESCRIPTOR";
		return (ERROR);
	}

	if (pMode == NULL)
	{
		perr = "S_tftpLib_INVALID_ARGUMENT";
		return (ERROR);
	}

	/* look for actual mode type from the passed mode */

	for (pTftpModes = tftpModes; pTftpModes->m_name != NULL; pTftpModes++)
	{
		if (strcmp (pMode, pTftpModes->m_name) == 0)
		{
			strcpy (pTftpDesc->mode, pTftpModes->m_mode);
			if (tftpVerbose)
				printf ("mode set to %s.\n", pTftpDesc->mode);
			return (OK);
		}
	}
	/* invalid mode, print error */
	perr = "S_tftpLib_INVALID_MODE";

#ifdef TFTPC_DEBUG
	printErr ("%s: unknown mode\n", pMode);
	printErr ("valid modes: [");

	for (pTftpModes = tftpModes; pTftpModes->m_name != NULL; pTftpModes++)
	{
		printErr ("%s%s", sep, pTftpModes->m_name);
		if (*sep == ' ')
			sep = " | ";
	}
	printErr (" ]\n");
#endif

	return (ERROR);
}

/*******************************************************************************
*
* tftpPeerSet - set the TFTP server address
*
* This routine sets the TFTP server (peer) address associated with the TFTP
* descriptor <pTftpDesc>.  <pHostname> is either the TFTP server name
* (e.g., "congo") or the server Internet address (e.g., "90.3").  A non-zero
* value for <port> specifies the server port number (zero means use
* the default TFTP server port number (69)).
*
* RETURNS: OK, or ERROR if unsuccessful.
*
* ERRNO
*  S_tftpLib_INVALID_DESCRIPTOR
*  S_tftpLib_INVALID_ARGUMENT
*  S_tftpLib_UNKNOWN_HOST
*/

STATUS tftpPeerSet
(
	TFTP_DESC *		pTftpDesc,		/* TFTP descriptor	*/
	char *		pHostname, 		/* server name/address	*/
	int    		port			/* port number		*/
)
{
	if (pTftpDesc == NULL)			/* validate arguments */
	{
		perr = "S_tftpLib_INVALID_DESCRIPTOR";
		return (ERROR);
	}

	if ((port < 0) || (pHostname == NULL))
	{
		perr = "S_tftpLib_INVALID_ARGUMENT";
		return (ERROR);
	}

	bzero ((char *) &pTftpDesc->serverAddr, sizeof (struct sockaddr_in));
	pTftpDesc->serverAddr.sin_family = AF_INET;

	/* get server address */
	if ((pTftpDesc->serverAddr.sin_addr.s_addr = inet_addr (pHostname)) == ERROR)
	{
		pTftpDesc->connected = FALSE;

#ifdef TFTPC_DEBUG
		printErr ("tftpPeerSet %s: unknown host.\n", pHostname);
#endif

		perr = "S_tftpLib_UNKNOWN_HOST";
		return (ERROR);
	}
	/* set port and hostname */
	strcpy (pTftpDesc->serverName, pHostname);
	pTftpDesc->serverPort = (port != 0) ? htons ((u_short) port) : htons ((u_short) TFTP_PORT);
	pTftpDesc->connected = TRUE;

	if (tftpVerbose)
		printf("Connected to %s [%d]\n", pHostname, ntohs (pTftpDesc->serverPort));

	return (OK);
}

/*******************************************************************************
*
* tftpPut - put a file to a remote system
*
* This routine puts data from a local file (descriptor) to a file on the remote
* system.  <pTftpDesc> is a pointer to the TFTP descriptor.  <pFilename> is
* the remote filename.  <fd> is the file descriptor from which it gets the
* data.  A call to tftpPeerSet() must be made prior to calling this routine.
*
* RETURNS: OK, or ERROR if unsuccessful.
*
* ERRNO
*  S_tftpLib_INVALID_DESCRIPTOR
*  S_tftpLib_INVALID_ARGUMENT
*  S_tftpLib_NOT_CONNECTED
*/

STATUS tftpPut
(
	TFTP_DESC *		pTftpDesc,		/* TFTP descriptor       */
	char      *		pFilename,		/* remote filename       */
	int	      		fd,			/* file descriptor       */
	int			clientOrServer 		/* which side is calling */
)
{
	int			size;			/* size of data to send */
	TFTP_MSG		tftpMsg;  		/* TFTP message         */
	TFTP_MSG		tftpAck;  		/* TFTP ack message     */
	int			port;			/* return port number   */
	unsigned short			block;			/* data block number    */
	BOOL		convert;		/* convert to ascii	*/
	char		charTemp;		/* temp char holder	*/

	if (pTftpDesc == NULL)			/* validate arguments */
	{
		perr = "S_tftpLib_INVALID_DESCRIPTOR";
		return (ERROR);
	}

	if ((fd < 0) || (pFilename == NULL))
	{
		perr = "S_tftpLib_INVALID_ARGUMENT";
		return (ERROR);
	}

	if (!pTftpDesc->connected)			/* must be connected */
	{
#ifdef TFTPC_DEBUG
		printErr ("tftpPut: No target machine specified.\n");
#endif
		perr = "S_tftpLib_NOT_CONNECTED";
		return (ERROR);
	}
	/* initialize variables */
	convert = (strcmp (pTftpDesc->mode, "netascii") == 0) ? TRUE : FALSE;
	charTemp = '\0';
	bzero ((char *) &tftpMsg, sizeof (TFTP_MSG));
	bzero ((char *) &tftpAck, sizeof (TFTP_MSG));
	pTftpDesc->serverAddr.sin_port = pTftpDesc->serverPort;
	block = 0;

	if (tftpVerbose)
		printf ("putting to %s:%s [%s]\n",  pTftpDesc->serverName, pFilename, pTftpDesc->mode);

	/*
	*  If we're a client, then
	*  create and send write request message.  Get ACK back with
	*  block 0, and a new port number (TID).  If we're a server,
	*  this isn't necessary.
	*/

	if (clientOrServer == TFTP_CLIENT)
	{
		size = tftpRequestCreate (&tftpMsg, TFTP_WRQ, pFilename,
		pTftpDesc->mode);
		if (tftpVerbose)
			printf("Sending WRQ to port %d\n", pTftpDesc->serverPort);

		if ((tftpSend (pTftpDesc, &tftpMsg, size, &tftpAck, TFTP_ACK, block, &port) == ERROR))
			return (ERROR);

		pTftpDesc->serverAddr.sin_port = port;
	}

	do
	{
		/*
		*  Read data from file - converting to ascii if necessary.
		*  All data packets, except the last, must have 512 bytes of data.
		*  The TFTP server sees packets with less than 512 bytes of
		*  data as end of transfer.
		*/
		if (convert)
			size = fileToAscii (fd, tftpMsg.th_data, TFTP_SEGSIZE, &charTemp);
		else
			size = fioRead (fd, tftpMsg.th_data, TFTP_SEGSIZE);

		if (size == ERROR) 		/* if error, send message and bail */
		{
			size = tftpErrorCreate (&tftpMsg, EUNDEF);
			(void) tftpSend (pTftpDesc, &tftpMsg, size, (TFTP_MSG *) NULL,  0, 0, (int *) NULL);
			close (fd);

#ifdef TFTPC_DEBUG
			printErr("tftpPut: Error occurred while reading the file.\n");
#endif
			return (ERROR);
		}

		/* send data message and get an ACK back with same block no. */

		block++;
		tftpMsg.th_opcode = htons ((u_short) TFTP_DATA);
		tftpMsg.th_block  = htons ((u_short) block);

		if (tftpSend (pTftpDesc, &tftpMsg, size + TFTP_DATA_HDR_SIZE, &tftpAck, TFTP_ACK, block, (int *) NULL) == ERROR)
			return (ERROR);

	}
	while (size == TFTP_SEGSIZE);

	return (OK);
}

/*******************************************************************************
*
* tftpGet - get a file from a remote system
*
* This routine gets a file from a remote system via TFTP.  <pFilename> is the
* filename.  <fd> is the file descriptor to which the data is written.
* <pTftpDesc> is a pointer to the TFTP descriptor.  The tftpPeerSet() routine
* must be called prior to calling this routine.
*
* RETURNS: OK, or ERROR if unsuccessful.
*
* ERRNO
*  S_tftpLib_INVALID_DESCRIPTOR
*  S_tftpLib_INVALID_ARGUMENT
*  S_tftpLib_NOT_CONNECTED
*/

STATUS tftpGet
(
	TFTP_DESC *		pTftpDesc,		/* TFTP descriptor       */
	char *		pFilename,		/* remote filename       */
	int			fd,			/* file descriptor       */
	int			clientOrServer 		/* which side is calling */
)
{
	TFTP_MSG		tftpMsg;		/* TFTP message		*/
	TFTP_MSG		tftpReply;		/* TFTP DATA		*/
	int			sizeMsg;		/* size to send		*/
	int			port;			/* return port		*/
	int	*		pPort;			/* port pointer		*/
	unsigned short			block;			/* block expected	*/
	char *		pBuffer; 		/* pointer to buffer	*/
	BOOL		convert;		/* convert from ascii 	*/
	int			numBytes;		/* number of bytes 	*/
	int			sizeReply;		/* number of data bytes */
	char		charTemp;		/* temp char holder 	*/
	STATUS		errorVal;		/* error value 		*/


	if (pTftpDesc == NULL)			/* validate arguments */
	{
		perr = "S_tftpLib_INVALID_DESCRIPTOR";
		return (ERROR);
	}

	if ((fd < 0) || (pFilename == NULL))
	{
		perr = "S_tftpLib_INVALID_ARGUMENT";
		return (ERROR);
	}

	if (!pTftpDesc->connected)			/* return if not connected  */
	{
#ifdef TFTPC_DEBUG
		printErr ("tftpGet: No target machine specified.\n");
#endif

		perr = "S_tftpLib_NOT_CONNECTED";
		return (ERROR);
	}
	/* initialize variables */
	bzero ((char *) &tftpMsg, sizeof (TFTP_MSG));
	bzero ((char *) &tftpReply, sizeof (TFTP_MSG));
	pPort = &port;
	convert = (strcmp (pTftpDesc->mode, "netascii") == 0) ? TRUE : FALSE;
	charTemp = '\0';
	pTftpDesc->serverAddr.sin_port = pTftpDesc->serverPort;
	block = 1;

	if (tftpVerbose)
		printf ("getting from %s:%s [%s]\n", pTftpDesc->serverName,  pFilename, pTftpDesc->mode);

	/*
	* If we're a server, then the first message is an ACK with block = 0.
	* If we're a client, then it's an RRQ.
	*/

	if (clientOrServer == TFTP_SERVER)
	{
		tftpMsg.th_opcode = htons ((u_short)TFTP_ACK);
		tftpMsg.th_block  = htons ((u_short)(0));
		sizeMsg = TFTP_ACK_SIZE;
	}
	else
	{
		/* formulate a RRQ message */
		sizeMsg = tftpRequestCreate (&tftpMsg, TFTP_RRQ, pFilename, pTftpDesc->mode);
	}

	FOREVER
	{
		/* send the RRQ or ACK - expect back DATA */

		sizeReply = tftpSend (pTftpDesc, &tftpMsg, sizeMsg, &tftpReply, TFTP_DATA, block, pPort);

		if (sizeReply == ERROR)			/* if error then bail */
		{
#ifdef TFTPC_DEBUG
			printErr("tftpGet: Error occurred while transferring the file.\n");
#endif
			return (ERROR);
		}

		sizeReply -= TFTP_DATA_HDR_SIZE;	/* calculate data size */

		/*
		*  The first block returns a new server port number (TID) to use.
		*  From now on, we use this port and tell tftpSend to validate it.
		*/

		if (block == 1)
		{
			pTftpDesc->serverAddr.sin_port = port;
			pPort = NULL;
		}

		/* write data to file, converting to ascii if necessary */

		if (convert)
		{
			errorVal = asciiToFile (fd, tftpReply.th_data, sizeReply, &charTemp, sizeReply < TFTP_SEGSIZE);
		}
		else
		{
			int	bytesWritten;

			for (bytesWritten = 0, pBuffer = tftpReply.th_data, errorVal = OK;
					bytesWritten < sizeReply;
					bytesWritten += numBytes, pBuffer += numBytes)
			{
				if ((numBytes = write (fd, pBuffer, sizeReply - bytesWritten)) <= 0)
				{
					errorVal = ERROR;
					break;
				}
			}
		}

		if (errorVal == ERROR)		/* if error send message and bail */
		{
			sizeMsg = tftpErrorCreate (&tftpMsg, EUNDEF);
			(void) tftpSend (pTftpDesc, &tftpMsg, sizeMsg, (TFTP_MSG *) NULL, 0, 0, (int *) NULL);

#ifdef TFTPC_DEBUG
			printErr("tftpGet: Error occurred while writing the file.\n");
#endif
			return (ERROR);
		}

		/* create ACK message */

		tftpMsg.th_opcode = htons ((u_short)TFTP_ACK);
		tftpMsg.th_block  = htons ((u_short)(block));
		sizeMsg = TFTP_ACK_SIZE;

		/*
		* if last packet received was less than 512 bytes then end
		* of transfer. Send final ACK, then we're outta here.
		*/

		if (sizeReply < TFTP_SEGSIZE)
		{
			(void) tftpSend (pTftpDesc, &tftpMsg, sizeMsg, (TFTP_MSG *) NULL, 0, 0, (int *) NULL);
			return (OK);
		}

		block++;
	}  /* end forever loop */
}

/*******************************************************************************
*
* tftpQuit - quit a TFTP session
*
* This routine closes a TFTP session associated with the TFTP descriptor
* <pTftpDesc>.
*
* RETURNS: OK, or ERROR if unsuccessful.
*
* ERRNO
* S_tftpLib_INVALID_DESCRIPTOR
*/

STATUS tftpQuit
(
		TFTP_DESC *		pTftpDesc			/* TFTP descriptor */
)
{
	if (pTftpDesc == NULL)
	{
		perr = "S_tftpLib_INVALID_DESCRIPTOR";
		return (ERROR);
	}

	close (pTftpDesc->sock);			/* close up shop */
	free ((char *) pTftpDesc);
	return (OK);
}

/*******************************************************************************
*
* tftpSend - send a TFTP message to the remote system
*
* This routine sends <sizeMsg> bytes of the passed message <pTftpMsg> to the
* remote system associated with the TFTP descriptor <pTftpDesc>.  If
* <pTftpReply> is not NULL, tftpSend() tries to get a reply message with a
* block number <blockReply> and an opcode <opReply>.  If <pPort> is NULL,
* the reply message must come from the same port to which the message
* was sent.  If <pPort> is not NULL, the port number from which the reply
* message comes is copied to this variable.
*
* INTERNAL
* This routine does the lock step acknowledgement and protocol processing for
* TFTP.  Basically, it sends a message to the server and (waits for and)
* receives the next message.  This is implemented via two main loops.  The
* outer loop sends the message to the TFTP server, the inner loop tries to
* get the next message.  If the reply message does not arrive in a specified
* amount of time, the inner loop is broken out to in order to retransmit
* the send message.
*
* RETURNS: The size of the reply message, or ERROR.
*
* ERRNO
*  S_tftpLib_TIMED_OUT
*  S_tftpLib_TFTP_ERROR
*/

int tftpSend
(
	TFTP_DESC *		pTftpDesc,		/* TFTP descriptor	*/
	TFTP_MSG *		pTftpMsg,		/* TFTP send message	*/
	int			sizeMsg,		/* send message size  	*/
	TFTP_MSG *		pTftpReply,		/* TFTP reply message	*/
	int			opReply,		/* reply opcode   	*/
	int			blockReply,		/* reply block number	*/
	int *		pPort 			/* return port number	*/
)
{
	int 		timeWait = 0;		/* time waited		*/
	int			maxWait;		/* max time to wait	*/
	struct timeval	reXmitTimer;		/* retransmission time	*/
	fd_set 		readFds;		/* select read fds	*/
	struct sockaddr_in  peer;			/* peer			*/
	int 		peerlen; 		/* peer len		*/
	int			num;			/* temp variable	*/
	int			amount;			/* amount in socket 	*/

	/* set up the timeout values */

	bzero ((char *) &reXmitTimer, sizeof (struct timeval));
	maxWait = (tftpTimeout < 0) ? (TFTP_TIMEOUT * 5) : tftpTimeout;
	reXmitTimer.tv_sec = min ((tftpReXmit < 0) ? (TFTP_TIMEOUT) : tftpReXmit,	maxWait);

	FOREVER 					/* send loop */
	{
		if (tftpTrace)
			tftpPacketTrace ("sent", pTftpMsg, sizeMsg);

	/* send message to server */
		if (sendto (pTftpDesc->sock, (caddr_t) pTftpMsg, sizeMsg, 0,
				(SOCKADDR *) &pTftpDesc->serverAddr,
				sizeof (struct sockaddr_in)) != sizeMsg)
			return (ERROR);

		if (pTftpReply == NULL)			/* return if no reply desired */
			return (0);

		FOREVER 				/* receive loop */
		{
			FD_ZERO (&readFds);
			FD_SET  (pTftpDesc->sock, &readFds);
			/* wait for reply message */

			if ((num = select (pTftpDesc->sock + 1, &readFds, (fd_set *) NULL, (fd_set *) NULL, &reXmitTimer)) == ERROR)
				return (ERROR);

			reXmitTimer.tv_sec = min ((tftpReXmit < 0) ? (TFTP_TIMEOUT) : tftpReXmit,	maxWait);

			if (num == 0) 			/* select timed out */
			{
				timeWait += reXmitTimer.tv_sec;
				if (timeWait >= maxWait)	/* return error, if waited */
					/* for too long 	   */
				{
#ifdef TFTPC_DEBUG
					printErr ("Transfer Timed Out.\n");
#endif

					g_lCpsFsFtError = CPS_FS_FT_ERROR_HOST;

					perr = "S_tftpLib_TIMED_OUT";
					return (ERROR);
				}

				/* break out to send loop in order to retransmit */
				break;
			}

			if (ioctl (pTftpDesc->sock, FIONREAD, (int)&amount) == ERROR)
				return (ERROR);

			if (amount == 0)			/* woke up but no data	*/
				continue;			/* just go back to sleep*/

			peerlen = sizeof (struct sockaddr_in);
			if ((num = recvfrom (pTftpDesc->sock, (caddr_t) pTftpReply,
					sizeof (TFTP_MSG), 0, (SOCKADDR *) &peer,
					(unsigned int*)&peerlen)) == ERROR)
				return (ERROR);

			if (tftpTrace)
				tftpPacketTrace ("received", pTftpReply, num);

			/* just return if we received an error message */

			if (ntohs (pTftpReply->th_opcode) == TFTP_ERROR)
			{
#ifdef TFTPC_DEBUG
				printErr ("Error code %d: %s\n", ntohs (pTftpReply->th_error), pTftpReply->th_errMsg);
#endif
				g_lCpsFsFtError = CPS_FS_FT_ERROR_REMOTE_FILE;

				perr = "S_tftpLib_TFTP_ERROR";
				return (ERROR);
			}
			/*
			*  ignore message if it does not have the correct opcode
			*  or it is not from the port we expect.
			*/

			if (((pPort == NULL) &&
					(peer.sin_port != pTftpDesc->serverAddr.sin_port)) ||
					(opReply != ntohs (pTftpReply->th_opcode)))
				continue;

		/* if got right block number, then we got the right packet! */

			if (ntohs (pTftpReply->th_block) == blockReply)
			{
				if (pPort != NULL)
					*pPort = peer.sin_port;
				return (num);
			}
		/*
		* somehow we didn't get the right block, lets flush out the
		* the socket, and try resynching if things look salvageable.
		*/

			FOREVER
			{
				TFTP_MSG	trash;

				if (ioctl (pTftpDesc->sock, FIONREAD, (int)&amount) == ERROR)
					return (ERROR);

				if (amount == 0)		/* done - socket cleaned out */
					break;

				recvfrom (pTftpDesc->sock, (caddr_t) &trash,	sizeof (TFTP_MSG), 0, (SOCKADDR *) NULL,	(unsigned int *) NULL);
			}

			/* break out to send loop in order to resynch by reresending */
			if (ntohs (pTftpReply->th_block) == (blockReply - 1))
				break;

			return (ERROR);			/* things look bad - return */
		}
	}
}

/*******************************************************************************
*
* fileToAscii - read data from file and convert to netascii.
*
* This routine reads data from the file descriptor <fd>, into the buffer
* specified by <pBuffer> whose width is <bufLen>, converting it to netascii
* format.  <charConv>, is a holding space that must be passed to each call
* to fileToAscii().  The first time fileToAscii() is called, the contents of
* <charConv> should contain the character (\0).
*
* The netascii conversions include:
*	lf (\n) -> cr,lf  (\r\n)
*	cr (\r) -> cr, nul (\r\0).
*
* RETURNS: The number of bytes inserted into buffer, or ERROR.
*/

LOCAL int fileToAscii
(
	int 		fd,			/* file descriptor	*/
	char *		pBuffer,		/* buffer 		*/
	int  		bufLen,			/* buffer length 	*/
	char *		charConv 		/* char being converted */
)

{
	char *		ptr;			/* pointer to buffer	*/
	char		currentChar;		/* current char 	*/
	int 		index;			/* count variable 	*/
	int 		numBytes;		/* num bytes read 	*/

	for (index = 0, ptr = pBuffer; index < bufLen; index++)
	{

		/* if previous char was a \n or \r, then expand it */

		if ((*charConv == '\n') || (*charConv == '\r'))
		{
			currentChar = ((*charConv == '\n') ? '\n' : '\0');
			*charConv = '\0';
		}
		else
		{
			if ((numBytes = read (fd, &currentChar, sizeof (char))) == ERROR)
			{
				perror("read");
				return (ERROR);
			}

			if (numBytes == 0) 				/* end of file 	*/
				break;

			if ((currentChar == '\n') || (currentChar == '\r'))
			{
				*charConv = currentChar;
				currentChar = '\r';			/* put in \r first */
			}
		}
		*ptr++ = currentChar;
	}

	return ((int) (ptr - pBuffer));
}

/*******************************************************************************
*
* asciiToFile - write data to file and convert from netascii
*
* Write <bufLen> bytes of data from buffer passed in <pBuffer> to file
* descriptor <fd>, converting from netascii format.  <charConv>, is a holding
* space that must be passed to each call to asciiToFile.  The first time
* asciiToFile is called the contents of <charConv> should contain the
* character (\0).  <isLastBuff> specifies if this is the last buffer to be
* converted.
*
* The conversions include:
*    cr, nul (\r\0) -> cr (\r)
*    cr, lf ( (\r\n) -> lf (\n)
*
* RETURNS: OK if successful, ERROR otherwise.
*/

LOCAL STATUS asciiToFile
(
	int			fd,			/* file descriptor 	*/
	char *		pBuffer,		/* buffer 		*/
	int			bufLen,			/* buffer length	*/
	char *		charConv,		/* char being converted */
	BOOL		isLastBuff		/* last buffer?		*/
)

{
	int			count = bufLen;		/* counter		*/
	char *		ptr = pBuffer;		/* buffer pointer	*/
	char		currentChar;		/* current character	*/
	BOOL		skipWrite; 		/* skip writing		*/

	while (count--)
	{
		currentChar = *ptr++;
		skipWrite = FALSE;

		if (*charConv == '\r')
		{
			/* write out previous (\r) for anything but (\n). */

			if (currentChar != '\n')
			{
				if (write (fd, charConv, sizeof (char)) <= 0)
				return (ERROR);
			}

			if (currentChar == '\0')	/* skip writing (\0) for \r\0 */
				skipWrite = TRUE;
		}

		if ((!skipWrite) && (currentChar != '\r'))
		{
			if (write (fd, &currentChar, sizeof (char)) <= 0)
			return (ERROR);
		}

		*charConv = currentChar;
	}

	/*
	*  Since we wait to write the (\r), if this is the last
	*  buffer and the last character is a (\r), flush it now.
	*/
	if (isLastBuff && (*charConv == '\r'))
	{
		if (write (fd, charConv, sizeof (char)) <= 0)
			return (ERROR);
		*charConv = '\0';
	}

	return (OK);
}

/*******************************************************************************
*
* tftpRequestCreate - create a TFTP read/write request message
*
* This routine creates a TFTP read/write request message. <pTftpMsg> is the
* message to be filled in.  <opcode> specifies the request (either TFTP_RRQ
* or TFTP_WRQ), <pFilename> specifies the filename and <pMode> specifies the
* mode.  The format of a TFTP read/write request message is:
*
*	2 bytes |   string | 1 byte | string | 1 byte
*	----------------------------------------------
*	OpCode  | filename |   0    | Mode   |   0
*
* RETURNS: size of message
*/

LOCAL int tftpRequestCreate
(
	TFTP_MSG *		pTftpMsg,		/* TFTP message pointer	*/
	int       		opCode,			/* request opCode 	*/
	char *		pFilename,		/* remote filename 	*/
	char *		pMode 			/* TFTP transfer mode	*/
)
{
	FAST char *		cp;			/* character pointer 	*/

	pTftpMsg->th_opcode = htons ((u_short) opCode);

	cp = pTftpMsg->th_request;			/* fill in file name	*/
	strcpy (cp, pFilename);
	cp += strlen (pFilename);
	*cp++ = '\0';

	strcpy (cp, pMode);	    		/* fill in mode		*/
	cp += strlen (pMode);
	*cp++ = '\0';

	return (cp - (char *) pTftpMsg);		/* return size of message */
}

/*******************************************************************************
*
* tftpErrorCreate - create a TFTP error message
*
* This routine creates a TFTP error message.  <pTftpMsg> is the TFTP message
* to be filled in.  <errorNum> is a TFTP defined error number.  The format of
* a TFTP error message is:
*
*	2 bytes | 2 bytes   | string | 1 byte
*	-------------------------------------
*	OpCode  | ErrorCode | ErrMsg |   0
*
* RETURNS: Size of message.
*
* NOMANUAL
*/

int tftpErrorCreate
(
	TFTP_MSG *		pTftpMsg,		/* TFTP error message	*/
	int			errorNum 		/* error number 	*/
)
{
	TFTP_ERRMSG	*	pError;			/* pointer to error	*/
	LOCAL TFTP_ERRMSG 	tftpErrors [] =		/* TFTP	defined errors	*/
	{
		{ EUNDEF,	"Undefined error code"		   },
		{ ENOTFOUND,"File not found" 		   },
		{ EACCESS,	"Access violation"		   },
		{ ENOSPACE,	"Disk full or allocation exceeded" },
		{ EBADOP,	"Illegal TFTP operation"	   },
		{ EBADID,	"Unknown transfer ID"		   },
		{ EEXISTS,	"File already exists"		   },
		{ ENOUSER,	"No such user"			   },
		{ -1,	NULL				   }
	};

	//    switch (errorNum)
	//	{
	//	case S_nfsLib_NFSERR_NOENT:
	//	case S_dosFsLib_FILE_NOT_FOUND:
	//	case S_netDrv_NO_SUCH_FILE_OR_DIR:
	//	    errorNum = ENOTFOUND;
	//	    break;
	//	}
	/* locate the passed error */
	for (pError = tftpErrors; pError->e_code >= 0; pError++)
	{
		if (pError->e_code == errorNum)
		{
			break;
		}
	}

	if (pError->e_code < 0) 	    		/* if not found, use EUNDEF */
	pError = tftpErrors;
	/* fill in error message */
	pTftpMsg->th_opcode = htons ((u_short) TFTP_ERROR);
	pTftpMsg->th_error  = htons ((u_short) pError->e_code);
	strcpy (pTftpMsg->th_errMsg, pError->e_msg);

	return (strlen (pError->e_msg) + 5);	/* return size of message */
}

/*******************************************************************************
*
* tftpPacketTrace - trace a TFTP packet
*
* tftpPacketTrace prints out diagnostic information about a TFTP packet.
* Tracing is enabled when the global variable tftpTrace is TRUE.  <pMsg> is a
* pointer to a diagnostic message that gets printed with each trace.
* <pTftpMsg> is the TFTP packet of <size> bytes.
*
* RETURNS: N/A
*/

LOCAL void tftpPacketTrace
(
	char *		pMsg,			/* diagnostic message 	*/
	TFTP_MSG *		pTftpMsg,		/* TFTP packet 		*/
	int 		size 			/* packet size 		*/
)
{
	u_short		op;			/* message op code 	*/
	char *		cp;			/* temp char pointer 	*/
	LOCAL char *	tftpOpCodes [] =	/* ascii op codes 	*/
	{
		"#0",
		"RRQ", 					/* read request		*/
		"WRQ",  				/* write request	*/
		"DATA", 				/* data message		*/
		"ACK", 					/* ack message		*/
		"ERROR"  				/* error message	*/
	};

	op = ntohs (pTftpMsg->th_opcode);

	if (op < TFTP_RRQ || op > TFTP_ERROR) 	/* unknown op code */
	{
		printf ("%s opcode=%x\n", pMsg, op);
		return;
	}
	printf ("%s %s ", pMsg, tftpOpCodes [op]);

	switch (op)
	{
	/* for request messages, display filename and mode */

	case TFTP_RRQ:
	case TFTP_WRQ:
		cp = index (pTftpMsg->th_request, '\0');
		printf ("<file=%s, mode=%s>\n", pTftpMsg->th_request, cp + 1);
	break;

	/* for the data messages, display the block number and bytes */

	case TFTP_DATA:
		printf ("<block=%d, %d bytes>\n", ntohs (pTftpMsg->th_block), size - TFTP_DATA_HDR_SIZE);
	break;

	/* for the ack messages, display the block number */

	case TFTP_ACK:
		printf ("<block=%d>\n", ntohs (pTftpMsg->th_block));
	break;

	/* for error messages display the error code and message */

	case TFTP_ERROR:
		printf ("<code=%d, msg=%s>\n", ntohs (pTftpMsg->th_error), pTftpMsg->th_errMsg);
	break;
	}
}

#if 0
int main(int argc, char* argv[])
{
	int fd;
	int loop = 0;

	if(5 != argc)
	{
		printf("input error , must be ip mode remote_file local_file\n");
		exit(1);
	}
	else
	{
		while(NULL != argv[loop])
		{
			printf("argv[%d] = %s\n", loop, argv[loop]);
			loop ++;
		}
	}

	if(0 == strcmp("get",argv[2]))
	{
		fd = open(argv[4], O_RDWR | O_CREAT);
		if(-1 == fd)
		{
			perror("open");
		}
	}

	if(0 == strcmp("put",argv[2]))
	{
		fd = open(argv[4], O_RDONLY);
		if(-1 == fd)
		{
			perror("open");
		}
	}

	if(ERROR == tftpCopy(argv[1], 0, argv[3], argv[2], "binary", fd))
	{
		printf("tftp error!\n");
		printf("err = %s\n", perr);
	}
	else
	{
		printf("tftp ok!\n");
	}
	close (fd);
	return 0;
}
#endif

#if 0
int tftp_deal(char* Remote, char* Local, char* ServerIp, unsigned int Port, char* Username, char* Password)
{
	int fd;

#if 0
	int loop = 0;

	if(5 != argc)
	{
		printf("input error , must be ip mode remote_file local_file\n");
		exit(1);
	}
	else
	{
		while(NULL != argv[loop])
		{
			printf("argv[%d] = %s\n", loop, argv[loop]);
			loop ++;
		}
	}
#endif

	if(0 == strcmp("get",argv[2]))
	{
		fd = open(argv[4], O_RDWR | O_CREAT);
		if(-1 == fd)
		{
			perror("open");
		}
	}

	if(0 == strcmp("put",argv[2]))
	{
		fd = open(argv[4], O_RDONLY);
		if(-1 == fd)
		{
			perror("open");
		}
	}

	if(ERROR == tftpCopy(argv[1], 0, argv[3], argv[2], "binary", fd))
	{
		printf("tftp error!\n");
		printf("err = %s\n", perr);
	}
	else
	{
		printf("tftp ok!\n");
	}
	close (fd);
	return 0;
}
#endif
