#include <syslog.h>
#include "livox_lidar_def.h"
#include "livox_lidar_api.h"


#include <unistd.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <cmath>
#include <time.h>

time_t high_start_time, high_end_time;
time_t mid_start_time, mid_end_time;
time_t low_start_time, low_end_time;
int high_count = 0;
int mid_count = 0;
int low_count = 0;
int not_high_count = 0;
int not_mid_count = 0;
int not_low_count = 0;

void PointCloudCallback(uint32_t handle, const uint8_t dev_type, LivoxLidarEthernetPacket* data, void* client_data) {
  if (data == nullptr) {
    return;
  }
  if(high_end_time - high_start_time >= 1){
    high_end_time = time(NULL);
    // printf("high speed:%ld\n",high_end_time - high_start_time);
    syslog(LOG_NOTICE,"high:%d\n",high_count);

    high_count = 0;
    not_high_count = 0;
    high_start_time = time(NULL);
  }
  if(mid_end_time - mid_start_time >= 1){
    mid_end_time = time(NULL);
    // printf("mid speed:%ld\n",mid_end_time - mid_start_time);
    syslog(LOG_NOTICE,"mid:%d\n",mid_count);
    mid_count = 0;
    not_mid_count = 0;
    mid_start_time = time(NULL);
  }
  if(low_end_time - low_start_time >= 1){
    low_end_time = time(NULL);
    // printf("low speed:%ld\n",(low_end_time - low_start_time));
    syslog(LOG_NOTICE,"low:%d\n",low_count);
    low_count = 0;
    not_low_count = 0;
    low_start_time = time(NULL);
  }


  if (data->data_type == kLivoxLidarCartesianCoordinateHighData) {
    LivoxLidarCartesianHighRawPoint *p_point_data = (LivoxLidarCartesianHighRawPoint *)data->data;
    for (uint32_t i = 0; i < data->dot_num; i++) {
      if(p_point_data[i].x == 0 && p_point_data[i].y == 0 && p_point_data[i].z == 0) {
        continue;
      }
      if(p_point_data[i].y >-100 && p_point_data[i].y < 100 && p_point_data[i].x > 0 && p_point_data[i].x < 500) {
        if(std::atan2(p_point_data[i].x ,p_point_data[i].z ) * 180 / 3.1415926 <= 55){
          high_count++;
        }else if(std::atan2(p_point_data[i].x ,p_point_data[i].z ) * 180 / 3.1415926 <= 70 && std::atan2(p_point_data[i].x ,p_point_data[i].z ) * 180 / 3.1415926 > 55){
          mid_count++;
        }else if(std::atan2(p_point_data[i].x ,p_point_data[i].z ) * 180 / 3.1415926 > 70){
          low_count++;
        }
        
      }else if(p_point_data[i].y >-100 && p_point_data[i].y < 100 && p_point_data[i].x > 0 && p_point_data[i].x > 500){
        if(std::atan2(p_point_data[i].x ,p_point_data[i].z ) * 180 / 3.1415926 <= 55 && std::atan2(p_point_data[i].x ,p_point_data[i].z ) * 180 / 3.1415926 > 40){
          not_high_count++;
        }else if(std::atan2(p_point_data[i].x ,p_point_data[i].z ) * 180 / 3.1415926 <= 70 && std::atan2(p_point_data[i].x ,p_point_data[i].z ) * 180 / 3.1415926 > 55){
          not_mid_count++;
        }else if(std::atan2(p_point_data[i].x ,p_point_data[i].z ) * 180 / 3.1415926 > 70){
          not_low_count++;
        }
      }
      // printf("%d,%d,%d,%d,%d,%d\n",low_count,not_low_count,mid_count,not_mid_count,high_count,not_high_count);

       
    }
  }
  else if (data->data_type == kLivoxLidarCartesianCoordinateLowData) {
    LivoxLidarCartesianLowRawPoint *p_point_data = (LivoxLidarCartesianLowRawPoint *)data->data;
  } else if (data->data_type == kLivoxLidarSphericalCoordinateData) {
    LivoxLidarSpherPoint* p_point_data = (LivoxLidarSpherPoint *)data->data;
  }
}

void ImuDataCallback(uint32_t handle, const uint8_t dev_type,  LivoxLidarEthernetPacket* data, void* client_data) {
  if (data == nullptr) {
    return;
  } 

}

// void OnLidarSetIpCallback(livox_vehicle_status status, uint32_t handle, uint8_t ret_code, void*) {
//   if (status == kVehicleStatusSuccess) {
//     // printf("lidar set ip slot: %d, ret_code: %d\n",
//       slot, ret_code);
//   } else if (status == kVehicleStatusTimeout) {
//     // printf("lidar set ip number timeout\n");
//   }
// }
     
void WorkModeCallback(livox_status status, uint32_t handle,LivoxLidarAsyncControlResponse *response, void *client_data) {
  if (response == nullptr) {
    return;
  }
  // printf("WorkModeCallack, status:%u, handle:%u, ret_code:%u, error_key:%u",
  //     status, handle, response->ret_code, response->error_key);

}

void RebootCallback(livox_status status, uint32_t handle, LivoxLidarRebootResponse* response, void* client_data) {
  if (response == nullptr) {
    return;
  }
  // printf("RebootCallback, status:%u, handle:%u, ret_code:%u",
  //     status, handle, response->ret_code);
}

void SetIpInfoCallback(livox_status status, uint32_t handle, LivoxLidarAsyncControlResponse *response, void *client_data) {
  if (response == nullptr) {
    return;
  }
  // printf("LivoxLidarIpInfoCallback, status:%u, handle:%u, ret_code:%u, error_key:%u",
  //     status, handle, response->ret_code, response->error_key);

  if (response->ret_code == 0 && response->error_key == 0) {
    LivoxLidarRequestReboot(handle, RebootCallback, nullptr);
  }
}

void QueryInternalInfoCallback(livox_status status, uint32_t handle, 
    LivoxLidarDiagInternalInfoResponse* response, void* client_data) {
  if (status != kLivoxLidarStatusSuccess) {
    // printf("Query lidar internal info failed.\n");
    QueryLivoxLidarInternalInfo(handle, QueryInternalInfoCallback, nullptr);
    return;
  }

  if (response == nullptr) {
    return;
  }

  uint8_t host_point_ipaddr[4] {0};
  uint16_t host_point_port = 0;
  uint16_t lidar_point_port = 0;

  uint8_t host_imu_ipaddr[4] {0};
  uint16_t host_imu_data_port = 0;
  uint16_t lidar_imu_data_port = 0;

  uint16_t off = 0;
  for (uint8_t i = 0; i < response->param_num; ++i) {
    LivoxLidarKeyValueParam* kv = (LivoxLidarKeyValueParam*)&response->data[off];
    if (kv->key == kKeyLidarPointDataHostIpCfg) {
      memcpy(host_point_ipaddr, &(kv->value[0]), sizeof(uint8_t) * 4);
      memcpy(&(host_point_port), &(kv->value[4]), sizeof(uint16_t));
      memcpy(&(lidar_point_port), &(kv->value[6]), sizeof(uint16_t));
    } else if (kv->key == kKeyLidarImuHostIpCfg) {
      memcpy(host_imu_ipaddr, &(kv->value[0]), sizeof(uint8_t) * 4);
      memcpy(&(host_imu_data_port), &(kv->value[4]), sizeof(uint16_t));
      memcpy(&(lidar_imu_data_port), &(kv->value[6]), sizeof(uint16_t));
    }
    off += sizeof(uint16_t) * 2;
    off += kv->length;
  }

  // printf("Host point cloud ip addr:%u.%u.%u.%u, host point cloud port:%u, lidar point cloud port:%u.\n",
  //     host_point_ipaddr[0], host_point_ipaddr[1], host_point_ipaddr[2], host_point_ipaddr[3], host_point_port, lidar_point_port);

  // printf("Host imu ip addr:%u.%u.%u.%u, host imu port:%u, lidar imu port:%u.\n",
  //   host_imu_ipaddr[0], host_imu_ipaddr[1], host_imu_ipaddr[2], host_imu_ipaddr[3], host_imu_data_port, lidar_imu_data_port);

}

void LidarInfoChangeCallback(const uint32_t handle, const LivoxLidarInfo* info, void* client_data) {
  if (info == nullptr) {
    // printf("lidar info change callback failed, the info is nullptr.\n");
    return;
  } 
  // printf("LidarInfoChangeCallback Lidar handle: %u SN: %s\n", handle, info->sn);
  
  // set the work mode to kLivoxLidarNormal, namely start the lidar
  SetLivoxLidarWorkMode(handle, kLivoxLidarNormal, WorkModeCallback, nullptr);

  QueryLivoxLidarInternalInfo(handle, QueryInternalInfoCallback, nullptr);

  // LivoxLidarIpInfo lidar_ip_info;
  // strcpy(lidar_ip_info.ip_addr, "192.168.1.10");
  // strcpy(lidar_ip_info.net_mask, "255.255.255.0");
  // strcpy(lidar_ip_info.gw_addr, "192.168.1.1");
  // SetLivoxLidarLidarIp(handle, &lidar_ip_info, SetIpInfoCallback, nullptr);
}

void LivoxLidarPushMsgCallback(const uint32_t handle, const uint8_t dev_type, const char* info, void* client_data) {
   struct in_addr tmp_addr;
   tmp_addr.s_addr = handle;  
  //  std::cout << "handle: " << handle << ", ip: " << inet_ntoa(tmp_addr) << ", push msg info: " << std::endl;
  //  std::cout << info << std::endl;
  return;
}

int main(int argc, const char *argv[]) {
  openlog("newsyslog", LOG_CONS|LOG_PID, LOG_USER);
  high_start_time = time(NULL);
  mid_start_time = time(NULL);
  low_start_time = time(NULL);
  if (argc != 2) {
    // printf("Params Invalid, must input config path.\n");
    return -1;
  }
  const std::string path = argv[1];

  // REQUIRED, to init Livox SDK2
  if (!LivoxLidarSdkInit(path.c_str())) {
    // printf("Livox Init Failed\n");
    LivoxLidarSdkUninit();
    return -1;
  }
  
  // REQUIRED, to get point cloud data via 'PointCloudCallback'
  SetLivoxLidarPointCloudCallBack(PointCloudCallback, nullptr);
  
  // OPTIONAL, to get imu data via 'ImuDataCallback'
  // some lidar types DO NOT contain an imu component
  SetLivoxLidarImuDataCallback(ImuDataCallback, nullptr);
  
  SetLivoxLidarInfoCallback(LivoxLidarPushMsgCallback, nullptr);
  
  // REQUIRED, to get a handle to targeted lidar and set its work mode to NORMAL
  SetLivoxLidarInfoChangeCallback(LidarInfoChangeCallback, nullptr);

  sleep(300);

  LivoxLidarSdkUninit();
  syslog(LOG_NOTICE, "Livox Quick Start Demo End!\n");
  closelog();
  return 0;
}
