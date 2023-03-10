#include <sys/time.h>
#include <sys/resource.h>

#include <android/log.h>
#include <log/logger.h>
#include <log/logprint.h>

#include "cereal/messaging/messaging.h"
#include "selfdrive/common/util.h"
#include "selfdrive/common/params.h"

#include <string>


// atom
typedef struct LiveNaviDataResult {
      int speedLimit = 0;  // int;
      float safetyDistance = 0;  // Float32;
      int safetySign = 0;    // int;
      int safetySignCam = 0;    // int;
      float roadCurvature = 0;    // Float32;
      int turnInfo = 0;    // int;
      float distanceToTurn = 0;    // Float32;
      //bool  mapValid;    // bool;
      //int  mapEnable;    // bool;
      long  tv_sec;
      long  tv_nsec;

      int waze_AlertId = 0;
      int waze_AlertDistance = 0;
      int waze_RoadSpeedLimit = 0;
      std::string waze_RoadName = "";
      int waze_NavSign = 0;
      int waze_NavDistance = 0;

      std::string opkr_0 = "";
      std::string opkr_1 = "";
      std::string opkr_2 = "";
      std::string opkr_3 = "";
      std::string opkr_4 = "";
      std::string opkr_5 = "";
      std::string opkr_6 = "";
      std::string opkr_7 = "";
      std::string opkr_8 = "";
      std::string opkr_9 = "";
} LiveNaviDataResult;


int main() {
  setpriority(PRIO_PROCESS, 0, -15);
  long  nDelta_nsec = 0;
  long  tv_nsec;
  float tv_nsec2;
  bool  sBump = false;
  int   naviSel = std::stoi(Params().get("OPKRNaviSelect"));
  bool  OPKR_Debug = Params().getBool("OPKRDebug");

	char str[50];
	int num = 0;

  ExitHandler do_exit;
  PubMaster pm({"liveNaviData"});
  LiveNaviDataResult res;

  log_time last_log_time = {};
  logger_list *logger_list = android_logger_list_alloc(ANDROID_LOG_RDONLY | ANDROID_LOG_NONBLOCK, 0, 0);

  while (!do_exit) {
    // setup android logging
    if (!logger_list) {
      logger_list = android_logger_list_alloc_time(ANDROID_LOG_RDONLY | ANDROID_LOG_NONBLOCK, last_log_time, 0);
    }
    assert(logger_list);

    struct logger *main_logger = android_logger_open(logger_list, LOG_ID_MAIN);
    assert(main_logger);
   // struct logger *radio_logger = android_logger_open(logger_list, LOG_ID_RADIO);
   // assert(radio_logger);
   // struct logger *system_logger = android_logger_open(logger_list, LOG_ID_SYSTEM);
   // assert(system_logger);
   // struct logger *crash_logger = android_logger_open(logger_list, LOG_ID_CRASH);
   // assert(crash_logger);
   // struct logger *kernel_logger = android_logger_open(logger_list, (log_id_t)5); // LOG_ID_KERNEL
   // assert(kernel_logger);

    while (!do_exit) {
      log_msg log_msg;
      int err = android_logger_list_read(logger_list, &log_msg);
      if (err <= 0) break;

      AndroidLogEntry entry;
      err = android_log_processLogBuffer(&log_msg.entry_v1, &entry);
      if (err < 0) continue;
      last_log_time.tv_sec = entry.tv_sec;
      last_log_time.tv_nsec = entry.tv_nsec;

      tv_nsec2 = entry.tv_nsec / 1000000;
      tv_nsec =  entry.tv_sec * 1000ULL + long(tv_nsec2); // per 1000 = 1s

      MessageBuilder msg;
      auto framed = msg.initEvent().initLiveNaviData();

   //  opkrspdlimit, opkrspddist, opkrsigntype, opkrcurveangle, opkrremaintime, opkradasicontype, opkraveragespd

      // code based from atom
      nDelta_nsec = tv_nsec - res.tv_nsec;
      //nDelta = entry.tv_sec - res.tv_sec;

      if (OPKR_Debug)
      {
        res.tv_sec = entry.tv_sec;
        res.tv_nsec = tv_nsec;
        if( strcmp( entry.tag, "opkr0" ) == 0 ) {
          res.opkr_0 = entry.message;
        } else if ( strcmp( entry.tag, "opkr1" ) == 0 ) {
          res.opkr_1 = entry.message;
        } else if ( strcmp( entry.tag, "opkr2" ) == 0 ) {
          res.opkr_2 = entry.message;
        } else if ( strcmp( entry.tag, "opkr3" ) == 0 ) {
          res.opkr_3 = entry.message;
        } else if ( strcmp( entry.tag, "opkr4" ) == 0 ) {
          res.opkr_4 = entry.message;
        } else if ( strcmp( entry.tag, "opkr5" ) == 0 ) {
          res.opkr_5 = entry.message;
        } else if ( strcmp( entry.tag, "opkr6" ) == 0 ) {
          res.opkr_6 = entry.message;
        } else if ( strcmp( entry.tag, "opkr7" ) == 0 ) {
          res.opkr_7 = entry.message;
        } else if ( strcmp( entry.tag, "opkr8" ) == 0 ) {
          res.opkr_8 = entry.message;
        } else if ( strcmp( entry.tag, "opkr9" ) == 0 ) {
          res.opkr_9 = entry.message;
        }
      }
      else if (naviSel == 3) {
        if( strcmp( entry.tag, "opkrwazereportid" ) == 0 ) {
          std::string opkr_log_msg = entry.message;
          std::size_t found1=opkr_log_msg.find("icon_report_speedlimit");
          std::size_t found2=opkr_log_msg.find("icon_report_camera");
          std::size_t found3=opkr_log_msg.find("icon_report_speedcam");
          std::size_t found4=opkr_log_msg.find("icon_report_police");
          std::size_t found5=opkr_log_msg.find("icon_report_hazard");
          std::size_t found6=opkr_log_msg.find("icon_report_traffic");
          if (found1!=std::string::npos) {
            res.waze_AlertId = 1;
          } else if (found2!=std::string::npos) {
            res.waze_AlertId = 1;
          } else if (found3!=std::string::npos) {
            res.waze_AlertId = 1;
          } else if (found4!=std::string::npos) {
            res.waze_AlertId = 2;
          } else if (found5!=std::string::npos) {
            res.waze_AlertId = 3;
          } else if (found6!=std::string::npos) {
            res.waze_AlertId = 4;
          }
        } else if( strcmp( entry.tag, "opkrwazealertdist" ) == 0 ) {
          std::string opkr_log_msg2 = entry.message;
          strcpy(str, opkr_log_msg2.c_str());
          for(int i=0; i<strlen(str); i++){
            if(str[i] > 47 && str[i] < 58) num = num*10 + str[i]-48;		
        	}
          res.waze_AlertDistance = num;
          res.tv_sec = entry.tv_sec;
          res.tv_nsec = tv_nsec;
        } else if( strcmp( entry.tag, "opkrwazeroadspdlimit" ) == 0 ) {
          res.waze_RoadSpeedLimit = atoi( entry.message );
        } else if( strcmp( entry.tag, "opkrwazeroadname" ) == 0 ) {
          res.waze_RoadName = entry.message;
        } else if( strcmp( entry.tag, "opkrwazenavsign" ) == 0 ) {
          res.waze_NavSign = atoi( entry.message );
        } else if( strcmp( entry.tag, "opkrwazenavdist" ) == 0 ) {
          res.waze_NavDistance = atoi( entry.message );
        } else if( nDelta_nsec > 3000 ) {
          res.tv_sec = entry.tv_sec;
          res.tv_nsec = tv_nsec;
          res.waze_AlertId = 0;
          res.waze_AlertDistance = 0;
        }
      }
      else if( strcmp( entry.tag, "opkrspddist" ) == 0 )
      {
        res.tv_sec = entry.tv_sec;
        res.tv_nsec = tv_nsec;
        res.safetyDistance = atoi( entry.message );
      }
      else if( strcmp( entry.tag, "opkrspdlimit" ) == 0 )
      {
        res.speedLimit = atoi( entry.message );
      }
      else if( strcmp( entry.tag, "opkrsigntype" ) == 0 )
      {
        res.tv_sec = entry.tv_sec;
        res.tv_nsec = tv_nsec;
        res.safetySignCam = atoi( entry.message );
        if (res.safetySignCam == 124 && naviSel == 1) {
          sBump = true;
        }
      }
      else if( strcmp( entry.tag, "opkrroadsigntype" ) == 0 )
      {
        res.safetySign = atoi( entry.message );
      }
      else if( naviSel == 1 && (res.safetyDistance > 1 && res.safetyDistance < 60) && (strcmp( entry.tag, "AudioFlinger" ) == 0) )  //   msm8974_platform
      {
        res.safetyDistance = 0;
        res.speedLimit = 0;
        res.safetySignCam = 0;
      }
      else if( strcmp( entry.tag, "opkrturninfo" ) == 0 )
      {
        res.turnInfo = atoi( entry.message );
      }
      else if( strcmp( entry.tag, "opkrdistancetoturn" ) == 0 )
      {
        res.distanceToTurn = atoi( entry.message );
      }
      else if( nDelta_nsec > 10000 && naviSel == 2)
      {
        res.tv_sec = entry.tv_sec;
        res.tv_nsec = tv_nsec;
        res.safetyDistance = 0;
        res.speedLimit = 0;
        res.safetySign = 0;
        // system("logcat -c &");
      }
      else if( nDelta_nsec > 5000 && naviSel == 1)
      {
        if (res.safetySignCam == 197 && res.safetyDistance < 100) {
          res.safetyDistance = 0;
          res.speedLimit = 0;
          res.safetySignCam = 0;
        }
        else if ( res.safetySignCam == 124 && (!sBump) )
        {
          res.safetySignCam = 0;
        }
        else if (res.safetySignCam != 0 && res.safetySignCam != 124 && res.safetyDistance < 50 && res.safetyDistance > 0)
        {
          res.safetyDistance = 0;
          res.speedLimit = 0;
          res.safetySignCam = 0;
        }
        else if( nDelta_nsec > 10000 )
        {
          res.tv_sec = entry.tv_sec;
          res.tv_nsec = tv_nsec;
          res.safetyDistance = 0;
          res.speedLimit = 0;
          res.safetySignCam = 0;
          // system("logcat -c &");
        }
      }

      framed.setSpeedLimit( res.speedLimit );  // int;
      framed.setSafetyDistance( res.safetyDistance );  // raw_target_speed_map_dist Float32;
      framed.setSafetySign( res.safetySign ); // int;
      framed.setSafetySignCam( res.safetySignCam ); // int;
      // framed.setRoadCurvature( res.roadCurvature ); // road_curvature Float32;
      framed.setTurnInfo( res.turnInfo );  // int;
      framed.setDistanceToTurn( res.distanceToTurn );  // Float32;
      framed.setTs( res.tv_sec );
      //framed.setMapEnable( res.mapEnable );
      //framed.setMapValid( res.mapValid );

      if (OPKR_Debug) {
        framed.setOpkr0( res.opkr_0 );
        framed.setOpkr1( res.opkr_1 );
        framed.setOpkr2( res.opkr_2 );
        framed.setOpkr3( res.opkr_3 );
        framed.setOpkr4( res.opkr_4 );
        framed.setOpkr5( res.opkr_5 );
        framed.setOpkr6( res.opkr_6 );
        framed.setOpkr7( res.opkr_7 );
        framed.setOpkr8( res.opkr_8 );
        framed.setOpkr9( res.opkr_9 );
      }
      if (naviSel == 3) {
        framed.setWazeAlertId( res.waze_AlertId );
        framed.setWazeAlertDistance( res.waze_AlertDistance );
        framed.setWazeRoadSpeedLimit( res.waze_RoadSpeedLimit );
        framed.setWazeRoadName( res.waze_RoadName );
        framed.setWazeNavSign( res.waze_NavSign );
        framed.setWazeNavDistance( res.waze_NavDistance );
      }

    /*
    iNavi Road signtype
    101 ?????? ??????
    102 ????????????
    107 ???????????????
    5 ?????????
    105 ????????????
    15 ?????????
    10 ??????
    9 ????????????
    111 ???????????????
    18 ????????? ??????
    203 ????????????

    iNavi Cam signtype
    11, 12 ????????????
    6 ???????????????
    2 ?????????????????????
    1 ????????????
    3 ??????????????????
    4, 7 ?????????????????? ??????
    5 ????????? ??????
    8 ???????????? ??????
    101 ?????? ??????
    15 ??????????????????
    16 ?????????
    18 ?????????
    118 ????????????
    20 ??????????????????
    203 ????????????
    204 ???????????????


    mappy signtype
    111 ????????? ?????????
    112 ?????? ?????????
    113 ????????????
    118, 127 ?????????????????????
    122 ???????????? ??????
    124 ???????????????
    129 ?????????
    131 ???????????????(?????????????????????)  
    135 ?????????(??????????????????)  - ??????
    150 ?????????(?????????????????????)  - ??????
    165 ????????????
    195, 197 ????????????, ????????????????????????
    198 ????????????????????????
    199 ????????????????????????
    129 ?????????????????????
    123 ???????????????
    200 ????????????(????????? ?????????)
    231 ??????(?????????, ????????????)
    246 ????????????????????????
    247 ????????????
    248 ??????????????????
    249 ??????????????????
    250 ????????????
    251 ??????????????????
   */  


      pm.send("liveNaviData", msg);
    }

    android_logger_list_free(logger_list);
    logger_list = NULL;
    util::sleep_for(500);
  }

  if (logger_list) {
    android_logger_list_free(logger_list);
  }

  return 0;
}
