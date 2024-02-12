// Include Arduino Library
#ifndef Arduino_h
	#include <Arduino.h>
#endif

// Configuration Definitions
// -------------------------
#define _AT_WAIT_DELAY_						(uint8_t)20

// Define Functon Timeout
// ----------------------
#define _TIMEOUT_AT_				    	(uint16_t)1000
#define _TIMEOUT_FCLASS_			    	(uint16_t)1000
#define _TIMEOUT_CGMI_				    	(uint16_t)1000
#define _TIMEOUT_CGMM_				    	(uint16_t)1000
#define _TIMEOUT_SWPKGV_			    	(uint16_t)1000
#define _TIMEOUT_CGSN_				    	(uint16_t)1000
#define _TIMEOUT_CFUN_				    	(uint16_t)1000
#define _TIMEOUT_CMEE_				    	(uint16_t)1000
#define _TIMEOUT_CEER_				    	(uint16_t)1000
#define _TIMEOUT_ATE_				    	(uint16_t)1000
#define _TIMEOUT_K_	    			    	(uint16_t)1000
#define _TIMEOUT_CPIN_				    	(uint16_t)1000
#define _TIMEOUT_CCID_				    	(uint16_t)1000
#define _TIMEOUT_SIMDET_			    	(uint16_t)1000
#define _TIMEOUT_GPIO_				    	(uint16_t)1000
#define _TIMEOUT_SLED_				    	(uint16_t)1000
#define _TIMEOUT_SLEDSAV_			    	(uint16_t)1000
#define _TIMEOUT_E2SLRI_			    	(uint16_t)1000
#define _TIMEOUT_CREG_				    	(uint16_t)1000
#define _TIMEOUT_CGDCONT_			    	(uint16_t)1000
#define _TIMEOUT_SGACT_				    	(uint16_t)60000
#define _TIMEOUT_WS46_				    	(uint16_t)1000
#define _TIMEOUT_RFSTS_				    	(uint16_t)1000
#define _TIMEOUT_MONI_				    	(uint16_t)1000
#define _TIMEOUT_SCFG_				    	(uint16_t)1000
#define _TIMEOUT_SCFGEXT_			    	(uint16_t)1000
#define _TIMEOUT_SCFGEXT2_			    	(uint16_t)1000
#define _TIMEOUT_SCFGEXT3_			    	(uint16_t)1000
#define _TIMEOUT_FRWL_	    		    	(uint16_t)1000
#define _TIMEOUT_ICMP_	    		    	(uint16_t)1000
#define _TIMEOUT_Ping_	    		    	(uint16_t)6000
#define _TIMEOUT_CCLK_	    		    	(uint16_t)1000
#define _TIMEOUT_CTZU_	    		    	(uint16_t)1000
#define _TIMEOUT_NITZ_	    		    	(uint16_t)1000
#define _TIMEOUT_CCLKMODE_	   		    	(uint16_t)1000
#define _TIMEOUT_SD_	    		    	(uint16_t)65000
#define _TIMEOUT_SH_	    		    	(uint16_t)5000
#define _TIMEOUT_SL_	    		    	(uint16_t)1000
#define _TIMEOUT_SA_	    		    	(uint16_t)15000
#define _TIMEOUT_SS_	    		    	(uint16_t)1000
#define _TIMEOUT_SO_	    		    	(uint16_t)5000
#define _TIMEOUT_SI_	    		    	(uint16_t)1000
#define _TIMEOUT_SSEND_	    		    	(uint16_t)2000
#define _TIMEOUT_SRECV_	    		    	(uint16_t)5000
#define _TIMEOUT_FTPCLOSE_                  (uint16_t)65000
#define _TIMEOUT_FTPCWD_                    (uint16_t)65000
#define _TIMEOUT_FTPFSIZE_                  (uint16_t)65000
#define _TIMEOUT_FTPGETPKT_					(uint16_t)65000
#define _TIMEOUT_FTPOPEN_					(uint16_t)65000
#define _TIMEOUT_FTPRECV_					(uint16_t)65000
#define _TIMEOUT_FTPTO_						(uint16_t)65000
#define _TIMEOUT_FTPTYPE_					(uint16_t)65000
#define _TIMEOUT_Z_							(uint16_t)5000
#define _TIMEOUT_SHDN_						(uint16_t)5000