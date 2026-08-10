#include "dev_imu.h"

void MPU6050_WriteReg(uint8_t reg_addr,uint8_t reg_data)
{
    HAL_I2C_Mem_Write(&hi2c1, MPU6050_SLAVE_ADDRESS, reg_addr, 1, &reg_data, 1, 100);
}
void MPU6050_ReadData(uint8_t reg_add, unsigned char *data, uint8_t num)
{
    HAL_I2C_Mem_Read(&hi2c1, MPU6050_SLAVE_ADDRESS, reg_add, 1, data, num, 100);
}
void dev_imu_init(void)
{
    HAL_Delay(20);
	MPU6050_WriteReg(MPU6050_RA_PWR_MGMT_1, 0x00);	     //解除休眠状态
	MPU6050_WriteReg(MPU6050_RA_SMPLRT_DIV , 0x07);	    //陀螺仪采样率
	MPU6050_WriteReg(MPU6050_RA_CONFIG , 0x06);	
	MPU6050_WriteReg(MPU6050_RA_ACCEL_CONFIG , 0x01);	  //配置加速度传感器工作在2G模式
	MPU6050_WriteReg(MPU6050_RA_GYRO_CONFIG, 0x18);     //陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
}

/**
  * @brief   读取MPU6050的加速度数据
  * @param   
  * @retval  
  */
void dev_imu_read_acc(AccelData *accData)
{
    uint8_t buf[6];
    MPU6050_ReadData(MPU6050_ACC_OUT, buf, 6);
    accData->x = (buf[0] << 8) | buf[1];
    accData->y = (buf[2] << 8) | buf[3];
    accData->z = (buf[4] << 8) | buf[5];
}

/**
  * @brief   读取MPU6050的角加速度数据
  * @param   
  * @retval  
  */
void dev_imu_read_gyro(GyroData *gyroData)
{
    uint8_t buf[6];
    MPU6050_ReadData(MPU6050_GYRO_OUT,buf,6);
    gyroData->x = (buf[0] << 8) | buf[1];
    gyroData->y = (buf[2] << 8) | buf[3];
    gyroData->z = (buf[4] << 8) | buf[5];
}