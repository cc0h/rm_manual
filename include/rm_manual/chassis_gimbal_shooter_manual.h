//
// Created by qiayuan on 5/22/21.
//

#ifndef RM_MANUAL_CHASSIS_GIMBAL_SHOOTER_MANUAL_H_
#define RM_MANUAL_CHASSIS_GIMBAL_SHOOTER_MANUAL_H_
#include "rm_manual/chassis_gimbal_manual.h"

namespace rm_manual {
class ChassisGimbalShooterManual : public ChassisGimbalManual {
 public:
  explicit ChassisGimbalShooterManual(ros::NodeHandle &nh) : ChassisGimbalManual(nh) {
    ros::NodeHandle shooter_nh(nh, "shooter");
    shooter_cmd_sender_ = new ShooterCommandSender(shooter_nh, data_.referee_);
    if (!shooter_nh.getParam("gimbal_error_limit", gimbal_error_limit_))
      ROS_ERROR("gimbal error limit no defined (namespace: %s)", shooter_nh.getNamespace().c_str());
  }
 protected:
  void leftSwitchDown() override {
    ChassisGimbalManual::leftSwitchDown();
    shooter_cmd_sender_->setMode(rm_msgs::ShootCmd::STOP);
  }
  void leftSwitchMid() override {
    rm_manual::ChassisGimbalManual::leftSwitchMid();
    if (state_ == RC)
      shooter_cmd_sender_->setMode(rm_msgs::ShootCmd::READY);
  }
  void leftSwitchUp() override {
    rm_manual::ChassisGimbalManual::leftSwitchUp();
    if (state_ == RC) {
      shooter_cmd_sender_->setMode(rm_msgs::ShootCmd::READY);
      if (data_.gimbal_des_error_.error < gimbal_error_limit_)
        shooter_cmd_sender_->setMode(rm_msgs::ShootCmd::PUSH);
    }
  }
  void rightSwitchDown() override {
    ChassisGimbalManual::rightSwitchDown();
    shooter_cmd_sender_->setMode(rm_msgs::ShootCmd::STOP);
  }
  void rightSwitchUp() override {
    ChassisGimbalManual::rightSwitchUp();
    shooter_cmd_sender_->setMode(pc_shooter_mode_);
  }
  void fPress() override { if (state_ == PC) pc_shooter_mode_ = rm_msgs::ShootCmd::STOP; }
  void mouseLeftPress() override {
    if (state_ == PC) {
      pc_shooter_mode_ = rm_msgs::ShootCmd::READY;
      shooter_cmd_sender_->setMode(rm_msgs::ShootCmd::PUSH);
    }
  }
  void sendCommand(const ros::Time &time) override {
    ChassisGimbalManual::sendCommand(time);
    shooter_cmd_sender_->sendCommand(time);
  }
  ShooterCommandSender *shooter_cmd_sender_;
  double gimbal_error_limit_{};
  int pc_shooter_mode_ = rm_msgs::ShootCmd::STOP;
};
}

#endif //RM_MANUAL_CHASSIS_GIMBAL_SHOOTER_MANUAL_H_
