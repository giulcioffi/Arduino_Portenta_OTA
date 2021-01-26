/*
   This file is part of ArduinoIoTCloud.

   Copyright 2020 ARDUINO SA (http://www.arduino.cc/)

   This software is released under the GNU General Public License version 3,
   which covers the main part of arduino-cli.
   The terms of this license can be found at:
   https://www.gnu.org/licenses/gpl-3.0.en.html

   You can be released from the requirements of the above licenses by purchasing
   a commercial license. Buying such a license is mandatory if you want to modify or
   otherwise use the software for commercial activities involving the Arduino
   software without disclosing the source code of your own applications. To purchase
   a commercial license, send an email to license@arduino.cc.
*/

/******************************************************************************
   INCLUDE
 ******************************************************************************/

#include "Arduino_Portenta_OTA_SD.h"

#include "BSP.h"
#include "stm32h7xx_hal_sd.h"

#include <assert.h>

using namespace arduino;

/******************************************************************************
   CONSTANTS
 ******************************************************************************/

static char const SD_UPDATE_FILENAME[] = "UPDATE.BIN";

/******************************************************************************
   CTOR/DTOR
 ******************************************************************************/

Arduino_Portenta_OTA_SD::Arduino_Portenta_OTA_SD(StorageTypePortenta const storage_type, uint32_t const data_offset)
: Arduino_Portenta_OTA(storage_type, data_offset)
, _bd{NULL}
, _block_device()
, _fs_sd{NULL}
{
  assert(_storage_type == SD_FATFS     ||
         _storage_type == SD_LITTLEFS  ||
         _storage_type == SD_FATFS_MBR ||
         _storage_type == SD_LITTLEFS_MBR);
}

/******************************************************************************
   PUBLIC MEMBER FUNCTIONS
 ******************************************************************************/

bool Arduino_Portenta_OTA_SD::init()
{
  if (_block_device.init())
    return false;

   if(_storage_type == SD_FATFS)
   {
    _fs_sd = new mbed::FATFileSystem("fs");
    int const err =  _fs_sd->mount(&_block_device);
    if (err) {
      Serial1.print("Error while mounting the filesystem. Err = ");
      Serial1.println(err);
      return false;
    }
    return true;
  }

  if (_storage_type == SD_FATFS_MBR)
  {
    _bd = new mbed::MBRBlockDevice(reinterpret_cast<mbed::BlockDevice *>(&_block_device), 1);
    _fs_sd = new mbed::FATFileSystem("fs");
    int const err =  _fs_sd->mount(_bd);
    if (err)
    {
      Serial1.print("Error while mounting the filesystem. Err = ");
      Serial1.println(err);
      return false;
    }
    return true;
  }

  return false;
}

bool Arduino_Portenta_OTA_SD::open()
{
  if (_storage_type == SD_FATFS || _storage_type == SD_FATFS_MBR)
  {
    DIR * dir = NULL;
    if ((dir = opendir("/fs")) != NULL)
    {
      /* print all the files and directories within directory */
      struct dirent * entry = NULL;
      while ((entry = readdir(dir)) != NULL)
      {
        if (String(entry->d_name) == "UPDATE.BIN")
        {
          struct stat stat_buf;
          stat("/fs/UPDATE.BIN", &stat_buf);
          _program_length = stat_buf.st_size;
          closedir(dir);
          return true;
        }
      }
      closedir(dir);
    }
    return false;
  }

  return false;
}
