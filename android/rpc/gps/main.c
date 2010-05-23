#include <stdio.h>
#include <stdlib.h>
#include <rpc/rpc.h>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <rpc/rpc_router_ioctl.h>
#include <debug.h>
#include <pthread.h>

typedef struct registered_server_struct {
	    /* MUST BE AT OFFSET ZERO!  The client code assumes this when it overwrites
	     *        the XDR for server entries which represent a callback client.  Those
	     *               server entries do not have their own XDRs.
	     *                   */
	    XDR *xdr;
	        /* Because the xdr is NULL for callback clients (as opposed to true
		 *        servers), we keep track of the program number and version number in this
		 *               structure as well.
		 *                   */
	        rpcprog_t x_prog; /* program number */
		    rpcvers_t x_vers; /* program version */

		        int active;
			    struct registered_server_struct *next;
			        SVCXPRT *xprt;
				    __dispatch_fn_t dispatch;
} registered_server;

struct SVCXPRT {
	    fd_set fdset;
	        int max_fd;
		    pthread_attr_t thread_attr;
		        pthread_t  svc_thread;
			    pthread_mutexattr_t lock_attr;
			        pthread_mutex_t lock;
				    registered_server *servers;
				        volatile int num_servers;
};



#define SEND_VAL(x) do { \
	val=x;\
	XDR_SEND_UINT32(clnt, &val);\
} while(0);

uint32_t client_ID;

bool_t xdr_args(XDR* clnt, void *data) {
	uint32_t val=0;
	switch((int)data) {
		case 0:
			//pdsm_client_init
			SEND_VAL(1);//INIT_PD
			return 1;
			break;
		case 1:
			SEND_VAL(client_ID);
			SEND_VAL(0x0049f3a0);
			SEND_VAL(0xa970bf81);
			SEND_VAL(0x0);
			SEND_VAL(0xf310ffff);//0xf310ffff ?
			//SEND_VAL(0xF310FFFF);//0xf310ffff ?
			SEND_VAL(0xa970bee1);
			return 1;
			break;
		case 2:
			//pdsm_client_ext_status_reg(0xDA3, 0x00, 0x00, 0x00, 0x04, -1);
			SEND_VAL(client_ID);
			SEND_VAL(0x0049f3a0);
			SEND_VAL(0xa970bf21); //SEND_VAL(0x00); ?
			SEND_VAL(0x0);
			SEND_VAL(0x00000007);
			SEND_VAL(0xa970be81);
			return 1;
		case 3:
		case 6:
			SEND_VAL(client_ID);
			return 1;
		case 4:
		case 7:
#if 0
			SEND_VAL((data==4) ? 0xb : 0xa);//0xA ?
			SEND_VAL(0x0);
			SEND_VAL(0x1);
			SEND_VAL(0x1);
			SEND_VAL(0x1);
			SEND_VAL(0x3B9AC9FF);
			SEND_VAL(0x1);
			SEND_VAL(0x0);
			SEND_VAL(0x0);
			SEND_VAL(0x0);
			SEND_VAL(0x0);
			SEND_VAL(0x0);
			SEND_VAL(0x0);
			SEND_VAL(0x0);
			SEND_VAL(0x0);
			SEND_VAL(0x0);
			SEND_VAL(0x0);
			SEND_VAL(0x0);
			SEND_VAL(0x0);
			SEND_VAL(0x0);
			SEND_VAL(0x0);
			SEND_VAL(0x0);
			SEND_VAL(0x0);
			SEND_VAL(0x0);
			SEND_VAL(0x1);
			SEND_VAL(0x32);
			SEND_VAL(0x2);
			SEND_VAL(client_ID);
#endif
			SEND_VAL(0xa970be41);
			SEND_VAL(0x0049f3a0);
		       SEND_VAL(	00000001 );
			       SEND_VAL(00000001 );
				 SEND_VAL(00000002 );
				 SEND_VAL(0x0000000a );
				 SEND_VAL(00000001 );
				 SEND_VAL(00000001 );
				 SEND_VAL(00000000 );
				 SEND_VAL(0x480eddc0 );
				 SEND_VAL(0x00006c1c );
				 SEND_VAL(00000000 );
				 SEND_VAL(00000000 );
				 SEND_VAL(00000000 );
				 SEND_VAL(00000000 );
				 SEND_VAL(00000000 );
				 SEND_VAL(0);
				SEND_VAL(00000000 );
				SEND_VAL(00000000 );
				SEND_VAL(00000000 );
				SEND_VAL(00000000 );
				SEND_VAL(00000000 );
				SEND_VAL(00000000 );
				SEND_VAL(00000000 );
				SEND_VAL(00000001 );
				SEND_VAL(0x00000032 );
				SEND_VAL(0x00000078 );
			SEND_VAL(client_ID);

			return 1;
		case 5:
			//pdsm_end_session
			SEND_VAL(0x0b);
			SEND_VAL(0x00);
			SEND_VAL(0x00);
			SEND_VAL(client_ID);
			return 1;
		default:
			return 0;
			break;
			
	};
}

bool_t xdr_result(XDR* clnt, void *data) {
	uint32_t val=0;
	switch((int)data) {
		case 0:
			XDR_RECV_UINT32(clnt, &val);
			client_ID=val;
			printf("\t=%d\n", val);
			return 1;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 42:
			XDR_RECV_UINT32(clnt, &val);
			printf("\t=%d\n", val);
			return 1;
			break;
		default:
			return 0;
			break;
	};
	
}

void dispatch(struct svc_req* a, registered_server* svc) {
	int i;
	uint32_t *data=svc->xdr->in_msg;
	printf("received some kind of event\n");
	for(i=0;i< svc->xdr->in_len/4;++i) {
		printf("%08x ", ntohl(data[i]));
	}
	printf("\n");
}

int main(int argc, char **argv, char **envp) {
	//timeout isn't taken in account by librpc
	struct timeval timeout;
	struct CLIENT *clnt=clnt_create(NULL, 0x3000005B, 0x90380d3d, NULL);
	int i;
	SVCXPRT *svc=svcrtr_create();
	xprt_register(svc);
	svc_register(svc, 0x3100005b, 0xb93145f7, dispatch,0);
	if(!clnt) {
		printf("Failed creating client\n");
		return -1;
	}
	if(!svc) {
		printf("Failed creating server\n");
		return -2;
	}

	printf("pdsm_client_deact(0xDA3);\n");
	if(clnt_call(clnt, 0x9, xdr_args, 6, xdr_result, 6, timeout)) {
		printf("\tfailed\n");
		return -1;
	}

	printf("pdsm_client_init(2)\n");
	if(clnt_call(clnt, 0x2, xdr_args, 0, xdr_result, 0, timeout)) {
		printf("\tfailed\n");
		return -1;
	}


	printf("pdsm_client_pd_reg(0x%x, 0x00, 0x00, 0x00, 0xF3F0FFFF, -1);\n", client_ID);
	if(clnt_call(clnt, 0x4, xdr_args, 1, xdr_result, 1, timeout)) {
		printf("\tfailed\n");
		return -1;
	}

	printf("pdsm_client_ext_status_reg(0xDA3, 0x00, 0x01, 0x00, 0x04, -1);\n");
	if(clnt_call(clnt, 0x7, xdr_args, 2, xdr_result, 2, timeout)) {
		printf("\tfailed\n");
		return -1;
	}

	printf("pdsm_client_act(0xDA3);\n");
	if(clnt_call(clnt, 0x9, xdr_args, 3, xdr_result, 3, timeout)) {
		printf("\tfailed\n");
		return -1;
	}

	printf("BIG BREATH\n");
	printf("BIGGER BREATH\n");

	/*
	printf("pdsm_client_end_session(0xb, 0x00, 0x00, 0xda3);\n");
	if(clnt_call(clnt, 0xc, xdr_args, 5, xdr_result, 5, timeout)) {
		printf("\tfailed\n");
		return -1;
	}*/
	printf("pdsm_client_get_position(0xda3, 0xb);\n");
	if(clnt_call(clnt, 0xB, xdr_args, 4, xdr_result, 4, timeout)) {
		printf("\tfailed\n");
		return -1;
	}

	return 0;
	while(1) {
		printf("pdsm_client_get_position(0xDA3, 0xa);\n");
		if(clnt_call(clnt, 0xB, xdr_args, 7, xdr_result, 7, timeout)) {
			printf("\tfailed\n");
			return -1;
		}
		sleep(1);
	}

	/*
	for(i=0;i<256;++i) {
		printf("clk_regime_sec_msm_get_clk_freq_khz(%d);\n", i);
		if(clnt_call(clnt, 0x24, xdr_args, i, xdr_result, 42, timeout)) {
			printf("\tfailed\n");
			return -1;
		}
	}*/
	return 0;
}
