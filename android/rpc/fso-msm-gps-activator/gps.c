/*
   Copyright 2010 Pierre-Hugues HUSSON
   Copyright 2010 Denis 'GNUtoo' Carikli

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include <stdio.h>
#include <stdlib.h>
#include <android-rpc/rpc.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <android-rpc/rpc_router_ioctl.h>
//#include <debug.h>
#include <pthread.h>
#ifdef ANDROID //ANDROID is defined when building with the android build system
#include <hardware_legacy/gps.h>
#else
#include "hardware_legacy_gps.h"
#endif

typedef struct registered_server_struct {
	/* MUST BE AT OFFSET ZERO!  The client code assumes this when it overwrites
	 * the XDR for server entries which represent a callback client.  Those
	 * server entries do not have their own XDRs.
	 */
	XDR *xdr;
	/* Because the xdr is NULL for callback clients (as opposed to true
	 * servers), we keep track of the program number and version number in this
	 * structure as well.
	 */
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

static uint32_t client_IDs[16];//highest known value is 0xb
static uint32_t can_send=1; //To prevent from sending get_position when EVENT_END hasn't been received

struct params {
	uint32_t *data;
	int length;
};
static struct CLIENT *_clnt;

static bool_t xdr_args(XDR *clnt, struct params *par) {
	int i;
	uint32_t val=0;
	for(i=0;par->length>i;++i)
		SEND_VAL(par->data[i]);
	return 1;
}

static bool_t xdr_result_int(XDR *clnt, uint32_t *result) {
	XDR_RECV_UINT32(clnt, result);
	return 1;
}

static struct timeval timeout;
static enum {
	A5225,
	A6125,
} amss;

static int pdsm_client_init(struct CLIENT *clnt, int client) {
	struct params par;
	uint32_t res;
	par.data=malloc(sizeof(int));
	par.length=1;
	par.data[0]=client;
#ifdef ANDROID
	if(clnt_call(clnt, amss==A6125 ? 0x2 : 0x3, xdr_args, &par, xdr_result_int, &res, timeout)) {
#else
	if(android_clnt_call(clnt, amss==A6125 ? 0x2 : 0x3, xdr_args, &par, xdr_result_int, &res, timeout)) {
#endif
		free(par.data);
		exit(-1);
	}
	free(par.data);
	client_IDs[client]=res;
	return 0;
}

int pdsm_atl_l2_proxy_reg(struct CLIENT *clnt, int val0, int val1, int val2) {
	struct params par;
	uint32_t res;
	par.data=malloc(sizeof(int)*3);
	par.length=3;
	par.data[0]=val0;
	par.data[1]=val1;
	par.data[2]=val2;
#ifdef ANDROID
	if(clnt_call(clnt, amss==A6125 ? 0x3 : 0x4, xdr_args, &par, xdr_result_int, &res, timeout)) {
#else
	if(android_clnt_call(clnt, amss==A6125 ? 0x3 : 0x4, xdr_args, &par, xdr_result_int, &res, timeout)) {
#endif
#ifdef ANDROID
		
#endif
		free(par.data);
		exit(-1);
	}
	free(par.data);
	return res;
}

int pdsm_atl_dns_proxy_reg(struct CLIENT *clnt, int val0, int val1) {
	struct params par;
	uint32_t res;
	par.data=malloc(sizeof(int)*2);
	par.length=2;
	par.data[0]=val0;
	par.data[1]=val1;
#ifdef ANDROID
	if(clnt_call(clnt, amss==A6125 ? 0x6 : 0x7 , xdr_args, &par, xdr_result_int, &res, timeout)) {
#else
	if(android_clnt_call(clnt, amss==A6125 ? 0x6 : 0x7 , xdr_args, &par, xdr_result_int, &res, timeout)) {
#endif
		free(par.data);
		exit(-1);
	}
	free(par.data);
	return res;
}

int pdsm_client_pd_reg(struct CLIENT *clnt, int client, int val0, int val1, int val2, int val3, int val4) {
	struct params par;
	uint32_t res;
	par.data=malloc(sizeof(int)*6);
	par.length=6;
	par.data[0]=client_IDs[client];
	par.data[1]=val0;
	par.data[2]=val1;
	par.data[3]=val2;
	par.data[4]=val3;
	par.data[5]=val4;
#ifdef ANDROID
	if(clnt_call(clnt, amss==A6125 ? 0x4 : 0x5, xdr_args, &par, xdr_result_int, &res, timeout)) {
#else
	if(android_clnt_call(clnt, amss==A6125 ? 0x4 : 0x5, xdr_args, &par, xdr_result_int, &res, timeout)) {
#endif
		free(par.data);
		exit(-1);
	}
	free(par.data);
	return res;
}

int pdsm_client_pa_reg(struct CLIENT *clnt, int client, int val0, int val1, int val2, int val3, int val4) {
	struct params par;
	uint32_t res;
	par.data=malloc(sizeof(int)*6);
	par.length=6;
	par.data[0]=client_IDs[client];
	par.data[1]=val0;
	par.data[2]=val1;
	par.data[3]=val2;
	par.data[4]=val3;
	par.data[5]=val4;
#ifdef ANDROID
	if(clnt_call(clnt, amss==A6125 ? 0x5 : 0x6, xdr_args, &par, xdr_result_int, &res, timeout)) {
#else
	if(android_clnt_call(clnt, amss==A6125 ? 0x5 : 0x6, xdr_args, &par, xdr_result_int, &res, timeout)) {
#endif
#ifdef ADNDROID
		
#endif
		free(par.data);
		exit(-1);
	}
	free(par.data);
	
	return res;
}

int pdsm_client_lcs_reg(struct CLIENT *clnt, int client, int val0, int val1, int val2, int val3, int val4) {
	struct params par;
	uint32_t res;
	par.data=malloc(sizeof(int)*6);
	par.length=6;
	par.data[0]=client_IDs[client];
	par.data[1]=val0;
	par.data[2]=val1;
	par.data[3]=val2;
	par.data[4]=val3;
	par.data[5]=val4;
#ifdef ANDROID
	if(clnt_call(clnt, amss==A6125 ? 0x6 : 0x7, xdr_args, &par, xdr_result_int, &res, timeout)) {
#else
	if(android_clnt_call(clnt, amss==A6125 ? 0x6 : 0x7, xdr_args, &par, xdr_result_int, &res, timeout)) {
#endif

		
		free(par.data);
		exit(-1);
	}
	free(par.data);
	
	return res;
}

int pdsm_client_ext_status_reg(struct CLIENT *clnt, int client, int val0, int val1, int val2, int val3, int val4) {
	struct params par;
	uint32_t res;
	par.data=malloc(sizeof(int)*6);
	par.length=6;
	par.data[0]=client_IDs[client];
	par.data[1]=val0;
	par.data[2]=val1;
	par.data[3]=val2;
	par.data[4]=val3;
	par.data[5]=val4;
#ifdef ANDROID
	if(clnt_call(clnt, amss==A6125 ? 0x8 : 0x9, xdr_args, &par, xdr_result_int, &res, timeout)) {
#else
	if(android_clnt_call(clnt, amss==A6125 ? 0x8 : 0x9, xdr_args, &par, xdr_result_int, &res, timeout)) {
#endif
		
		free(par.data);
		exit(-1);
	}
	free(par.data);
	
	return res;
}

int pdsm_client_xtra_reg(struct CLIENT *clnt, int client, int val0, int val1, int val2, int val3, int val4) {
	struct params par;
	uint32_t res;
	par.data=malloc(sizeof(int)*6);
	par.length=6;
	par.data[0]=client_IDs[client];
	par.data[1]=val0;
	par.data[2]=val1;
	par.data[3]=val2;
	par.data[4]=val3;
	par.data[5]=val4;
#ifdef ANDROID
	if(clnt_call(clnt, amss==A6125 ? 0x7 :0x8, xdr_args, &par, xdr_result_int, &res, timeout)) {
#else
	if(android_clnt_call(clnt, amss==A6125 ? 0x7 :0x8, xdr_args, &par, xdr_result_int, &res, timeout)) {
#endif
		
		free(par.data);
		exit(-1);
	}
	free(par.data);
	
	return res;
}

int pdsm_client_act(struct CLIENT *clnt, int client) {
	struct params par;
	uint32_t res;
	par.data=malloc(sizeof(int));
	par.length=1;
	par.data[0]=client_IDs[client];
#ifdef ANDROID
	if(clnt_call(clnt, amss==A6125 ? 0x9 : 0xa, xdr_args, &par, xdr_result_int, &res, timeout)) {
#else
	if(android_clnt_call(clnt, amss==A6125 ? 0x9 : 0xa, xdr_args, &par, xdr_result_int, &res, timeout)) {
#endif
		
		free(par.data);
		exit(-1);
	}
	free(par.data);
	
	return res;
}

int pdsm_get_position(struct CLIENT *clnt, int val0, int val1, int val2, int val3, int val4, int val5, int val6, int val7, int val8, int val9, int val10, int val11, int val12, int val13, int val14, int val15, int val16, int val17, int val18, int val19, int val20, int val21, int val22, int val23, int val24, int val25, int val26, int val27) {
	struct params par;
	uint32_t res;
	par.data=malloc(sizeof(int)*28);
	par.length=28;
	par.data[0]=val0;
	par.data[1]=val1;
	par.data[2]=val2;
	par.data[3]=val3;
	par.data[4]=val4;
	par.data[5]=val5;
	par.data[6]=val6;
	par.data[7]=val7;
	par.data[8]=val8;
	par.data[9]=val9;
	par.data[10]=val10;
	par.data[11]=val11;
	par.data[12]=val12;
	par.data[13]=val13;
	par.data[14]=val14;
	par.data[15]=val15;
	par.data[16]=val16;
	par.data[17]=val17;
	par.data[18]=val18;
	par.data[19]=val19;
	par.data[20]=val20;
	par.data[21]=val21;
	par.data[22]=val22;
	par.data[23]=val23;
	par.data[24]=val24;
	par.data[25]=val25;
	par.data[26]=val26;
	par.data[27]=val27;
#ifdef ANDROID
	if(clnt_call(clnt, amss==A6125 ? 0xb : 0xc, xdr_args, &par, xdr_result_int, &res, timeout)) {
#else
	if(android_clnt_call(clnt, amss==A6125 ? 0xb : 0xc, xdr_args, &par, xdr_result_int, &res, timeout)) {
#endif
		
		exit(-1);
	}
	
	return res;
}

enum pdsm_pd_events {
	PDSM_PD_EVENT_POS = 0x1,
	PDSM_PD_EVENT_VELOCITY = 0x2,
	PDSM_PD_EVENT_HEIGHT = 0x4,
	PDSM_PD_EVENT_DONE = 0x8,
	PDSM_PD_EVENT_END = 0x10,
	PDSM_PD_EVENT_BEGIN = 0x20,
	PDSM_PD_EVENT_COMM_BEGIN = 0x40,
	PDSM_PD_EVENT_COMM_CONNECTED = 0x80,
	PDSM_PD_EVENT_COMM_DONE = 0x200,
	PDSM_PD_EVENT_GPS_BEGIN = 0x4000,
	PDSM_PD_EVENT_GPS_DONE = 0x8000,
	PDSM_PD_EVENT_UPDATE_FAIL = 0x1000000,
};

#ifdef ANDROID
//From gps_msm7k.c
extern void update_gps_status(GpsStatusValue val);
extern void update_gps_svstatus(GpsSvStatus *val);
extern void update_gps_location(GpsLocation *fix);
#endif

void dispatch_pdsm_pd(uint32_t *data) {
	uint32_t event=ntohl(data[2]);
	if(event&PDSM_PD_EVENT_GPS_BEGIN) {
#ifdef ANDROID
		//Navigation started.
		update_gps_status(GPS_STATUS_SESSION_BEGIN);
#else
//TODO: print some message
#endif
	}
	if(event&PDSM_PD_EVENT_GPS_DONE) {
#ifdef ANDROID
		//Navigation ended (times out circa 10seconds ater last get_pos)
		update_gps_status(GPS_STATUS_SESSION_END);
#else
//TODO: print some message
#endif 
	}
	GpsLocation fix;
	fix.flags = 0;
	if(event&PDSM_PD_EVENT_POS) {
	
		GpsSvStatus ret;
		int i;
		ret.num_svs=ntohl(data[82]) & 0x1F;

		for(i=0;i<ret.num_svs;++i) {
			ret.sv_list[i].prn=ntohl(data[83+3*i]);
			ret.sv_list[i].elevation=ntohl(data[83+3*i+1]);
			ret.sv_list[i].azimuth=ntohl(data[83+3*i+2])/100;
			ret.sv_list[i].snr=ntohl(data[83+3*i+2])%100;
		}
		ret.used_in_fix_mask=ntohl(data[77]);
#ifdef ANDROID
		update_gps_svstatus(&ret);
#else
//TODO: print some messages
#endif

		fix.timestamp = ntohl(data[8]);
		if (!fix.timestamp) return;

		// convert gps time to epoch time ms
		fix.timestamp += 315964800; // 1/1/1970 to 1/6/1980
		fix.timestamp *= 1000; //ms

		fix.flags |= GPS_LOCATION_HAS_LAT_LONG;

		if (ntohl(data[75])) {
			fix.flags |= GPS_LOCATION_HAS_ACCURACY;
			fix.accuracy = (float)ntohl(data[75]) / 10.0f;
		}

		union {
			struct {
				uint32_t lowPart;
				int32_t highPart;
			};
			int64_t int64Part;
		} latitude, longitude;

		latitude.lowPart = ntohl(data[61]);
		latitude.highPart = ntohl(data[60]);
		longitude.lowPart = ntohl(data[63]);
		longitude.highPart = ntohl(data[62]);
		fix.latitude = (double)latitude.int64Part / 1.0E8;
		fix.longitude = (double)longitude.int64Part / 1.0E8;
	}
	if (event&PDSM_PD_EVENT_VELOCITY)
	{
		fix.flags |= GPS_LOCATION_HAS_SPEED|GPS_LOCATION_HAS_BEARING;
		fix.speed = (float)ntohl(data[66]) / 10.0f / 3.6f; // convert kp/h to m/s
		fix.bearing = (float)ntohl(data[67]) / 10.0f;
	}
	if (event&PDSM_PD_EVENT_HEIGHT)
	{
		fix.flags |= GPS_LOCATION_HAS_ALTITUDE;
		fix.altitude = (double)ntohl(data[64]) / 10.0f;
	}
	if (fix.flags)
	{
#ifdef ANDROID
		update_gps_location(&fix);
#else
//TODO: print some message
#endif
	}
	if(event&PDSM_PD_EVENT_DONE)
		can_send=1;
}

void dispatch_pdsm_ext(uint32_t *data) {

	GpsSvStatus ret;
	int i;

	ret.num_svs=ntohl(data[7]);
	for(i=0;i<ret.num_svs;++i) {
		ret.sv_list[i].prn=ntohl(data[101+12*i]);
		ret.sv_list[i].elevation=ntohl(data[101+12*i+4]);
		ret.sv_list[i].azimuth=ntohl(data[101+12*i+3]);
		ret.sv_list[i].snr=(float)ntohl(data[101+12*i+1])/10.0f;
	}

	ret.used_in_fix_mask=ntohl(data[8]);
	if (ret.num_svs) {
#ifdef ANDROID
		update_gps_svstatus(&ret);
#else
//TODO: print some message
#endif
	}

}

void dispatch_pdsm(uint32_t *data) {
	uint32_t procid=ntohl(data[5]);
	if(procid==1) 
		dispatch_pdsm_pd(&(data[10]));
	else if(procid==4) 
		dispatch_pdsm_ext(&(data[10]));

}

void dispatch_atl(uint32_t *data) {
	// No clue what happens here.
}

void dispatch(struct svc_req* a, registered_server* svc) {
	int i;
	uint32_t *data=svc->xdr->in_msg;
	uint32_t result=0;
	uint32_t svid=ntohl(data[3]);
	
	for(i=0;i< svc->xdr->in_len/4;++i) {
		
	}
	
	for(i=0;i< svc->xdr->in_len/4;++i) {
		
	}
	
	if(svid==0x3100005b) {
		dispatch_pdsm(data);
	} else if(svid==0x3100001d) {
		dispatch_atl(data);
	} else {
		//Got dispatch for unknown serv id!
	}
	//ACK
	svc_sendreply(svc, xdr_int, &result);
}

int pdsm_client_end_session(struct CLIENT *clnt, int id, int client) {
	struct params par;
	uint32_t res;
	par.data=malloc(sizeof(int)*4);
	par.length=4;
	par.data[0]=id;
	par.data[1]=0;
	par.data[2]=0;
	par.data[3]=client_IDs[client];
#ifdef ANDROID
	if(clnt_call(clnt, amss==A6125 ? 0xd : 0xe, xdr_args, &par, xdr_result_int, &res, timeout)) {
#else
	if(android_clnt_call(clnt, amss==A6125 ? 0xd : 0xe, xdr_args, &par, xdr_result_int, &res, timeout)) {
#endif
		
		free(par.data);
		exit(-1);
	}
	free(par.data);
	
	return 0;
}

int init_gps6125() {
#ifdef ANDROID
	struct CLIENT *clnt=clnt_create(NULL, 0x3000005B, 0, NULL);
	struct CLIENT *clnt_atl=clnt_create(NULL, 0x3000001D, 0, NULL);
#else
	struct CLIENT *clnt=android_clnt_create(NULL, 0x3000005B, 0x90380d3d, NULL);
	struct CLIENT *clnt_atl=android_clnt_create(NULL, 0x3000001D, 0x51c92bd8, NULL);
#endif
	int i;
	_clnt=clnt;
	SVCXPRT *svc=svcrtr_create();
	xprt_register(svc);
	svc_register(svc, 0x3100005b, 0xb93145f7, dispatch,0);
	svc_register(svc, 0x3100005b, 0, dispatch,0);
	svc_register(svc, 0x3100001d, 0/*xb93145f7*/, dispatch,0);
	if(!clnt) {
		
		return -1;
	}
	if(!svc) {
		
		return -2;
	}

	pdsm_client_init(clnt, 2);
	pdsm_client_pd_reg(clnt, 2, 0, 0, 0, 0xF3F0FFFF, 0);
	pdsm_client_ext_status_reg(clnt, 2, 0, 0, 0, 0x4, 0);
	pdsm_client_act(clnt, 2);
	pdsm_client_pa_reg(clnt, 2, 0, 2, 0, 0x7ffefe0, 0);
	pdsm_client_init(clnt, 0xb);
	pdsm_client_xtra_reg(clnt, 0xb, 0, 3, 0, 7, 0);
	pdsm_client_act(clnt, 0xb);
	pdsm_atl_l2_proxy_reg(clnt_atl, 1,0,0);
	pdsm_atl_dns_proxy_reg(clnt_atl, 1,0);
	pdsm_client_init(clnt, 4);
	pdsm_client_lcs_reg(clnt, 4, 0,0,0,0x3f0, 0);
	pdsm_client_act(clnt, 4);

	return 0;
}


int init_gps5225() {
#ifdef ANDROID
	struct CLIENT *clnt=clnt_create(NULL, 0x3000005B, 0, NULL);
	struct CLIENT *clnt_atl=clnt_create(NULL, 0x3000001D, 0, NULL);
#else
	struct CLIENT *clnt=android_clnt_create(NULL, 0x3000005B, 0, NULL);
	struct CLIENT *clnt_atl=android_clnt_create(NULL, 0x3000001D, 0, NULL);
#endif
	int i;
	_clnt=clnt;
	SVCXPRT *svc=svcrtr_create();
	xprt_register(svc);
	svc_register(svc, 0x3100005b, 0xb93145f7, dispatch,0);
	svc_register(svc, 0x3100005b, 0, dispatch,0);
	svc_register(svc, 0x3100001d, 0/*xb93145f7*/, dispatch,0);
	if(!clnt) {
		
		return -1;
	}
	if(!svc) {
		
		return -2;
	}

	pdsm_client_init(clnt, 2);
	pdsm_client_pd_reg(clnt, 2, 0, 0, 0, 0xF310FFFF, 0xffffffff);
	pdsm_client_ext_status_reg(clnt, 2, 0, 1, 0, 4, 0xffffffff);
	pdsm_client_act(clnt, 2);
	pdsm_client_pa_reg(clnt, 2, 0, 2, 0, 0x003fefe0, 0xffffffff);
	pdsm_client_init(clnt, 0xb);
	pdsm_client_xtra_reg(clnt, 0xb, 0, 3, 0, 7, 0xffffffff);
	pdsm_client_act(clnt, 0xb);
	pdsm_atl_l2_proxy_reg(clnt_atl, 1, 4, 5);
	pdsm_atl_dns_proxy_reg(clnt_atl, 1, 6);
	pdsm_client_init(clnt, 0x4);
	pdsm_client_lcs_reg(clnt, 0x4, 0, 7, 0, 0x30f, 8);
	pdsm_client_act(clnt, 0x4);

	return 0;
}


int init_gps_rpc() {
	int fd=open("/sys/class/htc_hw/amss", O_RDONLY);
	char buf[32];
	bzero(buf, 32);
	read(fd, buf, 32);
	if(strncmp(buf, "6125", 4)==0)
		amss=A6125;
	else if((strncmp(buf, "5225", 4)==0) || (strncmp(buf, "6150", 4)==0))
		amss=A5225;
	else
		amss=A6125; //Fallback to 6125 ATM
	if(amss==A6125)
		init_gps6125();
	else if(amss==A5225)
		init_gps5225();
	return 0;
}

int init_gps_rpc_htcdream(){
	amss = A6125;
	init_gps6125();
	return 0;
}

void gps_get_position() {
	int i;
	for(i=5;i;--i) if(!can_send) sleep(1);//Time out of 5 seconds on can_send
	can_send=0;
	pdsm_get_position(_clnt, 0, 0, 1, 1, 1, 0x3B9AC9FF, 1, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,1,32,2,client_IDs[2]);
}

void exit_gps_rpc() {
	if(amss==A6125)
		pdsm_client_end_session(_clnt, 0, 2);
	//5225 doesn't seem to like end_session ?
	//Bah it ends session on itself after 10seconds.
}


///////////////////////////////
//fsodeviced plugin interface//
//////////////////////////////

void gps_query_setup(){
	init_gps_rpc_htcdream(); //for now only htcdream is supported
}
void gps_query_iteration(){
	can_send=0;
	pdsm_get_position(_clnt, 0, 0, 1, 1, 1, 0x3B9AC9FF, 1, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,1,32,2,client_IDs[2]); 
}
void gps_query_shutdown(){
//	exit_gps_rpc(); //makes the phone crash
}
